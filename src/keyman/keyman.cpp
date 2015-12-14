////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: keyman.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "keyman.h"

#include "aosUtil/Alarm.h"
#include "Base64/Base64.h"
#include "Debug/Debug.h"

#include <time.h>
#include <string.h>
#include <iostream>
#include <Util/File.h>

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/rc4.h>
#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

#define AOS_KEYMGMT_KEYDATA_LEN 1024

#define KEY_FILE_NAME "/usr/local/rhc/config/keys.test"

#define AOS_KEYMGMT_KEYLEN 32

static unsigned char keydat[AOS_KEYMGMT_KEYDATA_LEN] = 
 "FA37jNCchRYdSBZAYY4CbwdXs22jJZHmAIrRv41MTZXLyVkwRo72Tk0lk0E1Zloz2NoxPpiHmfsV8Kp07u20O0j90Xa0gwxiiWO7j4OGhfzqXWq2rq0drjkretpvZXLfRxl8B7MTklhigHiH5jUwCfWRGjkDe3T5BecalY3EkiWyXef3v7z8ktX0NfE1ix5TJh22f5GxoA5lNimiqUoInjJ8yl8RQdIzsKzFOedaMgtxxQQVKcD5wke2FmTvxA2ZkAE6OPgy4I6ByUug67lAsxD5TuBo4Bnpb0tXNKwRsAqYuUeA0x9q5MvWg4jlGEIPEbLqTffjPGhjytRwZ0W2KqW1ufk8U2YwbJW3YcmLQC4m3TT2UN4CdZDIeVQ8VMF7tzapJuaz6cU86NaYydAMccsq7gx33a8wHgWrK40PhUXnF5jeiTYs3qRbGocHwkcdA6EiaD7rv4Mba5nqXjT0JK9n6l5CDfNdlswv3BVyGHHOL4eGn5E4POsT7vvLKjW5JsyM4tkI92wS4IyqOcsBYSv6nYP5fL8WcFIe81Uf1o736DrSOUsMMXQ8TFb7pk3zZMN5LHlM5qN92f2Q9uATpr1g4dnrvr0sbLxWqQGtfrDhGD5N5FFs4GI9R6ykvzMGiiAJ8eclFNCkqH5tlKVppCyeG5zbEjPUBnDIzQ3cBEw2jCtEkp4J1AYFFxQhQDcq0N8xB9JdNef7OIJ85LS4mQJZlxf9apz8cFFOPp1ADeFrUpx0ap2udMswhHGs6dAiRe4EB5eejUDchacpycULWmie1WG57enYqsC1vPfNJQZ019pzlhiiBywAsaFAm3wNt7OYW2JDSIBRRYodeHtPfZqI73gt6KgxR5uL7doXWXPLWdYaUrX8olOvp2ZtMdZEitnnEKlyH8kBliJeIHk408AnaxRWKOy1hWmTGFslNMX72GlIlFNkLlHTSwPAkmzzgWtUBVfnHauHPPqasdsbya4oETWZdwytqZnZUBmzLQhADFJ3SbeojiKYaEVnatP";

static void _setkey(unsigned char* key)
{
	key[0] = keydat[19];
	key[1] = keydat[65];
	key[2] = keydat[234];
	key[3] = keydat[872];
	key[4] = keydat[120];
	key[5] = keydat[499];
	key[6] = keydat[1022];
	key[7] = keydat[209];
	key[8] = keydat[632];
	key[9] = keydat[923];
	key[10] = keydat[72];
	key[11] = keydat[236];
	key[12] = keydat[644];
	key[13] = keydat[701];
	key[14] = keydat[823];
	key[15] = keydat[390];
	key[16] = 0;
}

static AES_KEY k_aes;
static int AES_num = 0;

static DES_key_schedule sch_des;
static DES_cblock k_des;
static DES_cblock i_des;

static RC4_KEY k_rc4;

#ifdef __DEBUG__
static void print_hex(unsigned char* buf, int len) 
{
	int i;
	for (i = 0; i < len; i++) 
	{
		printf("0x%x ", buf[i]);
	}
}
#endif

