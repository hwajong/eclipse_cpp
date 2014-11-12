#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

// convert urls to string special chars
// we need to convert the url so that we can have files with names
void url_encode(unsigned char *s, char *enc)
{
    char rfc[256] = {0};
    int i = 0;
    for (; i < 256; i++) {
        rfc[i] = isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_'
            ? i : 0;
    }

    char *tb = rfc;

    for (; *s; s++) {
        if (tb[*s]) sprintf(enc, "%c", tb[*s]);
        else        sprintf(enc, "%%%02X", *s);
        while (*++enc);
    }
}

void parse_url(char *url, char *hostname, int *port, char *path)
{
    char nurl[1024], nhostname[1024], portstr[16];
    char* tok_str_ptr = NULL;
    int offset = 1;

    // copy the original url to work on
    strcpy(nurl, url);
    char *ppath = &(nurl[0]);

    // check if the address starts with http://
    // e.g. somehost.com/index.php vs. http://somehost.com/index.php
    if(NULL != strstr(ppath, "http://"))
    {
        ppath = &(nurl[6]);
        offset += 6;
    }

    // finding the hostname
    tok_str_ptr = strtok(ppath, "/");
    sprintf(nhostname, "%s", tok_str_ptr);

    // check if the hostname also comes with a port no or not
    // e.g. somehost.com:8080/index.php
    if(NULL != strstr(nhostname, ":"))
    {
        tok_str_ptr = strtok(nhostname, ":");
        sprintf(hostname, "%s", tok_str_ptr);
        tok_str_ptr = strtok(NULL, ":");
        sprintf(portstr, "%s", tok_str_ptr);
        *port = atoi(portstr);
    } else {
        sprintf(hostname, "%s", nhostname);
    }

	// the rest of the url gives us the path
    // e.g. /index.php in somehost.com/index.php
    ppath = &(url[strlen(hostname) + offset]);
    sprintf(path, "%s", ppath);
    if(strcmp(path, "") == 0)
    {
        sprintf(path, "/");
    }
}

int create_server_socket(const char* ip, int port)
{
 	struct sockaddr_in serv_addr;
	int sockfd;

	bzero((char*)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ip);

	// create listening socket
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0)
	{
		perror("failed to initialize socket");
	}

	// bind port
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("failed to bind socket");
	}

	// start listening
	listen(sockfd, 50);

	return sockfd;
}

