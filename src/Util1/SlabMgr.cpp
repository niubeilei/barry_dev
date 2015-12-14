////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabMgr.cpp
// Description:
//	This class provides memory management. As soon as an application
//  starts, it calls setMemoryHandler() to change the default memory
//  handler to the one defined in this class. From then on, if a 
//  new failed, it will call memoryHandler() member function. This 
//  function will call all managed objects to release some memory. 
//  Hopefully, this resolves memory problem.
//
//  Singleton Dependency:
//  	OmnAlarmMgr
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#if 0


#include "Util1/SlabMgr.h"

#include "aos/aosKernelAlarm.h"
#include "aos/aosReturnCode.h"
#include "KernelUtil/KernelMem.h"
#include "KernelUtil/Slab.h"


struct aosSlabMgr *
aosSlabMgr_constructor()
{
	struct aosSlabMgr *mgr = (struct aosSlabMgr*)OmnKernelAlloc(sizeof(aosSlabMgr));

	if (!mgr)
	{
		aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	int i;
	for (i=0; i<eAosMaxSlabs; i++)
	{
		mgr->mSlabs[i] = 0;
	}

	mgr->mNumSlabs = 0;
	return mgr;
}


int
aosSlabMgr_destructor()
{
	return 0;
}


int
aosSlabMgr_start()
{
	return true;
}


int 
aosSlabMgr_stop()
{
	return true;
}


/*
int		
aosSlabMgr_addSlab(struct aosSlabMgr *self, 
				   struct aosSlab *slab)
{
	//
	// It adds the slab into the list.
	//
	//	sgLock.lock();
	int i;
	for (i=0; i<self->mNumSlabs; i++)
	{
		if (self->mSlabs[i]->mSize == slab->mSize)
		{
			// 
			// This is an error. The slab is already in the list
			//
			// sgLock.unlock();
			return aosAlarmStr2(eAosAlarmSlabAlreadyDefined, self->mSlabs[i]->mName, 
				slab->mName);
		}

		if (self->mSlabs[i]->mSize < slab->mSize)
		{
			continue;
		}

		if (self->mNumSlabs >= eAosMaxSlabs)
		{
			// sgLock.unlock();
			return aosAlarm(eAosAlarmTooManySlabs);
		}

		// 
		// The slab is larger than the current one. Shift the array.
		//
		int j;
		for (j=i; j<self->mNumSlabs-1; j++)
		{
			self->mSlabs[j+1] = self->mSlabs[j];
		}

		self->mSlabs[i] = slab;
		self->mNumSlabs++;
		// sgLock.unlock();
		return 0;
	}

	// 
	// Append the slab
	//
	if (self->mNumSlabs >= eAosMaxSlabs)
	{
		// sgLock.unlock();
		return aosAlarm(eAosAlarmTooManySlabs);
	}

	self->mSlabs[self->mNumSlabs] = slab;
	self->mNumSlabs++;
	// sgLock.unlock();
	return 0;
}
*/


/*
void		
aosSlabMgr_setMemoryHandler()
{
	//
	// It sets the memory handler: memoryHandler() and stores the
	// previous handler in mPreviousHandler. 
	//
	sgLock.lock();
	if (mPreviousHandler != 0)
	{
		//
		// This means the function has been called previously. In this
		// current implementation, this is not allowed.
		//
		OmnAlarm << "Try to set memory handler, but someone has already set it"
			<< enderr;
		sgLock.unlock();
		return;
	}

	//
	// Then set the memory handler. Store the existing one in mPreviousHandler
	//
	mPreviousHandler = set_new_handler(memoryHandlerStatic);
	sgLock.unlock();
}


void		
aosSlabMgr_restoreMemoryHandler()
{
	//
	// It resets the memory handler. Before calling this function, the
	// setMemoryHandler() should have called. Otherwise, it is an error.
	//
	sgLock.lock();
	if (mPreviousHandler == 0)
	{
		//
		// The setMemoryHandler() was not called. This is an error.
		//
		OmnAlarm << "Try to set memory handler, but someone has already set it"
			<< enderr;
		sgLock.unlock();
		return;
	}

	//
	// Otherwise, retore the handler.
	//
	set_new_handler(mPreviousHandler);
	sgLock.unlock();
}


void		
aosSlabMgr_memoryHandlerStatic()
{
	if (!aosSlabMgrSelf)
	{
		return;
	}

	aosSlabMgrSelf->memoryHandler();
}


void
aosSlabMgr_memoryHandler()
{
	//
	// This is the member function to be called when memory request
	// failed. This function checks whether there is any managed
	// object. If yes, it asks everyone to release some memory. 
	// If some memory is released, it returns. Otherwise, it means
	// it could not release any memory. It will abort the execution.
	//
	OmnTrace << "Memory is running low. Call memory handler to release"
		<< " some memory." << endl;

	sgLock.lock();
	sgMemPools.reset();
	uint sizeReleased = 0;
	while (sgMemPools.hasMore())
	{
		sizeReleased += (sgMemPools.crtValue())->purge();
		if (sizeReleased >= RELEASE_THRESHOLD)
		{
			//
			// It has released enough memory. Return.
			//
			OmnTrace << "Total memory released: " << sizeReleased << endl;
			sgLock.unlock();
			return;
		}

		sgMemPools.next();
	}

	sgLock.unlock();

	//
	// Check whether it has ever released something.
	//
	if (sizeReleased == 0)
	{
		//
		// It did not. This means that application can't release anything.
		//
		OmnAlarm << "Run out of memory" << enderr;
		abort();
	}

	//
	// It did released something, though not as much as we wanted.
	//
	return;
}
*/


/*
int
aosSlabMgr_getObjCnt() 
{
	OmnString str;

	sgLock.lock();
	sgMemPools.reset();
	while (sgMemPools.hasMore())
	{
		str << (sgMemPools.crtValue())->getObjCnt() << "\n";
		sgMemPools.next();
	}
	sgLock.unlock();

	str << OmnMemPoolMul_getObjCnt();
	return str;
}
*/

#endif