static int _key_exist(char* kname)
{
	OmnString line, tmp = kname;
	bool finished = false;

	OmnFile keyfile(KEY_FILE_NAME, OmnFile::eReadOnly);
	if(keyfile.isGood())
	{
		tmp += " ";
		while(!finished)
		{
			line = keyfile.getLine(finished);
			if (!strncmp(line, tmp, tmp.length()))
			{
				keyfile.closeFile();
				return 1;
			}
		}
		keyfile.closeFile();
	}
	else {
		OmnFile keyfile(KEY_FILE_NAME, OmnFile::eCreate);	
		keyfile.closeFile();
		return -1;
	}

	return 0;
}

int aos_keymagmt_add_key(char* keyname, char* key)
{
	int ret = _key_exist(keyname);
	
	if (ret == 1)
	{
		OmnTrace << "Key exists: " << keyname << endl;
		return -eAosRc_KeyExist;
	}
	else if (ret == -1)
	{
		OmnTrace << "Create a key file" << endl;
	}

	OmnFile keyfile(KEY_FILE_NAME, OmnFile::eAppend);
	if(!keyfile.isGood())
	{
		OmnTrace << "Can't open key file" << endl;
		return -eAosRc_KeyFile_NotExist; 
	}
	
	char ct[100];
	unsigned char mykey[17];
	_setkey(mykey);

	AES_set_encrypt_key(mykey, 16*8, &k_aes);
   	AES_num = 0;
	unsigned char iv1[16+1] = "1023456789abcdef"; // 16+1	for encryption
	AES_cfb128_encrypt((unsigned char*)key, (unsigned char*)ct,
		strlen(key), &k_aes, (unsigned char*)iv1, &AES_num, AES_ENCRYPT);	

	char ct_Base64[48]; 
	int len = EncodeBase64((unsigned const char*)ct, ct_Base64, strlen(key), 100);
	ct_Base64[len] = 0;

	OmnString key_adder = keyname;
	key_adder += " ";
	key_adder += ct_Base64;
	key_adder += "\n";
		
	if(!keyfile.append(key_adder))
	{
		OmnTrace << "Append key failure: " << key_adder << endl;

		keyfile.closeFile();
		return -eAosRc_KeyFile_RWErr;
	}

	keyfile.closeFile();
	return eAosRc_OK;
}

int aos_keymagmt_get_key(char* keyname, char* key, unsigned int buflen)
{	
	OmnString line;
	OmnString tmp = keyname;

	OmnFile keyfile(KEY_FILE_NAME, OmnFile::eReadOnly);
	if(keyfile.isGood() == false)
	{
		return -eAosRc_KeyFile_NotExist;
	}

	bool finished = false;
	tmp += " ";
	while(!finished)
	{
		line = keyfile.getLine(finished);
		if (!strncmp(line, tmp, tmp.length()))
		{
			char* blank = strchr(line,' ');
			if(blank == NULL)
			{
				return -eAosRc_KeyFile_FormatErr;
			}
			blank += 1;

			char ct_Base64[48];
			char ct[100];
			strcpy(ct_Base64, blank);

			int len = DecodeBase64(ct_Base64, (unsigned char*)ct, strlen(ct_Base64));
			if((u32)(len+1) > buflen)
			{
				keyfile.closeFile();
				return -eAosRc_KeyBuf_Overflow;
			}

			unsigned char mykey[17];
			_setkey(mykey);

			AES_set_encrypt_key(mykey, 16*8, &k_aes);
			unsigned char iv2[16+1] = {"1023456789abcdef"}; // 16+1 for decryption
			AES_num=0;
			AES_cfb128_encrypt((unsigned char*)ct, (unsigned char*)key,
				len, &k_aes, (unsigned char*)iv2, &AES_num, AES_DECRYPT);
	
			key[len] = 0;  

			keyfile.closeFile();
			return eAosRc_OK;
		}
	}
	keyfile.closeFile();
	return -eAosRc_KeyNotExist;
}

int aos_keymagmt_remove_key(char* keyname)
{
 	OmnFile keyfile(KEY_FILE_NAME, OmnFile::eReadOnly);
	if(!keyfile.isGood())
	{
		return -eAosRc_KeyFile_NotExist;
	}
	
	rewind(keyfile.getFile());

	OmnString bufLine;
	bool finished = false;
	u32 pos;

	OmnString keyname_copy = keyname;
	keyname_copy += " ";
	while(!finished)
	{
		pos = ftell(keyfile.getFile());
		bufLine = keyfile.getLine(finished);
		if (!strncmp(bufLine, keyname_copy, keyname_copy.length()))
		{
			keyfile.removeLine(pos);
			keyfile.closeFile();
			return eAosRc_OK;
		}
	}
	keyfile.closeFile();
	return -eAosRc_KeyNotExist;
}

