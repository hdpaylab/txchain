#include <string>
#include <vector>


using namespace std;


typedef struct {
	string	str;
	int	ii;
	double	dd;
	string	str2;
}	myst_t;


int	main(int ac, char *av[])
{
	vector<myst_t> arr;

	arr.resize(1000000);
	printf("size=%ld capa=%ld\n", arr.size(), arr.capacity());

	for (int ii = 0; ii < 1000000; ii++)
	{
		myst_t st;

		st.str = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		st.ii = 987650000 + ii;
		st.dd = 0.67890 + ii;
		st.str2 = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

		arr[ii] = st;
	}

	for (int ii = 0; ii < 10; ii++)
	{
		myst_t& st = arr[ii];

		printf("Array[%d]: str=%s ii=%d dd=%f str2=%s\n",
			ii, st.str.c_str(), st.ii, st.dd, st.str2.c_str());
	}

	for (int ii = 999990; ii < 1000000; ii++)
	{
		myst_t& st = arr[ii];

		printf("Array[%d]: str=%s ii=%d dd=%f str2=%s\n",
			ii, st.str.c_str(), st.ii, st.dd, st.str2.c_str());
	}

	return 0;
}
