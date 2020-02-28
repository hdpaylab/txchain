#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

using namespace std;

string tx_get(PGconn *conn)
{
	PGresult *res;

		// tx_parsing(recv_msg.data());
/*
		res = PQexec(conn, "SELECT * FRON testdb");

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//			printf("query failed [%s]\n", query);
		} 

		PQclear(res);    
*/
	return "null";
}

