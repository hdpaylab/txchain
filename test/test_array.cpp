#include <string>
#include "xarray.h"


using namespace std;


typedef struct {
	string	str;
	int	ii;
	double	dd;
	string	str2;
}	myst_t;


int	main(int ac, char *av[])
{
	xarray<myst_t> arr(10);

	for (int ii = 0; ii < 20; ii++)
	{
		myst_t st;

		st.str = "AAAAAAAAA";
		st.ii = 987654321;
		st.dd = 12345.67890;
		st.str2 = "BBBBBBBBB";
		printf("Add [%d]\n", ii);

		arr[ii] = st;
	}

	for (int ii = 0; ii < 20; ii++)
	{
		myst_t& st = arr[ii];

		printf("Array[%d]: str=%s ii=%d dd=%f str2=%s\n",
			ii, st.str.c_str(), st.ii, st.dd, st.str2.c_str());
	}

	return 0;
}
