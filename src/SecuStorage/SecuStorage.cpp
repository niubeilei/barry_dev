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

#include "SecuStorage/SecuStorage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "keyman/keyman.h"
#include "CommandMan/CommandMan.h"
#include <dirent.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <unistd.h>

#define SIGNATURE_LEN 20
#define MD5_DIGEST_LENGTH 16
#define MY_SHA_DIGEST_LENGTH SHA512_DIGEST_LENGTH
/*
 * encrypted file header
 */
typedef struct enc_file_hd {
	char enc_magic[2];
	uint16_t algo;
	uint8_t is_folder;
	uint8_t zip_flag;
	uint32_t enc_size;
	union {
		char md5[2 * MD5_DIGEST_LENGTH + 1];
		char sha1[2 * MY_SHA_DIGEST_LENGTH + 1];
	} sum;
} enc_file_hd_t;

#define IS_ENC(magic)		((magic[0] == 'S') && (magic[1] == 'S'))
#define MAX_PATH			256
#define HEADER_LEN			sizeof(enc_file_hd_t)
#define BUFF_SIZE			512

/*
 * encrypt the file to the defalult file
 * @filename
 * @keyname
 * @algo
 */
int aos_secured_storage_encrypt_file(char *file_name, char *key_name, int algo)
{
	char buffer[MAX_PATH];
	
	sprintf(buffer, "%s.enc", file_name);
	
	return aos_secured_storage_encrypt_file_to(file_name, buffer, key_name, algo);
}

/*
 * decrypt the file to the default file
 * @filename
 * @keyname
 * @algo
 */
int aos_secured_storage_decrypt_file(char *file_name, char *key_name, int algo)
{
	char buffer[MAX_PATH];
	
	sprintf(buffer, "%s.plain", file_name);
	
	return aos_secured_storage_decrypt_file_from(file_name, buffer, key_name, algo);
}

/*
 * compute the file length
 * @file_name
 */
uint32 aos_secured_storage_get_file_length(char* file_name)
{
	uint32 len;
	
	FILE *fp = NULL;
	
	if ((fp = fopen(file_name, "r")) == NULL) {
		printf("Failed to open file %s.", file_name);
		return 0;
	}
	
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	if (SECUSTORAGE_DEBUG)
		printf("file %s length is %d", file_name, len);
	fclose(fp);
	
	return len;
}

/*
 * encrypt the single file 
 */
int aos_secured_storage_encrypt_single_file_to(char *orig_file, char *encrypted_file, char *key_name, int algo)
{
	enc_file_hd_t header;
	char buff[BUFF_SIZE];
	char dst_buff[BUFF_SIZE];
	FILE *fp_orig = NULL;
	FILE *fp_encrypted = NULL;
	int size = 0;
	int i = 0, j = 0;
	int ret;
	
	/* validate the file path */
	if ((fp_orig = fopen(orig_file, "r")) == NULL) {
		printf("Failed to open file %s\n", orig_file);
		return -1;
	}
	
	if ((fp_encrypted = fopen(encrypted_file, "w+")) == NULL) {
		printf("Failed to open file %s\n", encrypted_file);
		return -1;
	}
	
	/* create the header information */
	header.enc_magic[0] = 'S';
	header.enc_magic[1] = 'S';
	header.algo = algo;
	header.is_folder = 0;
	header.zip_flag = 0;
	
	if (SECUSTORAGE_DEBUG)
		printf("encrypt algorithm id: %d", header.algo);
	
	header.enc_size = aos_secured_storage_get_file_length(orig_file);
	switch (algo) {
		case ALG_AES_with_MD5:
			aos_secured_storage_generate_md5(orig_file, header.sum.md5);
			break;
		case ALG_AES_with_SHA1:
			aos_secured_storage_generate_sha1(orig_file, header.sum.sha1);
			break;
		case ALG_DES_with_MD5:
			aos_secured_storage_generate_md5(orig_file, header.sum.md5);
			break;;
		case ALG_DES_with_SHA1:
			aos_secured_storage_generate_sha1(orig_file, header.sum.sha1);
			break;
		case ALG_3DES_with_MD5:
			aos_secured_storage_generate_md5(orig_file, header.sum.md5);
			break;
		case ALG_3DES_with_SHA1:
			aos_secured_storage_generate_sha1(orig_file, header.sum.sha1);
			break;
		default:
			printf("unsupported algorithm.\n");
			ret = -1;
			goto error;
	}
	
	/* add the header of encrypted content */
	fwrite(&header, 1, sizeof(header), fp_encrypted);
	
	/* add the body of encrypted content */
	while(!feof(fp_orig)) {
		size = fread(buff, 1, BUFF_SIZE, fp_orig);
		switch (algo) {
			case ALG_AES_with_MD5:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_AES, key_name);
				break;
			case ALG_AES_with_SHA1:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_AES, key_name);
				break;
			case ALG_DES_with_MD5:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_DES, key_name);
				break;;
			case ALG_DES_with_SHA1:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_DES, key_name);
				break;
			case ALG_3DES_with_MD5:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_3DES, key_name);
				break;
			case ALG_3DES_with_SHA1:
				aos_keymagmt_encrypt(buff, size, dst_buff, AOS_KEYMGMT_ENC_ALGO_3DES, key_name);
				break;
		}
		fwrite(dst_buff, 1, size, fp_encrypted);
	}
	
	fclose(fp_orig);
	fclose(fp_encrypted);
	
	return 0;
