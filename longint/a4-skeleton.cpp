//
// 'infinite' precision unsigned integer class
//	supported operations:
//		addition, assignment, input, output, etc.
//
#include <climits>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

using namespace std;

class longint
{
public:

	// default constructor
	longint(void)
	{
		// 0으로 초기화
		longint(0);
	}

	// constructor with unsigned as initial value
	longint(unsigned i)
	{
		while(i > 0)
		{
			// 뒤에서 부터 숫자 하나씩 추출해 스트링에 붙인다.
			int digit = i % 10;
			s += (char) (digit + 48); // int --> char 변환
			i /= 10;
		}
	}

	// constructor with numeric string as initial value
	longint(string s)
	{
		// 뒤에서 부터 숫자 하나씩 추출해 스트링에 붙인다.
		int size = s.size();
		for(int i = size - 1; i >= 0; i--)
		{
			// 숫자가 아닐경우 예외처리
			if(isdigit(s[i]) == 0) throw runtime_error(s + " is not numeric string");

			this->s += s[i];
		}

		// 앞자리 0 제거 (000111 --> 111)
		int index_0 = -1;
		for(int i = this->s.size() - 1; i >= 1; i--)
		{
			if(this->s[i] == '0') index_0 = i;
			else break;
		}

		if(index_0) this->s = this->s.substr(0, index_0);
//		cout << "[" << this->s << "]" << endl;
	}

	~longint(void)
	{
	} // included for completeness

	// return true if longint <= UINT_MAX
	bool is_integer(void)
	{
		const longint uint_max(UINT_MAX);

//		cout << s << endl;
//		cout << uint_max << endl;
		// UINT_MAX 와 같을 경우
		if(uint_max.s == s) return true;

		int size = s.size();
		int size_max = uint_max.s.size();

		// 사이즈가 UINT_MAX 보다 작을 경우
		if(size < size_max) return true;

		// 사이즈가 UINT_MAX 보다 클 경우
		if(size > size_max) return false;

		// 사이즈가 같으면 msb 숫자 부터 lsb 숫자까지 비교
		// unit_max 의 숫자 보다 같거나 작아야 integer
		for(int i = size - 1; i >= 0; i--)
		{
			if(s[i] > uint_max.s[i]) return false;
			if(s[i] < uint_max.s[i]) return true;
		}

		return true;
	}

	// return longint as an unsigned int
	unsigned to_integer(void)
	{
		// unsigned int 로 표현할 수 없을 경우
		if(!is_integer()) throw runtime_error(s + " is not unsigned int");

		// unsigned int 로 변환해 리턴
		unsigned r = 0;
		int size = s.size();
		for(int i = 0; i < size; i++)
		{
			r += (s[i] - 48) * pow(10.0, i);
		}

		return r;
	}

	// longint assignment, assigns x to this object
	// returns this object (*this)
	longint operator =(longint x)
	{
		if(&x != this)
		{	// do nothing for a = a;
			s = x.s;
		}
		return *this;
	}

	// returns longint + longint
	// first longint is this object
	longint operator +(longint x)
	{
		longint a, b, c;

		// 자리수가 긴걸 a로
		if(this->s.size() >= x.s.size())
		{
			a.s = this->s;
			b.s = x.s;
		}
		else
		{
			a.s = x.s;
			b.s = this->s;
		}

		int carry = 0;
		int i;

//		cout << "a : " << a.s << endl;
//		cout << "b : " << b.s << endl;
		for(i = 0; i < b.s.size(); i++)
		{
			int t = (a.s[i] - 48) + (b.s[i] - 48) + carry;
			c.s += (char) ((t % 10) + 48);
			carry = t / 10;
		}

		// a 의 나머지 숫자 처리
		for(; i < a.s.size(); i++)
		{
			int t = (a.s[i] - 48) + carry;
			c.s += (char) ((t % 10) + 48);
			carry = t / 10;
		}

		// 마지막 carry 처리
		if(carry)
		{
			c.s += '1';
		}

//		cout << c.s << endl;
		return c;
	}

