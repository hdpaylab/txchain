#ifndef __XSZ_H
#define __XSZ_H


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
	XSZ_HDRLEN_MASK		= 0xC0,
	XSZ_TYPE_MASK		= 0x3F,

	XSZ_TYPE_BINARY		= 0,	// has data length block
	XSZ_TYPE_STRING		= 1,	// has data length block

	XSZ_TYPE_INT8		= 2,
	XSZ_TYPE_INT16		= 3,
	XSZ_TYPE_INT32		= 4,
	XSZ_TYPE_INT64		= 5,

	XSZ_TYPE_UINT8		= 6,
	XSZ_TYPE_UINT16		= 7,
	XSZ_TYPE_UINT32		= 8,
	XSZ_TYPE_UINT64		= 9,

	XSZ_TYPE_FLOAT		= 10,
	XSZ_TYPE_DOUBLE		= 11,
};


const char *gettypename(int type);
uint32_t hdrlen2bytes(int hdrlen);
uint32_t size2hdrlen(size_t sz);
uint32_t size2hdrbytes(size_t sz);
void	setdatasize(void *buf, size_t sz, int hdrbytes);
uint64_t getdatasize(void *buf, int hdrbytes);
void	dumpbin(const char *buf, const size_t bufsz, const int spc = 0, const int nthnl = 0);


size_t	xserialize_func(char *buf, size_t bufsz, int datatype, void *data, size_t datasz);
size_t	xdeserialize_func(char *buf, size_t bufsz, int datatype, void *data, size_t datasz);

extern int _xsz_debug;


class xserialize {
public:
	xserialize(size_t sz = 1 * 1024)
	{
		bufsz_ = sz;
		if (bufsz_ <= 0)
			bufsz_ = 1 * 1024;
		inbuf_ = outbuf_ = buf_ = (char *)calloc(1, bufsz_);
		allocated_ = 1;
		inbufpos_ = outbufpos_ = 0;
		assert(buf_ != NULL);
		debug_ = 0;
	}
	~xserialize()
	{
		memset(buf_, 0, inbufpos_);	// safe clear
		if (allocated_ && buf_)
		{
			free(buf_);
		}
		bufsz_ = inbufpos_ = 0;
		inbuf_ = outbuf_ = buf_ = NULL;
	}

	void rewind()
	{
		outbuf_ = buf_;
		outbufpos_ = 0;
	}

	size_t size()
	{
		return inbufpos_;
	}

	// current position of outbuf
	size_t getcurpos()
	{
		return outbufpos_;
	}

	char *data()
	{
		return buf_;
	}

	// current data pointer of outbuf
	char *curdata()
	{
		return &buf_[outbufpos_];
	}

	string getstring()
	{
		string ss(buf_, inbufpos_);
		return ss;
	}

	// current(remaining) outbuf string
	string getcurstring()
	{
		string ss(curdata(), inbufpos_ - outbufpos_);
		return ss;
	}

	void setdata(char *data, size_t datalen)
	{
		if (data == NULL || datalen <= 0)
			return;
		clear();
		check_in_size(datalen);
		memcpy(inbuf_, data, datalen);
		update_inpos(datalen);
	}

	void setstring(string& str)
	{
		if (str.length() <= 0)
			return;
		clear();
		check_in_size(str.length());
		memcpy(inbuf_, str.c_str(), str.length());
		update_inpos(str.length());
	}

	int getcurtype()
	{
		return (*outbuf_ & XSZ_TYPE_MASK);
	}

	size_t getcursize()
	{
		int	type = getcurtype();
		int	hdrlen = (*outbuf_ & XSZ_HDRLEN_MASK) >> 6;
		int	hdrbytes = hdrlen2bytes(hdrlen);

		switch (type)
		{
		case XSZ_TYPE_BINARY: return getdatasize(outbuf_ + 1, hdrbytes);
		case XSZ_TYPE_STRING: return getdatasize(outbuf_ + 1, hdrbytes) + 1;

		case XSZ_TYPE_INT8:	return 1;
		case XSZ_TYPE_INT16:	return 2;
		case XSZ_TYPE_INT32:	return 4;
		case XSZ_TYPE_INT64:	return 8;

		case XSZ_TYPE_UINT8:	return 1;
		case XSZ_TYPE_UINT16:	return 2;
		case XSZ_TYPE_UINT32:	return 4;
		case XSZ_TYPE_UINT64:	return 8;

		case XSZ_TYPE_FLOAT:	return 4;
		case XSZ_TYPE_DOUBLE:	return 8;
		default:		return 0;
		}
	}

