////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemFwNat.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Global.h"

#include <stdio.h>
#include <iostream>
#include "aosUtil/List.h"  

#define MAX_INTERFACE 16
#define MAX_LIMIT_RATE 600000
#define MAX_PORTS 16

struct aos_list_head sgFwFilterList;
struct aos_list_head sgFwNatList;
struct fwVpnPass sgFwVpnPass;

using namespace std;

// kevin 07/18/2006 
// parse the port
// format: port or port1:port2
int string_to_number_ll(const char *s, unsigned long long min, unsigned long long max, unsigned long long *ret)
{
	unsigned long long number;
	char *end;

	number = strtoull(s, &end, 0);
	if (*end == '\0' && end != s) {
		if ( min <= number && (!max || number <= max)) {
			*ret = number;
			return 0;
		}
	}
	return -1;
}

int string_to_number_l(const char *s, unsigned long min, unsigned long max, unsigned long *ret)
{
	int result;
	unsigned long long number;

	result = string_to_number_ll(s, min, max, &number);
	*ret = (unsigned long)number;

	return result;
}

int string_to_number(const char *s, unsigned int min, unsigned int max, unsigned int *ret)
{
	int result;
	unsigned long number;

	result = string_to_number_l(s, min, max, &number);
	*ret = (unsigned int)number;

	return result;
}

unsigned int parse_port(const char *port)
{
	unsigned int portnum;

	if ((string_to_number(port, 0, 65535, &portnum)) != -1 )
	{	
		return (int)portnum;
	}
	else
	{
		printf("Error: invalid port!\n");
	}
	return -1;	
}

unsigned int parse_multi_ports(char *portstring, unsigned int *ports)
{
	char *buffer, *cp, *next;
	unsigned int i;

	buffer = strdup(portstring);

	for (cp=buffer, i=0; cp && i<MAX_PORTS; cp=next,i++)
	{
		next=strchr(cp, ':');
		if (next) *next++='\0';
		ports[i] = parse_port(cp);
	}
	//if (cp) printf("too many ports specified");
	free(buffer);
	return i;
}

int portlen(int port)
{
	//unsigned int len;
	if (port < 10) {return 1;}
	else if (port < 100) {return 2;}
	else if (port < 1000) {return 3;}
	else if (port < 10000) {return 4;}
	else if (port < 65536) {return 5;}
	else { cout << "Error: The port is too large!" << endl; return -1;}	
}

int ratelen(int port)
{
	//unsigned int len;
	if (port < 10) {return 1;}
	else if (port < 100) {return 2;}
	else if (port < 1000) {return 3;}
	else if (port < 10000) {return 4;}
	else if (port < 100000) {return 5;}
	else if (port <= 600000) {return 6;}
	else { cout << "Error: The rate is too large!" << endl; return -eAosRc_InvalidRate;}	
}

int parse_mac(char *mac, int *macarray)
{
	unsigned int i = 0;

	if (strlen(mac) != 6*3-1)
	{
		//cout << "Error: Invalid mac address(Length)!" << endl;
		return -eAosRc_InvalidMac;
	}
	
	for (i = 0; i < 6; i++) 
	{
		long number;
		char *end;
		number = strtol(mac + i*3, &end, 16);
		if (end == mac + i*3 + 2 && number >= 0 && number <= 255)
			macarray[i] = number;
		else
		{
			//cout << "Error: Invalid mac address!" << endl;
			return -eAosRc_InvalidMac;
		}
	}
	return 0;
}

