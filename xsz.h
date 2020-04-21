#ifndef __XSERIZ_H
#define __XSERIZ_H


#include <string.h>
#include <assert.h>
#include <string>
#include <streambuf>

using namespace std;


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


const char *gettypename(int type);
uint32_t hdrlen2bytes(int hdrlen);
uint32_t size2hdrlen(size_t sz);
uint32_t size2hdrbytes(size_t sz);
void	setdatasize(void *buf, size_t sz, int hdrbytes);
uint64_t getdatasize(void *buf, int hdrbytes);
void	dumpbin(char *buf, size_t bufsz);


size_t	xserialize(char *buf, size_t bufsz, int datatype, void *data, size_t datasz);
size_t	xdeserialize(char *buf, size_t bufsz, int datatype, void *data, size_t datasz);


class xserial {
public:
	xserial(size_t sz)
	{
		bufsz_ = sz;
		if (bufsz_ <= 0)
			bufsz_ = 64 * 1024;
		inbuf_ = outbuf_ = buf_ = (char *)calloc(1, bufsz_);
		allocated_ = 1;
		inbufpos_ = outbufpos_ = 0;
		assert(buf_ != NULL);
	}
	~xserial()
	{
		memset(buf_, 0, inbufpos_);	// safe clear
		if (allocated_ && buf_)
		{
			free(buf_);
		}
		bufsz_ = inbufpos_ = 0;
		inbuf_ = outbuf_ = buf_ = NULL;
	}

	size_t getsize()
	{
		return inbufpos_;
	}

	char *getbuf()
	{
		return buf_;
	}

	int	getcurtype()
	{
		return (*outbuf_ & SERIZ_TYPE_MASK);
	}

	size_t getcursize()
	{
		int	type = getcurtype();
		int	hdrlen = (*outbuf_ & SERIZ_HDRLEN_MASK) >> 6;
		int	hdrbytes = hdrlen2bytes(hdrlen);

		switch (type)
		{
		case SERIZ_TYPE_BINARY: return getdatasize(outbuf_ + 1, hdrbytes);
		case SERIZ_TYPE_STRING: return getdatasize(outbuf_ + 1, hdrbytes) + 1;

		case SERIZ_TYPE_INT8:	return 1;
		case SERIZ_TYPE_INT16:	return 2;
		case SERIZ_TYPE_INT32:	return 4;
		case SERIZ_TYPE_INT64:	return 8;

		case SERIZ_TYPE_UINT8:	return 1;
		case SERIZ_TYPE_UINT16:	return 2;
		case SERIZ_TYPE_UINT32:	return 4;
		case SERIZ_TYPE_UINT64:	return 8;

		case SERIZ_TYPE_FLOAT:	return 4;
		case SERIZ_TYPE_DOUBLE:	return 8;
		default:		return 0;
		}
	}

	void	setbuf(char *newbuf, size_t newbuflen)
	{
		clear();
		if (newbuf && newbuflen > 0)
		{
			free(buf_);
			inbuf_ = outbuf_ = buf_ = newbuf;
			bufsz_ = newbuflen;
			inbufpos_ = outbufpos_ = 0;
			allocated_ = 0;
		}
	}

	void clear()
	{
		memset(buf_, 0, inbufpos_);	// safe clear
		if (allocated_)
		{
			if (buf_)
				free(buf_);
			inbufpos_ = outbufpos_ = 0;
			if (bufsz_ <= 0)
				bufsz_ = 64 * 1024;
			inbuf_ = outbuf_ = buf_ = (char *)calloc(1, bufsz_);
			allocated_ = 1;
		}
		else
		{
			inbufpos_ = outbufpos_ = 0;
			inbuf_ = outbuf_ = buf_;
		}
	}

	// serialize

