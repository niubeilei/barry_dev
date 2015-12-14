////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: getHDinfo.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>


#include "getHDinfo.h"

#define MAXINTERFACES   3

struct opt opt;
static char *bad_index = "<BAD INDEX>";
static unsigned char mac_add[50];
static unsigned char serial_num[50];
/*
 * Copy a physical memory chunk into a memory buffer.
 * This function allocates memory.
 */
void *mem_chunk(size_t base, size_t len, const char *devmem)
{
	void *p;
	int fd;
#ifdef USE_MMAP
	size_t mmoffset;
	void *mmp;
#endif
	
	if((fd=open(devmem, O_RDONLY))==-1)
	{
		perror(devmem);
		return NULL;
	}
	
	if((p=malloc(len))==NULL)
	{
		perror("malloc");
		return NULL;
	}
	
#ifdef USE_MMAP
	mmoffset=base%getpagesize();
	/*
	 * Please note that we don't use mmap() for performance reasons here,
	 * but to workaround problems many people encountered when trying
	 * to read from /dev/mem using regular read() calls.
	 */
	mmp=mmap(0, mmoffset+len, PROT_READ, MAP_SHARED, fd, base-mmoffset);
	if(mmp==MAP_FAILED)
	{
		fprintf(stderr, "%s: ", devmem);
		perror("mmap");
		free(p);
		return NULL;
	}
	
	memcpy(p, (unsigned char *)mmp+mmoffset, len);
	
	if(munmap(mmp, mmoffset+len)==-1)
	{
		fprintf(stderr, "%s: ", devmem);
		perror("munmap");
	}
#endif /* USE_MMAP */
	
	if(close(fd)==-1)
		perror(devmem);
	
	return p;
}


int checksum(const unsigned char *buf, size_t len)
{
	unsigned char sum=0;
	size_t a;
	
	for(a=0; a<len; a++)
		sum+=buf[a];
	return (sum==0);
}


void dmi_dump(struct dmi_header *h, const char *prefix)
{
	int row, i;
	const char *s;
	
	P("%sHeader and Data:\n", prefix);
	for(row=0; row<((h->length-1)>>4)+1; row++)
	{
		P("%s\t", prefix);
		for(i=0; i<16 && i<h->length-(row<<4); i++)
			P("%s%02X", i?" ":"", ((unsigned char *)h)[(row<<4)+i]);
		P("\n");
	}

	if(((unsigned char *)h)[h->length] || ((unsigned char *)h)[h->length+1])
	{
		P("%sStrings:\n", prefix);
		i=1;
		while((s=dmi_string(h, i++))!=bad_index)
		{
			if(opt.flags & FLAG_DUMP)
			{
				int j, l = strlen(s)+1;
				for(row=0; row<((l-1)>>4)+1; row++)
				{
					P("%s\t", prefix);
					for(j=0; j<16 && j<l-(row<<4); j++)
						P("%s%02X", j?" ":"",
						       s[(row<<4)+j]);
					P("\n");
				}
				P("%s\t\"%s\"\n", prefix, s);
			}
			else
				P("%s\t%s\n", prefix, s);
		}
	}
}




/*
 * Type-independant Stuff
 */

char *dmi_string(struct dmi_header *dm, unsigned char s)
{
	char *bp=(char *)dm;
	size_t i, len;

	if(s==0)
		return "Not Specified";
	
	bp+=dm->length;
	while(s>1 && *bp)
	{
		bp+=strlen(bp);
		bp++;
		s--;
	}
	
	if(!*bp)
		return bad_index;
	
	/* ASCII filtering */
	len=strlen(bp);
	for(i=0; i<len; i++)
		if(bp[i]<32 || bp[i]==127)
			bp[i]='.';
	
	return bp;
}


/*
 * Main
 */

