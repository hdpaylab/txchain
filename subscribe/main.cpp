#include <stdio.h>
#include <libpq-fe.h>

#include "subscribe.h"

int main () {

	PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
				    user=postgres \
				    password=postgres \
				    dbname=testdb");

	if (PQstatus(conn) == CONNECTION_BAD) {
		printf("db connect failed\n");
	}


	// thread 	
	subscribe(conn);

	PQfinish(conn);

	return 0;
}
