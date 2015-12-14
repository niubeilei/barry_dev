////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: verify.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "getHDinfo.h"
#include "des.h"
#include "verify.h"


int generate_local_license(unsigned char *local_license, unsigned char *feature_list)
{
	unsigned char serial_number[16];
	unsigned char mac[12];
	unsigned char block[32];
	DES3_CBC_CTX ctx;
	unsigned char encrypt_block[32];
	unsigned char key[24];
	unsigned char iv[8];
	int i;

	
	memset(serial_number, 0, 16);
	memset(encrypt_block, 0, 32);
	memset(mac, 0, 12);
	memset(key, 0, 24);
	memset(iv, 0, 8);

	for(i=227;i<227+24;i++){
		key[i-227]=i&0x54;
	}
	for(i=227;i<227+8;i++){
		iv[i-227]=i&0x67;
	}	

	
	get_hardware_info(serial_number, mac);
	//printf("serial_number: %s\n", serial_number);
	//printf("mac: %s\n", mac);
	memset(block, 0, 32);
	memcpy(block, serial_number, 16);
	memcpy(block+16, mac, 12);
	memcpy(block+12+16, feature_list, 4);
/*
	for(i=0;i<32;i++){
		printf("%02x ", block[i]);
	}
	puts("");
*/

	
	DES3_CBCInit(&ctx, key, iv, 1);
	DES3_CBCUpdate(&ctx, encrypt_block, block, 32);
	DES3_CBCRestart(&ctx);

	memcpy(local_license, encrypt_block, 32);
/*
	puts("encrypt block: \n");
	for(i=0;i<32;i++){
		printf("%02x ", encrypt_block[i]);
	}
	puts("");	
*/

	

	return 0;
}

int verify_license(void)
{
	FILE *fp;
	unsigned char b64_license[256];
	int base64_length;
	unsigned char output_buf[256];
	unsigned char encrypt_license[32];
	unsigned char feature_list[4];
	unsigned char local_license[32];

	fp=fopen(LICENSE_PATH, "r");
	if(fp==NULL){
		perror("License file");
		return 0;
	}

	memset(b64_license, 0, 256);
	fscanf(fp, "%s\n", b64_license);
	//printf("%s\n", b64_license);

	memset(output_buf, 0, 256);
	base64_length=Base64Decode((char*)output_buf, (const char*)b64_license, strlen((char*)b64_license));
	//printf("%d\n", base64_length);
	memset(encrypt_license, 0, 32);
	memcpy(encrypt_license, output_buf, 32);
	
	memcpy(feature_list, output_buf+32, 4);
/*
	puts("encrypt_license:\n");
	for(i=0;i<32;i++){
		printf("%02x ", encrypt_license[i]);
	}
	puts("");
*/

	generate_local_license(local_license, feature_list);

	resume_hd_info(NULL, NULL, b64_license);	

	fclose(fp);
	if(memcmp(local_license, encrypt_license, 32)==0){
		return 1;
	}else{
		return 0;
	}

	return 1;
}

int resume_hd_info(unsigned char *serial_number, unsigned char *mac, unsigned char *b64_license)
{
	unsigned char encrypted_block[256], encrypted_license[32], decrypted_license[32];
	int base64_length;
	unsigned char key[24], iv[8];
	DES3_CBC_CTX ctx;
	int i;
	
	memset(encrypted_block, 0, 256);
	base64_length=Base64Decode((char*)encrypted_block, (const char*)b64_license, strlen((const char*)b64_license));
	memcpy(encrypted_license, encrypted_block, 32);

	for(i=227;i<227+24;i++){
		key[i-227]=i&0x54;
	}
	for(i=227;i<227+8;i++){
		iv[i-227]=i&0x67;
	}	
	
	DES3_CBCInit(&ctx, key, iv, 0);
	DES3_CBCUpdate(&ctx, decrypted_license, encrypted_license, 32);
	DES3_CBCRestart(&ctx);
/*
	puts("decrypted license:");
	for(i=0;i<32;i++){
		printf("%02x ", decrypted_license[i]);
	}
	puts(""); 
*/
	return 0;
}
