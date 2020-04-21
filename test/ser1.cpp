#include <string.h>
#include <assert.h>
#include <string>
#include <streambuf>

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


//
// upper 2bit: data length block size (00=1, 01=2, 02=4, 03=8 bytes)
// lower 6bit: data type
//
enum {
	SERIZ_HDRLEN_MASK	= 0xC0,
	SERIZ_TYPE_MASK		= 0x3F,

	SERIZ_TYPE_BINARY	= 0x00,	// has data length block
	SERIZ_TYPE_STRING	= 0x01,	// has data length block

	SERIZ_TYPE_INT8		= 0x02,
	SERIZ_TYPE_INT16	= 0x03,
	SERIZ_TYPE_INT32	= 0x04,
	SERIZ_TYPE_INT64	= 0x05,

	SERIZ_TYPE_UINT8	= 0x22,
	SERIZ_TYPE_UINT16	= 0x23,
	SERIZ_TYPE_UINT32	= 0x24,
	SERIZ_TYPE_UINT64	= 0x25,

	SERIZ_TYPE_FLOAT	= 0x06,
	SERIZ_TYPE_DOUBLE	= 0x07,
};


uint32_t hdrlen2bytes(int hdrlen)
{
	if (hdrlen == 0)	return 1;
	if (hdrlen == 1)	return 2;
	if (hdrlen == 2)	return 4;
	if (hdrlen == 3)	return 8;
	return 0;
}


uint32_t size2hdrlen(size_t sz)
{
	unsigned long mask = 0x01;

	if (sz < (mask << 8))	// 1 byte data length
		return 0 << 6;
	if (sz < (mask << 16))	// 2 byte data length
		return 1 << 6;
	if (sz < (mask << 32))	// 4 byte data length
		return 2 << 6;
	return 3 << 6;		// 8 byte data length
}


uint32_t size2hdrbytes(size_t sz)
{
	unsigned long mask = 0x01;

	if (sz < (mask << 8))	// 1 byte data length
		return 1;
	if (sz < (mask << 16))	// 2 byte data length
		return 2;
	if (sz < (mask << 32))	// 4 byte data length
		return 4;
	return 8;		// 8 byte data length
}


void	setdatasize(void *buf, size_t sz, int hdrbytes)
{
	switch (hdrbytes)
	{
	default:
	case 1: { *(uint8_t *)buf = (uint8_t) sz; break; }
	case 2: { *(uint16_t *)buf = (uint16_t) sz; break; }
	case 4: { *(uint32_t *)buf = (uint32_t) sz; break; }
	case 8: { *(uint64_t *)buf = (uint64_t) sz; break; }
	}
}


uint64_t getdatasize(void *buf, int hdrbytes)
{
	uint64_t sz = 0;

	switch (hdrbytes)
	{
	default:
	case 1: { uint8_t u8 = *(uint8_t *)buf; sz = u8; break; }
	case 2: { uint16_t u16 = *(uint16_t *)buf; sz = u16; break; }
	case 4: { uint32_t u32 = *(uint32_t *)buf; sz = u32; break; }
	case 8: { uint64_t u64 = *(uint64_t *)buf; sz = u64; break; }
	}
	return sz;
}


