#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>

#define  BUFF_SIZE   (1024 * 1024) // 1MB
char recv_buff[BUFF_SIZE] = {0,};
int nread = 0;

void print_usage(const char* prog_name)
{
	printf("Using:\n\t%s <proxy address> <proxy port> <url to retrieve>\n", prog_name);
}

void request_to_proxy(const char* proxy_ip, int proxy_port, const char* target_url)
{
	// CREATE SOCKET
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0)
	{
		perror("failed to create remote socket");
		exit(0);
	}

    struct sockaddr_in proxy_addr;
    bzero((char*)&proxy_addr, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
	proxy_addr.sin_addr.s_addr= inet_addr(proxy_ip);  // server address

	// CONNECT
    if(connect(sockfd, (struct sockaddr*)&proxy_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("error : failed to connect to proxy");
		goto EXIT_CLEAN_UP;
    }

    printf("*connected to ip: %s\n", inet_ntoa(proxy_addr.sin_addr));

	// SEND
	write(sockfd, target_url, strlen(target_url)+1);      // +1: NULL까지 포함해서 전송
    printf("*send ok\n");

	nread = read(sockfd, recv_buff, BUFF_SIZE);
	if(nread < 0)
	{
        perror("error : failed to recv from proxy");
		goto EXIT_CLEAN_UP;
	}

	int n;

	do
	{
		n = read(sockfd, recv_buff+nread, BUFF_SIZE-nread);
		if(n > 0)
		{
			nread += n;
		}
	}
	while(n >0);

	printf("*recv bytes : %d bytes\n", nread);

	close(sockfd);
	return;

EXIT_CLEAN_UP:
	close(sockfd);
	exit(0);
}

void save_to_file(const char* fname)
{
	FILE* fp = fopen(fname, "w");
	if(fp < 0)
	{
        perror("error : failed to open file");
		exit(0);
	}

	int nwrite = fwrite(recv_buff, 1, nread, fp);
	if(nwrite != nread)
	{
        perror("error : failed to write file");
		exit(0);
	}

	printf("*success to write! - %s\n", fname);
}

int main(int argc, char** argv)
{
	// process user input
	if(argc != 4)
	{
		print_usage(argv[0]);
		return -1;
	}

	char* proxy_ip = argv[1];
	int proxy_port = atoi(argv[2]);
	char* target_url = argv[3];

	printf("*proxy_ip   : %s\n", proxy_ip);
	printf("*proxy_port : %d\n", proxy_port);
	printf("*target_url : %s\n", target_url);

	request_to_proxy(proxy_ip, proxy_port, target_url);

	save_to_file(target_url);

	return 0;
}