int aos_keymagmt_encrypt(char *origdata, unsigned int datalen, char *encdata, unsigned char algo, char *keyname)
{		
	char key[AOS_KEYMGMT_KEYLEN+1];

	if(aos_keymagmt_get_key(keyname, key, AOS_KEYMGMT_KEYLEN+1) < 0)
	{
		OmnTrace << "Can't get the key" << endl;
		return -eAosRc_KeyNotExist;
	}

	switch (algo)
	{
	case AOS_KEYMGMT_ENC_ALGO_AES:
	{
		for(int i = strlen(key); i < AOS_KEYMGMT_KEYLEN; i++)
		{
			key[i] = ' ';
		}
		key[AOS_KEYMGMT_KEYLEN] = 0;
		
		AES_set_encrypt_key((unsigned char*)key, 32*8, &k_aes);
   		AES_num = 0;
		unsigned char iv1_aes[16+1] = "1023456789abcdef"; // 16+1	for encryption
		AES_cfb128_encrypt((unsigned char*)origdata, (unsigned char*)encdata,
			datalen, &k_aes, (unsigned char*)iv1_aes, &AES_num, AES_ENCRYPT);

		return eAosRc_OK;
	}

	case AOS_KEYMGMT_ENC_ALGO_DES:
	{
		for(int i = strlen(key); i < 9; i++)
		{
			key[i] = ' ';
		}
		key[9] = 0;

		unsigned char iv1_des[8] = "abcdefg";
		
		memcpy(k_des, key, 8);
		memcpy(i_des, iv1_des, 8);
		
		DES_set_key_unchecked(&k_des, &sch_des);
		DES_cfb_encrypt((unsigned char*)origdata, (unsigned char*)encdata, 8,
			datalen,&sch_des, &i_des,  DES_ENCRYPT);
		
		memcpy(k_des, key, 8);
		memcpy(i_des, iv1_des, 8);
		DES_set_key_unchecked(&k_des, &sch_des);
		DES_cfb_encrypt((unsigned char*)encdata, (unsigned char*)origdata, 8,
			datalen, &sch_des, &i_des, DES_DECRYPT);
		
		return eAosRc_OK;
	}

	case AOS_KEYMGMT_ENC_ALGO_RC4:
		RC4_set_key(&k_rc4, strlen(key), (unsigned char*)key);
		RC4(&k_rc4, datalen, (unsigned char*)origdata, (unsigned char*)encdata);

		return eAosRc_OK;
	
	case AOS_KEYMGMT_ENC_ALGO_3DES:
	{
		for(int i = strlen(key); i < 9; i++)
		{
			key[i] = ' ';
		}
		key[9] = 0;

		unsigned char iv1_3des[8] = "abcdefg";
		
		memcpy(k_des, key, 8);
		memcpy(i_des, iv1_3des, 8);
		
		DES_set_key_unchecked(&k_des, &sch_des);
		DES_ede3_cfb_encrypt((unsigned char*)origdata, (unsigned char*)encdata, 8,
			datalen, &sch_des,&sch_des,&sch_des,&i_des,  DES_ENCRYPT);

		return eAosRc_OK;
	}

    default:
		 return -eAosRc_NoAlg;
	}

	return eAosRc_OK;
}


