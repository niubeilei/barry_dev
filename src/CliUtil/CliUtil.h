////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliUtil.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliUtil_CliUtil_h
#define Omn_CliUtil_CliUtil_h

enum
{
	eMaxIntArgs = 32,
	eMaxStrArgs = 32,
	eMaxErrLen = 512,
	eMaxDataSize = 128000,
	eMaxCommandLen = 20000,
};

struct aosUserLandApiParms
{
	int				mIntegers[eMaxIntArgs];
	unsigned int	mNumIntegers;
	char *			mStrings[eMaxStrArgs];
	unsigned int	mNumStrings;
};

typedef int (*aosCliFunc)(char *data, 
					  unsigned int *optlen, 
					  struct aosUserLandApiParms *parms, 
					  char *errmsg, 
					  const int errlen);

/* 
 * exported function 
 */						  
int CliUtil_regCliCmd(char *id, aosCliFunc func);
int CliUtil_initCli(char** names, int num, short port);
int CliUtil_checkAndCopy(char *result, unsigned int *index, const unsigned int length, 
					const char *buf, const unsigned int len);
int CliUtil_sprintf(char *result, unsigned int *index, const unsigned int length,
					char* format, ...);
char *CliUtil_getBuff(char *data);

#endif

