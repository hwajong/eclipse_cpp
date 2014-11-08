/*
*** 10-4 문제

다음과 같이 정의된 두 개의 구조체 변수들을 포함하는 프로그램을 작성하여라

프로그램에서 두 개의 구조체의 각 원소에 초기값을 주되,
두 개의 구조체의 원소에 서로 다른 초기값을 사용하여라.

struct_swap() 이라는 함수를 작성하고 이 함수를 사용하여, var1과 var2의 내용을 서로 교환하여라.

교재 279쪽의 10-4 문제에 대한 프로그램을 작성하자. 즉, 이를 위해 다음의 함수 2 개를 구현하라.
(a) 참조를 이용하는 함수 : struct_swapr
(b) 포인터를 이용하는 함수 : struct_swapp
*/

#include <iostream>

using namespace std;

struct s_type
{
	int i;
	char ch;
	double d;
} var1, var2;

void print_s_type(s_type s)
{
	printf("[%d, %c, %f]\n", s.i, s.ch, s.d);
}

void struct_swapr(s_type& a, s_type& b)
{
	s_type t = a;
	a.i = b.i;
	a.ch = b.ch;
	a.d = b.d;

	b.i = t.i;
	b.ch = t.ch;
	b.d = t.d;
}

void struct_swapp(s_type* a, s_type* b)
{
	s_type t = *a;
	a->i = b->i;
	a->ch = b->ch;
	a->d = b->d;

	b->i = t.i;
	b->ch = t.ch;
	b->d = t.d;
}

int main()
{
	var1.i = 11;
	var1.ch = 'A';
	var1.d = 11.11;

	var2.i = 99;
	var2.ch = 'Z';
	var2.d = 99.99;

	print_s_type(var1);
	print_s_type(var2);

	printf("\n**swap using struct_swapr()\n");
	struct_swapr(var1, var2);

	print_s_type(var1);
	print_s_type(var2);

	printf("\n**swap using struct_swapp()\n");
	struct_swapp(&var1, &var2);

	print_s_type(var1);
	print_s_type(var2);

	return 0;
}