int aos_keymagmt_decrypt(char *origdata, unsigned int datalen, char *encdata, unsigned char algo, char *keyname)
{
	char key[AOS_KEYMGMT_KEYLEN+1];

	if(aos_keymagmt_get_key(keyname, key, AOS_KEYMGMT_KEYLEN+1) != 0)
	{
		OmnTrace << "Can't get the key: " << keyname << endl;
		return -eAosRc_KeyNotExist;
	}

	switch (algo)
	{
	case AOS_KEYMGMT_ENC_ALGO_AES:
	{
		for(int i = strlen(key); i < 32; i++)
		{
			key[i] = ' ';
		}
		key[32] = 0;
		
		AES_set_encrypt_key((unsigned char*)key, 32*8, &k_aes);
		unsigned char iv2_aes[16+1] = "1023456789abcdef"; // 16+1 for decryption
		AES_num=0;
		char* ori;
		ori = new char[datalen];
		AES_cfb128_encrypt((unsigned char*)encdata, (unsigned char*)ori,
			datalen, &k_aes, (unsigned char*)iv2_aes, &AES_num, AES_DECRYPT);	
		strncpy(origdata, ori, datalen);
		delete[] ori;

		return eAosRc_OK;
	}
		
	case AOS_KEYMGMT_ENC_ALGO_DES:
	{
		for(int i = strlen(key); i < 9; i++)
		{
			key[i] = ' ';
		}
		key[9] = 0;

		unsigned char iv2_des[8] = "abcdefg";		// initial vector

		memcpy(k_des, key, 8);
		memcpy(i_des, iv2_des, 8);

		DES_set_key_unchecked(&k_des, &sch_des);
		DES_cfb_encrypt((unsigned char*)encdata, (unsigned char*)origdata,8,
			datalen, &sch_des, &i_des, DES_DECRYPT);

		memset(k_des, 0, sizeof(k_des));
		memset(&sch_des, 0, sizeof(sch_des));
		memset(&i_des, 0, sizeof(i_des));

		return eAosRc_OK;
	}
		
	case AOS_KEYMGMT_ENC_ALGO_3DES:
	{
		for(int i = strlen(key); i < 9; i++)
		{
			key[i] = ' ';
		}
		key[9] = 0;

		unsigned char iv1_3des[8];		// initial vector
		strcpy((char*)iv1_3des,"abcdefg");
		
		memcpy(k_des, key, 8);
		memcpy(i_des, iv1_3des, 8);
		
		DES_set_key_unchecked(&k_des, &sch_des);
		DES_ede3_cfb_encrypt((unsigned char*)encdata, (unsigned char*)origdata, 8,
			datalen, &sch_des,&sch_des, &sch_des,&i_des,  DES_DECRYPT);

		return eAosRc_OK;
	}

	case AOS_KEYMGMT_ENC_ALGO_RC4:
		RC4_set_key(&k_rc4, strlen(key), (unsigned char*)key);
		RC4(&k_rc4, datalen, (unsigned char*)encdata, (unsigned char*)origdata);

		return eAosRc_OK;
		
    default:
		 return -eAosRc_NoAlg;     //Unrecognized algo
	}

	return eAosRc_OK;
}


int aos_keymagmt_generate_mac(char* origdata, unsigned int datalen, char* keyname, char* buf, unsigned char algo)
{
	char key[AOS_KEYMGMT_KEYLEN+1];

	if(aos_keymagmt_get_key(keyname, key, AOS_KEYMGMT_KEYLEN+1) != 0)
	{
		OmnTrace <<"fail to get key!" << endl;
		return -eAosRc_KeyNotExist;
	}
	
	switch(algo)
	{
	case AOS_KEYMGMT_MAC_ALGO_MD5:
		MD5_CTX c_md5;
		unsigned char md_md5[MD5_DIGEST_LENGTH];
	
		MD5_Init(&c_md5);
		MD5_Update(&c_md5, origdata, datalen);
		MD5_Update(&c_md5, key, strlen(key));
		MD5_Final((u8*)buf, &c_md5);

		for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(buf+2*i,"%02x", md_md5[i]);

		buf[2*MD5_DIGEST_LENGTH] = 0;
		return eAosRc_OK;
		
	case AOS_KEYMGMT_MAC_ALGO_SHA:
		unsigned char md_sha1[SHA_DIGEST_LENGTH];
		SHA_CTX c_sha1;

		SHA1_Init(&c_sha1);
		SHA1_Update(&c_sha1, origdata, datalen);
		SHA1_Update(&c_sha1, key, strlen(key));
		SHA1_Final(md_sha1, &c_sha1);
		
		for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(buf+2*i,"%02x", md_sha1[i]);

		buf[SHA_DIGEST_LENGTH*2] = 0;
		return eAosRc_OK;
		
	default:
		return -eAosRc_NoAlg; 
	}

	return eAosRc_OK;
}

