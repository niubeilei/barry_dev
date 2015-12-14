#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s <lines> <1|2>\n", argv[0]);
		printf("       1 - create stat table\n");
		printf("       2 - create join table\n");
		return -1;
	}
	long long n = atoll(argv[1]);
	long long num;
	int table = atoi(argv[2]);
	//printf("%lld \n", n);

	char buff[2048];
	buff[0] = 0;
	char* data = buff;
	const char* f1 = "\"%d\",";
	const char* prefix1 = "f";
	const char* prefix2 = "col";
	char *prefix;

	if (table == 1)
		prefix = (char *)prefix1;
	else
		prefix = (char *)prefix2;

	for (int i = 0; i < 3; i++) {
		data = strcat(data, f1);
	}

	for (int i = 4; i <= 10; i++) {
		sprintf(data, "%s\"%s%d_%%d\"", data, prefix, i);
		if (i == 10)
			data = strcat(data, "\n");
		else
			data = strcat(data, ",");
	}

	//printf("%s\n", buff);
	for (long long i=0; i<n; i++) {
		//printf("key_field11_%-8dkey_field21_%-8dkey_field22_%-8dkey_field31_%-8dkey_field32_%-8dkey_field33_%-8d%-5d%-8d\n", 
		if (table == 1)
			num = rand()%20;
		else
			num = i;

		printf(buff, 
				num,
				rand()%100,
				rand()%50,
				rand()%1000,
				rand()%1000,
				rand()%1000,
				rand()%1000,
				rand()%1000,
				rand()%1000,
				rand()%1000
				);
	}
	return 0;
}
