#include <string.h>
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

enum {
	SERIZ_TYPE_MASK		= 0x2F,

	SERIZ_TYPE_BINARY	= 0,
	SERIZ_TYPE_STRING	= 1,
	SERIZ_TYPE_DOUBLE	= 2,
	SERIZ_TYPE_FLOAT	= 3,

	SERIZ_TYPE_INT8		= 4,
	SERIZ_TYPE_INT16	= 5,
	SERIZ_TYPE_INT32	= 6,
	SERIZ_TYPE_INT64	= 7,

	SERIZ_TYPE_UINT8	= 8,
	SERIZ_TYPE_UINT16	= 9,
	SERIZ_TYPE_UINT32	= 10,
	SERIZ_TYPE_UINT64	= 11,
};


uint32_t mask2bytes(int mask)
{
	if (mask == 0)	return 1;
	if (mask == 1)	return 2;
	if (mask == 2)	return 4;
	if (mask == 3)	return 8;
}


uint32_t bytes2mask(size_t sz)
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


uint32_t databyte(size_t sz)
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


void	setdatasize(void *buf, size_t sz, int bytes)
{
	switch (bytes)
	{
	default:
	case 1:
		{
			uint8_t *i8p = (uint8_t *)buf;
			*i8p = (uint8_t) sz;
			break;
		}
	case 2:
		{
			uint16_t *i16p = (uint16_t *)buf;
			*i16p = (uint16_t) sz;
			break;
		}
	case 4:
		{
			uint32_t *i32p = (uint32_t *)buf;
			*i32p = (uint32_t) sz;
			break;
		}
	case 8:
		{
			uint64_t *i64p = (uint64_t *)buf;
			*i64p = (uint64_t) sz;
			break;
		}
	}
}


