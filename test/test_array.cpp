#include <string>
#include <vector>


using namespace std;


typedef struct {
	string	str;
	int	ii;
	int	jj;
	double	dd;
	string	str2;
}	myst_t;


void	vector_test();
void	vector_erase_test();


int	main(int ac, char *av[])
{
	vector_test();

	vector_erase_test();
}


void	vector_test()
{
	vector<myst_t> arr;

	arr.resize(1000000);
	printf("size=%ld capa=%ld\n", arr.size(), arr.capacity());

	for (int ii = 0; ii < 1000000; ii++)
	{
		myst_t st;

		st.str = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		st.ii = ii;
		st.jj = 10000 + ii;
		st.dd = 0.67890 + ii;
		st.str2 = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

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
}


void	vector_erase_test()
{
	vector<myst_t> arr;

	arr.resize(10);

	for (int ii = 0; ii < 10; ii++)
	{
		myst_t st;

		st.str = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		st.ii = ii;
		st.jj = ii;
		st.dd = 0.67890 + ii;
		st.str2 = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

		arr[ii] = st;
	}

	for (int ii = 0; ii < (ssize_t)arr.size(); ii++)
	{
		myst_t& st = arr[ii];

		printf("BEFORE [%d] ii=%d jj=%d\n", ii, st.ii, st.jj);
	}
	for (ssize_t ii = (ssize_t)arr.size() - 1; ii >= 0; ii--)
//	for (ssize_t ii = 0; ii < (ssize_t)arr.size(); ii++)
	{
		myst_t& st = arr[ii];

		st.ii = ii;
		if (st.jj % 2 == 0)
			arr.erase(arr.begin() + ii);
	}
	for (int ii = 0; ii < (ssize_t)arr.size(); ii++)
	{
		myst_t& st = arr[ii];

		printf("AFTER [%d] ii=%d jj=%d\n", ii, st.ii, st.jj);
	}
}
