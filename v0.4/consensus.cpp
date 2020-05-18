#include "txcommon.h"


cssmap	_cssmap;


int	update_consensus(txdata_t& txdata);


void	*thread_consensus(void *info_p)
{
	int	chainport = *(int *)info_p;
	int	count = 0;

	while (1)
	{
		txdata_t txdata;

		count++;
		txdata = _consensusq.pop();

		printf("\n-----Consenssus:\n");

		update_consensus(txdata);

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("    Consensus processed %d  consensusq=%ld\n", count, _consensusq.size());
	}

	fprintf(stderr, "\nConsensus END!\n");

	pthread_exit(NULL);

	return NULL;
}


int	update_consensus(txdata_t& txdata)
{
	tx_header_t *hp;

	hp = &txdata.hdr;


	if (hp->type == TX_VERIFY_REPLY)
	{
		_cssmap.add(hp->txid, hp->nodeid, hp->valid);
		printf("    Consensus: add %s nodeid=%d valid=%d\n", 
			hp->txid.c_str(), hp->nodeid, hp->valid);
		printf("        map size=%ld ratio=%.3f\n", _cssmap.size(), _cssmap.ratio(hp->txid));
	}

	return 1;
}