	// binary serialize 
	size_t seriz(uint8_t *bin, size_t binsz)
	{
		check_in_size(binsz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_BINARY, (void *)bin, binsz));
	}

	size_t operator <<(string str)
	{
		assert(buf_ != NULL);
		assert(bufsz_ > 0);
		size_t	sz = str.length();
		check_in_size(sz + 2);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_STRING, (void *)str.c_str(), sz));
	}

	size_t operator <<(char *str)
	{
		assert(buf_ != NULL);
		assert(bufsz_ > 0);
		if (str == NULL)
			return 0;
		size_t	sz = strlen(str);
		check_in_size(sz + 2);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_STRING, (void *)str, sz));
	}

	size_t operator <<(int8_t i8)
	{
		size_t	sz = sizeof(i8);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_INT8, (void *)&i8, sz));
	}

	size_t operator <<(int16_t i16)
	{
		size_t	sz = sizeof(i16);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_INT16, (void *)&i16, sz));
	}

	size_t operator <<(int32_t i32)
	{
		size_t	sz = sizeof(i32);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_INT32, (void *)&i32, sz));
	}

	size_t operator <<(int64_t i64)
	{
		size_t	sz = sizeof(i64);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_INT64, (void *)&i64, sz));
	}

	size_t operator <<(uint8_t u8)
	{
		size_t	sz = sizeof(u8);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_UINT8, (void *)&u8, sz));
	}

	size_t operator <<(uint16_t u16)
	{
		size_t	sz = sizeof(u16);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_UINT16, (void *)&u16, sz));
	}

	size_t operator <<(uint32_t u32)
	{
		size_t	sz = sizeof(u32);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_UINT32, (void *)&u32, sz));
	}

	size_t operator <<(uint64_t u64)
	{
		size_t	sz = sizeof(u64);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_UINT64, (void *)&u64, sz));
	}

	size_t operator <<(float ff)
	{
		size_t	sz = sizeof(ff);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_FLOAT, (void *)&ff, sz));
	}

	size_t operator <<(double dd)
	{
		size_t	sz = sizeof(dd);
		check_in_size(sz + 1);
		return update_inpos(xserialize(inbuf_, bufsz_ - inbufpos_, SERIZ_TYPE_DOUBLE, (void *)&dd, sz));
	}

	// de-serialize

	size_t deseriz(uint8_t *outbin, size_t outbinsz)
	{
		check_cur_type(SERIZ_TYPE_BINARY);
		check_out_size(outbinsz);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_BINARY, (void *)outbin, outbinsz));
	}

	size_t operator >>(string& str)
	{
		check_cur_type(SERIZ_TYPE_STRING);
		size_t sz = getcursize();
		char *buf = (char *)calloc(1, sz);
		int len = 0;
		if (buf)
		{
			len = xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_STRING, (void *)buf, sz);
			str = buf;
			free(buf);
		}
		return update_outpos(len);
	}

	size_t operator >>(char **str)
	{
		check_cur_type(SERIZ_TYPE_STRING);
		size_t sz = getcursize();
		*str = (char *)calloc(1, sz + 1);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_STRING, (void *)*str, sz));
	}

	size_t operator >>(int8_t& i8)
	{
		printf("i8: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		dumpbin(outbuf_, 2);
		size_t	sz = sizeof(i8);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_INT8, (void *)&i8, sz));
	}

	size_t operator >>(int16_t& i16)
	{
		printf("i16: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		dumpbin(outbuf_, 3);
		size_t	sz = sizeof(i16);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_INT16, (void *)&i16, sz));
	}

	size_t operator >>(int32_t& i32)
	{
		printf("i32: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		dumpbin(outbuf_, 5);
		size_t	sz = sizeof(i32);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_INT32, (void *)&i32, sz));
	}

	size_t operator >>(int64_t& i64)
	{
		printf("i64: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		dumpbin(outbuf_, 9);
		size_t	sz = sizeof(i64);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_INT64, (void *)&i64, sz));
	}

	size_t operator >>(uint8_t& u8)
	{
		size_t	sz = sizeof(u8);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT8, (void *)&u8, sz));
	}

	size_t operator >>(uint16_t& u16)
	{
		size_t	sz = sizeof(u16);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT16, (void *)&u16, sz));
	}

	size_t operator >>(uint32_t& u32)
	{
		size_t	sz = sizeof(u32);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT32, (void *)&u32, sz));
	}

	size_t operator >>(uint64_t& u64)
	{
		size_t	sz = sizeof(u64);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT64, (void *)&u64, sz));
	}

	size_t operator >>(float& ff)
	{
		size_t	sz = sizeof(ff);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT64, (void *)&ff, sz));
	}

	size_t operator >>(double& dd)
	{
		size_t	sz = sizeof(dd);
		return update_outpos(xdeserialize(outbuf_, bufsz_ - outbufpos_, SERIZ_TYPE_UINT64, (void *)&dd, sz));
	}

private:
	// resize buffer
	void	check_in_size(size_t reqsz)
	{
		if (!allocated_)
		{
			if (inbufpos_ + reqsz > bufsz_)
			{
				fprintf(stderr, "WARNING: bufsz %ld < curlen %ld + reqsz %ld\n",
					bufsz_, inbufpos_, reqsz);
			}
		}
		else if (allocated_ && inbufpos_ + reqsz > bufsz_ && buf_)
		{
			size_t	newsz = (inbufpos_ + reqsz) + 64 * 1024;
			char	*newbuf = (char *)calloc(1, newsz);
			if (newbuf)
			{
				bufsz_ = newsz;
				memcpy(newbuf, buf_, inbufpos_);
				memset(buf_, 0, inbufpos_);	// safe clear
				free(buf_);
				buf_ = newbuf;
				inbuf_ = buf_ + inbufpos_;
				allocated_ = 1;
			}
		}
	}

	int	check_out_size(size_t reqsz)
	{
		int	hdrlen = (*outbuf_ & SERIZ_HDRLEN_MASK) >> 6;
		int	hdrbytes = hdrlen2bytes(hdrlen);
		size_t	sz = getdatasize(outbuf_ + 1, hdrbytes);

		if (reqsz < sz)
		{
			fprintf(stderr, "WARNING: reqsz %ld < cur size %ld\n", reqsz, sz);
			return 0;
		}
		return 1;
	}

	int	check_cur_type(int type)
	{
		if (getcurtype() != type)
		{
			fprintf(stderr, "WARNING: cur type %s != type %s\n",
				gettypename(getcurtype()), gettypename(type));
			return 0;
		}
		return 1;
	}

	size_t	update_inpos(size_t len)
	{
		inbuf_ += len;
		inbufpos_ += len;

		return len;
	}

	size_t	update_outpos(size_t len)
	{
		outbuf_ += len;
		outbufpos_ += len;

		return len;
	}

	char	*buf_;		// buffer
	size_t	bufsz_;		// buffer size
	char	*inbuf_;	// last position of buffer
	size_t	inbufpos_;	// current data length
	char	*outbuf_;	// last position of buffer
	size_t	outbufpos_;	// current data length
	int	allocated_;	// 1 if calloc() called
};


#endif	// __XSERIZ_H
