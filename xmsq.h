
#ifndef _AI_MSQ_H_
#define _AI_MSQ_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


struct msgbuf_t {
	long    mtype;
	char    mtext[1];
};

extern int ai_msq_open(int key, int queue_size);
extern struct msgbuf_t *ai_msq_recv(int msqid, long mtype, int expected_size, int *len);
extern struct msgbuf_t *ai_msq_recv_key(int key, long mtype, int expected_size, int *len);
extern int ai_msq_send(int key, char *data1, int len1, char *data2, int len2);
extern int ai_msq_send_type(int key, int mtype, char *data1, int len1, char *data2, int len2);
extern void ai_msq_clean(int key);
extern void ai_msq_clean_mtype(int key, int mtype);

#endif

