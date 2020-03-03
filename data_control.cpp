#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

using namespace std;

int tx_save(PGconn *conn, string tx)
{
	PGresult *res;

		// tx_parsing(recv_msg.data());
/*
		res = PQexec(conn, "INSERT INTO testdb \
					VALUES(1,'Audi',52642)");

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//			printf("query failed [%s]\n", query);
		} 

		PQclear(res);    
*/
	return 0;
}

