////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// OmnNew is used to monitor the memory allocation. It may degrade the
// system performance a little bit. To turn it off, do the following:
//   #define OmnNew new
//   #define OmnDelete delete 
//   
//
// Modification History:
// 2013/01/18 Reworked by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_OmnNew_h
#define Snt_Util_OmnNew_h

#include <iostream>
#include "Util/MemoryMonitor.h"


// Ken Lee, 2013/01/22
// #define AOS_USE_SELF_DEFINE_NEW

#ifndef AOS_USE_SELF_DEFINE_NEW

// Below is how to turn it off
#ifndef OmnNew
#define OmnNew new
#endif

#ifndef OmnNewArray
#define OmnNewArray new []
#endif

#ifndef OmnDelete
#define OmnDelete delete
#endif

#ifndef OmnDeleteArray
#define OmnDeleteArray delete []
#endif

#ifndef AosObjCountCreateInstance
#define AosObjCountCreateInstance
#endif

#ifndef AosObjCountDeleteInstance
#define AosObjCountDeleteInstance
#endif

#else

void * operator new[](std::size_t size, const char *fname, const int line, int, int, int);
void * operator new(std::size_t size, const char *fname, const int line, int, int, int);
// void * operator new(std::size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);

// Below is how to turn OmnNew on
#ifndef OmnNew
#define OmnNew ::new(__FILE__, __LINE__, 1, 2, 3) 
#endif

#ifndef OmnNewArray
#define OmnNewArray ::new [](__FILE__, __LINE__, 1, 2, 3)
#endif

#ifndef OmnDelete
#define OmnDelete delete
#endif

#ifndef OmnDeleteArray
#define OmnDeleteArray(data) ::operator delete[](data, __FILE__, __LINE__)
#endif

// Ken Lee, 2013/01/24
/*
#ifndef AosObjCountCreateInstance
#define AosObjCountCreateInstance \
	sgAosCountObjInstCountLock.lock(); \
	sgAosCountObjInstCount++; \
	sgAosCountObjTotalCount++; \
	sgAosCountObjInstCountLock.unlock(); 
#endif

#ifndef AosObjCountDeleteInstance
#define AosObjCountDeleteInstance \
	sgAosCountObjInstCountLock.lock(); \
	sgAosCountObjInstCount--; \
	sgAosCountObjInstCountLock.unlock(); 
#endif
*/

#ifndef AosObjCountCreateInstance
#define AosObjCountCreateInstance \
	sgAosCountObjInstCount++; \
	sgAosCountObjTotalCount++; 
#endif

#ifndef AosObjCountDeleteInstance
#define AosObjCountDeleteInstance \
	sgAosCountObjInstCount--; 
#endif

#endif

#endif

