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

    /* queue size ������ */
    if (queue_size > 0 && msgctl(msqid, IPC_STAT, &buf) == 0) {
        if (buf.msg_qbytes < queue_size) {
            buf.msg_qbytes = queue_size;
            msgctl(msqid, IPC_SET, &buf);
		}
    }

    return msqid;
}

/* ���� �տ� �ִ� �޽��� ���� */
/* RETURN: �����̸� 0, ���и� -1 */
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


/* �޽���ť�� ������� �޽��� ��� ���� */
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

/* FUNCTION: �޽��� ����. �޽����� ũ�⿡ ���߾ ���� �Ҵ��ؼ� �޽��� ���� */
/* RETURN: ���ŵ� msg */
/* OUTPUT: len�� ���ŵ� �޽��� ���� ����. ������ ������ len 0, ������ len -1 */
/* exptected_size�� ����Ǵ� �޽��� ũ�� �Է� */
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

	mb = (struct msgbuf_t *)realloc(mb, mb_size + sizeof(long) + 1);	// 1�� '\0' �߰� ���

	nread = msgrcv(msqid, mb, mb_size, mtype, IPC_NOWAIT);
	if (nread < 0) {
		if (errno == ENOMSG || errno == EAGAIN)
			*len = 0;
		else if (errno == E2BIG) {
			/* ���� ũ�� �÷��� ��õ� */
			mb_size *= 2;
			goto msq_recv_retry;
		} else {
			*len = -1;
			/* message queue�� �̻��ϹǷ� �ٽ� open */
		}

		free(mb);
		return NULL;

	} else {
		*len = nread;
		/* data�� string�� �ǵ��� '\0' �߰� */
		mb->mtext[nread] = '\0';
		return mb;
	}
}


/* FUNCTION: �޽��� ����. �޽����� ũ�⿡ ���߾ ���� �Ҵ��ؼ� �޽��� ���� */
/* RETURN: ���ŵ� msg */
/* OUTPUT: len�� ���ŵ� �޽��� ���� ����. ������ ������ len 0, ������ len -1 */
/* exptected_size�� ����Ǵ� �޽��� ũ�� �Է� */
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


/* RETURN: �����̸� 0, ���и� -1 */
int ai_msq_send(int key, char *data1, int len1, char *data2, int len2)
{
	return ai_msq_send_type(key, 1, data1, len1, data2, len2);
}

/* RETURN: �����̸� 0, ���и� -1 */
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