	friend longint operator +(longint x, unsigned y);
	friend longint operator +(unsigned x, longint y);
	friend istream & operator >>(istream & in, longint & x);
	friend ostream & operator <<(ostream & out, const longint & x);

	// 구현한 로직 유닛테스트 - 내부로직 검증
	static void unit_test()
	{
		// 생성자 테스트
		longint a(1234);
		assert(a.s == "4321");

		longint b("001234");
		assert(b.s == "4321");

		// is_integer
		assert(a.is_integer());
		assert(longint(0).is_integer());
		assert(longint(UINT_MAX).is_integer());
		assert(!(longint(UINT_MAX) + 1).is_integer());

		// to_integer
		assert(a.to_integer() == 1234);
		assert(b.to_integer() == 1234);
		assert(longint(123456).to_integer() == 123456);
		assert(longint(1234567890).to_integer() == 1234567890);
		assert(longint(UINT_MAX).to_integer() == UINT_MAX);
		assert(longint(0).to_integer() == 0);

		// + 테스트
		longint c = a + b;
		assert(c.s == "8642");

		longint d = c + c;
		assert(d.s == "6394");

		longint e = longint(9999) + longint(9000);
		assert(e.s == "99981");

		longint f = longint(10) + 0;
		assert(f.s == "01");

		longint g = longint(66) + 944;
		assert(g.s == "0101");
	}

private:
	// string of digits making up longint, LEAST significant digit first
	string s;
};

longint operator +(longint x, unsigned y)
{
	return x + longint(y);
}

longint operator +(unsigned x, longint y)
{
	return longint(y) + x;
}

// overload >> to input longint
//	Skip leading whitespace characters, input terminated
//	by a non-numeric character or whitespace character which
//	will be 'returned' to input stream (using unget()).
//	Non-numeric input will be treated as zero but will not cause >>
//	to fail which is the case for input of ordinary integers.
istream & operator >>(istream & in, longint & x)
{
	string instr;

	// 입력받은 문자열에 숫자가 아닌문자열이 있으면 0으로 처리함.
	in >> instr;
	for(int i=0; i<instr.size(); i++)
	{
		if(!isdigit(instr[i]))
		{
			instr = "0";
			break;
		}
	}

	// 중간에 공백을 포함할 경우 공백 뒤 문자들 무시함.
	// ex) 1    564 --> 1
	cin.clear();
	cin.ignore(INT_MAX,'\n');

	x = longint(instr);

	return in;
}

// overload << to output longint
ostream & operator <<(ostream & out, const longint & x)
{
	int size = x.s.size();
	for(int i = size - 1; i >= 0; i--)
	{
		out << x.s[i];
	}

	return out;
}

// test is_integer
void test_is_integer(longint x)
{
	cout << x << " is ";
	cout << (x.is_integer() ? "an integer" : "a longint") << endl;
}

// main test program
//	terminates by a pair of 0 input
//	can also be terminated by a single non-digit character!
int main(void)
{
	longint::unit_test();

	longint a(UINT_MAX), b("00112233");

	cout << "(longint)UINT_MAX is " << a << endl;
	cout << "(unsigned)UINT_MAX is " << UINT_MAX << endl;

	test_is_integer(a);
	test_is_integer(a + 2);
	test_is_integer(10 + a);

	cout << "00112233 is ";
	test_is_integer(b);

	cout << "Sum of " << a << " and " << b << " is ";
	a = a + b;
	cout << a << endl;

	while(true)
	{
		cout << "Input first long integer : ";
		cin >> a;
		cout << "Input second long integer : ";
		cin >> b;
		if(a.is_integer() && b.is_integer() && (a.to_integer() + b.to_integer() == 0)) break;
		cout << "Sum of " << a << " and " << b << " is ";
		cout << a + b << endl;
	}
}
