#ifndef __BST__bst__
#define __BST__bst__

#include <stdio.h>

#define MAX_STR_LEN 1024

// 노드정의
typedef struct node
{
    char name[MAX_STR_LEN];
    char addr[MAX_STR_LEN];
    char phone[MAX_STR_LEN];
    char email[MAX_STR_LEN];

    struct node* left;
    struct node* right;
} node;

node* insert_node(node* root, const char* name, const char* addr, const char* phone, const char* email);
node* delete_node(node* root, const char* name);
node* search(node* root, const char* name);
int print_all_include_addr(node* root, const char* addr);
void free_bst(node* root);
void print_node(node* nd, FILE* fp);
void print_inorder(node* root, FILE* fp);
void trim(char* s);


#endif
