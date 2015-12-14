////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "des.h"
#include "getHDinfo.h"
#include "base64.h"

int main(int argc, char *argv[])
{
	unsigned char key[24]="", iv[8]="";
	unsigned char mbsn[20]="", mac[14]="";
	unsigned char block[50]="", encrypted_block[50]="", feature_list[4]="";
	unsigned char license[256]="";
	DES3_CBC_CTX ctx;
	int i;
	int base_length;
	uid_t user_id;

	user_id=getuid();
	if(user_id!=0){
		printf("Please use root user\n");
		exit(1);
	}
	
        for(i=0;i<24;i++){
                key[i]=i;
        }
        for(i=0;i<8;i++){
                iv[i]=i;
        }
        
        get_hardware_info(mbsn, mac);
        memcpy(block, mbsn, 16);
        memcpy(block+16, mac, 12);
        memcpy(block+16+12, feature_list, 4);
        
        DES3_CBCInit(&ctx, key, iv, 1);
        DES3_CBCUpdate(&ctx, encrypted_block, block, 32);
        DES3_CBCRestart(&ctx);	

        base_length=Base64Encode((char*)license, (const char *)encrypted_block, 32);
        
        printf("%s\n", license);
	if(argc>1)
	{
		printf("mac:%s\n",mac);
		printf("mbsn:%s\n",mbsn);
	}
	return 0;
}
	
	
	
