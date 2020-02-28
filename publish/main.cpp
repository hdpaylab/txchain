#include <stdio.h>
#include <libpq-fe.h>

#include "publish.h"

int main () {

	PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
				    user=postgres \
				    password=postgres \
				    dbname=testdb");

	if (PQstatus(conn) == CONNECTION_BAD) {

		fprintf(stderr, "Connection to database failed: %s\n",
				PQerrorMessage(conn));
	}

	publish(conn);

	PQfinish(conn);

	return 0;
}
