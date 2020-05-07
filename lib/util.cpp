#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


double	xgetclock()
{
	double	ustime = 0;
	struct timeval	localxtm;

	gettimeofday(&localxtm, NULL);

	ustime = localxtm.tv_sec + localxtm.tv_usec / 1000000.0;

	return ustime;
}
