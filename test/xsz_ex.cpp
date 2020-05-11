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
int	test_c(seriz_test_t& tx);
int	test_cpp(seriz_test_t& tx);
int	stcmp(seriz_test_t *tx1, seriz_test_t *tx2);


int	_debug = 0;


main()
{
	seriz_test_t	tx;
	int	ii = 0, ok = 0;

	set_struct(tx);
	test_c(tx);

	set_struct(tx);
	for (ii = 0; ii < 1000000; ii++)
	{
		ok += test_cpp(tx);
	}
	printf("ii = %d OK=%d\n", ii, ok);
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


int	test_c(seriz_test_t& tx)
{
	seriz_test_t	tx2;
	char	buf[1024] = {0};
	int	len = 0, len2 = 0;

	if (_debug) printf("--------------------------------------------------------------------------------\n");
	if (_debug) printf("C: SERIALIZATION C test\n\n");

	len = 0;
	if (_debug) printf("STRING: %s\n", tx.str1.c_str());
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_STRING, (void *)tx.str1.c_str(), tx.str1.length());

	if (_debug) printf("INT8:	%d==0x%02X\n", tx.i8, tx.i8 & 0x00FF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT8, (void *)&tx.i8, sizeof(tx.i8));
	if (_debug) printf("INT16:	%d==0x%04X\n", tx.i16, tx.i16 & 0x00FFFF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT16, (void *)&tx.i16, sizeof(tx.i16));
	if (_debug) printf("INT32:	%d==0x%08X\n", tx.i32, tx.i32);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT32, (void *)&tx.i32, sizeof(tx.i32));
	if (_debug) printf("INT64:	%ld==0x%016lX\n", tx.i64, tx.i64);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_INT64, (void *)&tx.i64, sizeof(tx.i64));

	if (_debug) printf("UINT8:	%u==0x%02X\n", tx.u8, tx.u8 & 0x00FF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT8, (void *)&tx.u8, sizeof(tx.u8));
	if (_debug) printf("UINT16:	%u==0x%04X\n", tx.u16, tx.u16 & 0x00FFFF);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT16, (void *)&tx.u16, sizeof(tx.u16));
	if (_debug) printf("UINT32:	%u==0x%08X\n", tx.u32, tx.u32);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT32, (void *)&tx.u32, sizeof(tx.u32));
	if (_debug) printf("UINT64:	%lu==0x%016lX\n", tx.u64, tx.u64);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_UINT64, (void *)&tx.u64, sizeof(tx.u64));

	if (_debug) printf("FLOAT:	%g==0x%08X\n", tx.ff, *(uint32_t *)&tx.ff);
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_FLOAT, (void *)&tx.ff, sizeof(tx.ff));
	if (_debug) printf("DOUBLE:	%lg==0x%016lX\n", tx.dd, *(uint64_t *)(&tx.dd));
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_DOUBLE, (void *)&tx.dd, sizeof(tx.dd));

	if (_debug) printf("STRING: %s\n", tx.str2.c_str());
	len += xserialize(buf + len, sizeof(buf) - len, XSZ_TYPE_STRING, (void *)tx.str2.c_str(), tx.str2.length());

	if (_debug) printf("Total length=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
		{
			if (_debug) printf("\n");
		}
		if (_debug) printf("%02X ", buf[ii] & 0x00FF);
	}
	if (_debug) printf("\n\n");

	char	strbuf[255] = {0};

	len2 = 0;
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str1 = strbuf;
	if (_debug) printf("len2=%d str1=%s\n", len2, tx2.str1.c_str());

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT8, (void *)&tx2.i8, sizeof(tx2.i8));
	if (_debug) printf("INT8:	%d==0x%02X\n", tx2.i8, tx2.i8 & 0x00FF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT16, (void *)&tx2.i16, sizeof(tx2.i16));
	if (_debug) printf("INT16:	%d==0x%04X\n", tx2.i16, tx2.i16 & 0x00FFFF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT32, (void *)&tx2.i32, sizeof(tx2.i32));
	if (_debug) printf("INT32:	%d==0x%08X\n", tx2.i32, tx2.i32);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_INT64, (void *)&tx2.i64, sizeof(tx2.i64));
	if (_debug) printf("INT64:	%ld==0x%016lX\n", tx2.i64, tx2.i64);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT8, (void *)&tx2.u8, sizeof(tx2.u8));
	if (_debug) printf("UINT8:	%u==0x%02X\n", tx2.u8, tx2.u8 & 0x00FF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT16, (void *)&tx2.u16, sizeof(tx2.u16));
	if (_debug) printf("UINT16:	%u==0x%04X\n", tx2.u16, tx2.u16 & 0x00FFFF);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT32, (void *)&tx2.u32, sizeof(tx2.u32));
	if (_debug) printf("UINT32:	%u==0x%08X\n", tx2.u32, tx2.u32);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_UINT64, (void *)&tx2.u64, sizeof(tx2.u64));
	if (_debug) printf("UINT64:	%lu==0x%016lX\n", tx2.u64, tx2.u64);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_FLOAT, (void *)&tx2.ff, sizeof(tx2.ff));
	if (_debug) printf("FLOAT:	%g==0x%08X\n", tx2.ff, *(uint32_t *)&tx2.ff);
	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_DOUBLE, (void *)&tx2.dd, sizeof(tx2.dd));
	if (_debug) printf("DOUBLE:	%lg==0x%016lX\n", tx2.dd, *(uint64_t *)&tx2.dd);

	len2 += xdeserialize(buf + len2, len - len2, XSZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str2 = strbuf;
	if (_debug) printf("len2=%d str2=%s\n", len2, tx2.str2.c_str());

	if (stcmp(&tx, &tx2) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int	test_cpp(seriz_test_t& tx)
{
	seriz_test_t	tx2;
	char	*bp = NULL;
	int	len = 0;

	if (_debug) printf("--------------------------------------------------------------------------------\n");
	if (_debug) printf("C++: SERIALIZATION C++ test\n\n");

	xserial xsz(64 * 1024);

	if (_debug) printf("STRING: %s\n", tx.str1.c_str());
	xsz << tx.str1;

	if (_debug) printf("INT8:	%d==0x%02X\n", tx.i8, tx.i8 & 0x00FF);
	xsz << tx.i8;
	if (_debug) printf("INT16:	%d==0x%04X\n", tx.i16, tx.i16 & 0x00FFFF);
	xsz << tx.i16;
	if (_debug) printf("INT32:	%d==0x%08X\n", tx.i32, tx.i32);
	xsz << tx.i32;
	if (_debug) printf("INT64:	%ld==0x%016lX\n", tx.i64, tx.i64);
	xsz << tx.i64;

	if (_debug) printf("UINT8:	%u==0x%02X\n", tx.u8, tx.u8 & 0x00FF);
	xsz << tx.u8;
	if (_debug) printf("UINT16:	%u==0x%04X\n", tx.u16, tx.u16 & 0x00FFFF);
	xsz << tx.u16;
	if (_debug) printf("UINT32:	%u==0x%08X\n", tx.u32, tx.u32);
	xsz << tx.u32;
	if (_debug) printf("UINT64:	%lu==0x%016lX\n", tx.u64, tx.u64);
	xsz << tx.u64;

	if (_debug) printf("FLOAT:	%g==0x%08X\n", tx.ff, *(uint32_t *)&tx.ff);
	xsz << tx.ff;
	if (_debug) printf("DOUBLE:	%lg==0x%016lX\n", tx.dd, *(uint64_t *)&tx.dd);
	xsz << tx.dd;

	if (_debug) printf("STRING: %s\n", tx.str2.c_str());
	xsz << tx.str2;

	len = xsz.getsize();
	bp = xsz.getbuf();
	if (_debug) printf("Total length=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
		{
			if (_debug) printf("\n");
		}
		if (_debug) printf("%02X ", bp[ii] & 0x00FF);
	}
	if (_debug) printf("\n\n");

	xsz >> tx2.str1;
	if (_debug) printf("str1=%s\n", tx2.str1.c_str());
	xsz >> tx2.i8;
	if (_debug) printf("INT8:	%d==0x%02X\n", tx2.i8, tx2.i8 & 0x00FF);
	xsz >> tx2.i16;
	if (_debug) printf("INT16:	%d==0x%04X\n", tx2.i16, tx2.i16 & 0x00FFFF);
	xsz >> tx2.i32;
	if (_debug) printf("INT32:	%d==0x%08X\n", tx2.i32, tx2.i32);
	xsz >> tx2.i64;
	if (_debug) printf("INT64:	%ld==0x%016lX\n", tx2.i64, tx2.i64);

	xsz >> tx2.u8;
	if (_debug) printf("UINT8:	%u==0x%02X\n", tx2.u8, tx2.u8 & 0x00FF);
	xsz >> tx2.u16;
	if (_debug) printf("UINT16:	%u==0x%04X\n", tx2.u16, tx2.u16 & 0x00FFFF);
	xsz >> tx2.u32;
	if (_debug) printf("UINT32:	%u==0x%08X\n", tx2.u32, tx2.u32);
	xsz >> tx2.u64;
	if (_debug) printf("UINT64:	%lu==0x%016lX\n", tx2.u64, tx2.u64);

	xsz >> tx2.ff;
	if (_debug) printf("FLOAT:	%g==0x%08X\n", tx2.ff, *(uint32_t *)&tx2.ff);
	xsz >> tx2.dd;
	if (_debug) printf("DOUBLE:	%lg==0x%016lX\n", tx2.dd, *(uint64_t *)(&tx2.dd));

	xsz >> tx2.str2;
	if (_debug) printf("str2=%s\n", tx2.str2.c_str());

	if (stcmp(&tx, &tx2) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int	stcmp(seriz_test_t *tx1, seriz_test_t *tx2)
{
	if (tx1->str1 != tx2->str1)	return -1;
	if (tx1->dd != tx2->dd)		return -1;
	if (tx1->ff != tx2->ff)		return -1;
	if (tx1->i64 != tx2->i64)	return -1;
	if (tx1->i32 != tx2->i32)	return -1;
	if (tx1->i16 != tx2->i16)	return -1;
	if (tx1->i8 != tx2->i8)		return -1;
	if (tx1->u64 != tx2->u64)	return -1;
	if (tx1->u32 != tx2->u32)	return -1;
	if (tx1->u16 != tx2->u16)	return -1;
	if (tx1->u8 != tx2->u8)		return -1;
	if (tx1->str2 != tx2->str2)	return -1;
	
	return 0;
}
