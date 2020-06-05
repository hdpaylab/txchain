//
// Usage: tx [-cPORT] maxnode publish_port peer1:port1 [peer2:port2 ...]
//		-cPORT : Client input mode (auto data generation mode disabled)
// ex:
//	tx 4 7000 192.168.1.11:7000 192.168.1.11:7001 192.168.1.11:7002 192.168.1.11:7003
//	tx 4 7001 192.168.1.11:7000 192.168.1.11:7001 192.168.1.11:7002 192.168.1.11:7003
//	tx 4 7002 192.168.1.11:7000 192.168.1.11:7001 192.168.1.11:7002 192.168.1.11:7003
//	tx 4 7003 192.168.1.11:7000 192.168.1.11:7001 192.168.1.11:7002 192.168.1.11:7003
//

#include "txcommon.h"


int	_npeer = 0;
char	_peerlist[MAX_NODE + 1][40] = {0};


safe_queue<txdata_t>	_sendq;		// send queue for publisher
safe_queue<txdata_t>	_verifyq;	// stores received tx for verifier
safe_queue<txdata_t>	_mempoolq;	// stores verifier result
safe_queue<txdata_t>	_leveldbq;	// leveldb queue 
safe_queue<txdata_t>	_consensusq;	// verification result queue


void	init();
void	init_keypair();
void	init_block();
void	parse_command_line(int ac, char *av[]);
void	create_main_threads();
void	create_subscriber_threads();
void	create_verifier_threads(int nverifiers);


int	main(int ac, char *av[])
{
	// default values 
	_options.auto_mode = 1;
	_options.max_node = 1;
	_options.max_verifier = MAX_VERIFY_THREADS;
	_options.client_port = DEFAULT_CLIENT_PORT;	
	_options.chain_port = DEFAULT_CHAIN_PORT;

	parse_command_line(ac, av);

	printf("Start txchain main: pid=%d\n", getpid());

	init();

	create_main_threads();

	create_subscriber_threads();			// SUBSCRIBER

	create_verifier_threads(_options.max_verifier);		// VERIFIER

	while (1)
	{
		sleep(1);
		fflush(stdout);
		fflush(stderr);
	}

	return 0;
}


void	init()
{
	extern FILE *_logfp;

	_sendq.setmax(MAX_PUBLISH_QUEUE_SIZE);
	_verifyq.setmax(MAX_VERIFY_QUEUE_SIZE);
	_mempoolq.setmax(MAX_MEMPOOL_QUEUE_SIZE);
	_consensusq.setmax(MAX_CONSENSUS_QUEUE_SIZE);

	mkdir("db", 0700);
	mkdir("blocks", 0700);
	mkdir("logs", 0700);

	init_keypair();

	char	filename[256] = {0};
	sprintf(filename, "logs/debug-%d.log", _options.client_port);
	_logfp = fopen(filename, "a+b");
	assert(_logfp != NULL);

	init_block();
}


void	init_keypair()
{
	load_params_dat("../lib/params.dat");
}


void	init_block()
{
	char	path[256];
	struct stat st;

	snprintf(path, sizeof(path), "blocks/block-%06d-%d.dat", _last_block_file_no, _options.client_port);
	int ret = stat(path, &st);
	if (ret < 0)
	{
		_keypair = create_keypair();

		if (make_genesis_block(path) < 0)
		{
			logprintf(0, "Genesis block creation failed!\n");
			exit(-1);
		}
	}
	else if (st.st_size < GENESIS_BLOCK_SIZE)
	{
		logprintf(0, "Wrong size genesis block!\n");
		exit(-1);
	}
	else
	{
		_keypair = load_genesis_block(path);
	}

	if (_options.block_check)
	{
		check_blocks();
	}
}


//
// Parsing command line parameters
//
// Usage: tx maxnode publish_port peer1:port1 [peer2:port2 ...]
//
void	parse_command_line(int ac, char *av[])
{
	int	ii = 0;

	if (ac >= 2 && strncmp(av[1], "-c", 2) == 0)
	{
		_options.auto_mode = 0;
		_options.client_port = atoi(&av[1][2]);
		if (_options.client_port <= 0)
			_options.client_port = DEFAULT_CLIENT_PORT;
		ac--, av++;
	}
	printf("Auto mode = %d\n", _options.auto_mode);
	printf("Client port = %d\n", _options.client_port);

	if (ac >= 2 && strcmp(av[1], "--block_check") == 0)
	{
		if (av[1][13] == '=' && isdigit(av[1][14]))
			_options.block_check = atoi(&av[1][14]);
		else
			_options.block_check = 1;
		ac--, av++;
	}
	printf("Block check = %d\n", _options.block_check);

	// get maxnode 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_options.max_node = atoi(av[1]);
		if (_options.max_node > 100)
			_options.max_node = 100;	// ?ִ? node?? 100????..
		ac--, av++;
	}
	printf("Max node = %d\n", _options.max_node);


	// get chain port
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_options.chain_port = atoi(av[1]);
		if (_options.chain_port <= 10 || _options.chain_port > 65535)
		{
			fprintf(stderr, "ERROR: port range error (1 ~ 65535)\n");
			exit(-1);
		}
		ac--, av++;
	}
	printf("Chain port = %d\n", _options.chain_port);


	// get peer list
	for (ii = 1; ii < ac && _npeer < MAX_NODE; ii++)
	{
		if ((uint32_t)_npeer > _options.max_node)
		{
			fprintf(stderr, "WARNING: Number of peer %d > Max node %d\n", _npeer, _options.max_node);
			continue;
		}

		strcpy(_peerlist[_npeer], av[ii]);
		printf("peer[%d] = %s\n", _npeer, _peerlist[_npeer]);
		_npeer++;
	}
	printf("\n\n");
}


