#include <iostream>
#include <libpq-fe.h>

using namespace std;

int tx_save(PGconn *conn, string tx);
