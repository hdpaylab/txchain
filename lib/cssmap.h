#ifndef __CSSMAP_H
#define __CSSMAP_H


#include <string.h>
#include <assert.h>
#include <string>
#include <map>


using namespace std;


typedef struct {
	int		num;
	uint32_t	nodeid[64];
	int		valid[64];		// 1=valid 0=invalid
}	noderesp_t;


class cssmap {
public:
	cssmap()
	{
	}
	~cssmap()
	{
	}

	int add(string txid, uint32_t nodeid, int valid)
	{
		noderesp_t list = nodemap[txid];

		for (int ii = 0; ii < (int)list.num; ii++)
		{
			if (list.nodeid[ii] == nodeid)
			{
				// updated
				list.valid[ii] = valid;
				nodemap[txid] = list;
				printf("UPDATE %d %d\n", nodeid, valid);

				return 1;
			}
		}

		// newly added
		list.nodeid[list.num] = nodeid;
		list.valid[list.num] = valid;
		list.num++;
		nodemap[txid] = list;
		printf("NEW %d %d\n", nodeid, valid);

		return 1;
	}

	void remove(string txid)
	{
		nodemap.erase(txid);
	}

	size_t size()
	{
		return nodemap.size();
	}

	int totalcount(string txid)
	{
		noderesp_t list = nodemap[txid];

		return list.num;
	}
	
	int validcount(string txid)
	{
		int count = 0;
		noderesp_t list = nodemap[txid];

		for (int ii = 0; ii < (int)list.num; ii++)
		{
			if (list.valid[ii] == 1)
				count++;
		}
		return count;
	}

	int invalidcount(string txid)
	{
		int count = 0;
		noderesp_t list = nodemap[txid];

		for (int ii = 0; ii < (int)list.num; ii++)
		{
			if (list.valid[ii] == 0)
				count++;
		}
		return count;
	}

	double ratio(string txid)
	{
		int total = totalcount(txid);

		if (total <= 0)
			return 0.0;
		else
			return (double)validcount(txid) / (double)total;
	}

private:

	map<string, noderesp_t> nodemap;
};


#endif	// __CSSMAP_H