void	create_main_threads()
{
	pthread_t thrid;
	int	ret = 0;

	// Send thread
	printf("Create [publisher] thread: sendport=%d\n", _options.chain_port);
	ret = pthread_create(&thrid, NULL, thread_publisher, (void *)&_options.chain_port);
	if (ret < 0)
	{
		perror("thread_publisher() thread creation error");
		exit(-1);
	}
	pthread_detach(thrid);
	sleepms(1);

	// Send test thread
	if (_options.auto_mode)
	{
		printf("Create [send test] thread: auto mode=%d\n", _options.auto_mode);
		ret = pthread_create(&thrid, NULL, thread_send_test, (void *)&_options.chain_port);
		if (ret < 0)
		{
			perror("thread_send_test() thread creation error");
			exit(-1);
		}
		pthread_detach(thrid);
		sleepms(1);
	}

	// block sync thread
	printf("Create [block sync] thread\n");
	ret = pthread_create(&thrid, NULL, thread_txid_info, (void *)&_options.chain_port);
	if (ret < 0)
	{
		perror("thread_txid_info() thread creation error");
		exit(-1);
	}
	pthread_detach(thrid);
	sleepms(1);

	// level db thread
	printf("Create [leveldb] thread\n");
	ret = pthread_create(&thrid, NULL, thread_levledb, (void *)&_options.chain_port);
	if (ret < 0)
	{
		perror("thread_levledb() thread creation error");
		exit(-1);
	}
	pthread_detach(thrid);
	sleepms(1);
}

void	create_subscriber_threads()
{
	pthread_t cthrid, thrid[100];
	int	ret = 0;

	// Client thread
	if (_options.auto_mode == 0)
	{
		printf("Create [client] thread\n");
		ret = pthread_create(&cthrid, NULL, thread_client, (void *)&_options.client_port);
		if (ret < 0)
		{
			perror("thread_client() thread creation error");
			exit(-1);
		}
		pthread_detach(cthrid);
		sleepms(1);
	}

	// Subscriber threads
	for (int idx = 0; idx < _npeer; idx ++)
	{
		char	*peer = _peerlist[idx];

		char	*tp = strchr(peer, ':');
		if (tp == NULL)
		{
			fprintf(stderr, "ERROR: peer format is IP:PORT. %s skipped\n", peer);
			exit(-1);
		}

		// skip subscriber on self node
		if (_options.max_node > 1 && atoi(tp+1) == (int)_options.chain_port)
		{
			printf("Peer %s skipped.\n", peer);
			printf("\n");
			continue;
		}

		printf("Create subscriber thread [%d]=%s\n", idx, peer);

		ret = pthread_create(&thrid[idx], NULL, thread_subscriber, (void *)_peerlist[idx]);
		if (ret < 0)
		{
			perror("thread_subscriber() thread creation error");
			exit(-1);
		}
		pthread_detach(thrid[idx]);
		sleepms(1);
	}
}


void	create_verifier_threads(int nverifiers)
{
	pthread_t thrid[100], cthrid;

	// consensus thread
	printf("Create [consensus] thread\n");
	int ret = pthread_create(&cthrid, NULL, thread_consensus, (void *)&_options.chain_port);
	if (ret < 0)
	{
		perror("thread_consensus() thread creation error");
		exit(-1);
	}
	pthread_detach(cthrid);
	sleepms(1);

	// Verifier thread creation
	for (int idx = 0; idx < nverifiers; idx++)
	{
		int id = idx;
	        int ret = pthread_create(&thrid[idx], NULL, thread_verifier, (void *)&id);
                if (ret < 0) {
			perror("thread_verifier() thread creation error");
                        exit(-1);
                }
		pthread_detach(thrid[idx]);
                sleepms(1);
	}
}
