#include "lib.h"


void	new_params_test(const char *path);


int	main(int ac, char *av[])
{
	Params_type_t params = load_params("../lib/params.dat");

	return 0;
}
