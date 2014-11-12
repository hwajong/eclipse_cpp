#include <stdio.h>
#include "bst.h"

// 메뉴 출력
static void _print_menu()
{
    printf("\n");
    printf("-----------------\n");
    printf("1. 연락처 입력\n");
    printf("2. 연락처 삭제\n");
    printf("3. 연락처 수정\n");
    printf("4. 주소록 삭제\n");
    printf("5. 이름으로 찾기\n");
    printf("6. 주소로 찾기\n");
    printf("7. 연락처 출력\n");
    printf("8. 주소록 내보내기\n");

    printf("0. 프로그램 종료\n");
    printf("-----------------\n");
}

// 사용자 메뉴 입력을 받는다.
static int _user_input_from_menu()
{
    _print_menu();

    char instr[1024] = {0,};
    while(1)
    {
        printf("> 메뉴를 선택하세요. _ ");

        fgets(instr, sizeof(instr), stdin);
        int c = instr[0]-48;
        if(c >= 0 && c <= 8)
        {
            return c;
        }
    }
}

static node* _insert_contact(node* root)
{
    printf("> 연락처 입력\n");

    printf("- name  : ");
    char name[MAX_STR_LEN] = {0,};
    fgets(name, sizeof(name), stdin);

    printf("- addr  : ");
    char addr[MAX_STR_LEN] = {0,};
    fgets(addr, sizeof(addr), stdin);

    printf("- phone : ");
    char phone[MAX_STR_LEN] = {0,};
    fgets(phone, sizeof(phone), stdin);

    printf("- email : ");
    char email[MAX_STR_LEN] = {0,};
    fgets(email, sizeof(email), stdin);

    return insert_node(root, name, addr, phone, email);
}

static node* _delete_contact(node* root)
{
    printf("> 연락처 삭제\n");

    printf("- name  : ");
    char name[MAX_STR_LEN] = {0,};
    fgets(name, sizeof(name), stdin);

    trim(name);

    return delete_node(root, name);
}

static void _print_all_contact(node* root)
{
    printf("> 연락처 출력\n");
    if(root == NULL)
    {
        printf("\tcontact is empty!\n");
    }
    else
    {
        print_inorder(root, stdout);
    }
}

static void _export_all_contact(node* root)
{
    printf("> 연락처 내보내기\n");
    if(root == NULL)
    {
        printf("\tcontact is empty!\n");
    }

    const char* fname = "address.txt";
    FILE* fp = fopen(fname, "w");
    if(fp == NULL)
    {
        printf("\tcan not create file - %s\n", fname);
        return;
    }

    print_inorder(root, fp);
    printf("\texported!\n");

}

static void _edit_contact(node* root)
{
    printf("> 연락처 수정\n");

    printf("- name  : ");
    char name[MAX_STR_LEN] = {0,};
    fgets(name, sizeof(name), stdin);
    trim(name);

    node* nd = search(root, name);
    if(nd == NULL)
    {
        printf("\t%s does not exist!\n", name);
        return;
    }

    printf("- addr  : ");
    fgets(nd->addr, sizeof(nd->addr), stdin);
    trim(nd->addr);

    printf("- phone : ");
    fgets(nd->phone, sizeof(nd->phone), stdin);
    trim(nd->phone);

    printf("- email : ");
    fgets(nd->email, sizeof(nd->email), stdin);
    trim(nd->email);

    printf("\tedited!\n");
}

static node* _delete_all_contact(node* root)
{
    free_bst(root);
    printf("\tAll deleted!\n");
    return NULL;
}

static void _search_by_name(node* root)
{
    printf("> 이름으로 찾기\n");

    printf("- name  : ");
    char name[MAX_STR_LEN] = {0,};
    fgets(name, sizeof(name), stdin);
    trim(name);

    node* nd = search(root, name);
    if(nd == NULL)
    {
        printf("\t%s does not exist!\n", name);
        return;
    }

    print_node(nd, stdout);
}

static void _search_by_addr(node* root)
{
    printf("> 주소로 찾기\n");

    printf("- addr  : ");
    char addr[MAX_STR_LEN] = {0,};
    fgets(addr, sizeof(addr), stdin);
    trim(addr);

    int n = print_all_include_addr(root, addr);
    if(n == 0)
    {
        printf("\tno contact!\n");
    }
}


int main()
{
    node* root = NULL;

    while(1)
    {
        int i = _user_input_from_menu();
        switch(i)
        {
            case 0: // quit
                printf("Bye~\n");
                return 0;

            case 1: // 연락처 입력
                root = _insert_contact(root);
                break;

            case 2: // 연락처 삭제
                root = _delete_contact(root);
                break;

            case 3: // 연락처 수정
                _edit_contact(root);
                break;

            case 4: // 주소록 삭제
                root = _delete_all_contact(root);
                break;

            case 5: // 이름으로 찾기
                _search_by_name(root);
                break;

            case 6: // 주소로 찾기
                _search_by_addr(root);
                break;

            case 7: // 연락처 출력
                _print_all_contact(root);
                break;

            case 8: // 주소록 내보내기
                _export_all_contact(root);
                break;
        }

    }

    return 0;
}
