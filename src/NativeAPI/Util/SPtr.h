////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SPtr.h
// Description:
//	This is a smart pointer. We should refrain from using native pointers
//  in our program.
//
//  Warning: 
//	    When using smart pointer, be careful about the following two cases:
//		Case 1: 
//		  T::f()
//		  {
//			OmnSPtr<T> sptr = this;
//			...
//		  }
//
//		  When one calls T::f(), it will delete 'this'. For example:
//		  OmnSPtr<T> sptr = new T();
//		  sptr->f();
//
//		  sptr->anyfunction();		// Now the instance pointed to by sptr has
//									// already been deleted (tell me why!)
//
//		Case 2:
//		  T::g()
//		  {
//			anotherfunc(this);
//			...
//		  }
//
//		  void anotherfunc(const OmnSPtr<T> &sptr)
//		  {
//			...
//		  }
//
//		  main()
//		  {
//			T t;
//			t.g();
//
//		    // now the instance has already been deleted (tell me why).
//		  }
//
//		In Case 1, we can use one of the following:
//		  OmnSPtr<T> sptr = this;
//		  sptr.addRef();
//	    Or
//			
//	
// Modification History:
//    For circular referring: 
//		Class A
//		{
//			OmnSPtr<B>		mB;
//			...
//		};
//	
//		Class B
//		{
//			OmnSPtr<A>		mA;
//			...
//		};
//	
//		OmnSPtr<A> a = new A;
//		OmnSPtr<B> b = new B;
//		a->setB(b);
//		b->setA(a);
//
//	In this example, a and b will not get deleted. 
//  If the above example means that "if a is pointed to by b only, 
//  it is safe to delete a", then we can break the loop. We will do
//  the following:
//		a. When assigning mA, call the following function:
//		   mA->setCircleBreakPointer(this);
//		b. Implement the following function:
//		   B::checkSmartPointerCircularPointing()
//		   {
//				if (mA.getRef() == 1 && !mA.isNull())
//				{
//					mA = 0;
//				}
//		   }
//   
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SPtr_h
#define Omn_Util_SPtr_h

#ifndef OmnPtrDecl
#define OmnPtrDecl(x, y) class x; typedef OmnSPtr<x> y;
#endif

#include "Porting/ThreadDef.h"
#include "Util/RCObject.h"

extern bool OmnAppExecutionIsActive;

template <class T>
class OmnSPtr 
{
private:
	T *				mPtr;
	mutable OmnMutexType    mLock;
	bool			mDelFlag;	// If true and if the reference is 0, it deletes the object

public:
	OmnSPtr()
	:
	mPtr(0),
	mDelFlag(true)
	{
		OmnInitMutex(mLock);
	}

	OmnSPtr(T *ptr, const bool delFlag = true)
	:
	mPtr(ptr),
	mDelFlag(delFlag)
	{
		OmnInitMutex(mLock);
		OmnMutexLock(mLock);
		if (mPtr)
		{
			mPtr->addRef();
		}
		OmnMutexUnlock(mLock);
	}

	OmnSPtr(const OmnSPtr &rhs)
	:
	mPtr(rhs.mPtr),
	//mDelFlag(rhs.mDelFlag)
	mDelFlag(true)	// Ken Lee, 2013/01/09
	{
		//
		// Note: this is a copy constructor. A smart pointer has two member
		// data: mPtr and mDelFlag. In this copy constructor, we did not
		// copy mDelFlag. This is quite dangerous because people may 
		// consider a copy constructor always copy all the member data. 
		// The reason we did not copy mDelFlag is obvious: preventing
		// memory leak. Whoever uses mDelFlag to prevent a smart pointer
		// from deleting its pointer, it has to make an explicit effort.
		//
		OmnInitMutex(mLock);
		OmnMutexLock(mLock);
		if (mPtr)
		{
			//mPtr->addRef();
		}
		OmnMutexUnlock(mLock);
	}

	// Chen Ding, 11/10/2012
	// int getRef() const {return (mPtr)?mPtr->getRef():0;}
	int	getRef() const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		if (!mPtr) 
		{
			OmnMutexUnlock(((OmnSPtr *)this)->mLock);
			return 0;
		}

