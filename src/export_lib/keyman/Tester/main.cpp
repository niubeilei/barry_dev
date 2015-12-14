#include "aosApi.h"
#include <stdio.h>
#include <string.h>

int main()
{
	char* name = "test1";
	char* key = "key1";
	
	char data[11] = "aaaaaaaaaa";
	char encryptdata[11];

	int datalen = strlen(data);

	int ret = aos_keymagmt_add_key(name, key);
	printf("ret = %d\n", ret);

	//ret = aos_keymagmt_remove_key(name);
	//printf("ret = %d\n", ret);

	printf("data = %s\n", data);

	ret = aos_keymagmt_encrypt(data, datalen, encryptdata, 
		AOS_KEYMGMT_ENC_ALGO_3DES, name);
	printf("ret = %d\n", ret);

	ret = aos_keymagmt_decrypt(encryptdata, datalen, data, 
		AOS_KEYMGMT_ENC_ALGO_3DES, name);
	printf("ret = %d, data = %s\n", ret, data);

	ret = aos_keymagmt_encrypt(data, datalen, encryptdata, 
		AOS_KEYMGMT_ENC_ALGO_AES, name);
	printf("ret = %d\n", ret);

	ret = aos_keymagmt_decrypt(encryptdata, datalen, data, 
		AOS_KEYMGMT_ENC_ALGO_AES, name);
	printf("ret = %d, data = %s\n", ret, data);

	return 0;
}

