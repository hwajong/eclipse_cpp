#include <iostream>

using namespace std;

// 8개의 비트값 출력.
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
// 비트 shift 연산자를 이용해 4칸 왼쪽으로 shift 시킨것과 4칸 오른쪽으로 shift 시킨것을 or 연산으로 합친다.
const unsigned char flipflop(const unsigned char number)
{
	return (number << 4) | (number >> 4);
}

int main(void)
{
	disp_binary(15);
	disp_binary(240);
	disp_binary(flipflop(15));
	disp_binary(flipflop(240));

	return 0;
}




