void child_process(int newsockfd)
{
	// receive proxy client request
	char buffer[4096] = {0,};
	recv(newsockfd, buffer, 4096, 0);
	printf("*proxy client request - recv data : %s\n", buffer);

	// we only receive target url
	char url[4096];
	sscanf(buffer, "%s", url);

	printf("*target url : %s\n", url);

	if(strlen(url) == 0)
	{
		// invalid request
		sprintf(buffer, "BAD REQUEST");
		send(newsockfd, buffer, strlen(buffer), 0);
		goto EXIT_CLEAN_UP;
	}

	// break down the url to know the host and path
	char url_host[256], url_path[256];
	int port = 80;		// default http port - can be overridden
	parse_url(url, url_host, &port, url_path);

	// encoding the url for later use
	char url_encoded[4096];	// used for cahce filenames
	url_encode((unsigned char*)url, url_encoded);

	printf("*\t-> url_host: %s\n", url_host);
	printf("*\t-> port: %d\n", port);
	printf("*\t-> url_path: %s\n", url_path);
	printf("*\t-> url_encoded: %s\n", url_encoded);

	// we need to find the ip for the host
	struct hostent *hostent;
	if((hostent = gethostbyname(url_host)) == NULL)
	{
		fprintf(stderr, "failed to resolve %s: %s\n", url_host, strerror(errno));
		goto EXIT_CLEAN_UP;
	}

	// create socket
	int rsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(rsockfd < 0)
	{
		perror("failed to create remote socket");
		goto EXIT_CLEAN_UP;
	}

	struct sockaddr_in host_addr;
	bzero((char*)&host_addr, sizeof(host_addr));
	host_addr.sin_port = htons(port);
	host_addr.sin_family = AF_INET;
	bcopy((char*)hostent->h_addr, (char*)&host_addr.sin_addr.s_addr, hostent->h_length);

	// try connecting to the remote host
	if(connect(rsockfd, (struct sockaddr*)&host_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("failed to connect to remote server");
		goto EXIT_CLEAN_UP;
	}

	printf("*\t-> connected to host: %s w/ ip: %s\n", url_host, inet_ntoa(host_addr.sin_addr));

	// CACHING CHECK
	char filepath[256]; 	// used for cache file paths
	sprintf(filepath, "./cache/%s", url_encoded);
	if (0 != access(filepath, 0)) {
		sprintf(buffer,"GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", url_path, url_host);
		printf("*\t-> no cached...\n");

		goto request;
	}

	// GET CACHE DATA
	int cfd = open (filepath, O_RDWR);
	bzero((char*)buffer, 4096);
	// reading the first chunk is enough
	read(cfd, buffer, 4096);
	close(cfd);

	// find the first occurunce of "Date:" in response -- NULL if none.
	// ex. Date: Fri, 18 Apr 2014 02:57:20 GMT
	char* dateptr = strstr(buffer, "Date:");
	if(NULL != dateptr)
	{
		// response has a Date field, like Date: Fri, 18 Apr 2014 02:57:20 GMT

		char datetime[256];	// the date-time when we last cached a url
		bzero((char*)datetime, 256);
		// skip 6 characters, namely "Date: "
		// and copy 29 characters, like "Fri, 18 Apr 2014 02:57:20 GMT"
		strncpy(datetime, &dateptr[6], 29);

		// send CONDITIONAL GET
		// If-Modified-Since the date that we cached it
		sprintf(buffer,"GET %s HTTP/1.0\r\nHost: %s\r\nIf-Modified-Since: %s\r\nConnection: close\r\n\r\n", url_path, url_host, datetime);

		printf("*\t-> conditional GET...\n");
		printf("*\t-> If-Modified-Since: %s\n", datetime);

	}
	else {
		// generally all http responses have Date filed, but just in case!
		sprintf(buffer,"GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", url_path, url_host);
		printf("*\t-> the response had no date field\n");
	}

	int n;

request:
	// send the request to remote host
	n = send(rsockfd, buffer, strlen(buffer), 0);

	if(n < 0)
	{
		perror("failed to write to remote socket");
		goto EXIT_CLEAN_UP;
	}

DO_CACHE:
	// now we have sent the request, we need to get the response and
	// cache it for later uses

	/*
	 * CASE1: we had no previous cache
	 * CASE2: we had previous cache, but it did not have a Date field
	 * CASE3: we had a previous cache, and we have sent a conditional GET
	 * 		sub-CASE31: our cached copy is stale
	 * 		sub-CASE32: our cached copy is up-to-date
	 */
	cfd = -1; // cache file descriptor

	// since the response can be huge, we might need to iterate to
	// read all of it
	do
	{
		bzero((char*)buffer, 4096);

		// recieve from remote host
		n = recv(rsockfd, buffer, 4096, 0);
		// if we have read anything - otherwise END-OF-FILE
		if(n > 0)
		{
			// if this is the first time we are here
			// meaning: we are reading the http response header
			if(cfd == -1)
			{
				float ver;
				// read the first line to discover the response code
				// ex. HTTP/1.0 200 OK
				// ex. HTTP/1.0 304 Not Modified
				// we only care about these two!
				int response_code;
				sscanf(buffer, "HTTP/%f %d", &ver, &response_code);

				printf("*\t-> response_code: %d\n", response_code);

				// if it is not 304 -- anything other than sub-CASE32
				if(response_code != 304)
				{
					// create the cache-file to save the content
					sprintf(filepath, "./cache/%s", url_encoded);
					if((cfd = open(filepath, O_RDWR|O_TRUNC|O_CREAT, S_IRWXU)) < 0)
					{
						perror("failed to create cache file");
						goto EXIT_CLEAN_UP;
					}

					printf("*\t-> from remote host...\n");

				} else {
					// if it is 304 -- sub-CASE32
					// our content is already up-to-date
					printf("***\t-> not modified\n");
					printf("***\t-> from local cache...\n");

					// send the response to client from local cache
					goto FROM_CACHE;
				}
			}

			// save to cache
			write(cfd, buffer, n);
		}
	} while(n > 0);
	close(cfd);

	// up to here we only cached the response, now we need to send it
	// back to the requesting client

FROM_CACHE:

	// read from cache file
	sprintf(filepath, "./cache/%s", url_encoded);
	if((cfd = open(filepath, O_RDONLY)) < 0)
	{
		perror("failed to open cache file");
		goto EXIT_CLEAN_UP;
	}
	do
	{
		bzero((char*)buffer, 4096);
		n = read(cfd, buffer, 4096);
		if(n > 0)
		{
			// send it to the client
			send(newsockfd, buffer, n, 0);
		}
	} while(n > 0);
	close(cfd);

EXIT_CLEAN_UP:
	close(rsockfd);

}

void main_loop(int sockfd)
{
	struct sockaddr_in cli_addr;
	bzero((char*)&cli_addr, sizeof(cli_addr));
	int clilen = sizeof(cli_addr);


	while(1)
	{
		int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t *)&clilen);

		pid_t chpid = fork();
		if(chpid == 0) // if child process
		{
			child_process(newsockfd);

			printf("*\t-> child process exiting...\n");
			close(newsockfd);
			close(sockfd);
			exit(0);
		}

		// parent process
		close(newsockfd);
	}
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("Using:\n\t%s <ip to bind> <port to bind>\n", argv[0]);
		return -1;
	}

	const char* ip = argv[1];
	int port = atoi(argv[2]);
	printf("proxy server(%s:%d) starting...\n", ip, port);


	// checking if the cache directory exists
	struct stat st = {0};
	if (stat("./cache/", &st) == -1) {
		mkdir("./cache/", 0700);
	}

	int sockfd = create_server_socket(ip, port);
	main_loop(sockfd);


	close(sockfd);
	return 0;
}








