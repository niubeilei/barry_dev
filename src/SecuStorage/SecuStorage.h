////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: keyman.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_SecuStorage_SecuStorage_H
#define AOS_SecuStorage_SecuStorage_H

#include <linux/types.h>

#define SECUSTORAGE_DEBUG 1
/* return value */
enum {
	ERR_OK = 0,
	ERR_NOT_ENC,
	ERR_ALLOC_MEM,
	ERR_CHECKSUM,
	ERR_FILE_CREATE,
	ERR_FILE_OPEN,
	ERR_NOT_ENCFILE,
	ERR_FILE_READ,
};

/* algorithm */
enum {
	ALG_AES_with_MD5 = 100,
	ALG_AES_with_SHA1,
	ALG_DES_with_MD5,
	ALG_DES_with_SHA1,
	ALG_3DES_with_MD5,
	ALG_3DES_with_SHA1,
};


/* expotable functions */

/*
 * encrypt the file to the defalult file
 * @filename
 * @keyname
 * @algo
 */
extern int aos_secured_storage_encrypt_file(char *file_name, char *key_name, int algo);

/*
 * decrypt the file to the default file
 * @filename
 * @keyname
 * @algo
 */
extern int aos_secured_storage_decrypt_file(char *file_name, char *key_name, int algo);

/*
 * encrypt the file to the destination file
 * @orig_file
 * @encrypted_file
 * @key_name
 * @algo
 */
extern int aos_secured_storage_encrypt_file_to(char *orig_file, char *encrypted_file, char *key_name, int algo);

/*
 * decrypt the file to the destination file
 * @orig_file
 * @decrypted_file
 * @key_name
 * @algo
 */
extern int aos_secured_storage_decrypt_file_from(char *orig_file, char *decrypted_file, char *key_name, int algo);

/*
 * generate the md5 code of file
 * @file_name
 * @md5
 */
extern int aos_secured_storage_generate_md5(char* file_name, char* md5);

/*
 * validate the md5 code of file
 * @file_name
 * @md5
 */
extern int aos_secured_storage_validate_md5(char* file_name, char* md5);

/*
 * generate the sha1 code of file
 * @file_name
 * @sha1
 */
extern int aos_secured_storage_generate_sha1(char* file_name, char* sha1);

/*
 * validate the sha1 code of file
 * @file_name
 * @sha1
 */
extern int aos_secured_storage_validate_sha1(char* file_name, char* sha1);

/*
 * encrypt directory
 * @path
 * @key_name
 * @algo
 */
extern int aos_secured_storage_encrypt_dir(char* path, char* key_name, int algo);

/*
 * decrypt directory
 * @file_name
 * @key_name
 * @algo
 */
extern int aos_secured_storage_decrypt_dir(char* file_name, char* key_name, int algo);

/*
 * encrypt directory by giving distination file name
 * @orig_path
 * @encrypted_file
 * @key_name
 * @algo
 */
extern int aos_secured_storage_encrypt_dir_to(char* orig_path, char* encrypted_file, char* key_name, int algo);

/*
 * decrypt directory by giving distination file name
 * @orig_file
 * @decrypted_path
 * @key_name
 * @algo
 */
extern int aos_secured_storage_decrypt_dir_from(char* orig_file, char* decrypted_path, char* key_name, int algo);
#endif
