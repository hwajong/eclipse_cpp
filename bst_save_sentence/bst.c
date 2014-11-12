#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_WORD_LEN 256
#define VERSION 1.0f

// 노드정의
typedef struct node
{
	char word[MAX_WORD_LEN];
	int count;
	struct node* left;
	struct node* right;
} node;

// 문자열의 앞뒤 공백을 제거한다.
static void trim(char* s)
{
	char *p = s;
	int l = strlen(p);

	while(isspace(p[l - 1])) p[--l] = 0;
	while(*p && isspace(*p)) ++p, --l;

	memmove(s, p, l + 1);
}

// 트리의 모든 데이터를 출력한다.
static void print_tree(node* nd)
{
    if (nd == NULL) return;

    /* display node data */
    printf("|\n");
    printf("%s(%d)", nd->word, nd->count);
    if(nd->left != NULL)  printf("(L:%s(%d))", nd->left->word, nd->left->count);
    if(nd->right != NULL) printf("(R:%s(%d))", nd->right->word, nd->right->count);
    printf("\n");

    print_tree(nd->left);
    print_tree(nd->right);
}

// 노드 생성
static node* create_node(const char* word, int count)
{
	node *new_node = (node*)malloc(sizeof(node));
	if(new_node == NULL)
	{
		fprintf(stderr, "Out of memory!!! (create_node)\n");
		exit(1);
	}

	strcpy(new_node->word, word);
	new_node->count = count;
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

// 노드 삽입
static node* insert_node(node *root, const char* word, int count)
{
	if(root == NULL)
	{
		return create_node(word, count);
	}

	int is_left = 0;
	node* cursor = root;
	node* prev = NULL;

	// search leaf
	while(cursor != NULL)
	{
		prev = cursor;
		int comp = strcasecmp(word, cursor->word);
		if(comp == 0) // 찾음
		{
			cursor->count += count;
			return root;
		}
		else if(comp < 0)
		{
			is_left = 1;
			cursor = cursor->left;
		}
		else if(comp > 0)
		{
			is_left = 0;
			cursor = cursor->right;
		}
	}

	// 새로운 데이터일 경우 leaf 에 노드를 만들어 저장
	if(is_left)
	{
		prev->left = create_node(word, count);
	}
	else
	{
		prev->right = create_node(word, count);
	}

	return root;
}

// 노드 삭제
static node* delete_node(node* root, const char* word)
{
	if(root == NULL)
	{
		return NULL;
	}

	node *cursor;
	int comp = strcasecmp(word, root->word);
	if(comp < 0)
	{
		root->left = delete_node(root->left, word);
	}
	else if(comp > 0)
	{
		root->right = delete_node(root->right, word);
	}
	else // 찾음
	{
		if(root->left == NULL)
		{
			cursor = root->right;
			free(root);
			root = cursor;
		}
		else if(root->right == NULL)
		{
			cursor = root->left;
			free(root);
			root = cursor;
		}
		else // left right 모두 leaf 가 아닐때
		{
			cursor = root->right;
			node *parent = NULL;

			// find leaf
			while(cursor->left != NULL)
			{
				parent = cursor;
				cursor = cursor->left;
			}

			strcpy(root->word, cursor->word);
			root->count = cursor->count;

			if(parent != NULL)
			{
				parent->left = delete_node(parent->left, parent->left->word);
			}
			else
			{
				root->right = delete_node(root->right, root->right->word);
			}
		}
	}

	return root;
}

// 노드 찾기
static node* search(node *root, const char* word)
{
	if(root == NULL)
	{
		return NULL;
	}

	int comp = strcasecmp(word, root->word);
	if(comp == 0) // 찾음
	{
		return root;
	}
    if(comp < 0) // 왼쪽 트리 검색
    {
    	return search(root->left, word);
    }
    else // 오른쪽 트리 검색
    {
    	return search(root->right, word);
    }
}

// inorder 로 순회하며 출력
static void print_inorder(node* nd)
{
	if(nd == NULL) return;

	print_inorder(nd->left);
	printf("%s(%d)\n", nd->word, nd->count);
	print_inorder(nd->right);
}

// 트리 메모리를 해제 한다.
static void dispose(node* root)
{
	if(root != NULL)
	{
		dispose(root->left);
		dispose(root->right);
		free(root);
	}
}

// 메뉴 출력
static void print_menu()
{
	printf("\n");
	printf("1. 새 트리 구성\n");
	printf("2. 문장 입력\n");
	printf("3. 알파벳 순으로 보기\n");
	printf("4. 단어 찾기\n");
	printf("5. 파일로 저장\n");
	printf("6. 파일로부터 읽기\n");
	printf("7. 종료\n");
}

// 사용자 메뉴 입력을 받는다.
static int user_input_from_menu()
{
	print_menu();

	char instr[1024] = {0,};
	while(1)
	{
		printf("\t\t해당 메뉴를 선택하세요. _ ");

		fgets(instr, sizeof(instr), stdin);
		int c = instr[0]-48;
		if(c >= 1 && c <= 7)
		{
			return c;
		}
	}
}

// 문장을 입력 받는다.
static node* user_input_sentence(node* root)
{
	printf("\t\t문장을 입력하세요. _ ");

	char instr[1024] = {0,};
	fgets(instr, sizeof(instr), stdin);

	// 문장을 파싱하여 트리에 삽입
	const char* deli = " .?\t\r\n";
	char* token = strtok(instr, deli);
	node* new_root = root;
	while(token != NULL )
	{
		// printf("token = %s\n", token );
		new_root = insert_node(new_root, token, 1);
		token = strtok(NULL, deli);
	}

	printf("\n*입력된 트리 정보\n");
	print_tree(new_root);

	return new_root;
}

// 앞에 존제하는 단어들 출력
static void print_before_words(node* root, const char* word)
{
	if(root == NULL) return;

	print_before_words(root->left, word);

	if(strcasecmp(root->word, word) >= 0) return;

	printf("%s(%d) ", root->word, root->count);

	print_before_words(root->right, word);

}

// 뒤에 존제하는 단어들 출력
static void print_after_words(node* root, const char* word)
{
	if(root == NULL) return;

	print_after_words(root->left, word);

	if(strcasecmp(root->word, word) > 0)
	{
		printf("%s(%d) ", root->word, root->count);
	}

	print_after_words(root->right, word);

}

// 단어 검색
static void search_word(node* root)
{
	printf("\t\t찾을 단어를 입력하세요. _ ");
	char instr[1024] = {0,};
	fgets(instr, sizeof(instr), stdin);

	trim(instr);
	node* found = search(root, instr);

	if(found == NULL)
	{
		printf("\n*찾는 단어가 없습니다.\n");
		return;
	}

	printf("\n*success : %s(%d)\n", found->word, found->count);
	printf("*이 단어의 앞에 존제하는 단어 : ");
	print_before_words(root, instr);

	printf("\n*이 단어의 뒤에 존제하는 단어 : ");
	print_after_words(root, instr);
	printf("\n");
}

// 알파벳 순서로 트리 데이터 출력
static void print_alphabet_order(node* root)
{
	if(root == NULL)
	{
		printf("데이터가 없습니다.\n");
		return;
	}

	print_inorder(root);
}

typedef struct file_header
{
	float version;
	char etc_info[20];
} file_header;

// preorder 로 순회하며 저장한다.
static void save_tree_in_preorder(FILE* fp, node* root)
{
	if(root == NULL) return;

	unsigned char word_len = strlen(root->word);
	fwrite(&word_len, 1, sizeof(unsigned char), fp);
	fwrite(root->word, 1, word_len, fp);
	fwrite(&(root->count), 1, sizeof(int), fp);

	save_tree_in_preorder(fp, root->left);
	save_tree_in_preorder(fp, root->right);
}

// 트리 데이터 파일 저장
static void save_to_file(node* root)
{
	if(root == NULL)
	{
		printf("\n*저장할 데이터가 없습니다.\n");
		return;
	}

	printf("\t\t저장할 파일명을 입력하세요. _ ");
	char instr[1024] = {0,};
	fgets(instr, sizeof(instr), stdin);

	trim(instr);

	if(access(instr, F_OK) == 0)
	{
		printf("\n*이미 존제하는 파일입니다.\n");
		printf("*덮어 쓰겠습니까?(y/n) _ ");
		char inchar[1024] = {0,};
		fgets(inchar, sizeof(inchar), stdin);

		if(inchar[0] != 'y')
		{
			printf("*취소됨\n");
			return;
		}
	}

	printf("*저장중...\n");

	FILE* fp = fopen(instr, "wb");
	if(fp == NULL)
	{
		printf("*file open error : %s\n", instr);
		return;
	}

	// 파일에 트리 데이터를 쓴다.
	file_header header;
	header.version = VERSION;
	memset(header.etc_info, 0, 20);
	strcpy(header.etc_info, "etc data...");

	fwrite(&header, 1, sizeof(file_header), fp);
	save_tree_in_preorder(fp, root);
	char null_char = 0;
	fwrite(&null_char, 1, 1, fp);

	fclose(fp);

	printf("*success !\n");
}

// 파일에서 트리데이터를 읽는다.
static node* read_from_file()
{
	printf("\t\t읽을 파일명을 입력하세요. _ ");
	char instr[1024] = {0,};
	fgets(instr, sizeof(instr), stdin);

	trim(instr);

	FILE* fp = fopen(instr, "rb");
	if(fp == NULL)
	{
		printf("*file open error : %s\n", instr);
		return NULL;
	}

	// 파일 트리 데이터를 읽는다.
	file_header header;
	memset(header.etc_info, 0, 20);
	fread(&header, 1, sizeof(file_header), fp);

	printf("\n*version : %2.1f\n", header.version);
	printf("*etc info : %s\n", header.etc_info);

	unsigned char word_len = 0;
	fread(&word_len, 1, sizeof(unsigned char), fp);

	int count = 0;
	node* new_root = NULL;
	while(word_len)
	{
		char word[MAX_WORD_LEN] = {0,};
		fread(word, 1, word_len, fp);
		fread(&count, 1, sizeof(int), fp);

		printf("*word : %-10.10s *count : %d\n", word, count);

		new_root = insert_node(new_root, word, count);

		word_len = 0;
		fread(&word_len, 1, sizeof(unsigned char), fp);
	}

	fclose(fp);

	return new_root;
}

// 트리데이터를 없엔다.
static void clear_bst(node** root)
{
	if(*root != NULL)
	{
		printf("\n*데이터를 파일에 저장 하겠습니까?(y/n) _ ");
		char inchar[1024] = {0,};
		fgets(inchar, sizeof(inchar), stdin);

		if(inchar[0] == 'y')
		{
			save_to_file(*root);
		}
	}

	dispose(*root);
	*root = NULL;
}


// -------------------
// M A I N
// -------------------
int main()
{
	node* root = NULL;

	while(1)
	{
		int i = user_input_from_menu();
		switch(i)
		{
		case 1: // 새 트리구성
			clear_bst(&root);
			printf("\n*새 트리가 구성 되었습니다.\n");
			break;

		case 2: // 문장입력
			root = user_input_sentence(root);
			break;

		case 3: // 알파벳 순으로 보기
			printf("\n*알파벳 순으로 출력\n");
			print_alphabet_order(root);
			printf("\n");
			break;

		case 4: // 단어찾기
			search_word(root);
			break;

		case 5: // 파일로 저장
			save_to_file(root);
			break;

		case 6: // 파일로부터 읽기
			clear_bst(&root);
			root = read_from_file();
			break;

		case 7: // 종료
			printf("\n*Quit!\n\n");
			return 0;
		}
	}

	return 0;
}


