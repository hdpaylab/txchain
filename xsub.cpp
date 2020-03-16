#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "zhelpers.hpp"
#include "xdb.h"
#include "xparams.h"

using namespace std;


void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;

	/***
	PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
			user=postgres \
			password=postgres \
			dbname=testdb");

	if (PQstatus(conn) == CONNECTION_BAD) {
		printf("db connect failed\n");
	}
	***/

	// params set
	Params_type_t params = paramsget("params.dat");

	while (1)
	{
		FILE	*outfp = NULL;
		char	peerstr[100] = {0}, endmark[100] = {0};
		char	tmp[100] = {0}, *tp = NULL;
		const char *filter = "!@#$";	// s_sendmore()로 publisher에서 보내는 것만 수용함 

		fprintf(stderr, "Subscriber START! peer=%s\n\n", peer);

		zmq::context_t context_sub(1);
		zmq::socket_t xsock(context_sub, ZMQ_SUB);

		strcpy(tmp, peer);
		strcat(tmp, ".out");
		tp = strchr(tmp, ':');
		assert(tp != NULL);
		*tp = '_';
		outfp = fopen(tmp, "w+b");
		assert(outfp != NULL);

		sprintf(peerstr, "tcp://%s", peer);
		xsock.connect(peerstr);

		xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
		int bufsize = 4 * 1024 * 1024;
		xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

		sprintf(endmark, "%s CLOSE", filter);

		sleep (1);

		fprintf(stderr, "Subscriber START RECV!\n\n");

		while (1)
		{
			std::string data = s_recv(xsock);
			char	signature[256] = {0};

			count++;
			fprintf(outfp, "%d: %s\n", count, data.c_str());

			strcpy(tmp, data.c_str());
			if (strlen(tmp) >= 12)	// 4 byte filter, 8 byte number
				strcpy(signature, &tmp[12]);

			if (data == endmark)
			{
				fprintf(stderr, "Subscriber RECV CLOSE!\n");
				break;
			}
			else if (count % 100 == 0)
			{
				fprintf(stderr, "\r%s	%8d    ", peer, count);
				fflush(stderr);
			}

			int verify_check = verify_message(
					"HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw",
					signature,
					"Hdac Technology, Solution Dev Team, Test Text.",
					&params.AddrHelper);

			printf("verify-Message: %s signature=%s\n",
				verify_check? "true" : "false", signature);

		}

		fclose(outfp);

		fprintf(stderr, "\r%s	%8d    \n", peer, count);

		fprintf(stderr, "\nSubscriber END! peer=%s\n", peer);

		xsock.close();

		break;
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	pthread_exit(NULL);

	return 0;
}

