//
// Performance: 300000/sec 
//

#include "xserialize.h"


#define toliteral(v)	#v


int	_xsz_debug = 0;


////////////////////////////////////////////////////////////////////////////////
// xserialize class implementations

xserialize::xserialize(size_t sz)
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


xserialize::~xserialize()
{
	memset(buf_, 0, inbufpos_);	// safe clear
	if (allocated_ && buf_)
	{
		free(buf_);
	}
	bufsz_ = inbufpos_ = 0;
	inbuf_ = outbuf_ = buf_ = NULL;
}


void xserialize::rewind()
{
	outbuf_ = buf_;
	outbufpos_ = 0;
}

size_t xserialize::size()
{
	return inbufpos_;
}

// current position of outbuf
size_t xserialize::getcurpos()
{
	return outbufpos_;
}

char *xserialize::data()
{
	return buf_;
}

// current data pointer of outbuf
char *xserialize::curdata()
{
	return &buf_[outbufpos_];
}

string xserialize::getstring()
{
	string ss(buf_, inbufpos_);
	return ss;
}

// current(remaining) outbuf string
string xserialize::getcurstring()
{
	string ss(curdata(), inbufpos_ - outbufpos_);
	return ss;
}

void xserialize::setdata(char *data, size_t datalen)
{
	if (data == NULL || datalen <= 0)
		return;
	clear();
	check_in_size(datalen);
	memcpy(buf_, data, datalen);
	update_inpos(datalen);
}

void xserialize::setstring(string& str)
{
	if (str.size() <= 0)
		return;
	clear();
	check_in_size(str.size());
	memcpy(buf_, str.c_str(), str.size());
	update_inpos(str.size());
}

int xserialize::getcurtype()
{
	return (*outbuf_ & XSZ_TYPE_MASK);
}

size_t xserialize::getcursize()
{
	int	type = getcurtype();
	int	hdrlen = (*outbuf_ & XSZ_HDRLEN_MASK) >> 6;
	int	hdrbytes = hdrlen2bytes(hdrlen);

	switch (type)
	{
	case XSZ_TYPE_BINARY: return getdatasize(outbuf_ + 1, hdrbytes);
	case XSZ_TYPE_STRING: return getdatasize(outbuf_ + 1, hdrbytes);

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

void xserialize::setbuf(char *newbuf, size_t newbuflen)
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

void xserialize::clear()
{
	ninput_ = 0;
	noutput_ = 0;
	memset(buf_, 0, bufsz_);	// safe clear

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
size_t xserialize::seriz(uint8_t *bin, size_t binsz)
{
	ninput_++;
	check_in_size(binsz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_BINARY, (void *)bin, binsz));
}

size_t xserialize::operator <<(string str)
{
	assert(buf_ != NULL);
	assert(bufsz_ > 0);
	ninput_++;
	size_t	sz = str.length();
	check_in_size(sz + 2);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_STRING, (void *)str.c_str(), sz));
}

size_t xserialize::operator <<(char *str)
{
	assert(buf_ != NULL);
	assert(bufsz_ > 0);
	if (str == NULL)
		return 0;
	ninput_++;
	size_t	sz = strlen(str);
	check_in_size(sz + 2);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_STRING, (void *)str, sz));
}

size_t xserialize::operator <<(int8_t i8)
{
	ninput_++;
	size_t	sz = sizeof(i8);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT8, (void *)&i8, sz));
}

size_t xserialize::operator <<(int16_t i16)
{
	ninput_++;
	size_t	sz = sizeof(i16);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT16, (void *)&i16, sz));
}

size_t xserialize::operator <<(int32_t i32)
{
	ninput_++;
	size_t	sz = sizeof(i32);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT32, (void *)&i32, sz));
}

size_t xserialize::operator <<(int64_t i64)
{
	ninput_++;
	size_t	sz = sizeof(i64);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_INT64, (void *)&i64, sz));
}

size_t xserialize::operator <<(uint8_t u8)
{
	ninput_++;
	size_t	sz = sizeof(u8);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT8, (void *)&u8, sz));
}

