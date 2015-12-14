////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: regex.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Ktcpvs/regex.h"




// 
// Simulation of the regular expression
//
int             /* 0 success, REG_NOMATCH failure */
regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags)
{
	// 
	// We always return success
	// Chen Ding, 07/11/2005
	//
	// return REG_NOMATCH;
	return 0;
}


int             /* 0 success, otherwise REG_something */
regcomp(regex_t *preg, const char *pattern, int cflags)
{
	return 0;
}


void
regfree(regex_t *preg)
{
}

