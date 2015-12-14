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

#ifndef AOS_KEYMAN_H
#define AOS_KEYMAN_H

enum {
	AOS_KEYMGMT_ENC_ALGO_AES = 1,
	AOS_KEYMGMT_ENC_ALGO_DES,
	AOS_KEYMGMT_ENC_ALGO_3DES,
	AOS_KEYMGMT_ENC_ALGO_RC4,


	AOS_KEYMGMT_MAC_ALGO_MD5,
	AOS_KEYMGMT_MAC_ALGO_SHA,
};

enum {
	eAosRc_OK,
	eAosRc_KeyExist,
	eAosRc_KeyNotExist,
	eAosRc_KeyBuf_Overflow,
	eAosRc_KeyFile_NotExist,
	eAosRc_KeyFile_RWErr,
	eAosRc_KeyFile_FormatErr,
	eAosRc_NoAlg,
	eAosRc_Encrypt,
	eAosRc_Decrypt,
};

/*
 * Key operation functions
 */
extern int aos_keymagmt_add_key(char*, char*);
extern int aos_keymagmt_remove_key(char*);
extern int aos_keymagmt_get_key(char*, char*, unsigned int);

/*
 * encryption/decryption functions 
 */
extern int aos_keymagmt_encrypt(char*, unsigned int, char*, unsigned char, char*);
extern int aos_keymagmt_decrypt(char*, unsigned int, char*, unsigned char, char*);

/*
 * MAC generating function 
 */
extern int aos_keymagmt_generate_mac(char*, unsigned int, char*, char*, unsigned char);

#endif

