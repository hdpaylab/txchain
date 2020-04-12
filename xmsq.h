#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFF_SIZE (1 * 1024 * 1024)

typedef struct {
	long    mtype;
	char    mtext[BUFF_SIZE];
} data_t;

int ai_msq_open(int key, int queue_size);


