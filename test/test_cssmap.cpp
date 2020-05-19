#include "cssmap.h"


cssmap _cssmap;


int main(void)
{
	int	count = 0;

	for (int tt = 0; tt < 10; tt++)
	{
		char txid[100];
		sprintf(txid, "%d TXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXID", tt + 1);

		for (int nn = 0; nn < 50; nn++)
		{
			int nodeid = nn + 1;

		//	for (int vv = 0; vv < 2; vv++)
			{
				int valid = tt * nn % 2 == 0 ? 1 : 0;

				count++;
				_cssmap.add(txid, nodeid, valid);
				printf("%d: ADD: %s nodeid=%d valid=%d\n", 
					count, txid, nodeid, valid);
			}
		}
	}

	for (int tt = 0; tt < 10; tt++)
	{
		char txid[100];
		sprintf(txid, "%d TXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXIDTXID", tt + 1);

		printf("\ntxid=%s\n", txid);
		printf("total=%d\n", _cssmap.totalcount(txid));
		printf("valid=%d\n", _cssmap.validcount(txid));
		printf("invalid=%d\n", _cssmap.invalidcount(txid));
		printf("ratio=%.3f\n", _cssmap.ratio(txid));
	}
}
