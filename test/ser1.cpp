#include "xsz.h"


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
} seriz_test_t;


void	set_struct(seriz_test_t& tx);
void	test_c(seriz_test_t& tx);
void	test_cpp(seriz_test_t& tx);


main()
{
	seriz_test_t	tx;

	set_struct(tx);
	test_c(tx);

	set_struct(tx);
	test_cpp(tx);
}


void	set_struct(seriz_test_t& tx)
{
	tx.str1 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA123456789";

	tx.dd = 12345678.9012345e+125;
	tx.ff = 1234.5678e+17;

	tx.i8 = 0x89;
	tx.i16 = 0x5678;
	tx.i32 = 0x98765432;
	tx.i64 = 0x1234567812345678;

	tx.u8 = 0xF9;
	tx.u16 = 0xF678;
	tx.u32 = 0xF8765432;
	tx.u64 = 0xF234567812345678;

	tx.str2 = "한국어 잘 되는지 테스트";
}


void	test_c(seriz_test_t& tx)
{
	seriz_test_t	tx2;
	char	buf[1024] = {0};
	int	len = 0, len2 = 0;

	printf("--------------------------------------------------------------------------------\n");
	printf("C: SERIALIZATION C test\n\n");

	len = 0;
	printf("STRING: %s\n", tx.str1.c_str());
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_STRING, (void *)tx.str1.c_str(), tx.str1.length());

	printf("INT8:	%d==0x%02X\n", tx.i8, tx.i8 & 0x00FF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT8, (void *)&tx.i8, sizeof(tx.i8));
	printf("INT16:	%d==0x%04X\n", tx.i16, tx.i16 & 0x00FFFF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT16, (void *)&tx.i16, sizeof(tx.i16));
	printf("INT32:	%d==0x%08X\n", tx.i32, tx.i32);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT32, (void *)&tx.i32, sizeof(tx.i32));
	printf("INT64:	%ld==0x%016lX\n", tx.i64, tx.i64);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT64, (void *)&tx.i64, sizeof(tx.i64));

	printf("UINT8:	%u==0x%02X\n", tx.u8, tx.u8 & 0x00FF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT8, (void *)&tx.u8, sizeof(tx.u8));
	printf("UINT16:	%u==0x%04X\n", tx.u16, tx.u16 & 0x00FFFF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT16, (void *)&tx.u16, sizeof(tx.u16));
	printf("UINT32:	%u==0x%08X\n", tx.u32, tx.u32);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT32, (void *)&tx.u32, sizeof(tx.u32));
	printf("UINT64:	%lu==0x%016lX\n", tx.u64, tx.u64);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT64, (void *)&tx.u64, sizeof(tx.u64));

	printf("FLOAT:	%g==0x%08X\n", tx.ff, *(uint32_t *)&tx.ff);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_FLOAT, (void *)&tx.ff, sizeof(tx.ff));
	printf("DOUBLE:	%lg==0x%016lX\n", tx.dd, *(uint64_t *)(&tx.dd));
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_DOUBLE, (void *)&tx.dd, sizeof(tx.dd));

	printf("STRING: %s\n", tx.str2.c_str());
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_STRING, (void *)tx.str2.c_str(), tx.str2.length());

	printf("Total length=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
			printf("\n");
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n\n");

	char	strbuf[255] = {0};

	len2 = 0;
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str1 = strbuf;
	printf("len2=%d str1=%s\n", len2, tx2.str1.c_str());

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT8, (void *)&tx2.i8, sizeof(tx2.i8));
	printf("INT8:	%d==0x%02X\n", tx2.i8, tx2.i8 & 0x00FF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT16, (void *)&tx2.i16, sizeof(tx2.i16));
	printf("INT16:	%d==0x%04X\n", tx2.i16, tx2.i16 & 0x00FFFF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT32, (void *)&tx2.i32, sizeof(tx2.i32));
	printf("INT32:	%d==0x%08X\n", tx2.i32, tx2.i32);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT64, (void *)&tx2.i64, sizeof(tx2.i64));
	printf("INT64:	%ld==0x%016lX\n", tx2.i64, tx2.i64);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT8, (void *)&tx2.u8, sizeof(tx2.u8));
	printf("UINT8:	%u==0x%02X\n", tx2.u8, tx2.u8 & 0x00FF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT16, (void *)&tx2.u16, sizeof(tx2.u16));
	printf("UINT16:	%u==0x%04X\n", tx2.u16, tx2.u16 & 0x00FFFF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT32, (void *)&tx2.u32, sizeof(tx2.u32));
	printf("UINT32:	%u==0x%08X\n", tx2.u32, tx2.u32);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT64, (void *)&tx2.u64, sizeof(tx2.u64));
	printf("UINT64:	%lu==0x%016lX\n", tx2.u64, tx2.u64);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_FLOAT, (void *)&tx2.ff, sizeof(tx2.ff));
	printf("FLOAT:	%g==0x%08X\n", tx2.ff, *(uint32_t *)&tx2.ff);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_DOUBLE, (void *)&tx2.dd, sizeof(tx2.dd));
	printf("DOUBLE:	%lg==0x%016lX\n", tx2.dd, *(uint64_t *)&tx2.dd);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str2 = strbuf;
	printf("len2=%d str2=%s\n", len2, tx2.str2.c_str());
}


