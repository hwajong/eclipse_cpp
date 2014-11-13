#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

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

int makelist(char *s, const char *delimiters, char** list, int MAX_LIST)
{
	char *snew = NULL;
	int numtokens = 0;

	if((s == NULL) || (delimiters == NULL)) return -1;

	snew = s + strspn(s, delimiters); /* delimiters를 skip */
	if((list[numtokens] = strtok(snew, delimiters)) == NULL) return numtokens;

	numtokens = 1;

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
	int pid = 0;

	pid = fork();

	// fork error
	if(pid == -1) exit(0);

	// 부모 프로세스를 끝낸다.
	if(pid > 0) exit(0);

	// child process
	execvp(cmdvector[0], cmdvector);
	fatal("exec error");
}

// 명령어 실행
void execute_cmdgrp(char *cmdgrp)
{
	int count = 0;
	char* last_arg = NULL;
	int len = 0;

	count = makelist(cmdgrp, " \t", cmdvector, MAX_CMD_ARG);
	last_arg = cmdvector[count-1];
	len = strlen(last_arg);

	if(len && last_arg[len-1] == '&')
	{
		last_arg[len-1] = '\0';
		if(strlen(last_arg) == 0) cmdvector[count-1] = NULL;
		background_run();
	}

	execvp(cmdvector[0], cmdvector);
	fatal("exec error");
}

// 사용자가 입력한 명령이 exit 이면 프로그램을 종료한다.
void check_exit(const char* cmdgrp)
{
	char buff[BUFSIZ] = {0,};
	strcpy(buff, cmdgrp);
	makelist(buff, " \t", cmdvector, MAX_CMD_ARG);

	if(strcasecmp(cmdvector[0], "exit") == 0)
	{
		exit(0);
	}
}

// 사용자가 입력한 명령이 cd 이면 chdir() 로 디렉토리를 바꾼다.
int check_cd(const char* cmdgrp)
{
	char buff[BUFSIZ] = {0,};
	strcpy(buff, cmdgrp);
	makelist(buff, " \t", cmdvector, MAX_CMD_ARG);

	if(strcmp(cmdvector[0], "cd") == 0)
	{
		chdir(cmdvector[1]);
		return 1;
	}

	return 0;
}

// cmdline 파싱
// exit 체크
// cd 명령 수행
// execute_cmdgrp() 호출
void execute_cmdline(char* cmdline)
{
	int i = 0;
	int count = 0;
	int cd_ok = 0;

	count = makelist(cmdline, ";", cmdgrps, MAX_CMD_GRP);

	for(i = 0; i < count; ++i)
	{
		// exit 명령
		check_exit(cmdgrps[i]);

		// cd 명령
		cd_ok = check_cd(cmdgrps[i]);
		if(cd_ok)
		{
			continue;
		}

		switch(fork())
		{
		case -1:
			fatal("fork error");
			break;

		case 0: // child process
			execute_cmdgrp(cmdgrps[i]);
			exit(0);

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








