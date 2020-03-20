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
#include "xmsq.h"
#include "xverify.h"

using namespace std;

void	*thread_verify(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;
	int index = 0;
	int ret = 0;
	int msqid = -1;

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
		char	ESC = 27;
		char	message[4096] = {0};
		char	signature[256] = {0};
		char	peerstr[100] = {0}, endmark[100] = {0};
		char	tmp[4096] = {0}, *tp = NULL;
		const char *filter = "!@#$";	// s_sendmore()로 publisher에서 보내는 것만 수용함 

		fprintf(stderr, "Subscriber START! peer=%s\n\n", peer);

		// message queue connect
		msqid = ai_msq_open(1234, (BUFF_SIZE * 10));

		strcpy(tmp, peer);
		strcat(tmp, ".out");
		tp = strchr(tmp, ':');
		assert(tp != NULL);
		*tp = '_';
		outfp = fopen(tmp, "w+b");
		assert(outfp != NULL);

		sprintf(peerstr, "tcp://%s", peer);

		int bufsize = 4 * 1024 * 1024;

		sprintf(endmark, "%s CLOSE", filter);

		sleep (1);

		fprintf(stderr, "Subscriber START RECV!\n\n");

		while (1)
		{
			message[0] = 0;
			signature[0] = 0;
			data_t msq_data;

			// message queue recv
			if (msgrcv(msqid, &msq_data, 
				sizeof(data_t) - sizeof(long), 
				(count%MAX_VERIFY), 0) == -1) {
				fprintf(outfp, "message queue recv error - %d", count);
			}

			std::string data(msq_data.mtext);

			count++;
			fprintf(outfp, "%d: %s\n", count, data.c_str());

			strcpy(tmp, data.c_str());
			if (strlen(tmp) >= strlen(filter) + 8)	// 4 byte filter, 8 byte number
			{
				// "!@#$####### ESCmessageESCsignature"
				char	*mp = strchr(tmp, ESC), *sp = NULL;
				if (mp)
				{
					sp = strchr(mp + 1, ESC);
					if (sp)
					{
						*sp = 0;
						strcpy(message, &mp[1]);
						strcpy(signature, &sp[1]);
					}
				}
			}
			if (message[0] == 0 || signature[0] == 0)
			{
				fprintf(stderr, "Message length %d or Signature length %d error!\n",
					strlen(message), strlen(signature));
				continue;
			}

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
					signature, message, &params.AddrHelper);

			fprintf(outfp, "verify-Message: %s signature=%s\n",
				verify_check ? "true" : "false", signature);
		}

		fclose(outfp);

		fprintf(stderr, "\r%s	%8d    \n", peer, count);

		fprintf(stderr, "\nSubscriber END! peer=%s\n", peer);

		break;
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	pthread_exit(NULL);

	return 0;
}

