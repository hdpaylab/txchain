#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "xmsq.h"

int ai_msq_open(int key, int queue_size)
{
    int     msqid = -1;
    struct msqid_ds buf;

    if ((msqid = msgget(key, IPC_CREAT|0666)) < 0)
        return -1;

    /* queue size 재조정 */
    if (queue_size > 0 && msgctl(msqid, IPC_STAT, &buf) == 0) {
        if (buf.msg_qbytes < queue_size) {
            buf.msg_qbytes = queue_size;
            msgctl(msqid, IPC_SET, &buf);
	}
    }

    return msqid;
}