size_t xserialize::operator <<(uint16_t u16)
{
	ninput_++;
	size_t	sz = sizeof(u16);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT16, (void *)&u16, sz));
}

size_t xserialize::operator <<(uint32_t u32)
{
	ninput_++;
	size_t	sz = sizeof(u32);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT32, (void *)&u32, sz));
}

size_t xserialize::operator <<(uint64_t u64)
{
	ninput_++;
	size_t	sz = sizeof(u64);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_UINT64, (void *)&u64, sz));
}

size_t xserialize::operator <<(float ff)
{
	ninput_++;
	size_t	sz = sizeof(ff);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_FLOAT, (void *)&ff, sz));
}

size_t xserialize::operator <<(double dd)
{
	ninput_++;
	size_t	sz = sizeof(dd);
	check_in_size(sz + 1);
	return update_inpos(xserialize_func(inbuf_, bufsz_ - inbufpos_, XSZ_TYPE_DOUBLE, (void *)&dd, sz));
}

// de-serialize

size_t xserialize::deseriz(uint8_t *outbin, size_t outbinsz)
{
	if (check_out_size(outbinsz) == 0)
		return 0;
	if (check_cur_type(XSZ_TYPE_BINARY) == 0)
		return 0;
	noutput_++;
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_BINARY, (void *)outbin, outbinsz));
}

size_t xserialize::operator >>(string& str)
{
	size_t sz = getcursize();
	if (check_cur_type(XSZ_TYPE_STRING) == 0)
		return 0;
	noutput_++;
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

size_t xserialize::operator >>(char **str)
{
	size_t sz = getcursize();
	if (check_cur_type(XSZ_TYPE_STRING) == 0)
		return 0;

	noutput_++;
	*str = (char *)calloc(1, sz + 1);
	if (*str)
		return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, 
			XSZ_TYPE_STRING, (void *)*str, sz));
	else
		return 0;
}

size_t xserialize::operator >>(int8_t& i8)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: i8: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("i8: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(i8);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT8, (void *)&i8, sz));
}

size_t xserialize::operator >>(int16_t& i16)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: i16: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("i16: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(i16);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT16, (void *)&i16, sz));
}

size_t xserialize::operator >>(int32_t& i32)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: i32: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("i32: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(i32);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT32, (void *)&i32, sz));
}

size_t xserialize::operator >>(int64_t& i64)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: i64: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("i64: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(i64);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_INT64, (void *)&i64, sz));
}

size_t xserialize::operator >>(uint8_t& u8)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: u8: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("u8: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(u8);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT8, (void *)&u8, sz));
}

size_t xserialize::operator >>(uint16_t& u16)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: u16: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("u16: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(u16);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT16, (void *)&u16, sz));
}

size_t xserialize::operator >>(uint32_t& u32)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: u32: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("u32: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(u32);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT32, (void *)&u32, sz));
}

size_t xserialize::operator >>(uint64_t& u64)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: u64: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("u64: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(u64);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&u64, sz));
}

size_t xserialize::operator >>(float& ff)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: float: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("ff: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(ff);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&ff, sz));
}

size_t xserialize::operator >>(double& dd)
{
	size_t sz = getcursize();
	if (sz <= 0)
	{
		fprintf(stderr, "WARNING: double: cursz=%ld\n", sz);
		return 0;
	}
	if (debug_) printf("dd: outbuf=0x%02X outpos=%ld\n", *outbuf_, outbufpos_);
	noutput_++;
	sz = sizeof(dd);
	return update_outpos(xdeserialize_func(outbuf_, bufsz_ - outbufpos_, XSZ_TYPE_UINT64, (void *)&dd, sz));
}

void	xserialize::dump(int nthnl, int spc)
{
	size_t sz = size();
	char *buf = data();
	dumpbin(buf, sz, nthnl, spc);
}

// resize buffer
void	xserialize::check_in_size(size_t reqsz)
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
			outbuf_ = buf_ + outbufpos_;
			allocated_ = 1;
		}
	}
}