	void setbuf(char *newbuf, size_t newbuflen)
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
			bufsz_ = 1 * 1024;
			inbufpos_ = outbufpos_ = 0;
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
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_BINARY, (void *)bin, binsz));
	}

	size_t operator <<(string str)
	{
		assert(buf_ != NULL);
		assert(bufsz_ > 0);
		size_t	sz = str.length();
		check_in_size(sz + 2);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_STRING, (void *)str.c_str(), sz));
	}

	size_t operator <<(char *str)
	{
		assert(buf_ != NULL);
		assert(bufsz_ > 0);
		if (str == NULL)
			return 0;
		size_t	sz = strlen(str);
		check_in_size(sz + 2);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_STRING, (void *)str, sz));
	}

	size_t operator <<(int8_t i8)
	{
		size_t	sz = sizeof(i8);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT8, (void *)&i8, sz));
	}

	size_t operator <<(int16_t i16)
	{
		size_t	sz = sizeof(i16);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT16, (void *)&i16, sz));
	}

	size_t operator <<(int32_t i32)
	{
		size_t	sz = sizeof(i32);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT32, (void *)&i32, sz));
	}

	size_t operator <<(int64_t i64)
	{
		size_t	sz = sizeof(i64);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT64, (void *)&i64, sz));
	}

	size_t operator <<(uint8_t u8)
	{
		size_t	sz = sizeof(u8);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT8, (void *)&u8, sz));
	}

	size_t operator <<(uint16_t u16)
	{
		size_t	sz = sizeof(u16);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT16, (void *)&u16, sz));
	}

	size_t operator <<(uint32_t u32)
	{
		size_t	sz = sizeof(u32);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT32, (void *)&u32, sz));
	}

	size_t operator <<(uint64_t u64)
	{
		size_t	sz = sizeof(u64);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT64, (void *)&u64, sz));
	}

	size_t operator <<(float ff)
	{
		size_t	sz = sizeof(ff);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_FLOAT, (void *)&ff, sz));
	}

	size_t operator <<(double dd)
	{
		size_t	sz = sizeof(dd);
		check_in_size(sz + 1);
		return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_DOUBLE, (void *)&dd, sz));
	}

	// de-serialize

	size_t deseriz(uint8_t *outbin, size_t outbinsz)
	{
		check_cur_type(XSZ_TYPE_BINARY);
		check_out_size(outbinsz);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_BINARY, (void *)outbin, outbinsz));
	}

	size_t operator >>(string& str)
	{
		check_cur_type(XSZ_TYPE_STRING);
		size_t sz = getcursize();
		char *buf = (char *)calloc(1, sz + 1);
		int len = 0;
		if (buf)
		{
			len = xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_STRING, (void *)buf, sz);
			str.assign(buf, sz);
			free(buf);
			return update_outpos(len);
		}
		else
			return 0;
	}

	size_t operator >>(char **str)
	{
		check_cur_type(XSZ_TYPE_STRING);
		size_t sz = getcursize();
		*str = (char *)calloc(1, sz + 1);
		if (*str)
			return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, 
				XSZ_TYPE_STRING, (void *)*str, sz));
		else
			return 0;
	}

	size_t operator >>(int8_t& i8)
	{
		if (debug_) printf("i8: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(i8);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT8, (void *)&i8, sz));
	}

	size_t operator >>(int16_t& i16)
	{
		if (debug_) printf("i16: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(i16);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT16, (void *)&i16, sz));
	}

	size_t operator >>(int32_t& i32)
	{
		if (debug_) printf("i32: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(i32);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT32, (void *)&i32, sz));
	}

	size_t operator >>(int64_t& i64)
	{
		if (debug_) printf("i64: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(i64);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT64, (void *)&i64, sz));
	}

	size_t operator >>(uint8_t& u8)
	{
		if (debug_) printf("u8: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(u8);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT8, (void *)&u8, sz));
	}

	size_t operator >>(uint16_t& u16)
	{
		if (debug_) printf("u16: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(u16);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT16, (void *)&u16, sz));
	}

	size_t operator >>(uint32_t& u32)
	{
		if (debug_) printf("u32: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(u32);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT32, (void *)&u32, sz));
	}

	size_t operator >>(uint64_t& u64)
	{
		if (debug_) printf("u64: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(u64);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&u64, sz));
	}

	size_t operator >>(float& ff)
	{
		if (debug_) printf("ff: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(ff);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&ff, sz));
	}

	size_t operator >>(double& dd)
	{
		if (debug_) printf("dd: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
		size_t	sz = sizeof(dd);
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&dd, sz));
	}

	void	dump(int nthnl = 0, int spc = 0)
	{
		size_t sz = size();
		char *buf = data();
		dumpbin(buf, sz, nthnl, spc);
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
			size_t	newsz = (inbufpos_ + reqsz) + 1 * 1024;
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
		int	hdrlen = (*outbuf_ & XSZ_HDRLEN_MASK) >> 6;
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

	void	set_debug(int debug)
	{
		debug_ = debug;
		_xsz_debug = debug;
	}

	char	*buf_;		// buffer
	size_t	bufsz_;		// buffer size
	char	*inbuf_;	// last position of buffer
	size_t	inbufpos_;	// current data length
	char	*outbuf_;	// last position of buffer
	size_t	outbufpos_;	// current data length
	int	allocated_;	// 1 if calloc() called
	int	debug_;		// 1=print debug information
};


#endif	// __XSZ_H
