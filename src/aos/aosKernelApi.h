////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosKernelApi_h
#define aos_aosKernelApi_h

#include <KernelSimu/compiler.h>

#include "aos/KernelEnum.h"
#include "aos/aosCoreComm.h"
#include "aos/KernelApiStruct.h"

enum
{
	eAosMaxKernelApiIntegers = 10,
	eAosMaxKernelApiStrings = 20
};

struct aosKernelApiParms
{
	int				mIntegers[eAosMaxKernelApiIntegers];
	unsigned int	mNumIntegers;
	char *			mStrings[eAosMaxKernelApiStrings];
	unsigned int	mNumStrings;
	char *			mCmd;
};

typedef int (*aosCliFunc)(char *data, 
						  unsigned int *optlen, 
						  struct aosKernelApiParms *parms, 
						  char *errmsg, 
						  const int errlen);

struct OmnKernelApiData;
struct OmnKernelApi_GetMac;


extern int OmnKernelApi_init(void);
extern int OmnProcKernelApi(char *data, 
							unsigned int *optlen, 
							char *errmsg, 
							const int errlen);
extern int aosKernelApi_getMac(struct OmnKernelApi_GetMac *data);
extern int OmnKernelApi_addCliCmds(void);
// extern int aosKernelApi_convert(char *data, unsigned int datalen, 
// 								struct aosKernelApiParms *parms);
extern int OmnKernelApi_addCliCmd(char *id, aosCliFunc func);
extern int OmnKernelApi_delCliCmd(char *id);
extern int aosKernelApi_getStr1(unsigned int index, 
						char **value, 
						char *data, 
						const unsigned int optlen);
extern int aosKernelApi_getStr2(unsigned int index, 
						char **value1, 
						char **value2, 
						char *data, 
						const unsigned int optlen);
extern int aosKernelApi_getStr3(unsigned int index, 
						char **value1, 
						char **value2, 
						char **value3, 
						char *data, 
						const unsigned int optlen);
extern int aosKernelApi_getStr4(unsigned int index, 
						char **value1, 
						char **value2, 
						char **value3, 
						char **value4, 
						char *data, 
						const unsigned int optlen);
extern int aosKernelApi_getInt1(int index, int *value, char *data);
extern int aosKernelApi_getInt2(int index, int *v1, int *v2, char *data);
extern int aosKernelApi_getInt3(int index, 
						int *v1, int *v2, int *v3, char *data);
extern int aosKernelApi_getInt4(int index, 
						int *v1, int *v2, int *v3, int *v4, char *data);

static inline int aosKernelApi_getNumInt(char *data)
{
	return ((int*)data)[0];
}

static inline int aosKernelApi_getNumStr(char *data)
{
	return ((int*)data)[1];
}

static inline int *aosKernelApi_getLength(char *data)
{
	return (int*)&data[0];
}

static inline char *aosKernelApi_getBuff(char *data)
{
	return 	&data[4];
}

#ifndef AosKAPI_checkNumStrArgs
#define AosKAPI_checkNumStrArgs(parms, num_args, rc, errmsg) 	\
	if (parms->mNumStrings != num_args)							\
	{															\
		sprintf(errmsg, "String arguments mismatch."			\
						" Expect: %d but got: %d", 				\
			parms->mNumStrings, num_args);						\
		return rc;												\
	}	
#endif

#ifndef AosKAPI_checkStr
#define AosKAPI_checkStr(arg, msg, rc, errmsg) 					\
	if (!(arg))													\
	{															\
		sprintf(errmsg, "%s", msg);								\
		return rc;												\
	}	
#endif
#endif

