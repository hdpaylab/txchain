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
	xserialize(size_t sz = 1 * 1024);
	xserialize(string str);
	~xserialize();

	void rewind();

	size_t size();

	// current position of outbuf
	size_t getcurpos();

	char *data();

	// current data pointer of outbuf
	char *curdata();

	string getstring();

	// current(remaining) outbuf string
	string getcurstring();

	void setdata(char *data, size_t datalen);

	void setstring(string& str);

	int getcurtype();

	size_t getcursize();

	void setbuf(char *newbuf, size_t newbuflen);

	void clear();

	// ===== serialize

	// binary serialize 
	size_t seriz(uint8_t *bin, size_t binsz);

	size_t operator <<(string str);

	size_t operator <<(char *str);

	size_t operator <<(int8_t i8);

	size_t operator <<(int16_t i16);

	size_t operator <<(int32_t i32);

	size_t operator <<(int64_t i64);

	size_t operator <<(uint8_t u8);

	size_t operator <<(uint16_t u16);

	size_t operator <<(uint32_t u32);

	size_t operator <<(uint64_t u64);

	size_t operator <<(float ff);

	size_t operator <<(double dd);

	// ===== de-serialize

	size_t deseriz(uint8_t *outbin, size_t outbinsz);

	size_t operator >>(string& str);

	size_t operator >>(char **str);

	size_t operator >>(int8_t& i8);

	size_t operator >>(int16_t& i16);

	size_t operator >>(int32_t& i32);

	size_t operator >>(int64_t& i64);

	size_t operator >>(uint8_t& u8);

	size_t operator >>(uint16_t& u16);

	size_t operator >>(uint32_t& u32);

	size_t operator >>(uint64_t& u64);

	size_t operator >>(float& ff);

	size_t operator >>(double& dd);

	void	dump(int nthnl = 0, int spc = 0);

private:
	// check buffer size & resize 
	void	check_in_size(size_t reqsz);

	int	check_out_size(size_t reqsz);

	int	check_cur_type(int type);

	size_t	update_inpos(size_t len);

	size_t	update_outpos(size_t len);

	void	set_debug(int debug);

	char	*buf_;		// buffer
	size_t	bufsz_;		// buffer size
	char	*inbuf_;	// last position of buffer
	size_t	inbufpos_;	// current data length
	char	*outbuf_;	// last position of buffer
	size_t	outbufpos_;	// current data length
	int	allocated_;	// 1 if calloc() called
	size_t	ninput_;	// serialization input count
	size_t	noutput_;	// serialization output count
	int	debug_;		// 1=print debug information
};


#endif	// __XSZ_H
