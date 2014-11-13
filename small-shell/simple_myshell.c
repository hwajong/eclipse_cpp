#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CMD_ARG 10
#define MAX_CMD_GRP 10

const char *prompt = "myshell> ";
char* cmdgrps[MAX_CMD_GRP];
char* cmdvector[MAX_CMD_ARG];
char cmdline[BUFSIZ];

void fatal(char *str)
{
	perror(str);
	exit(1);
}

// 문자열을 명령어 단위로 분해한다.
int makelist(char *s, const char *delimiters, char** list, int MAX_LIST)
{
	if((s == NULL) || (delimiters == NULL)) return -1;

	char *snew = s + strspn(s, delimiters); /* delimiters를 skip */
	if((list[0] = strtok(snew, delimiters)) == NULL) return 0;

	int numtokens = 1;

	while(1)
	{
		if((list[numtokens] = strtok(NULL, delimiters)) == NULL) break;
		if(numtokens == (MAX_LIST - 1)) return -1;
		numtokens++;
	}

	return numtokens;
}

// 백그라운드로 명령을 실행한다.
void background_run()
{
	int pid = fork();

	// 부모 프로세스를 죽이고 고아 프로세스로 만든다.
	if(pid != 0) exit(0);

	switch(pid)
	{
	case -1:
		fatal("fork error");
		break;

	case 0: // child process
		execvp(cmdvector[0], cmdvector);
		fatal("exec error");
		break;
	}
}

// 명령어 실행
void execute_cmdgrp(char *cmdgrp)
{
	int count = makelist(cmdgrp, " \t", cmdvector, MAX_CMD_ARG);

//	for(int i=0; i<count; i++)
//	{
//		printf("cmdvector[%d] :%s\n", i, cmdvector[i]);
//	}

	int size = strlen(cmdvector[count-1]);
	if(size > 0 && cmdvector[count-1][size-1] == '&')
	{
		cmdvector[count-1][size-1] = '\0';
		if(strlen(cmdvector[count-1]) == 0) cmdvector[count-1] = NULL;
		background_run();
		exit(0);
	}

	execvp(cmdvector[0], cmdvector);
	fatal("exec error");
}

// exit 명령이면 종료한다.
void check_exit(const char* cmdgrp)
{
	char* s = strdup(cmdgrp);
	makelist(s, " \t", cmdvector, MAX_CMD_ARG);

	if(strncasecmp(cmdvector[0], "exit", 4) == 0)
	{
		free(s);
		exit(0);
	}

	free(s);
}

// cd 명령어이면 chmod() 로 디렉토리를 바꾼다.
int check_cd(const char* cmdgrp)
{
	char* s = strdup(cmdgrp);
	makelist(s, " \t", cmdvector, MAX_CMD_ARG);

	if(strncmp(cmdvector[0], "cd", 2) == 0)
	{
		chdir(cmdvector[1]);
		free(s);
		return 1;
	}

	free(s);
	return 0;
}

// 입력받은 cmdline 문자열을 파싱하여 각 명령을 수행한다.
void execute_cmdline(char* cmdline)
{
	int count = makelist(cmdline, ";", cmdgrps, MAX_CMD_GRP);

	for(int i = 0; i < count; ++i)
	{
		// exit 명령 처리
		check_exit(cmdgrps[i]);

		// cd 명령 처리
		if(check_cd(cmdgrps[i])) continue;

		switch(fork())
		{
		case -1:
			fatal("fork error");
			break;

		case 0: // child process
			execute_cmdgrp(cmdgrps[i]);
			exit(0);
			break;

		default: // parent process
			wait(NULL);
			fflush(stdout);
		}
	}
}

int main(int argc, char**argv)
{
	while(1)
	{
		fputs(prompt, stdout);
		fgets(cmdline, BUFSIZ, stdin);
		cmdline[strlen(cmdline) - 1] = '\0';

		execute_cmdline(cmdline);
	}
}








