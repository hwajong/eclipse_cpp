#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <ctype.h>
#include "common.h"

#define MAX_WORD_TABLE_LEN 30

typedef struct
{
	char word[MAX_WORD_LEN];
	int freq;
} word_freq;

word_freq word_freq_table[MAX_WORD_TABLE_LEN];
int word_freq_table_size = 0;

bst_node* obst = NULL;
bst_node* fbst = NULL;

// 문자열의 앞뒤 공백을 제거한다.
void trim(char* s)
{
	char *p = s;
	int l = strlen(p);

	while(isspace(p[l - 1]))
		p[--l] = 0;
	while(*p && isspace(*p))
		++p, --l;

	memmove(s, p, l + 1);
}

void print_word_freq_table()
{
	printf("*word frequencies\n");
	printf("*order : word : frequency\n");
	for(int i = 0; i < word_freq_table_size; i++)
	{
		printf("%2d : %10s : %d\n", i, word_freq_table[i].word, word_freq_table[i].freq);
	}
}

int find_word_in_table(const char* w)
{
	for(int i = 0; i < word_freq_table_size; i++)
	{
		if(strcmp(word_freq_table[i].word, w) == 0)
		{
			return i;
		}
	}

	return -1;
}

void init_word_freq_table()
{
	for(int i = 0; i < MAX_WORD_TABLE_LEN; i++)
	{
		memset(word_freq_table[i].word, 0, MAX_WORD_LEN);
		word_freq_table[i].freq = 0;
	}
}

void load_word_freq(const char* fname)
{
	init_word_freq_table();

	FILE* fp = fopen(fname, "r");
	if(fp == NULL)
	{
		fprintf(stderr, "** Error : fail to read logfile - %s\n", fname);
		exit(-1);
	}

	char word[MAX_WORD_LEN];
	while(fscanf(fp, "%s", word) == 1)
	{
		//printf("%s\n", word);
		int index = find_word_in_table(word);
		if(index == -1)
		{
			if(word_freq_table_size == MAX_WORD_TABLE_LEN)
			{
				fprintf(stderr, "** Error : check logfile word count\n");
				exit(-1);
			}

			strcpy(word_freq_table[word_freq_table_size].word, word);
			word_freq_table[word_freq_table_size].freq = 1;
			word_freq_table_size++;
		}
		else
		{
			word_freq_table[index].freq++;
		}
	}

	fclose(fp);
}

// make order based binary search tree
void make_obst()
{
	print_word_freq_table();

	for(int i = 0; i < word_freq_table_size; i++)
	{
		obst = insert_node(obst, word_freq_table[i].word);
	}
}

int comp_freq(const void* p1, const void* p2)
{
	word_freq* wf1 = (word_freq*) p1;
	word_freq* wf2 = (word_freq*) p2;

	if(wf1->freq == wf2->freq) return 0;
	if(wf1->freq < wf2->freq) return 1;
	return -1;
}

void make_fbst()
{
	// sort table by frequency
	qsort(word_freq_table, word_freq_table_size, sizeof(word_freq), comp_freq);

	// print_word_freq_table();

	for(int i = 0; i < word_freq_table_size; i++)
	{
		fbst = insert_node(fbst, word_freq_table[i].word);
	}
}

void encode(bst_node* root, const char* msg, const char* desc)
{
	printf("*%s\n", desc);

	char* msg_cp = strdup(msg);
	char* p = strtok(msg_cp, " ");
	while(p != NULL)
	{
		//printf("[%s]", p);

		char encoded[100] =
		{ 0, };
		if(encode_word(root, p, encoded) == -1)
		{
			fprintf(stderr, "**Error : fail to find word - %s\n", p);
			exit(-1);
		}

		printf("%s  ", encoded);

		p = strtok(NULL, " ");
	}

	printf("\n");

	free(msg_cp);
}

void usage(char* progname)
{
	printf("*Usage : %s logfile msgfile\n", basename(progname));
	exit(-1);
}

void load_msg(const char* fname, char* msg)
{
	FILE* fp = fopen(fname, "r");
	if(fp == NULL)
	{
		fprintf(stderr, "** Error : fail to read msgfile - %s\n", fname);
		exit(-1);
	}

	char word[MAX_WORD_LEN];
	while(fscanf(fp, "%s", word) == 1)
	{
		//printf("%s\n", word);
		strcat(msg, word);
		strcat(msg, " ");
	}
}

int main(int argc, char** argv)
{
	if(argc != 3) usage(argv[0]);

	//const char* msg = "crocodile koala crocodile koala leopard octopus lion koala shark leopard koala weasel crocodile leopard lama koala";
	const char* fname_logfile = argv[1];
	const char* fname_msgfile = argv[2];

	load_word_freq(fname_logfile);

	make_obst();
	make_fbst();

	FILE* fp = fopen(fname_msgfile, "r");
	if(fp == NULL)
	{
		fprintf(stderr, "** Error : fail to read msgfile - %s\n", fname_msgfile);
		exit(-1);
	}

	char line[1024];
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		trim(line);
		if(strlen(line) == 0) continue;

		// printf("*original xxx : %s\n", line);
		char msg[1024] =
		{ 0, };
		char word[MAX_WORD_LEN];
		int idx = 0;
		while(sscanf(line + idx, "%s", word) == 1)
		{
			//printf("***%s\n", word);
			strcat(msg, word);
			strcat(msg, " ");
			idx += (strlen(word) + 1);
		}

		printf("\n*Original msg : %s\n", msg);

		encode(obst, msg, "Encoded Msg by OBST");
		encode(fbst, msg, "Encoded Msg by FBST");
	}

	cleanup(obst);
	cleanup(fbst);

	return 0;
}

