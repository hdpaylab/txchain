#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct A {
	int a;
	char *str;
} test_struct_t;

char *serialize(test_struct_t t) {
	int str_len = strlen(t.str);

	int size = 2 * sizeof(int) + str_len;
	char *buf = (char*)malloc(sizeof(char) * (size+1));

	memcpy(buf, &t.a, sizeof(int));
	memcpy(buf + sizeof(int), &str_len, sizeof(int));
	memcpy(buf + sizeof(int) * 2, t.str, str_len);
	buf[size] = '\0';

	return buf;
}

test_struct_t deserialize(char *buf) {
	test_struct_t t;

	memcpy(&t.a, buf, sizeof(int));

	int str_len;
	memcpy(&str_len, buf+sizeof(int), sizeof(int));

	t.str = (char*)malloc(sizeof(char) * (str_len+1));
	memcpy(t.str, buf+2*sizeof(int), str_len);
	t.str[str_len] = '\0';

	return t;
}

int main() {
	char str[15] = "Hello, world!";

	test_struct_t t;
	t.a = 123;
	t.str = (char*)malloc(strlen(str) + 1);
	strcpy(t.str, str);
	printf("original values: %d %s\n", t.a, t.str);

	char *buf = serialize(t);
	printf("buf[%s]\n", buf);
	test_struct_t new_t = deserialize(buf);
	printf("new values: %d %s\n", new_t.a, new_t.str);

	return 0;
}
