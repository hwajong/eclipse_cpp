#include <iostream>

using namespace std;

// array 출력 함수
void print_array(int array[], int size)
{
	printf("{");
	for(int i = 0; i < size; i++)
	{
		if(i != 0) printf(", ");
		printf("%d", array[i]);
	}
	printf("}\n");
}

// array 에서 앞 b_size 의 데이터를 나머지 데이터들 위치와 바꾼다.
// 단 메모리가 제한된 상황을 가정해 int 몇개만 사용 가능함.
void reverse_array(int array[], int a_size, int b_size)
{
	// B 배열의 맨 뒤부터 하나씩
	for(int i = b_size - 1; i >= 0; i--)
	{
		// C 배열의 전체를 한깐 앞으로 땡기고 그 뒤로 이동 시킨다.
		for(int j = 0; j < a_size - b_size; j++)
		{
			int t = array[i + j]; // swap 을 위한 임시 변수
			array[i + j] = array[i + j + 1];
			array[i + j + 1] = t;
		}
	}
}

int main()
{
	int array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	// reverse_array() 로 reverse 하기 전 배열 출력
	print_array(array, 10);

	// reverse !!
	reverse_array(array, 10, 3);

	// reverse_array() 로 reverse 한 후 배열 출력
	print_array(array, 10);

	// 사이즈를 다르게 하여 테스트
	reverse_array(array, 10, 1);
	print_array(array, 10);

	reverse_array(array, 10, 6);
	print_array(array, 10);

	return 0;
}