error:	
	fclose(fp_orig);
	fclose(fp_encrypted);
	
	return -1;
}

/*
 * encrypt the directory
 */
int aos_secured_storage_encrypt_directory_to(char *orig_path, char *encrypted_file, char *key_name, int algo)
{
	char cmd[BUFF_SIZE];
	char tmp_file[BUFF_SIZE];
	enc_file_hd_t header;
	int ret;
	
	sprintf(tmp_file, "/tmp/SecuStorage.tmp");
	/* compress the directory by system command*/
	sprintf(cmd, "%s", orig_path);
	ret = chdir(cmd);
	if (ret < 0) {
		return -1;
	} 
	
	memset(cmd, 0, BUFF_SIZE);
	sprintf(cmd, "tar czf %s ./", tmp_file);
	if (SECUSTORAGE_DEBUG)
		printf("%s\n", cmd);
	
	ret = system(cmd);
	if (ret < 0) {
		printf("Failed to create compress file.");
		return -1;
	}
	
	
	return aos_secured_storage_encrypt_file_to(tmp_file, encrypted_file, key_name, algo);
}
/*
 * encrypt the file to the destination file
 * @orig_file
 * @encrypted_file
 * @key_name
 * @algo
 */
int aos_secured_storage_encrypt_file_to(char *orig_file, char *encrypted_file, char *key_name, int algo)
{
	return aos_secured_storage_encrypt_single_file_to(orig_file, encrypted_file, key_name, algo);
}

/*
 * decrypt the file to the destination file
 * @orig_file
 * @decrypted_file
 * @key_name
 * @algo
 */
