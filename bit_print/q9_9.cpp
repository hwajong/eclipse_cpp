// *** 9-9 문제
// 한 바이트의 하위 4비트와 상위 4비트를 서로 교환하는 프로그램을 작성하여라.
// 이 전에 작성한 disp-binary() 함수를 사용하여,
// 교환하기 전의 바이트의 내용과 교한한 후의 바이트의 내용을 출력함으로서 이 프로그램이 옳게 작성되었음을 보여라.

// 1. 교재245쪽의9-9문제에대한프로그램을작성하되,단한줄의명령어로작성해보자.
//   즉,이를위해다음과 같은 함수 flipflop() 의 밑줄을 채워라. 이 flipflop() 함수는 비트 연산자(225쪽)를 이용하여 주어진 바이트의 상위 4비트와 하위 4비트를 서로 교환해서 반환한다.

//2. 교재 245 쪽의 9-9 문제에 대한 프로그램을 비트 필드(269쪽)와 공용체(272쪽)를 사용하여 구현하시오. 이번 에는 한 줄로 구현할 필요는 없다.

#include <iostream>

using namespace std;

// 8개의 비트값 출력
void disp_binary(unsigned char c)
{
	printf("%3d : ", (int)c);

	for(int i=7; i>=0; i--)
	{
		if((c & (1 << i)) > 0) printf("1");
		else printf("0");
	}

	printf("\n");
}

// 상위 4비트 하위 4비트가 서로 교환된 값 리턴 - 한줄 구현
const unsigned char flipflop1(const unsigned char number)
{
	return (number << 4) | (number >> 4);
}

union uchar_union
{
	struct bits
	{
		unsigned char upper : 4;
		unsigned char lower : 4;
	} bits;

	unsigned char whole;
};

// 상위 4비트 하위 4비트가 서로 교환된 값 리턴 - 비트필드, 공용체 이용
const unsigned char flipflop2(const unsigned char number)
{
	uchar_union x;
	x.whole = number;

	unsigned char t = x.bits.upper;
	x.bits.upper = x.bits.lower;
	x.bits.lower = t;

	return x.whole;
}

int main(void)
{
	disp_binary(15);
	disp_binary(240);
	disp_binary(flipflop1(15));
	disp_binary(flipflop2(15));
	disp_binary(flipflop1(240));
	disp_binary(flipflop2(240));

	printf("\n");

	disp_binary(1);
	disp_binary(16);
	disp_binary(flipflop1(1));
	disp_binary(flipflop2(1));
	disp_binary(flipflop1(16));
	disp_binary(flipflop2(16));
}




























