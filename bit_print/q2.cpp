#include <iostream>

using namespace std;

// 8개의 비트값 출력
void disp_binary(unsigned char c)
{
	printf("%3d : ", (int) c);

	for(int i = 7; i >= 0; i--)
	{
		if((c & (1 << i)) > 0) printf("1");
		else printf("0");
	}

	printf("\n");
}

union uchar_union
{
	struct bits
	{
		// 상 하위 4비트를 교환하기 쉽게 하기 위해 4비트씩 나눈다.
		unsigned char upper :4;
		unsigned char lower :4;
	} bits;

	// 8비트 전체
	unsigned char whole;
};

// 상위 4비트 하위 4비트가 서로 교환된 값 리턴 - 비트필드, 공용체 이용
const unsigned char flipflop(const unsigned char number)
{
	uchar_union x;
	x.whole = number;

	unsigned char t = x.bits.upper; // swap 을 위한 임시 변수

	// 상하위 4비트 스왑
	x.bits.upper = x.bits.lower;
	x.bits.lower = t;

	// 스왑된 전체 unsigned char 값 리턴
	return x.whole;
}

int main(void)
{
	disp_binary(15);
	disp_binary(240);
	disp_binary(flipflop(15));
	disp_binary(flipflop(240));

	return 0;
}

