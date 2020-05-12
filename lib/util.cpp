#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string>


using namespace std;


double	xgetclock()
{
	double	ustime = 0;
	struct timeval	localxtm;

	gettimeofday(&localxtm, NULL);

	ustime = localxtm.tv_sec + localxtm.tv_usec / 1000000.0;

	return ustime;
}


string	bin2hex(const char *bin, const size_t binlen)
{
	char	*buf = (char *) calloc(1, binlen * 2);

	for (int ii = 0; ii < (ssize_t)binlen; ii++)
	{
		char	ch = bin[ii];
		char	ch1 = (ch & 0x00F0) >> 4;
		char	ch2 = (ch & 0x000F);

		buf[ii * 2] = (ch1 < 10 ? (ch1 + '0') : (ch1 + 'A' - 10));
		buf[ii * 2 + 1] = (ch2 < 10 ? (ch2 + '0') : (ch2 + 'A' - 10));
	}

	string ss(buf, binlen * 2);

	return ss;
}


string	hex2bin(const char *hexstr, const size_t len)
{
	char	*buf = (char *) calloc(1, len / 2);
	const char *hp = hexstr;

	for (int ii = 0; ii < (ssize_t)len / 2; ii += 2)
	{
		char	ch1 = hp[ii];
		char	ch2 = hp[ii + 1];

		if (ch1 >= '0' && ch1 <= '9')
			ch1 = ch1 - '0';
		else if (ch1 >= 'A' && ch1 <= 'F')
			ch1 = ch1 - 'A' + 10;
		if (ch2 >= '0' && ch2 <= '9')
			ch2 = ch2 - '0';
		else if (ch2 >= 'A' && ch2 <= 'F')
			ch2 = ch2 - 'A' + 10;

		buf[ii/2] = (ch1 << 4) | ch2;
	}

	string ss(buf, len / 2);
	free(buf);

	return ss;
}


string	hex2bin(const string& hexstr)
{
	return hex2bin(hexstr.c_str(), hexstr.length());
}