int	xserialize::check_out_size(size_t reqsz)
{
	size_t	sz = getcursize();

	if (sz == 0)
	{
		fprintf(stderr, "WARNING: no more deserializable data...\n");
		return 0;
	}
	if (reqsz < sz)
	{
		fprintf(stderr, "WARNING: deserialize reqsz %ld < cur size %ld\n", reqsz, sz);
		return 0;
	}
	return 1;
}

int	xserialize::check_cur_type(int type)
{
	if (getcurtype() != type)
	{
		if (outbufpos_ < inbufpos_)
		{
			fprintf(stderr, "WARNING: deserialize cur type %s != type %s\n",
				gettypename(getcurtype()), gettypename(type));
		}
		return 0;
	}
	return 1;
}

size_t	xserialize::update_inpos(size_t len)
{
	inbuf_ += len;
	inbufpos_ += len;

	return len;
}

size_t	xserialize::update_outpos(size_t len)
{
	outbuf_ += len;
	outbufpos_ += len;

	return len;
}

void	xserialize::set_debug(int debug)
{
	debug_ = debug;
	_xsz_debug = debug;
}





const char *gettypename(int datatype)
{
	switch (datatype)
	{
	case XSZ_TYPE_BINARY:	return toliteral(XSZ_TYPE_BINARY);
	case XSZ_TYPE_STRING:	return toliteral(XSZ_TYPE_STRING);

	case XSZ_TYPE_INT8:	return toliteral(XSZ_TYPE_INT8);
	case XSZ_TYPE_INT16:	return toliteral(XSZ_TYPE_INT16);
	case XSZ_TYPE_INT32:	return toliteral(XSZ_TYPE_INT32);
	case XSZ_TYPE_INT64:	return toliteral(XSZ_TYPE_INT64);

	case XSZ_TYPE_UINT8:	return toliteral(XSZ_TYPE_UINT8);
	case XSZ_TYPE_UINT16:	return toliteral(XSZ_TYPE_UINT16);
	case XSZ_TYPE_UINT32:	return toliteral(XSZ_TYPE_UINT32);
	case XSZ_TYPE_UINT64:	return toliteral(XSZ_TYPE_UINT64);

	case XSZ_TYPE_FLOAT:	return toliteral(XSZ_TYPE_FLOAT);
	case XSZ_TYPE_DOUBLE:	return toliteral(XSZ_TYPE_DOUBLE);
	}
	static char tmp[50] = {0};
	sprintf(tmp, "UNKNOWN(%d)", datatype);

	return tmp;
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
	default: break;
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
	default: break;
	case 1: { uint8_t u8 = *(uint8_t *)buf; sz = u8; break; }
	case 2: { uint16_t u16 = *(uint16_t *)buf; sz = u16; break; }
	case 4: { uint32_t u32 = *(uint32_t *)buf; sz = u32; break; }
	case 8: { uint64_t u64 = *(uint64_t *)buf; sz = u64; break; }
	}
	return sz;
}


void	dumpbin(const char *buf, const size_t bufsz, const int nthnl, const int spc)
{
	for (size_t ii = 0; ii < bufsz; ii++)
	{
		if (ii > 0 && nthnl > 0 && ii % nthnl == 0)
			printf("\n");
		printf("%02X%s", buf[ii] & 0x00FF, spc ? " " : "");
	}
	printf("\n");
}


