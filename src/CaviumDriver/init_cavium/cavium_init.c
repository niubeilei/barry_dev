////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_init.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include "cavium_init.h"

int CSP1_driver_handle=-1;

int main (int argc, char** argv)
{
	Csp1InitBuffer init;
	int size, cnt;
	int fd;
	int i;
	char version[VERSION_LEN+1];
	char sram_address[SRAM_ADDRESS_LEN+1];
		
	if (CSP1_driver_handle < 0)
	{
		CSP1_driver_handle = open("/dev/pkp_dev", 0);
			if (CSP1_driver_handle < 0) 
				{
					printf("can't open device pkp_dev\n");
					exit(-1);
				}
	}

	memset(&init,0,sizeof(init));	

	for (i=1; i<argc; i++)
	{
		fd = open(argv[i],O_RDONLY,0);
		if (fd < 0)
		{
			printf("File %s; Could not open\n",argv[i]);
			perror("error");
			goto init_error;
   		}
		printf("%d\n", __LINE__);
		/* version */
		cnt = read(fd,init.version_info[init.size],VERSION_LEN);
		if (cnt != VERSION_LEN)
		{
			printf("File %s; Could not read version\n",argv[i]);
			close(fd);

			goto init_error;
		}
		version[VERSION_LEN] = 0;
		memcpy(version,init.version_info[init.size],VERSION_LEN);
		printf("File %s; Version = %32s\n",argv[i],version);

		/* code length */
		cnt = read(fd,&init.code_length[init.size],4);
		if (cnt != 4)
		{
			close(fd);
			printf("File %s; Could not read code length\n",argv[i]);
			goto init_error;
		}
		/* keep size consistent in byte lengths */
		init.code_length[init.size] = ntohl(init.code_length[init.size])*4;
		size = init.code_length[init.size];
		printf("File %s; Code length = %d\n",argv[i],size);
	
		/* data length */
        	cnt = read(fd,&init.data_length[init.size],4);
		if (cnt != 4)
		{
			printf("File %s; Could not read data length\n",argv[i]);
			close(fd);

			goto init_error;
		}

        	init.data_length[init.size] = ntohl(init.data_length[init.size]);
		size = init.data_length[init.size];
		printf("File %s; Data length = %d\n",argv[i],size);
	
		/* sram address */
		cnt = read(fd,init.sram_address[init.size],SRAM_ADDRESS_LEN);
		if (cnt != SRAM_ADDRESS_LEN)
		{
			printf("File %s; Could not read sram address\n",argv[i]);
			close(fd);

			goto init_error;
		}
		sram_address[SRAM_ADDRESS_LEN] = 0;
		memcpy(sram_address,init.sram_address[init.size],SRAM_ADDRESS_LEN);
		printf("File %s; SRAM address = %llx\n",argv[i],*(Uint64*)(init.sram_address[init.size]));
		
		/* code */
		size = ROUNDUP16(init.code_length[init.size]);
		init.code[init.size] = malloc(size);
		cnt = read(fd,init.code[init.size],size); 
		if (cnt != size)
		{
			printf("File %s; Could not read code\n",argv[i]);
			close(fd);

			goto init_error;
		}


		/* data */
		size = ROUNDUP16(init.data_length[init.size]);
	        init.data[init.size] = malloc(size);
        	cnt = read(fd,init.data[init.size],size);
		if (cnt != size)
		{
			printf("File %s; Could not read data\n",argv[i]);
			close(fd);
			goto init_error;
		}

		/* signature */
		cnt = read(fd,init.signature[init.size],256);
		if (cnt != 256)
		{
			printf("File %s; Could not read signature\n",argv[i]);
			close(fd);
			goto init_error;
		}

		init.size++;	

		close(fd);
	}
		printf("%d\n", __LINE__);
 
	if(ioctl(CSP1_driver_handle,IOCTL_N1_INIT_CODE,(Uint32*)&init) != 0)
		printf("CSP1 init failed\n");

init_error:

	for (i=0; i<init.size; i++)
	{
		if (init.code[i])
			free(init.code[i]);

		if (init.data[i])
			free(init.data[i]);	
	}
 
	if(CSP1_driver_handle != -1)
		close(CSP1_driver_handle);

	return 0;
}
