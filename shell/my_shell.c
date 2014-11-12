#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define WRITE_REDIRECT  1
#define APEND_REDIRECT  2
#define INPUT_REDIRECT  3

const char *fname_hist = ".history";
const char sep[] = " \t";
const char *my_path[] = {
	"./",
	"/usr/bin/",
	"/bin/",
	NULL
};

// 문자열의 앞뒤 공백을 제거한다.
static void trim(char * s)
{
	char * p = s;
	int l = strlen(p);

	while(isspace(p[l - 1])) p[--l] = 0;
	while(* p && isspace(* p)) ++p, --l;

	memmove(s, p, l + 1);
}

// 명령어가 실행가능한지 여부 리턴한다.
// 실행가능 명령일 경우 절대경로를 fpath 에 복사한다.
static int is_executable(const char *cmd, char *fpath)
{
	for(int i=0; i<(int)(sizeof(my_path)/sizeof(char*)-1); i++)
	{
		sprintf(fpath, "%s%s", my_path[i], cmd);
		//printf("[%s]\n", fpath);

		if(access(fpath, X_OK) == 0) return 1;
	}

	return 0;
}

// 명령어를 히스토리 파일에 저장한다.
static void save_cmd_history(const char *cmd)
{
	FILE *fp = fopen(fname_hist, "a");
	if(!fp)
	{
		printf("error - can not open history file\n");
		exit(-1);
	}

	fprintf(fp, "%s\n", cmd);

	fclose(fp);
}

// 명령어 히스토리 저장 파일 삭제
static void reset_cmd_history()
{
	remove(fname_hist);
}

// 명령어 히스토리를 출력한다.
static void print_cmd_history()
{
	char cmd[1024];
	int ncount = 0;
	FILE *fp = fopen(fname_hist, "r");
	if(fp == NULL)
	{
		printf("no history!\n");
		return;
	}

	while(fgets(cmd, 1024, fp) != NULL)
	{
		printf("%d.%s", ++ncount, cmd);
	}

	fclose(fp);
}

// 명령어 히스토리 파일에서 지정한 순번의 명령어를 가져온다.
static int get_cmd_in_history(char *index_str, char *buf)
{
	int index = 0;
	if(*index_str == '!') index = -1;
	else index = atoi(index_str);

	//printf("index : %d\n", index);

	if(index == 0)
	{
		printf("error - wrong ! cmd");
		return -1;
	}

	FILE *fp = fopen(fname_hist, "r");
	if(fp == NULL)
	{
		printf("no history!\n");
		return -1;
	}

	char cmd[1024];
	int ncount = 0;
	while(fgets(cmd, 1024, fp) != NULL)
	{
		ncount++;

		if(index != -1 && ncount == index)
		{
			break;
		}
	}

	if(index == -1 || ncount == index)
	{
		// 지정한 순번의 명령어를 찾았을 경우
		strcpy(buf, cmd);
		return 0;
	}

	// 찾지 못한경우
	printf("error - out of history index");
	return -1;
}

// 리다이렉트 기호 포함 명령어일 경우 파싱한다.
static void parse_redirect_cmd(char *cmd, int *is_redirect_cmd, char *redirect_fname)
{
	char *index = strstr(cmd, ">>");
	if(index)
	{
		*index = 0;
		*(index+1) = 0;
		trim(index+2);
		sprintf(redirect_fname, "./%s", index+2);
		*is_redirect_cmd = APEND_REDIRECT;
	}
	else
	{
		index = strstr(cmd, ">");
		if(index)
		{
			*index = 0;
			trim(index+1);
			sprintf(redirect_fname, "./%s", index+1);
			*is_redirect_cmd = WRITE_REDIRECT;
		}
		else
		{
			index = strstr(cmd, "<");
			if(index)
			{
				*index = 0;
				trim(index+1);
				sprintf(redirect_fname, "./%s", index+1);
				*is_redirect_cmd = INPUT_REDIRECT;
			}
		}
	}

}

// 파이프 기호 포함 명령어일 경우 파싱한다.
static void parse_pipe_cmd(char *cmd, int *is_pipe_cmd, char *pipe_cmd)
{
	char *index = strstr(cmd, "|");
	if(index)
	{
		*index = 0;
		trim(index+1);
		sprintf(pipe_cmd, "./%s", index+1);
		*is_pipe_cmd = 1;
	}
}

// 히스트로 출력 명령어 여부 리턴한다.
static int is_history_cmd(const char *cmd)
{
	if(strcmp(cmd, "history") == 0) return 1;

	return 0;
}

