#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <unistd.h>

#include "zhelpers.hpp"
#include "xdb.h"
#include "xparams.h"

using namespace std;


void	*thread_publisher(void *info_p)
{
        int	sendport = *(int *)info_p;
	int	count = 0;
	const char *filter = "!@#$";
	const char ESC = 27;

	/****
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

	for (int loop = 1; loop <= 1; loop++)
	{
		char	bindstr[100] = {0}, data[4096] = {0}, tmp[1024] = {0};
		char	*message = "Hdac Technology, Solution Dev Team, Test Text. 잘 가는지 검사하는 것임.";
		char 	*pubkey = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";

		fprintf(stderr, "Publisher: %d START! sendport=%d\n\n", loop, sendport);

		//  Prepare our context and publisher
		zmq::context_t context_pub(1);
		zmq::socket_t xpub(context_pub, ZMQ_PUB);
		sprintf(bindstr, "tcp://*:%d", sendport);
		xpub.bind(bindstr);

		sleep(2);

		fprintf(stderr, "Publisher: %d START SEND!\n\n", loop);
		fprintf(stderr, "Message=%s\n\n", message);

		char	*signature = sign_message(
			"LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm",	// privkey
			message,
			&params.PrivHelper, &params.AddrHelper);

		printf("Signature: %s\n\n", signature);

		for (int ii = 0; ii < 100000; ii++)
		{
		//	if (ii % 10 == 0)
				usleep(50);

			// send 260 bytes
			count++;
			memset(tmp, 0x00, sizeof(tmp));
			strcpy(tmp, signature);

			sprintf(data, "%s%7d %c%s%c%s%c%s", filter, count, ESC, pubkey, ESC, message, ESC, tmp);
			s_sendmore(xpub, data);

			// send 260 bytes
			count++;
//			strcpy(tmp, "123456789012345678901234567890");
//			sprintf(data, "%s%6d==========%s%s%s%s%s%s%s%s", filter, count,
//				tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp);

			s_send(xpub, data);

		//	cout << "s_send: " << data << endl;

		//	처음 문자열이 filter가 아니면 수신하지 못함
		//	sprintf(data, "1234 %s pubid=%d: s_sendmore DATA %d", filter, loop, rand());
		//	s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

		//	s_sendmore(xpub, "TEST");
		//	cout << "s_sendmore: " << "TEST" << endl;
		//	s_send(xpub, data);
		//	cout << "s_send: " << data << endl;

		}

		free(signature);

		sprintf(data, "%s CLOSE", filter);
		printf("SEND: %s\n", data);
		s_send(xpub, data);

		fprintf(stderr, "Publisher: FINISH! sendport=%d\n", sendport);

		sleep(1);
		fprintf(stderr, "Publisher: END! sendport=%d\n", sendport);

		xpub.close();
	}
	
	/***
        PQfinish(conn);
	***/

	pthread_exit(NULL);

	return 0;
}