//
// byte1: 2bit = data length in bytes / 6bit = data type
// byte2~9: real data length
// data...
//
size_t	xserialize_func(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
{
	if (datatype < 0 || datatype > XSZ_TYPE_MASK)
	{
		printf("ERROR: data tyep unknown=0x%08X\n", datatype);
		return 0;
	}
	if (buf == NULL || bufsz <= 0 || data == NULL)
	{
		printf("ERROR: buf=%lX or data=%lX: bufsz=%ld datasz=%ld\n", 
			(unsigned long)buf, (unsigned long)data, bufsz, datasz);
		return 0;
	}
	if (datasz > bufsz)
	{
		printf("ERROR: bufsz %ld < datasz %ld + 2\n", bufsz, datasz);
		return 0;
	}

	char	*bp = buf;
	uint32_t hdrlen = 0, hdrbytes = 0;

	switch (datatype)
	{
	case XSZ_TYPE_BINARY:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		if (_xsz_debug) printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		// hdr data length block copy
		hdrbytes = size2hdrbytes(datasz);
		setdatasize(bp, datasz, hdrbytes);
		bp += hdrbytes;

		// data body copy
		memcpy(bp, data, datasz);

		return datasz + 2 + hdrbytes;

	case XSZ_TYPE_STRING:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		if (_xsz_debug) printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		// hdr data length block copy
		hdrbytes = size2hdrbytes(datasz);
		setdatasize(bp, datasz, hdrbytes);
		bp += hdrbytes;

		// data body copy
		memcpy(bp, data, datasz);
	//	printf("SZ.str: "); dumpbin((char *)buf, datasz + 2 + hdrbytes, 10, 1);

		return datasz + 2 + hdrbytes;


	case XSZ_TYPE_INT8:
	case XSZ_TYPE_INT16:
	case XSZ_TYPE_INT32:
	case XSZ_TYPE_INT64:

	case XSZ_TYPE_UINT8:
	case XSZ_TYPE_UINT16:
	case XSZ_TYPE_UINT32:
	case XSZ_TYPE_UINT64:

	case XSZ_TYPE_FLOAT:
	case XSZ_TYPE_DOUBLE:
		// hdr 1 byte copy
		hdrlen = size2hdrlen(datasz);
		*bp = datatype | hdrlen;
		if (_xsz_debug) printf("    HDR=0x%02X: hdrlen=0x%02X type=0x%02X\n", *bp & 0x00FF, hdrlen, datatype);
		bp++;

		memcpy(bp, data, datasz);
		bp += datasz;
	//	printf("NUM: "); dumpbin((char *)buf, datasz + 1, 10, 1);
		return datasz + 1;

	default:
		return 0;
	}
}


size_t	xdeserialize_func(char *buf, size_t bufsz, int datatype, void *data, size_t datasz)
{
	if (datatype < 0 || datatype > XSZ_TYPE_MASK)
		return 0;
	if (buf == NULL || bufsz <= 0 || data == NULL)
		return 0;

	char	*bp = buf, *dp = (char *)data;
	int	hdrlen = (*bp & XSZ_HDRLEN_MASK) >> 6;
	int	hdrbytes = hdrlen2bytes(hdrlen);
	int	type = (*bp & XSZ_TYPE_MASK);
	size_t	sz = 0;

	bp++;		// skip type
	switch (type)
	{
	case XSZ_TYPE_BINARY:
		sz = getdatasize(bp, hdrbytes);
		bp += hdrbytes;
		if (datasz < sz)
		{
			fprintf(stderr, "WARNING: Binary real length=%ld copyed %ld\n",
				sz, datasz);
			memcpy(data, bp, datasz); 
		}
		else
		{
			memcpy(data, bp, sz);
		}
		return sz + 2 + hdrbytes;

	case XSZ_TYPE_STRING:		// same as binary
		sz = getdatasize(bp, hdrbytes);
		bp += hdrbytes;
		if (datasz < sz)
		{
			fprintf(stderr, "WARNING: String real length=%ld copyed %ld\n",
				sz, datasz);
			memcpy(data, bp, datasz); 
		}
		else
		{
			memcpy(data, bp, sz); 
		}
	//	printf("DSZ.str: "); dumpbin((char *)data, sz, 10, 1);
		return sz + 2 + hdrbytes;

	case XSZ_TYPE_UINT8:
	case XSZ_TYPE_UINT16:
	case XSZ_TYPE_UINT32:
	case XSZ_TYPE_UINT64:

	case XSZ_TYPE_INT8:
	case XSZ_TYPE_INT16:
	case XSZ_TYPE_INT32:
	case XSZ_TYPE_INT64:

	case XSZ_TYPE_FLOAT:
	case XSZ_TYPE_DOUBLE:
		memcpy(data, bp, datasz);
	//	dumpbin((char *)data, datasz, 10, 1);
		return datasz + 1;

	default:
		return 0;
	}
}
