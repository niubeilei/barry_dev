////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RCObject.h
// Description:
//	This class defines an interface for resource countable
//  object. It is used by smart pointers to manage the pointer.
//
//  This is an interface. Its implementation is in OmnRCObjImp.
//
//	If a class wants to use smart pointer, it must implement
//  this interface. Normally, we do the following:
//	
//	class ObjWantToUseSPTR : public OmnRCObject
//	{
//	private:
//		OmnRCObjImp	mRCObjImp;
//	...
//  public:
//  ...
//		virtual void addRef() {mRCObjImp.addRef();}
//		virtual void removeRef() {mRCObjImp.removeRef();}
//		...
//  };   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_RCObject_h
#define Omn_Util_RCObject_h

#include "aosUtil/Types.h"
#include "Porting/Mutex.h"
// #include "Util/PtrHash.h"


class OmnRCObject 
{
public:
	virtual void	addRef() = 0;
	virtual int		removeRef(const bool delFlag) = 0;
	virtual int		getRef() const = 0;
	virtual void 	setRCObjInUse(const u32 f) = 0;
	virtual void 	resetRCObjInUse() = 0;

	// 
	// Chen Ding, 05/04/2004
	// This function is used to break circular referencing.
	//
	virtual void	checkSmartPointerCircularPointing() {}

	virtual ~OmnRCObject() {}

	// 
	// Chen Ding, 02/07/2005, Portion 2
	//
	// Chen Ding, 08/12/2010
	// static int	getNumPtrsAdded() {return mPtrTable.getNumPtrsAdded();}
	// static int	getNumPtrsDeleted() {return mPtrTable.getNumPtrsDeleted();}
	// static int	getNumIdleElems() {return mPtrTable.getNumIdleElems();}
	// static int	getNumElemsCreated() {return mPtrTable.getNumElemsCreated();}
	// static int	getNumElemsDeleted() {return mPtrTable.getNumElemsDeleted();}
	// static int	getNumErrorAdd() {return mPtrTable.getNumErrorAdd();}
	// static int	getNumFailedAllocation() {return mPtrTable.getNumFailedAllocation();}
	// static int	getNumFailedQuery() {return mPtrTable.getNumFailedQuery();}
	//
	static void * operator new (size_t size)
	{
		void * ptr = ::operator new(size);
		// mPtrTable.addPtr((const void *)ptr);
		return ptr;
	}

	static void operator delete(void *ptr)
	{
		//if (mPtrTable.inTable((const void *)ptr, true))
		//{
			::operator delete(ptr);
		//}
		//else
		//{
			// 
			// This is an error;
			// 
		//}
	}

	// Chen Ding, 08/12/2010
	// bool isOnHeap() const
	// {
		// 
		// Checks whether it is a heap object
		//
	// 	const void *ptr = dynamic_cast<const void *>(this);
	// 	return mPtrTable.inTable((const void *)ptr, false);
	// }
	// End of Portion 2

};

#endif