uint64_t getdatasize(void *buf, int bytes)
{
	uint64_t sz = 0;

	switch (bytes)
	{
	default:
	case 1:
		{
			uint8_t u8 = *(uint8_t *)buf;
			sz = u8;
			break;
		}
	case 2:
		{
			uint16_t u16 = *(uint16_t *)buf;
			sz = u16;
			break;
		}
	case 4:
		{
			uint32_t u32 = *(uint32_t *)buf;
			sz = u32;
			break;
		}
	case 8:
		{
			uint64_t u64 = *(uint64_t *)buf;
			sz = u64;
			break;
		}
	}
	return sz;
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

	char	*bp = buf;
	int	bytes = 0, mask = 0;

	switch (datatype)
	{
	case SERIZ_TYPE_BINARY:
		mask = bytes2mask(datasz);
		bytes = databyte(datasz);
		*bp++ = datatype | mask;
		setdatasize(bp, datasz, bytes);
		bp += bytes;

		printf("HDR=%02X %02X datasz=%ld databyte=%02X\n", *(bp-2), *(bp-1), datasz, databyte(datasz + 1));
		memcpy(bp, data, datasz);
		return datasz + 1 + bytes;

	case SERIZ_TYPE_STRING:
		mask = bytes2mask(datasz + 1);
		bytes = databyte(datasz + 1);
		*bp++ = datatype | mask;
		setdatasize(bp, datasz, bytes);
		bp += bytes;

		printf("HDR=%02X %02X datasz=%ld databyte=%02X\n", *(bp-2), *(bp-1), datasz, databyte(datasz + 1));
		strncpy(bp, (const char *)data, datasz);
		if (bufsz >= datasz + 2 + bytes)
			bp[datasz] = 0;
		return datasz + 2 + bytes;

	case SERIZ_TYPE_DOUBLE:
	case SERIZ_TYPE_FLOAT:

	case SERIZ_TYPE_INT8:
	case SERIZ_TYPE_INT16:
	case SERIZ_TYPE_INT32:
	case SERIZ_TYPE_INT64:

	case SERIZ_TYPE_UINT8:
	case SERIZ_TYPE_UINT16:
	case SERIZ_TYPE_UINT32:
	case SERIZ_TYPE_UINT64:
		mask = bytes2mask(datasz);
		bytes = databyte(datasz);
		*bp++ = datatype | mask;
		setdatasize(bp, datasz, bytes);
		bp += bytes;

		printf("HDR=%02X %02X datasz=%ld databyte=%02X\n", *(bp-2), *(bp-1), datasz, databyte(datasz + 1));
		memcpy(bp, data, datasz);
		return datasz + 1 + bytes;

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

	char	*bp = buf, *dp = NULL;
	int	mask = (*bp & 0x00FF) >> 6;
	int	bytes = mask2bytes(mask);
	int	type = (*bp & SERIZ_TYPE_MASK);
	uint64_t sz = getdatasize(bp + 1, bytes);
	printf("DESER: bytes=%d type=%d sz=%ld\n", bytes, type, sz);

	if (datasz < sz)
	{
		printf("ERROR: data size %ld too small (real size=%ld)\n", datasz, sz);
		return 0;
	}

	bp++;
	bp += sz;

	switch (datatype)
	{
	case SERIZ_TYPE_BINARY:
	//	printf("HDR=%02X datasz=%ld databyte=%02X\n", *(bp-1), datasz, databyte(datasz));
		memcpy(data, bp, sz);
		return sz + 1 + bytes;

	case SERIZ_TYPE_STRING:
	//	printf("HDR=%02X datasz=%ld databyte=%02X\n", *(bp-1), datasz, databyte(datasz + 1));
		strncpy((char *)data, bp, sz);
		dp = (char *)data;
		if (datasz > sz + 1)
			dp[datasz] = 0;
		bp++;
		return sz + 2 + bytes;

	case SERIZ_TYPE_DOUBLE:
	case SERIZ_TYPE_FLOAT:

	case SERIZ_TYPE_INT8:
	case SERIZ_TYPE_INT16:
	case SERIZ_TYPE_INT32:
	case SERIZ_TYPE_INT64:

	case SERIZ_TYPE_UINT8:
	case SERIZ_TYPE_UINT16:
	case SERIZ_TYPE_UINT32:
	case SERIZ_TYPE_UINT64:
	//	printf("HDR=%02X datasz=%ld databyte=%02X\n", *(bp-1), datasz, databyte(datasz));
		memcpy(data, bp, sz);
		return sz + 1 + bytes;

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
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_STRING, (void *)tx.str1.c_str(), tx.str1.length());

	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_DOUBLE, (void *)&tx.dd, sizeof(tx.dd));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_FLOAT, (void *)&tx.ff, sizeof(tx.ff));

	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT8, (void *)&tx.i8, sizeof(tx.i8));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT16, (void *)&tx.i16, sizeof(tx.i16));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT32, (void *)&tx.i32, sizeof(tx.i32));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_INT64, (void *)&tx.i64, sizeof(tx.i64));

	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT8, (void *)&tx.u8, sizeof(tx.u8));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT16, (void *)&tx.u16, sizeof(tx.u16));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT32, (void *)&tx.u32, sizeof(tx.u32));
	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_UINT64, (void *)&tx.u64, sizeof(tx.u64));

	len += seriz(buf + len, sizeof(buf) - len, SERIZ_TYPE_STRING, (void *)tx.str2.c_str(), tx.str2.length());

	printf("LEN=%d\n", len);
	for (int ii = 0; ii < len; ii++)
	{
		if (ii % 10 == 0)
			printf("\n");
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n");

	char	strbuf[100] = {0};

	len2 = 0;
	len2 += deseriz(buf, len, SERIZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str1 = strbuf;
	printf("len2=%d\n", len2);
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_STRING, strbuf, sizeof(strbuf));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_DOUBLE, (void *)&tx2.dd, sizeof(tx2.dd));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_FLOAT, (void *)&tx2.ff, sizeof(tx2.ff));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT8, (void *)&tx2.i8, sizeof(tx2.i8));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT16, (void *)&tx2.i16, sizeof(tx2.i16));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT32, (void *)&tx2.i32, sizeof(tx2.i32));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_INT64, (void *)&tx2.i64, sizeof(tx2.i64));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT8, (void *)&tx2.u8, sizeof(tx2.u8));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT16, (void *)&tx2.u16, sizeof(tx2.u16));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT32, (void *)&tx2.u32, sizeof(tx2.u32));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_UINT64, (void *)&tx2.u64, sizeof(tx2.u64));
	len2 += deseriz(buf + len2, len - len2, SERIZ_TYPE_STRING, strbuf, sizeof(strbuf));
	tx2.str2 = strbuf;
}
