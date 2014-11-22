#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "common.h"

// 노드 생성
static bst_node* create_node(const char* word)
{
	bst_node *new_node = (bst_node*) malloc(sizeof(bst_node));
	if(new_node == NULL)
	{
		fprintf(stderr, "Out of memory!!! (create_node)\n");
		exit(1);
	}

	strcpy(new_node->word, word);
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

// 노드 삽입
bst_node* insert_node(bst_node *root, const char* word)
{
	if(root == NULL) return create_node(word);

	int is_left = 0;
	bst_node* curr = root;
	bst_node* prev = NULL;

	// search leaf
	while(curr != NULL)
	{
		prev = curr;
		int cmp = strcmp(word, curr->word);
		if(cmp == 0) // 찾음
		{
			printf("** Warn : already exist word - %s\n", word);
			return root;
		}
		else if(cmp < 0)
		{
			is_left = 1;
			curr = curr->left;
		}
		else
		{
			is_left = 0;
			curr = curr->right;
		}
	}

	// 새로운 데이터일 경우 leaf 에 노드를 만들어 저장
	if(is_left) prev->left = create_node(word);
	else prev->right = create_node(word);

	return root;
}


// 트리 메모리를 해제 한다.
void cleanup(bst_node* root)
{
	if(root != NULL)
	{
		cleanup(root->left);
		cleanup(root->right);
		free(root);
	}
}

// fail : -1
// success : 0
int encode_word(bst_node* root, const char* word, char* encoded)
{
	if(root == NULL) return -1;

	if(strlen(encoded) == 0) strcpy(encoded, "0");

	int cmp = strcmp(word, root->word);
	if(cmp == 0) // 찾음
	{
		return 0;
	}
	if(cmp < 0) // 왼쪽 트리 검색
	{
		strcat(encoded, "0");
		return encode_word(root->left, word, encoded);
	}
	else // 오른쪽 트리 검색
	{
		strcat(encoded, "1");
		return encode_word(root->right, word, encoded);
	}

}