void dmi_decode(unsigned char *data, unsigned short ver)
{
	struct dmi_header *h=(struct dmi_header *)data;
	char *serial;
	
	/*
	 * Note: DMI types 37, 38 and 39 are untested
	 */
	switch(h->type)
	{
		case 2: /* 3.3.3 Base Board Information */
			//P("\tSerial Number: %s\n",dmi_string(h, data[0x07]));
			memset(mac_add, 0, 50);
			serial=(char*)dmi_string(h, data[0x07]);
			memset(serial_num, 0, 50);
			memcpy(serial_num, serial, strlen(serial));
			P("%s\n", serial_num);	
			
			if(memcmp(serial_num, "None", strlen("None"))==0 || strlen((const char *)serial_num)==0){
				memset(serial_num, 0x30, 16);
			}
			P("%s\n", serial_num);
			
			break;
		default:
/*
			P("%s Type\n",
				h->type>=128?"OEM-specific":"Unknown");
			dmi_dump(h, "\t");
*/
			;
	}
}

		
void dmi_table(unsigned int base, unsigned short len, unsigned short num, unsigned short ver, const char *devmem)
{
	unsigned char *buf;
	unsigned char *data;
	int i=0;
	
	if(!(opt.flags & FLAG_QUIET))
	{
		if(opt.type==NULL)
			P("%u structures occupying %u bytes.\n"
				"Table at 0x%08X.\n",
				num, len, base);
		P("\n");
	}
	
	if((buf=(unsigned char*)mem_chunk(base, len, devmem))==NULL)
	{
#ifndef USE_MMAP
		P("Table is unreachable, sorry. Try compiling dmidecode with -DUSE_MMAP.\n");
#endif
		return;
	}
	
	data=buf;
	while(i<num && data+sizeof(struct dmi_header)<=buf+len)
	{
		unsigned char *next;
		struct dmi_header *h=(struct dmi_header *)data;
		int display=((opt.type==NULL || opt.type[h->type])
			&& !((opt.flags & FLAG_QUIET) && h->type>39)
			&& !opt.string);

		/* In quiet mode, stop decoding at end of table marker */
		if((opt.flags & FLAG_QUIET) && h->type==127)
			break;
		
		if(display && !(opt.flags & FLAG_QUIET))
			//P("Handle 0x%04X, DMI type %d, %d bytes\n",HANDLE(h), h->type, h->length);
		
		/* look for the next handle */
		next=data+h->length;
		while(next-buf+1<len && (next[0]!=0 || next[1]!=0))
			next++;
		next+=2;
		if(display)
		{
			if(next-buf<=len)
			{
				if(opt.flags & FLAG_DUMP)
					dmi_dump(h, "\t");
				else
					dmi_decode(data, ver);
			}
			else
				P("\t<TRUNCATED>\n");
			//P("\n");
		}
		else if(opt.string!=NULL
		     && opt.string->type==h->type
		     && opt.string->offset<h->length)
		{
			if (opt.string->lookup!=NULL)
				P("%s\n", opt.string->lookup(data[opt.string->offset]));
			else if (opt.string->print!=NULL) {
				opt.string->print(data+opt.string->offset);
				P("\n");
			}
			else
				P("%s\n", dmi_string(h, data[opt.string->offset]));
		}
		
		data=next;
		i++;
	}
	
	if(!(opt.flags & FLAG_QUIET))
	{
		if(i!=num)
			P("Wrong DMI structures count: %d announced, "
				"only %d decoded.\n", num, i);
		if(data-buf!=len)
			P("Wrong DMI structures length: %d bytes "
				"announced, structures occupy %d bytes.\n",
				len, (unsigned int)(data-buf));
	}
	
	free(buf);
}


int smbios_decode(unsigned char *buf, const char *devmem)
{
	if(checksum(buf, buf[0x05])
	 && memcmp(buf+0x10, "_DMI_", 5)==0
	 && checksum(buf+0x10, 0x0F))
	{
		if(!(opt.flags & FLAG_QUIET))
			P("SMBIOS %u.%u present.\n",
				buf[0x06], buf[0x07]);
		dmi_table(DWORD(buf+0x18), WORD(buf+0x16), WORD(buf+0x1C),
			(buf[0x06]<<8)+buf[0x07], devmem);
		return 1;
	}
	
	return 0;
}

