#include "bst.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

// 문자열의 앞뒤 공백을 제거한다.
void trim(char* s)
{
    char *p = s;
    int l = (int)strlen(p);

    while(l-1 >= 0 && !(p[l-1] & 0x80) && isspace(p[l-1])) p[--l] = 0;
    while(*p && !(*p & 0x80) && isspace(*p)) ++p, --l;

    memmove(s, p, l + 1);
}

// 노드 생성
static node* _create_node(const char* name, const char* addr, const char* phone, const char* email)
{
    node* new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL)
    {
        fprintf(stderr, "Out of memory!!! (create_node)\n");
        exit(1);
    }

    // 노드 데이터 초기화
    strncpy(new_node->name, name, MAX_STR_LEN);
    strncpy(new_node->addr, addr, MAX_STR_LEN);
    strncpy(new_node->phone, phone, MAX_STR_LEN);
    strncpy(new_node->email, email, MAX_STR_LEN);

    trim(new_node->name);
    trim(new_node->addr);
    trim(new_node->phone);
    trim(new_node->email);

    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// 노드 삽입
node* insert_node(node* root, const char* name, const char* addr, const char* phone, const char* email)
{
    if(root == NULL) return _create_node(name, addr, phone, email);

    int is_left = 0;
    node* cursor = root;
    node* prev = NULL;

    // search leaf
    while(cursor != NULL)
    {
        prev = cursor;
        int comp = strcasecmp(name, cursor->name);
        if(comp == 0) // 같은 이름이 있으면 무시
        {
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
    if(is_left) prev->left = _create_node(name, addr, phone, email);
    else prev->right = _create_node(name, addr, phone, email);

    return root;
}

// 노드 삭제
node* delete_node(node* root, const char* name)
{
    if(root == NULL) return NULL;

    node *cursor;
    int comp = strcasecmp(name, root->name);
    if(comp < 0)
    {
        root->left = delete_node(root->left, name);
    }
    else if(comp > 0)
    {
        root->right = delete_node(root->right, name);
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

            strcpy(root->name, cursor->name);

            if(parent != NULL)
            {
                parent->left = delete_node(parent->left, parent->left->name);
            }
            else
            {
                root->right = delete_node(root->right, root->right->name);
            }
        }

        printf("\tdeleted!\n");
    }

    return root;
}

// 노드 찾기
node* search(node* root, const char* name)
{
    if(root == NULL) return NULL;

    int comp = strcasecmp(name, root->name);
    if(comp == 0) // 찾음
    {
        return root;
    }
    if(comp < 0) // 왼쪽 트리 검색
    {
        return search(root->left, name);
    }
    else // 오른쪽 트리 검색
    {
        return search(root->right, name);
    }
}

// addr 을 포함하는 주소의 정보 모두 출력
int print_all_include_addr(node* root, const char* addr)
{
    if(root == NULL) return 0;

    int count = 0;

    if(strstr(root->addr, addr) != NULL)
    {
        print_node(root, stdout);
        count++;
    }

    count += print_all_include_addr(root->left, addr);
    count += print_all_include_addr(root->right, addr);

    return count;
}

// 트리 메모리를 해제 한다.
void free_bst(node* root)
{
    if(root != NULL)
    {
        free_bst(root->left);
        free_bst(root->right);
        free(root);
    }
}

void print_node(node* nd, FILE* fp)
{
    fprintf(fp, "\t*name : %s\n", nd->name);
    fprintf(fp, "\t*addr : %s\n", nd->addr);
    fprintf(fp, "\t*phone: %s\n", nd->phone);
    fprintf(fp, "\t*email: %s\n", nd->email);
    fprintf(fp, "\n");
}

void print_inorder(node* root, FILE* fp)
{
    if(root == NULL) return;

    print_inorder(root->left, fp);
    print_node(root, fp);
    print_inorder(root->right, fp);
}

