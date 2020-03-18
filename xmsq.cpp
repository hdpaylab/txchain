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

/* 제일 앞에 있는 메시지 삭제 */
/* RETURN: 성공이면 0, 실패면 -1 */
void ai_msq_clean(int key)
{
	struct msgbuf_t	*mb;
	int		msqid;
	int		len;

	msqid = ai_msq_open(key, -1);
	if (msqid < 0) {
		return;
	}

	while ((mb = ai_msq_recv(msqid, 0, 1024, &len)) != NULL) {
		free(mb);
	}
}


/* 메시지큐에 대기중인 메시지 모두 삭제 */
void ai_msq_clean_mtype(int key, int mtype)
{
	struct msgbuf_t	*mb;
	int		msqid;
	int		len;

	msqid = ai_msq_open(key, -1);
	if (msqid < 0) {
		return;
	}

	while ((mb = ai_msq_recv(msqid, mtype, 1024, &len)) != NULL) {
		free(mb);
	}
}

/* FUNCTION: 메시지 수신. 메시지가 크기에 맞추어서 버퍼 할당해서 메시지 수신 */
/* RETURN: 수신된 msg */
/* OUTPUT: len에 수신된 메시지 길이 저장. 데이터 없으면 len 0, 에러면 len -1 */
/* exptected_size에 예상되는 메시지 크기 입력 */
struct msgbuf_t *ai_msq_recv(int msqid, long mtype, int expected_size, int *len)
{
	int		nread = 0;
	int		mb_size;
	struct	msgbuf_t	*mb = NULL;

	if (msqid < 0 || expected_size <= 0) {
		*len = -1;
		return NULL;
	}

	mb_size = expected_size;

msq_recv_retry:

	mb = (struct msgbuf_t *)realloc(mb, mb_size + sizeof(long) + 1);	// 1은 '\0' 추가 고려

	nread = msgrcv(msqid, mb, mb_size, mtype, IPC_NOWAIT);
	if (nread < 0) {
		if (errno == ENOMSG || errno == EAGAIN)
			*len = 0;
		else if (errno == E2BIG) {
			/* 버퍼 크기 늘려서 재시도 */
			mb_size *= 2;
			goto msq_recv_retry;
		} else {
			*len = -1;
			/* message queue가 이상하므로 다시 open */
		}

		free(mb);
		return NULL;

	} else {
		*len = nread;
		/* data가 string이 되도록 '\0' 추가 */
		mb->mtext[nread] = '\0';
		return mb;
	}
}


/* FUNCTION: 메시지 수신. 메시지가 크기에 맞추어서 버퍼 할당해서 메시지 수신 */
/* RETURN: 수신된 msg */
/* OUTPUT: len에 수신된 메시지 길이 저장. 데이터 없으면 len 0, 에러면 len -1 */
/* exptected_size에 예상되는 메시지 크기 입력 */
struct msgbuf_t *ai_msq_recv_key(int key, long mtype, int expected_size, int *len)
{
	int		msqid;

	msqid = ai_msq_open(key, -1);
	if (msqid < 0) {
		*len = -1;
		return NULL;
	}

	return ai_msq_recv(msqid, mtype, expected_size, len);
}


/* RETURN: 성공이면 0, 실패면 -1 */
int ai_msq_send(int key, char *data1, int len1, char *data2, int len2)
{
	return ai_msq_send_type(key, 1, data1, len1, data2, len2);
}

/* RETURN: 성공이면 0, 실패면 -1 */
int ai_msq_send_type(int key, int mtype, char *data1, int len1, 
						char *data2, int len2)
{
	struct msgbuf_t *msg;
	int 	ret;
	int		msqid;

	msqid = ai_msq_open(key, -1);
	if (msqid < 0) {
		return -1;
	}

	if (data1 == NULL || len1 < 0) {
		len1 = 0;
	}
	if (data2 == NULL || len2 < 0) {
		len2 = 0;
	}

	msg = (struct msgbuf_t *)malloc(sizeof(long) + len1 + len2);
	msg->mtype = mtype;
	if (len1 > 0) {
		memcpy(msg->mtext, data1, len1);
	}
	if (len2 > 0) {
		memcpy(msg->mtext + len1, data2, len2);
	}

	ret = msgsnd(msqid, msg, len1 + len2, IPC_NOWAIT);
	free(msg);
	
	if (ret < 0) {
		return -1;
	} else {
		return 0;
	}
}

