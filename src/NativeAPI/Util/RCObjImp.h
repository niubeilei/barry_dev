////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RCObjImp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_RCObjImp_h
#define Omn_Util_RCObjImp_h

#include <iostream>
#include "Porting/Mutex.h"		// Chen Ding, 02/06/2005
#include "aosUtil/Types.h"


class OmnRCObject;

//
// Define the micro to implement the two member functions of OmnRCObjImp
//
#ifndef OmnDefineRCObject
#define OmnDefineRCObject \
	private: \
	OmnRCObjImp			mRCObj; \
\
	public: \
	virtual void		addRef() {mRCObj.addRef();} \
	virtual int			getRef() const {return mRCObj.getRef();} \
	virtual int			removeRef(const bool delFlag)  \
						{ \
							int result = mRCObj.removeRef(); \
							if (result == 0 && delFlag && mRCObj.tryDel()) delete this ; \
							return result; \
						} \
	virtual void 		setRCObjInUse(const u32 f) {mRCObj.setRCObjInUse(f);} \
	virtual void 		resetRCObjInUse() {mRCObj.resetRCObjInUse();} \
	void setCircleBreakPointer(OmnRCObject *ptr) {mRCObj.setCircleBreakPointer(ptr);} \
	void removeCircleBreakPointer() {mRCObj.removeCircleBreakPointer();}
#endif

enum AosRCObjClassId
{
	eAosRCObjClassId_XmlTag = 10
};

class OmnRCObjImp 
{
private:
	int				mRefCount;
	int				mPoison;
	OmnMutexType	mRefCountLock;		// Chen Ding, 02/06/2005
	u32				mRCObjInUse;		// Chen Ding, 11/13/2011, debugging purpose

	// Chen Ding, 05/04/2004
	// For circular referencing, we need to implement a mechanism to break
	// the circle so that objects can be deleted. If mRefCount == 1 and 
	// mPointer is not null, we need to inform mPointer that it is the
	// only one that points to this object. It is up to that pointer to 
	// determine whether it should remove the pointer or not. 
	OmnRCObject	   *mPointer;

	static unsigned int		mErr;

public:
	OmnRCObjImp()
		:
	mRefCount(0),
	mPoison(0x125325af),
	mRCObjInUse(0),
	mPointer(0)
	{
		// 
		// Chen Ding, 02/06/2005
		//
		OmnInitMutex(mRefCountLock);
	}

	OmnRCObjImp(const OmnRCObjImp & rhs)
		:
	mRefCount(0),
	mPoison(0x125325af),
	mRCObjInUse(0),
	mPointer(0)
	{
		// 
		// Chen Ding, 02/06/2005
		//
		OmnInitMutex(mRefCountLock);
	}

	OmnRCObjImp & operator = (const OmnRCObjImp &rhs)
	{
		return *this;
	}

	virtual ~OmnRCObjImp()
	{
		// Chen Ding, 02/06/2005
		OmnDestroyMutex(mRefCountLock);
	}

	//void	addRef(){mLock.lock(); mRefCount++;mLock.unlock(); }
	void	addRef()
			{
				// 
				// Chen Ding, 02/06/2005
				//
				OmnMutexLock(mRefCountLock);
				mRefCount++;
				OmnMutexUnlock(mRefCountLock);
			}

	int		getRef() const {return mRefCount;}
	int		removeRef()
			{
				// 
				// Chen Ding, 02/06/2005
				//
				OmnMutexLock(mRefCountLock);
				int count = --mRefCount;
				OmnMutexUnlock(mRefCountLock);
				return count;
			}
	// Chen Ding, 05/04/2004
	void	setCircleBreakPointer(OmnRCObject* ptr) {mPointer = ptr;}
	void	removeCircleBreakPointer() {mPointer = 0;}

	// 
	// Chen Ding, 06/01/2005
	//
	bool	tryDel() 
	{
		if (mPoison != 0x125325af || mRefCount != 0)
		{
			// One tries to delete a pointer whose poison flag indicates that
			// the object has already been deleted or the reference count
			// is not 0. Do not delete it.
			mErr++;
			return false;
		}
		if (mRCObjInUse)
		{
			std::cout << "************* Trying to delete object that is in use: " 
				<< mRCObjInUse << std::endl;
			mErr++;
			return false;
		}
		return true;
	}

	// Chen Ding, 11/13/2011
	void setRCObjInUse(const u32 f) {mRCObjInUse = f;}
	void resetRCObjInUse() {mRCObjInUse = 0;}
};

#endif