static int legacy_decode(unsigned char *buf, const char *devmem)
{
	if(checksum(buf, 0x0F))
	{
		if(!(opt.flags & FLAG_QUIET))
			P("Legacy DMI %u.%u present.\n",
				buf[0x0E]>>4, buf[0x0E]&0x0F);
		dmi_table(DWORD(buf+0x08), WORD(buf+0x06), WORD(buf+0x0C),
			((buf[0x0E]&0xF0)<<4)+(buf[0x0E]&0x0F), devmem);
		return 1;
	}
	
	return 0;
}


int get_main_sn(void)
{
	int ret=0;                  /* Returned value */
	int found=0;
	size_t fp;
	unsigned char *buf;
	
	if(sizeof(unsigned char)!=1 || sizeof(unsigned short)!=2 || sizeof(unsigned int)!=4 || '\0'!=0)
	{
		fprintf(stderr, "compiler incompatibility\n");
		exit(255);
	}

	/* Set default option values */
	opt.devmem=DEFAULT_MEM_DEV;
	opt.flags=0;

	if(opt.flags & FLAG_VERSION)
	{
		P("%s\n", VERSION);
		goto exit_free;
	}
	
	if(!(opt.flags & FLAG_QUIET))
		P("# dmidecode %s\n", VERSION);
////////////////////////////////////////////
	if((buf=(unsigned char*)mem_chunk(0xF0000, 0x10000, opt.devmem))==NULL)
	{
		ret=1;
		goto exit_free;
	}
	
	for(fp=0; fp<=0xFFF0; fp+=16)
	{
		//getchar();
		if(memcmp(buf+fp, "_SM_", 4)==0 && fp<=0xFFE0)
		{
			if(smbios_decode(buf+fp, opt.devmem))
				found++;
			fp+=16;
		}
		else if(memcmp(buf+fp, "_DMI_", 5)==0)
		{
			if (legacy_decode(buf+fp, opt.devmem))
				found++;
		}
	}
	
	free(buf);

	
	if(!found && !(opt.flags & FLAG_QUIET))
		P("# No SMBIOS nor DMI entry point found, sorry.\n");

exit_free:
	free(opt.type);

	return ret;
}

int get_mac_address(void)
{
   register int fd, intrface, retn = 0;
   struct ifreq buf[MAXINTERFACES];
   //struct arpreq arp;
   struct ifconf ifc;
   
//============Part ONE: Get Netcard MAC (Eth0)
   if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
      ifc.ifc_len = sizeof buf;
      ifc.ifc_buf = (caddr_t) buf;
      if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) {
         intrface = ifc.ifc_len / sizeof (struct ifreq);
         P("interface num is intrface=%d\n",intrface);
	intrface--;
         while (intrface >= 0)
          {
            //Get eth0 MAC
            if (strcasecmp(buf[intrface].ifr_name,"eth0")!=0)
            {
            	intrface--;
            	continue;
            }
            P ("net device %s\n", buf[intrface].ifr_name);

            if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &buf[intrface])))
                {
                 //puts ("HW address is:");
			memset(mac_add, 0, 50);

                 sprintf((char*)mac_add, "%02x%02x%02x%02x%02x%02x",
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[0],
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[1],
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[2],
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[3],
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[4],
                                (unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]);





                 //puts("");
		//memcpy(mac_add, buf[intrface].ifr_hwaddr.sa_data, 6);
                 //printf("%s\n", mac_add);	
                 
                }
		break;
          }
       }
    }
    close (fd);
//============End of Part ONE

//============Part TWO: Get Base Board Serial Number

	

//============End of Part TWO

    return retn;
}

int get_hardware_info(unsigned char *serial_number, unsigned char *mac){
	
	get_main_sn();
	get_mac_address();
	memset(serial_number, 0, 16);
	memset(mac, 0, 12);
	memcpy(serial_number, serial_num, 16);
	memcpy(mac, mac_add, 12);
	return 0;
}
