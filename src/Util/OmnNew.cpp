////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OmnNew.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Util/OmnNew.h"

#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Util/String.h"


#ifdef AOS_USE_SELF_DEFINE_NEW
void * operator new[](std::size_t size, const char *fname, const int line, int, int, int)
{
	cout <<__FILE__ << ":" << __LINE__  << ":" << OmnGetCurrentThreadId()
		<< ":============" 
		<< fname << ":" << line << ": To new memory(array): " << size << endl;
	char *mem = (char *)malloc(size + AosMemoryMonitor::eMetaSize);
	// void *ptr = AosMemoryMonitor::memAllocated(mem, size, fname, line);
	return mem;
}

// void * operator new(std::size_t size)
// {
// 	// cout << "============= To new memory: " << size << endl;
// 	char *mem = (char *)malloc(size + AosMemoryMonitor::eMetaSize);
// 	void *ptr = AosMemoryMonitor::memAllocated(mem, size, "unknown", 0);
// 	return ptr;
// }

void * operator new(std::size_t size, const char *fname, const int line, int, int, int)
{
	cout << __FILE__ << ":" << __LINE__  << ":" << OmnGetCurrentThreadId()
		<< ":=============" << fname << ":" << line << ": To new memory: " << size << endl;
	char *mem = (char *)malloc(size + AosMemoryMonitor::eMetaSize);
	// void *ptr = AosMemoryMonitor::memAllocated(mem, size, fname, line);
	// return ptr;
	return mem;
}

void operator delete(void *ptr)
{
	// cout << __FILE__ << ":" << __LINE__  << ":" << OmnGetCurrentThreadId()
	// 	<< ":============= To delete memory: " << endl;
	// void *new_ptr = AosMemoryMonitor::memDeleted(ptr);
	// free(new_ptr);
	free(ptr);
}

void operator delete [] (void *ptr)
{
	// cout << __FILE__ << ":" << __LINE__  << ":" << OmnGetCurrentThreadId()
	// 	<< ":============= To delete memory[]: " << endl;
	// void *new_ptr = AosMemoryMonitor::memDeleted(ptr);
	// free(new_ptr);
	free(ptr);
}

/*
static int gMemMagic = 0x37215460;
void * operator new (size_t size, char * file,  int line)
{
	int *flags;
    char *ptr = (char *)malloc((size+2*sizeof(int)));
    if(ptr == 0)
    {
        cout << "40++++ No Memory when maalloc("<< size << ") ,"<<  file <<":" <<line<<endl;
        throw(0);
    }

    flags = (int *)ptr;
    flags[0] = gMemMagic;
    flags[1] = OmnMemMtr::memCreated(file,line,size);

    return (void *)(flags+2);

}

void * operator new [](size_t size, char * file,  int line)
{
	OmnMemMtr::mTotalNewArray ++;
	return operator new(size, file, line);
}


void operator delete(void * p)
{

	int *flags;
    if(p == 0)
    {
        cout << "44++++ free null point "<< endl;
        return;
    }
    flags = (int *)((char*)p - 2*sizeof(int));
    if(flags[0] != gMemMagic)
    {
        cout << "45++++ free not controlled :"  << (void*)p << endl;
        free(p);
        return;
    }
    OmnMemMtr::memDeleted(__FILE__,__LINE__,flags[1]);
    free((void*)flags);
}

void operator delete [] (void * p)
{
	int *flags;
    flags = (int *)((char*)p - 2*sizeof(int));
    if(flags[0] != gMemMagic)
    {
        cout << "46++++ free not controlled point :"  << (void *)p << endl;
        free(p);
        return;
    }
    OmnMemMtr::memDeleted(__FILE__,__LINE__,flags[1]);
	OmnMemMtr::mTotalDelArray++;
    free((void*)flags);
}
*/


/*
bool
AosMemLeak::addEntry(
		const char *fname, 
		const int line, 
		void *ptr)
{
	// 1. Convert (fname, line) into a u64 ID
	// 2. Add (ptr, id) into the hash table.
	// 3. Add the counter, if needed
	// 4. Increments the counter.
	int len = strlen(fname);
	u32 id = AosStrHashFunc(fname, len);
	mLock->lock();
	for (int i=0; i<mNumEntries; i++)
	{
		if (mIds[i] == id)
		{
			if (mFnames[i] == fname)
			{
				OmnAlarm << "File name conflicts: " << mFnames[i]
					<< ":" << fname << endl;
				return false;
			}

			// Found the entry. Increment the counter.
			mCounters[i]++;
			mHash->add(fname, len, id);
			mLock->unlock();
			return true;
		}
	}

	// Did not find the entry. Add one.
	aos_assert_rl(mNumEntries < eMaxEntries, mLock, false);
	mIds[mNumEntries] = id;
	mFnames[mNumEntries] = fname;
	mCounters[mNumEntries] = 1;
	mHash->add(fname, len, id);
	mLock->unlock();
	return true;
}


bool
AosMemLeak::delEntry(void *ptr)
{
	mLock->lock();
*/
		
#endif

