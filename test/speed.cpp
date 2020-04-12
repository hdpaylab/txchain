//
// calloc() : 1,000,000 / 0.0001 sec	@100억/sec
// strcpy   : 1,000,000 / 0.17 sec	@588만/sec
// memcpy() : 1,000,000 / 0.17 sec	@588만/sec
// memset() : 1,000,000 / 0.02 sec	@5000만/sec
// assert() : 1,000,000 / 0.164 sec	@600a만/sec
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


main()
{
	int	loop = 1000000;
	char	*tp = NULL;
	char	*sp = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
	int	len = strlen(sp);

	printf("Start calloc...\n");
	for (int ii = 0; ii < loop; ii++)
	{
		tp = (char *) calloc(1, 256);
		assert(tp != NULL);
		*tp = 0;
	//	memset(tp, 0, 256);
	//	strcpy(tp, sp);
	//	memcpy(tp, sp, strlen(sp));
		if (ii % 100000 == 0)
			printf("calloc() %d\n", ii);
	}
	printf("End calloc...\n");
}

