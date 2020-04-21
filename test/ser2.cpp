#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;


typedef struct {
	string str1;
	double dd;
	float ff;

	int64_t i64;
	int32_t i32;
	int16_t i16;
	int8_t i8;

	uint64_t u64;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;

	string str2;
} tx_t;


main()
{
	tx_t	tx, tx2;

	tx.str1 = "ABCDEF123456";

	tx.dd = 12345678.9012345e+125;
	tx.ff = 1234.5678e+19;

	tx.i8 = 0x89;
	tx.i16 = 0x5678;
	tx.i32 = 0x98765432;
	tx.i64 = 0x1234567812345678;

	tx.u8 = 0xF9;
	tx.u16 = 0xF678;
	tx.u32 = 0xF8765432;
	tx.u64 = 0xF234567812345678;

	tx.str2 = "한국어 잘 되는지 테스트ABCD";

	char	buf[1024] = {0};
	stringstream ssm;

	ssm << tx.str1;
	ssm << tx.dd;
	ssm << tx.ff;

	ssm << tx.i8;
	ssm << tx.i16;
	ssm << tx.i32;
	ssm << tx.i64;

	ssm << tx.u8;
	ssm << tx.u16;
	ssm << tx.u32;
	ssm << tx.u64;

	ssm << tx.str2;


	string	ss;

	ssm.seekg(0, ssm.end);
	size_t smsize = ssm.tellg();
	ssm.seekg(0, ssm.beg);
//	ssm >> ss;
//	size_t smsize = ssm.tellg();
//	ssm.seek(0, 0);
	ssm >> buf;

	printf("STRING=%s\n", ss.c_str());
	printf("BUF LEN=%ld\n", smsize);
	printf("BUF=%s\n", buf);
	for (int ii = 0; ii < smsize; ii++)
	{
		if (ii % 10 == 0)
			printf("\n");
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n");
}