// 파이프 기호 포함 명령어를 실행한다.
// 입출력 파이프를 생성한 후 자식 프로세스를 포크시켜 처리한다.
static void execute_pipe_cmd(char **argv, char *fpath, char *pipe_cmd)
{
	//printf("pipe cmd !!! - %s\n", pipe_cmd);

	int pipefd[2];
	pipe(pipefd); // 입출력 파이프 오픈

	if(fork() == 0) // 자식 프로세스
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1); // make stdout go to pipe
		close(pipefd[1]);

		if(is_history_cmd(argv[0]))
		{
			print_cmd_history();
		}
		else
		{
			argv[0] = fpath;
			execv(fpath, argv);
		}
		exit(0);
	}
	else // 부모 프로세스
	{
		close(pipefd[1]);
		dup2(pipefd[0], 0); // make stdin go to pipe

		char *cmd_ref = pipe_cmd;
		int ntoken = 0;
		char *pipe_argv[100] = {NULL,};
		char *pipe_token = NULL;
		while ((pipe_token = strsep(&cmd_ref, sep)) != NULL)
		{
			pipe_argv[ntoken++] = pipe_token;
			//printf("%d - [%s]\n", ntoken, pipe_token);
		}

		int pipe_status = 0;
		char pipe_cmd_fpath[1024] = {0,};
		if(!is_executable(pipe_argv[0], pipe_cmd_fpath))
		{
			printf("error - pipe cmd is not executable!\n");
			wait(&pipe_status);
			exit(0);
		}

		wait(&pipe_status);
		pipe_argv[0] = pipe_cmd_fpath;
		execv(pipe_cmd_fpath, pipe_argv);
		exit(0);
	}
}

// 리다이렉트 처리를 위한 파일디스크립터를 오픈한다.
static int open_redirect_fd(int *fd, int is_redirect_cmd, char *redirect_fname)
{
	int oflag = 0;
	switch(is_redirect_cmd)
	{
		case APEND_REDIRECT:
			oflag = O_WRONLY | O_CREAT | O_APPEND;
			break;

		case WRITE_REDIRECT:
			oflag = O_WRONLY | O_CREAT | O_TRUNC;
			break;

		case INPUT_REDIRECT:
			oflag = O_RDONLY;
			break;
	}

	*fd = open(redirect_fname, oflag, S_IRUSR | S_IWUSR);
	if(*fd < 0)
	{
		printf("can't open redirect file - %s\n", redirect_fname);
		return -1;
	}

	switch(is_redirect_cmd)
	{
		case APEND_REDIRECT:
		case WRITE_REDIRECT:
			dup2(*fd, 1); // make stdout go to file
			break;

		case INPUT_REDIRECT:
			dup2(*fd, 0); // make stdin go to file
			break;

		default:
			printf("error - redirect_cmd\n");
			return -1;
	}

	return 0;
}

// -------------------------
// M A I N
// -------------------------
int main()
{
	char *token = NULL;
	char instr_org[1024];
	char instr[1024];

	reset_cmd_history();

	while(1)
	{
		int is_cmd_for_save = 1;
		int is_pipe_cmd = 0;
		char pipe_cmd[1024] = {0,};
		int is_redirect_cmd = 0;
		char redirect_fname[1024] = {0,};

		// Wait for input
		printf("\nprompt> ");
		memset(instr, 0, 1024);

		fflush(stdin);
		fgets(instr, 1024-1, stdin);

		strcpy(instr_org, instr);
		trim(instr_org);

		// 히스토리 명령 처리
		if(*instr == '!')
		{
			if(get_cmd_in_history(instr+1, instr) < 0)
			{
				continue;
			}

			is_cmd_for_save = 0;
		}

		parse_redirect_cmd(instr, &is_redirect_cmd, redirect_fname);
		if(!is_redirect_cmd)
		{
			parse_pipe_cmd(instr, &is_pipe_cmd, pipe_cmd);
		}

		trim(instr);

		// Parse input
		char *instr_ref = instr;
		int ntoken = 0;
		char *argv[100] = {NULL,};
		while ((token = strsep(&instr_ref, sep)) != NULL)
		{
			argv[ntoken++] = token;
		}

		printf("\n");

		// exit 명령 처리
		if(strcmp(argv[0], "exit") == 0)
		{
			exit(0);
		}

		// Check if executable exists and is executable
		char fpath[1024] = {0,};
		if(!is_executable(argv[0], fpath) && !is_history_cmd(argv[0]))
		{
			printf("error - cmd is not executable : %s\n", argv[0]);
			continue;
		}

		if(is_cmd_for_save && !is_history_cmd(argv[0])) save_cmd_history(instr_org);

		// Launch executable
		if(fork() == 0) // 자식 프로세스
		{
			int fd = 0;
			if(is_redirect_cmd)
			{
				if(open_redirect_fd(&fd, is_redirect_cmd, redirect_fname) < 0)
				   	continue;
			}
			else if(is_pipe_cmd)
			{
				execute_pipe_cmd(argv, fpath, pipe_cmd);
				exit(0);
			}

			if(is_history_cmd(argv[0]))
			{
				print_cmd_history();
			}
			else
			{
				argv[0] = fpath;
				execv(fpath, argv);
			}

			if(fd) close(fd);
			exit(0);
		}
		else // 부모 프로세스
		{
			int status = 0;
			wait(&status);
		}
	}

	return 0;
}