		int ref = 0;//mPtr->getRef();
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return ref;
	}

	OmnSPtr & operator = (T *ptr)
	{
		OmnMutexLock(mLock);
		if (mPtr == ptr)
		{
			OmnMutexUnlock(mLock);
			return *this;
		}
	
		if (mPtr)
		{
			//mPtr->removeRef(mDelFlag);
		}
	
		mDelFlag = true;
		mPtr = ptr;
		if (mPtr)
		{
			//mPtr->addRef();
		}
		OmnMutexUnlock(mLock);
		return *this;
	}

	template<class newType>
	operator OmnSPtr<newType>()
	{
		// Chen Ding, 11/10/2012
		// return OmnSPtr<newType>(mPtr);
		OmnMutexLock(mLock);
		OmnSPtr<newType> newptr(mPtr);
		OmnMutexUnlock(mLock);
		return newptr;
	}

	OmnSPtr & operator = (const OmnSPtr &rhs)
	{
		OmnMutexLock(mLock);
		if (this == &rhs)
		{
			OmnMutexUnlock(mLock);
			return *this;
		}

		if (mPtr == rhs.mPtr)
		{
			OmnMutexUnlock(mLock);
			return *this;
		}

		if (mPtr)
		{
			// gAosLogFile << (unsigned int)OmnGetCurrentThreadId 
			// 	<< ":SPtr:" << __LINE__ << ">:" << (u64)mPtr << ":" << mPtr->getRef() << endl;
			//mPtr->removeRef(mDelFlag);
		}

		// Chen Ding, 11/10/2012
		// Ice reported that if 'rhs' is doing the operation 'rhs = 0', 
		// and the reference count of rhs.mPtr is 0, 'rhs' will delete 
		// rhs.mPtr. While 'rhs' is in the middle of deleting 'rhs.mPtr'
		// but before setting rhs.mPtr to 0, its thread is interrupted, 
		// if another thread tries to do the following:
		// 		mPtr = rhs.mPtr;
		// this new thread will hold mPtr that will be deleted by 'rhs'. 
		// To avoid this problem, we need to call 'rhs.getPtr()', which
		// will lock 'rhs' and get its mPtr. That should be thread safe.
		// mPtr = rhs.mPtr;
		mPtr = rhs.getPtr();

		//
		// For the same reason as explained in the copy constructor, 	
		// we do not copy mDelFlag. 
		// If one wants to keep its value, it has to do it explicitly.
		//
		// mDelFlag = true;
		// Chen Ding, 04/29/2011

		//mDelFlag = rhs.mDelFlag;
		mDelFlag = true;	// Ken Lee, 2013/01/09
		if (mPtr)
		{
			// gAosLogFile << (unsigned int)OmnGetCurrentThreadId 
			// 	<< ":SPtr:" << __LINE__ << ">:" << (u64)mPtr << ":" << mPtr->getRef() << endl;
			//mPtr->addRef();
		}
		OmnMutexUnlock(mLock);

		return *this;
	}

	~OmnSPtr()
	{
		OmnMutexLock(mLock);
		if (OmnAppExecutionIsActive && mPtr)
		{
			//int rr = 0;
			//mPtr->getRef();
			// gAosLogFile << (unsigned int)OmnGetCurrentThreadId 
			// 	<< ":SPtr:" << __LINE__ << ">:" << (u64)mPtr << ":" << mPtr->getRef() << endl;
			//mPtr->removeRef(mDelFlag);
		}
		mPtr = 0;
		OmnMutexUnlock(mLock);
		OmnDestroyMutex(mLock);
	}

	// Chen Ding, 11/10/2012
	// bool		isNull() const {return !mPtr;}
	bool		isNull() const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = !mPtr;
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	// Chen Ding, 11/10/2012
	// bool		notNull() const {return mPtr;}
	bool		notNull() const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = mPtr;
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}
	void		setDelFlag(const bool b) {mDelFlag = b;}

	// Operators
	// Chen Ding, 11/10/2012
	// T * operator ->() const {return mPtr;}
	T * operator ->() const 
	{
		OmnMutexLock(mLock);
		T *ptr = mPtr;
		OmnMutexUnlock(mLock);
		return ptr;
	}

	// Chen Ding, 2014/07/26
	inline T * getPtrNoLock() const
	{
		return mPtr;
	}

	// Chen Ding, 11/10/2012
	// T * getPtr() const {return mPtr;}
	T * getPtr() const
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		T * ptr = mPtr;
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return ptr;
	}

	// Chen Ding, 11/10/2012
	// T& operator *() const {return *mPtr;}
	T& operator *() const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		T &value = *mPtr;
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return value;
	}

	// Chen Ding, 11/10/2012
	// bool operator !() const {return !mPtr;}
	bool operator !() const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = !mPtr;
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	// Chen Ding, 11/10/2012
	// operator bool () const {return mPtr != 0;}
	operator bool () const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = (mPtr != 0);
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	// Chen Ding, 11/10/2012
	// bool operator != (const OmnSPtr<T> &rhs) const 
	bool operator != (const OmnSPtr<T> &rhs) const 
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = (mPtr != rhs.getPtr());
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	// Chen Ding, 11/10/2012
	// bool operator == (const OmnSPtr<T> &rhs) const
	bool operator == (const OmnSPtr<T> &rhs) const
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = (mPtr == rhs.getPtr());
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	bool operator == (T *rhs) const
	{
		OmnMutexLock(((OmnSPtr *)this)->mLock);
		bool rslt = (mPtr == rhs);
		OmnMutexUnlock(((OmnSPtr *)this)->mLock);
		return rslt;
	}

	void setNull()
	{
		OmnMutexLock(mLock);
		if (!mPtr)
		{
			OmnMutexUnlock(mLock);
			return;
		}
		// gAosLogFile << (unsigned int)OmnGetCurrentThreadId 
		// 	<< ":SPtr:" << __LINE__ << ">:" << (u64)mPtr << ":" << mPtr->getRef() << endl;

		//mPtr->removeRef(mDelFlag);
		mPtr = 0;
		// Shawn 04/29/2011 
		mDelFlag = true;
		OmnMutexUnlock(mLock);
	}

	// Chen Ding, 05/04/2004
	void setCircleBreakPointer(OmnRCObject *ptr)
	{
		OmnMutexLock(mLock);
		if (mPtr)
		{
			mPtr->setCircleBreakPointer(ptr);
		}
		OmnMutexUnlock(mLock);
	}
};
#endif

