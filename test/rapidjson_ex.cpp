//
// rapidjson/example/simpledom/simpledom.cpp
//
// Performance: rawtx.json parsing speed: 150,000/sec @ubuntu i7-6700 3.4GHz
//

#include <iostream>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
 

using namespace rapidjson;
using namespace std;


char	*file_get(char *path);


int	main(int ac, char *av[])
{
	if (ac != 2)
	{
		cout << "Usage : " << av[0] << " [JSON FILE PATH]" << endl;
		return 0;
	}

	/*
	const char *json =  "{ \
	  \"publishers\":[ \
	   \"14Zs9naW81CFsHp3AwWPG3XmKPw4mwWj9WvYmA\" \
	  ], \
	  \"key\":\"HDACT\", \ 
	  \"data\":\"3836343030203130302031302e303030303030202d202d\", \
	  \"confirmations\":1162, \
	  \"blocktime\":1518496716, \
	  \"txid\":\"451cfdd2b55f67f26ebb86e234e194f08dc39785edac72a772a5d8b730bab4f9\" \
	}";
	*/

	// const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";

	printf("FILE: %s\n", av[1]);
	char *json = file_get(av[1]);

	// 1. Parse a JSON string into DOM.
//	for (int ii = 0; ii < 1000000; ii++)
	{
		Document d;
		d.Parse(json);

		// 2. Modify it by DOM.
		// Value& s = d["blocktime"];
		// s.SetInt(s.GetInt() + 1);

		Value& ss = d["vin"][0]["txid"];
		string sval = ss.GetString();
		cout << "TXID=" << sval << endl;

		Value& vv = d["vout"][1]["value"];
		double val = vv.GetDouble();
		cout << "DOUBLE=" << val << endl;
		vv.SetDouble(vv.GetDouble() + 1.0);
		val = vv.GetDouble();
		cout << "DOUBLE+1.0=" << val << endl;

		Value& vin = d["vin"][0]["scriptSig"]["hex"];
		string shex = vin.GetString();
		cout << "VIN.HEX=" << shex << endl;

		Value& pk = d["vout"][0]["scriptPubKey"]["asm"];
		string sasm = pk.GetString();
		cout << "VOUT.ASM=" << sasm << endl;
		cout << endl << endl;

		// 3. Stringify the DOM
		StringBuffer buffer;
	//	Writer<StringBuffer> writer(buffer);
		PrettyWriter<StringBuffer> writer(buffer);
		d.Accept(writer);

		// Output {"project":"rapidjson","stars":11}
		cout << "Result: " << buffer.GetString() << endl;
	}

	printf("\n\n");

	return 0;
}


char	*file_get(char *path)
{
	FILE *fp = NULL;
	static char json[1048576];
	struct   stat  file_info;
	int res = 0;
	int size;

	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("file open error\n");
		return 0;
	}

	res = stat(path, &file_info);
	if (res < 0) {
		printf("file size get error\n");
		return 0;
	}
	
	size = file_info.st_size + 1;
	res = fread(json, size, 1, fp); 
	if (res < 0) {
		printf("file read error\n");
		return 0;
	}

	if (fp != NULL) {
		fclose(fp);
	}

	// printf("read [%s]\n", json);

	return json;
}
