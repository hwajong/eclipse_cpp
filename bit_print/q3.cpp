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

// 참조를 사용해 구조체 값을 교환
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

// 포인터를 사용해 구조체 값을 교환
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

	// var1 var2 를 교환하기 전의 값 출력
	print_s_type(var1);
	print_s_type(var2);

	// struct_swapr() 를 이용해 var1 var2 를 교환한 후의 출력
	printf("\n**swap using struct_swapr()\n");
	struct_swapr(var1, var2);

	print_s_type(var1);
	print_s_type(var2);

	// struct_swapp() 를 이용해 var1 var2 를 교환한 후의 출력
	printf("\n**swap using struct_swapp()\n");
	struct_swapp(&var1, &var2);

	print_s_type(var1);
	print_s_type(var2);

	return 0;
}
