#include "xsz.h"


#define toliteral(v)	#v


const char *gettypename(int datatype)
{
	switch (datatype)
	{
	case SERIZ_TYPE_BINARY:	return toliteral(SERIZ_TYPE_BINARY);
	case SERIZ_TYPE_STRING:	return toliteral(SERIZ_TYPE_STRING);

	case SERIZ_TYPE_INT8:	return toliteral(SERIZ_TYPE_INT8);
	case SERIZ_TYPE_INT16:	return toliteral(SERIZ_TYPE_INT16);
	case SERIZ_TYPE_INT32:	return toliteral(SERIZ_TYPE_INT32);
	case SERIZ_TYPE_INT64:	return toliteral(SERIZ_TYPE_INT64);

	case SERIZ_TYPE_UINT8:	return toliteral(SERIZ_TYPE_UINT8);
	case SERIZ_TYPE_UINT16:	return toliteral(SERIZ_TYPE_UINT16);
	case SERIZ_TYPE_UINT32:	return toliteral(SERIZ_TYPE_UINT32);
	case SERIZ_TYPE_UINT64:	return toliteral(SERIZ_TYPE_UINT64);

	case SERIZ_TYPE_FLOAT:	return toliteral(SERIZ_TYPE_FLOAT);
	case SERIZ_TYPE_DOUBLE:	return toliteral(SERIZ_TYPE_DOUBLE);
	}
	return "UNKNOWN";
}


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
size_t	xserialize(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
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


size_t	xdeserialize(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
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