int aos_secured_storage_decrypt_file_from(char *orig_file, char *decrypted_file, char *key_name, int algo)
{
	FILE *fp_orig = NULL;
	FILE *fp_decrypted = NULL;
	char buff[BUFF_SIZE];
	char dst_buff[BUFF_SIZE];
	enc_file_hd_t header;
	u8 header_algo;
	int size = 0;
	int ret = 0;
	
	if (aos_secured_storage_get_file_length(orig_file) < HEADER_LEN) {
		printf("invalid decrypted file.");
		return -1;
	}
	if ((fp_orig = fopen(orig_file, "r")) == NULL) {
		printf("Failed to open file %s.", orig_file);
		return -1;
	}
	
	if ((fp_decrypted = fopen(decrypted_file, "w+")) == NULL) {
		printf("Failed to open file %s.", decrypted_file);
		return -1;
	}
	
	fread(&header, 1, sizeof(header), fp_orig);
	header_algo = header.algo;
	/* here parameter 'algo' given will be ommitted if invalid, we will give some warning.*/
	if (header_algo != algo && IS_ENC(header.enc_magic)) {
		printf("warning:your given decryption method is not correct for file %s.\nBut it will be ommitted", orig_file);
		return -1;
	}
	while (!feof(fp_orig)) {
		size = fread(buff, 1, BUFF_SIZE, fp_orig);
		switch (header_algo) {
			case ALG_AES_with_MD5:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_AES, key_name);
				break;
			case ALG_AES_with_SHA1:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_AES, key_name);
				break;
			case ALG_DES_with_MD5:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_DES, key_name);
				break;
			case ALG_DES_with_SHA1:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_DES, key_name);
				break;
			case ALG_3DES_with_MD5:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_3DES, key_name);
				break;
			case ALG_3DES_with_SHA1:
				ret |= aos_keymagmt_decrypt(dst_buff, size, buff, AOS_KEYMGMT_ENC_ALGO_3DES, key_name);
				break;
		}
		fwrite(dst_buff, 1, size, fp_decrypted);
	}
	
	fclose(fp_decrypted);
	fclose(fp_orig);
	
	/* validate the sum code of file */
	switch (header_algo) {
		case ALG_AES_with_MD5:
			ret = aos_secured_storage_validate_md5(decrypted_file, header.sum.md5);
			break;
		case ALG_AES_with_SHA1:
			ret = aos_secured_storage_validate_sha1(decrypted_file, header.sum.sha1);
			break;
		case ALG_DES_with_MD5:
			ret = aos_secured_storage_validate_md5(decrypted_file, header.sum.md5);
			break;
		case ALG_DES_with_SHA1:
			ret = aos_secured_storage_validate_sha1(decrypted_file, header.sum.sha1);
			break;
		case ALG_3DES_with_MD5:
			ret = aos_secured_storage_validate_md5(decrypted_file, header.sum.md5);
			break;
		case ALG_3DES_with_SHA1:
			ret = aos_secured_storage_validate_sha1(decrypted_file, header.sum.sha1);
			break;
	}
	if (ret < 0) {
		printf("Failed to pass the csum validatation.\n");
		return -1;
	}
	
	return ret;
error:	
	fclose(fp_decrypted);
	fclose(fp_orig);
	
	return -1;
}

/*
 * generate the md5 code of file
 * @file_name
 * @md5
 */
int aos_secured_storage_generate_md5(char* file_name, char* md5)
{
	return aos_Generate_MD5(file_name, md5);
}

/*
 * validate the md5 code of file
 * @file_name
 * @md5
 */
int aos_secured_storage_validate_md5(char* file_name, char* md5)
{
	char buff[2 * MD5_DIGEST_LENGTH + 1];
	
	aos_Generate_MD5(file_name, buff);
	
	if (SECUSTORAGE_DEBUG) {
		printf("\naos_secured_storage_validate_md5:\ngenerate code:%s\n", buff);
		printf("\naos_secured_storage_validate_md5:\nvalidate code:%s\n", md5);
	}
		 
	if (strncmp(buff, md5, MD5_DIGEST_LENGTH) == 0) {
		return 0;
	}
	
	return -1;
}

/*
 * generate the sha1 code of file
 * @file_name
 * @sha1
 */
