//
// Usage: tx maxnode publish_port peer1:port1 [peer2:port2 ...]
// ex:
//	tx 4 7000 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7001 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7002 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7003 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "xsub.h"
#include "xpub.h"


#define MAX_NODE	100


int	maxnode = 1;		// ���߿� �������� �� �� 
int	sendport = 7000;
int	npeer = 0;
char	peers[MAX_NODE][40] = {0};


void	load_config(int ac, char *av[]);


int	main(int ac, char *av[])
{
	
	pthread_t thrid[5];
	int	ret = 0;
	int	ii = 0;


	load_config(ac, av);


	// �߽��� thread
	printf("Create publisher sendport=%d\n", sendport);
	ret = pthread_create(&thrid[0], NULL, thread_publisher, (void *)&sendport);
	if (ret < 0)
	{
		perror("thread create error : ");
		return 0;
	}
	usleep(100 * 1000);


	// ������ 
	for (ii = 1; ii <= npeer; ii ++)
	{
		char	*peer = peers[ii-1];

		printf("Create subscriber [%d]=%s\n", ii - 1, peer);

		char	*tp = strchr(peer, ':');
		if (tp == NULL)
		{
			fprintf(stderr, "ERROR: peer format is IP:PORT. %s skipped\n", av[ii]);
			exit(-1);
		}

		// �ټ��� ���� �׽�Ʈ�� ���� �ڽ��� �ڽ��� ���μ������� �߼� ��û�� ���� ����.
		if (maxnode > 1 && atoi(tp+1) == sendport)
		{
			printf("Peer %s skipped.\n", peer);
			printf("\n");
			continue;
		}

		ret = pthread_create(&thrid[ii], NULL, thread_subscriber, (void *)peers[ii-1]);
		if (ret < 0)
		{
			perror("thread create error : ");
			return 0;
		}
		usleep(10 * 1000);

	}

	for (ii = 0; ii <= npeer; ii++) {
		pthread_detach(thrid[ii]);
	}

	while (1)
	{
		sleep(1);
		fflush(stdout);
	}

	return 0;
}


void	load_config(int ac, char *av[])
{
	int	ii = 0;

	// �ִ� ��� ���� ���� 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		maxnode = atoi(av[1]);
		if (maxnode > 100)
			maxnode = 100;	// �ִ� node�� 100����..
		ac--, av++;
	}
	printf("Max node = %d\n", maxnode);


	// �߼� ��Ʈ ���� 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		sendport = atoi(av[1]);
		if (sendport > 65535)
		{
			fprintf(stderr, "ERROR: port range error (1 ~ 65535)\n");
			exit(-1);
		}
		ac--, av++;
	}
	printf("Send port = %d\n", sendport);


	// ������ peer ���� (�׽�Ʈ ���� ���̳����ϰ� �ٲ��� �ʰ� ��������..)
	for (ii = 1; ii < ac && npeer < MAX_NODE; ii++)
	{
		if (npeer > maxnode)
		{
			fprintf(stderr, "WARNING: Number of peer %d > Max node %d\n", npeer, maxnode);
			continue;
		}

		strcpy(peers[npeer], av[ii]);
		printf("peer[%d] = %s\n", npeer, peers[npeer]);
		npeer++;
	}
	printf("\n\n");
}


#include <stdint.h>
#include <keys/hs_keys_wrapper.h>
#include <stdio.h>
#include <stdlib.h>

void	test_create_stream_publish_tx(void)
{
	struct PrivateKeyHelpInfo privinfo = {
		"8075fa23", "cb507245"
	};

	char * rawTx = create_stream_publish_tx("key1", "tested by moony",
			"a0b59e8c6f2fd144485d19632f62708f88116fb11a46411dd7d1e211ec92ce9a",
			"a9143e45d3a48882576ad5900978303705e1a6000305871473706b6511feed9499be6fb101e0f59119d3fe15751473706b700800000000000000fffffffffbfe095c75",
			"db84077722b74c9c9a799cf58d6c7a265f214f003b5ef15cae368a8add8d33f8", 0,
			"5221027e75736b41474547b7e2443d7235f4030cbb378093bbd2e98ea36ded6d703c2b21038d7724f227aab828d771eb6ab697f333e615d39b585944d99737ce7b7ae650fd52ae",
			"VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp", &privinfo);

	printf("raw-Tx: %s\n", rawTx);
	free(rawTx);
}

void	test_create_key_pairs(void)
{
	struct PrivateKeyHelpInfo privinfo = {
		"8075fa23", "cb507245"
	};

	struct WalletAddrHelpInfo addrinfo = {
		"003fd61c", "0571a3e6", "cb507245"
	};

	keypairs_type_t *keypairs = create_key_pairs(&privinfo, &addrinfo);

	printf("address : %s\n", keypairs->walletaddr);
	printf("pubkeyhash : %s\n", keypairs->pubkeyhash);
	printf("pubkey : %s\n", keypairs->pubkey);
	printf("privatekey : %s\n", keypairs->privatekey);
	free(keypairs);
}

void	test_create_asset_send_tx(void)
{
	struct PrivateKeyHelpInfo privinfo = {
		"8075fa23", "cb507245"
	};

	struct WalletAddrHelpInfo addrinfo = {
		"003fd61c", "0571a3e6", "cb507245"
	};

	char * rawassetTx = create_asset_send_tx("1WCRNaPb3jAjb4GE9t34uLiLtPseA8JKEvdtg5", 10,
			"44fdb8103f4e13d6ef2011d54933f2747b455c613b3cfe4886d187330d50b640", 10,
			"76a9143ab53060d41b5fa662a2d4575a69464b5759839588ac1c73706b7174f23349d51120efd6134e3f10b8fd44ac2600000000000075",
			"030374d736a70c5faf5d16887d2263e812cb896938bedeefd44c128417e2460a", 1,
			990.0,
			"",
			"VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp",
			&privinfo, &addrinfo);

	printf("raw-asset-Tx: %s\n", rawassetTx);
	free(rawassetTx);
}

void	test_sign_message(void)
{
	struct PrivateKeyHelpInfo privinfo = {
		"8075fa23", "cb507245"
	};

	struct WalletAddrHelpInfo addrinfo = {
		"003fd61c", "0571a3e6", "cb507245"
	};

	char * signMessage = sign_message(
			"VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp",
			"Hdac Technology, Solution Dev Team, Test Text.",
			&privinfo, &addrinfo);

	printf("sign-Message: %s\n", signMessage);
	free(signMessage);
}

void	test_verify_message(void)
{
	struct WalletAddrHelpInfo addrinfo = {
		"003fd61c", "0571a3e6", "cb507245"
	};

	int verify_check = verify_message(
			"18wD7MBodeTYRAvN5bRuWYB11jwHdkGVCBLSnB",
			"IJKPyPUFEgnlrcixdqbfgAks89Gi29uzGAyMUYICz8VAWEs6VlOpjzregZ2WrcarZoNtXD7aLC2S6VWJ8XowH9c=",
			"Hdac Technology, Solution Dev Team, Test Text.",
			&addrinfo);

	printf("verify-Message: %s\n", verify_check? "true" : "false" );
}

int	main2()
{
	test_create_stream_publish_tx();

	test_create_key_pairs();

	test_create_asset_send_tx();
	
	test_sign_message();

	test_verify_message();

	return 0;
}
