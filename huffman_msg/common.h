#ifndef COMMON_H_
#define COMMON_H_

#define MAX_WORD_LEN 100

// 노드정의
typedef struct bst_node
{
	char word[MAX_WORD_LEN];
	struct bst_node* left;
	struct bst_node* right;
} bst_node;

bst_node* insert_node(bst_node *root, const char* word);
int encode_word(bst_node* root, const char* word, char* encoded);
void cleanup(bst_node* root);


#endif /* COMMON_H_ */