int aos_secured_storage_generate_sha1(char* file_name, char* sha1)
{
	char SystemKey[BUFF_SIZE] = "12345678";
	struct stat buff_stat;
	FILE* fp = 0;
	int i = 0;
	char buff[BUFF_SIZE];
	unsigned long len;
	SHA512_CTX c_sha;
	unsigned char sha_buff[MY_SHA_DIGEST_LENGTH];
	
	if(stat(file_name, &buff_stat) < 0)
	{
		return -1;   
	}

	if(!S_ISREG(buff_stat.st_mode)) 
	{
		return -1;
	}

	if((fp = fopen(file_name, "r")) == NULL) {
		return -1;
	} 

	SHA512_Init(&c_sha);
	while (!feof(fp)) {
		len = fread(buff, 1, BUFF_SIZE, fp);		
		SHA512_Update(&c_sha, buff, len);
	}
	
	SHA512_Update(&c_sha, SystemKey, strlen(SystemKey));
	SHA512_Final(sha_buff, &c_sha);
	
	for (i = 0; i < MY_SHA_DIGEST_LENGTH; i++)
		sprintf(sha1 + 2 * i,"%02x", sha_buff[i]);
	sha1[2 * MY_SHA_DIGEST_LENGTH] = 0;	
	
	fclose(fp);

	return 0;
}

/*
 * validate the sha1 code of file
 * @file_name
 * @sha1
 */
int aos_secured_storage_validate_sha1(char* file_name, char* sha1)
{
	char buff[2 * MY_SHA_DIGEST_LENGTH + 1];
	
	aos_secured_storage_generate_sha1(file_name, buff);
	
	if (SECUSTORAGE_DEBUG) {
		printf("\naos_secured_storage_validate_sha1:\n generate sha1 code:%s\n", buff);
		printf("\naos_secured_storage_validate_sha1:\n validate sha1 code:%s\n", sha1);
	}
		
	if (strncmp(buff, sha1, 2 * MY_SHA_DIGEST_LENGTH) == 0) {
		return 0;
	}
	
	return -1;
}

/*
 * encrypt directory
 * @path
 * @key_name
 * @algo
 */
int aos_secured_storage_encrypt_dir(char* path, char* key_name, int algo)
{
	char buff[BUFF_SIZE];
	int ret;
	
	sprintf(buff, "/tmp/default_encrypt_directory.tar.gz");
	
	return aos_secured_storage_encrypt_dir_to(path, buff, key_name, algo);
}

/*
 * decrypt directory
 * @file_name
 * @key_name
 * @algo
 */
int aos_secured_storage_decrypt_dir(char* file_name, char* key_name, int algo)
{
	char buff[BUFF_SIZE];
	char cmd[BUFF_SIZE];
	int ret = 0;
	
	sprintf(buff, "/tmp");
	
	ret = aos_secured_storage_decrypt_dir_from(file_name, buff, key_name, algo);
	
	
	return ret;
}

/*
 * encrypt directory by giving distination file name
 * @orgi_path
 * @encrypted_file
 * @key_name
 * @algo
 */
int aos_secured_storage_encrypt_dir_to(char* orig_path, char* encrypted_file, char* key_name, int algo)
{
	return aos_secured_storage_encrypt_directory_to(orig_path, encrypted_file, key_name, algo);
}

/*
 * decrypt directory by giving distination file name
 * @orig_file
 * @decrypted_path
 * @key_name
 * @algo
 */
int aos_secured_storage_decrypt_dir_from(char* orig_file, char* decrypted_path, char* key_name, int algo)
{
	char buff[BUFF_SIZE];
	char cmd[BUFF_SIZE];
	int ret = 0;
	
	sprintf(buff, "/tmp/default_secured_storage_dir_decrypt.tmp");
	ret = aos_secured_storage_decrypt_file_from(orig_file, buff, key_name, algo);
	if (ret < 0) {
		return -1;
	}
	
	sprintf(cmd, "%s", decrypted_path);
	if (SECUSTORAGE_DEBUG)
	 	printf("%s\n", cmd);
	 	 
	ret = chdir(cmd);
	if (ret < 0) {
		return -1;
	}
	
	memset(cmd, 0, BUFF_SIZE);
	sprintf(cmd, "tar xvfz %s ", buff);
	if (SECUSTORAGE_DEBUG)
		printf("%s\n", cmd);
	
	ret = system(cmd);
	if (ret < 0) {
		
		return -1;
	}
	
	return 0;
}
