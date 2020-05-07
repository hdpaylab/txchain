// rapidjson/example/simpledom/simpledom.cpp
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
 

using namespace rapidjson;
using namespace std;

char *file_get(char *path);

int main(int ac, char *av[]) {

	if (ac != 2) {
		cout << "Usage : " << av[0] << 
				" [JSON FILE PATH]" << endl;
		return 0;
	}

	// ==== test data ====

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

	char *json = file_get(av[1]);

	// 1. Parse a JSON string into DOM.
	Document d;
	d.Parse(json);

	// 2. Modify it by DOM.
	// Value& s = d["blocktime"];
	// s.SetInt(s.GetInt() + 1);

	// 3. Stringify the DOM
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	// Output {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;
/*
	if (buffer.GetString() == 0) {
		printf("[%s] error\n", av[1]);
	} else {
		printf("[%s] OK\n", av[1]);
	}
*/
	return 0;
}

char *file_get(char *path)
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