void	test_cpp(seriz_test_t& tx)
{
	seriz_test_t	tx2;
	char	*bp = NULL;
	int	len = 0;

	printf("--------------------------------------------------------------------------------\n");
	printf("C++: SERIALIZATION C++ test\n\n");

	xserial xsz(64 * 1024);

	printf("STRING: %s\n", tx.str1.c_str());
	xsz << tx.str1;

	printf("INT8:	%d==0x%02X\n", tx.i8, tx.i8 & 0x00FF);
	xsz << tx.i8;
	printf("INT16:	%d==0x%04X\n", tx.i16, tx.i16 & 0x00FFFF);
	xsz << tx.i16;
	printf("INT32:	%d==0x%08X\n", tx.i32, tx.i32);
	xsz << tx.i32;
	printf("INT64:	%ld==0x%016lX\n", tx.i64, tx.i64);
	xsz << tx.i64;

	printf("UINT8:	%u==0x%02X\n", tx.u8, tx.u8 & 0x00FF);
	xsz << tx.u8;
	printf("UINT16:	%u==0x%04X\n", tx.u16, tx.u16 & 0x00FFFF);
	xsz << tx.u16;
	printf("UINT32:	%u==0x%08X\n", tx.u32, tx.u32);
	xsz << tx.u32;
	printf("UINT64:	%lu==0x%016lX\n", tx.u64, tx.u64);
	xsz << tx.u64;

	printf("FLOAT:	%g==0x%08X\n", tx.ff, *(uint32_t *)&tx.ff);
	xsz << tx.ff;
	printf("DOUBLE:	%lg==0x%016lX\n", tx.dd, *(uint64_t *)&tx.dd);
	xsz << tx.dd;

	printf("STRING: %s\n", tx.str2.c_str());
	xsz << tx.str2;

	len = xsz.getsize();
	bp = xsz.getbuf();
	printf("Total length=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
			printf("\n");
		printf("%02X ", bp[ii] & 0x00FF);
	}
	printf("\n\n");

	xsz >> tx2.str1;
	printf("str1=%s\n", tx2.str1.c_str());
	xsz >> tx2.i8;
	printf("INT8:	%d==0x%02X\n", tx2.i8, tx2.i8 & 0x00FF);
	xsz >> tx2.i16;
	printf("INT16:	%d==0x%04X\n", tx2.i16, tx2.i16 & 0x00FFFF);
	xsz >> tx2.i32;
	printf("INT32:	%d==0x%08X\n", tx2.i32, tx2.i32);
	xsz >> tx2.i64;
	printf("INT64:	%ld==0x%016lX\n", tx2.i64, tx2.i64);

	xsz >> tx2.u8;
	printf("UINT8:	%u==0x%02X\n", tx2.u8, tx2.u8 & 0x00FF);
	xsz >> tx2.u16;
	printf("UINT16:	%u==0x%04X\n", tx2.u16, tx2.u16 & 0x00FFFF);
	xsz >> tx2.u32;
	printf("UINT32:	%u==0x%08X\n", tx2.u32, tx2.u32);
	xsz >> tx2.u64;
	printf("UINT64:	%lu==0x%016lX\n", tx2.u64, tx2.u64);

	xsz >> tx2.ff;
	printf("FLOAT:	%g==0x%08X\n", tx2.ff, *(uint32_t *)&tx2.ff);
	xsz >> tx2.dd;
	printf("DOUBLE:	%lg==0x%016lX\n", tx2.dd, *(uint64_t *)(&tx2.dd));

	xsz >> tx2.str2;
	printf("str2=%s\n", tx2.str2.c_str());
}