void	dumpbin(char *buf, size_t bufsz)
{
	printf("DUMP:	");
	for (size_t ii = 0; ii < bufsz; ii++)
	{
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n");
}


//
// byte1: 2bit = data length in bytes / 6bit = data type
// byte2~9: real data length
// data...
//
int	seriz(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
{
	if (datatype < 0 || datatype > SERIZ_TYPE_MASK)
	{
		printf("ERROR: data tyep unknown=0x%08X\n", datatype);
		return 0;
	}
	if (buf == NULL || bufsz <= 0 || data == NULL || datasz <= 0)
	{
		printf("ERROR: buf or data is NULL or empty: bufsz=%ld datasz=%ld\n", bufsz, datasz);
		return 0;
	}
	if (datasz > bufsz + 2)
	{
		printf("ERROR: bufsz %ld < datasz %ld + 2\n", bufsz, datasz);
		return 0;
	}

	char	*sp = buf, *bp = buf;
	uint32_t hdrlen = 0, hdrbytes = 0;

	switch (datatype)
	{
	case SERIZ_TYPE_BINARY:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		// hdr data length block copy
		hdrbytes = size2hdrbytes(datasz);
		setdatasize(bp, datasz, hdrbytes);
		bp += hdrbytes;

		// data body copy
		memcpy(bp, data, datasz);
		dumpbin(sp, datasz + 1 + hdrbytes);
		return datasz + 1 + hdrbytes;

	case SERIZ_TYPE_STRING:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		// hdr data length block copy
		hdrbytes = size2hdrbytes(datasz);
		setdatasize(bp, datasz, hdrbytes);
		bp += hdrbytes;

		// data body copy
		memcpy(bp, data, datasz);
		if (bufsz >= datasz + 2 + hdrbytes)
			bp[datasz] = 0;

		dumpbin(sp, datasz + 2 + hdrbytes);
		return datasz + 2 + hdrbytes;


	case SERIZ_TYPE_INT8:
	case SERIZ_TYPE_INT16:
	case SERIZ_TYPE_INT32:
	case SERIZ_TYPE_INT64:

	case SERIZ_TYPE_UINT8:
	case SERIZ_TYPE_UINT16:
	case SERIZ_TYPE_UINT32:
	case SERIZ_TYPE_UINT64:

	case SERIZ_TYPE_FLOAT:
	case SERIZ_TYPE_DOUBLE:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		memcpy(bp, data, datasz);
		bp += datasz;
		dumpbin(sp, datasz + 1);
		return datasz + 1;

	default:
		return 0;
	}
}


int	deseriz(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
{
	if (datatype < 0 || datatype > SERIZ_TYPE_MASK)
		return 0;
	if (buf == NULL || bufsz <= 0 || data == NULL || datasz <= 0)
		return 0;

	char	*bp = buf, *dp = (char *)data;
	int	hdrlen = (*bp & SERIZ_HDRLEN_MASK) >> 6;
	int	hdrbytes = hdrlen2bytes(hdrlen);
	assert(hdrbytes != 0);
	int	type = (*bp & SERIZ_TYPE_MASK);
	size_t	sz = 0;

	bp++;
	switch (type)
	{
	case SERIZ_TYPE_BINARY:
		sz = getdatasize(bp, hdrbytes);
		assert(datasz >= sz);
		bp += hdrbytes;
		memcpy(data, bp, sz);
		dumpbin((char *)data, sz);
		return sz + 1 + hdrbytes;

	case SERIZ_TYPE_STRING:
		sz = getdatasize(bp, hdrbytes);
		assert(datasz >= sz + 1);
		bp += hdrbytes;
		memcpy(data, bp, sz); dp[sz] = 0;
		bp++;
		dumpbin((char *)data, sz + 1);
		return sz + 2 + hdrbytes;

	case SERIZ_TYPE_UINT8:
	case SERIZ_TYPE_UINT16:
	case SERIZ_TYPE_UINT32:
	case SERIZ_TYPE_UINT64:

	case SERIZ_TYPE_INT8:
	case SERIZ_TYPE_INT16:
	case SERIZ_TYPE_INT32:
	case SERIZ_TYPE_INT64:

	case SERIZ_TYPE_FLOAT:
	case SERIZ_TYPE_DOUBLE:
		memcpy(data, bp, datasz);
		dumpbin((char *)data, datasz);
		return datasz + 1;

	default:
		return 0;
	}
}


main()
{
	tx_t	tx, tx2;
	char	buf[1024] = {0};
	int	len = 0, len2 = 0;

	tx.str1 = "ABCDEF123456";

	tx.dd = 12345678.9012345e+125;
	tx.ff = 1234.5678e+57;

	tx.i8 = 0x89;
	tx.i16 = 0x5678;
	tx.i32 = 0x98765432;
	tx.i64 = 0x1234567812345678;

	tx.u8 = 0xF9;
	tx.u16 = 0xF678;
	tx.u32 = 0xF8765432;
	tx.u64 = 0xF234567812345678;

	tx.str2 = "한국어 잘 되는지 테스트";

	len = 0;
	printf("STRING: %s\n", tx.str1.c_str());
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_STRING, (void *)tx.str1.c_str(), tx.str1.length());

	printf("INT8:	%d==0x%02X\n", tx.i8, tx.i8 & 0x00FF);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT8, (void *)&tx.i8, sizeof(tx.i8));
	printf("INT16:	%d==0x%04X\n", tx.i16, tx.i16 & 0x00FFFF);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT16, (void *)&tx.i16, sizeof(tx.i16));
	printf("INT32:	%d==0x%08X\n", tx.i32, tx.i32);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT32, (void *)&tx.i32, sizeof(tx.i32));
	printf("INT64:	%ld==0x%016lX\n", tx.i64, tx.i64);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT64, (void *)&tx.i64, sizeof(tx.i64));

	printf("UINT8:	%u==0x%02X\n", tx.u8, tx.u8 & 0x00FF);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT8, (void *)&tx.u8, sizeof(tx.u8));
	printf("UINT16:	%u==0x%04X\n", tx.u16, tx.u16 & 0x00FFFF);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT16, (void *)&tx.u16, sizeof(tx.u16));
	printf("UINT32:	%u==0x%08X\n", tx.u32, tx.u32);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT32, (void *)&tx.u32, sizeof(tx.u32));
	printf("UINT64:	%lu==0x%016lX\n", tx.u64, tx.u64);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT64, (void *)&tx.u64, sizeof(tx.u64));

	printf("FLOAT:	%g==0x%08lX\n", tx.ff, *(uint32_t *)&tx.ff);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_FLOAT, (void *)&tx.ff, sizeof(tx.ff));
	printf("DOUBLE:	%lg==0x%016lX\n", tx.dd, *(uint64_t *)&tx.dd);
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_DOUBLE, (void *)&tx.dd, sizeof(tx.dd));

	printf("STRING: %s\n", tx.str2.c_str());
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_STRING, (void *)tx.str2.c_str(), tx.str2.length());

	printf("Total length=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
			printf("\n");
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n\n");

	char	strbuf[100] = {0};

	len2 = 0;
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str1 = strbuf;
	printf("len2=%d str1=%s\n", len2, tx2.str1.c_str());
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT8, (void *)&tx2.i8, sizeof(tx2.i8));
	printf("INT8:	%d==0x%02X\n", tx2.i8, tx2.i8 & 0x00FF);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT16, (void *)&tx2.i16, sizeof(tx2.i16));
	printf("INT16:	%d==0x%04X\n", tx2.i16, tx2.i16 & 0x00FFFF);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT32, (void *)&tx2.i32, sizeof(tx2.i32));
	printf("INT32:	%d==0x%08X\n", tx2.i32, tx2.i32);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT64, (void *)&tx2.i64, sizeof(tx2.i64));
	printf("INT64:	%ld==0x%016lX\n", tx2.i64, tx2.i64);

	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT8, (void *)&tx2.u8, sizeof(tx2.u8));
	printf("UINT8:	%u==0x%02X\n", tx2.u8, tx2.u8 & 0x00FF);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT16, (void *)&tx2.u16, sizeof(tx2.u16));
	printf("UINT16:	%u==0x%04X\n", tx2.u16, tx2.u16 & 0x00FFFF);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT32, (void *)&tx2.u32, sizeof(tx2.u32));
	printf("UINT32:	%u==0x%08X\n", tx2.u32, tx2.u32);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT64, (void *)&tx2.u64, sizeof(tx2.u64));
	printf("UINT64:	%lu==0x%016lX\n", tx2.u64, tx2.u64);

	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_FLOAT, (void *)&tx2.ff, sizeof(tx2.ff));
	printf("FLOAT:	%g==0x%08lX\n", tx2.ff, *(uint32_t *)&tx2.ff);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_DOUBLE, (void *)&tx2.dd, sizeof(tx2.dd));
	printf("DOUBLE:	%lg==0x%016lX\n", tx2.dd, *(uint64_t *)&tx2.dd);

	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str2 = strbuf;
	printf("len2=%d str2=%s\n", len2, tx2.str2.c_str());
}
