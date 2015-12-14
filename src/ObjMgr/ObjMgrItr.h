////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjMgrItr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_ObjMgrItr_h
#define Omn_ObjMgr_ObjMgrItr_h



template <class C, unsigned int size>
class OmnObjMgrItr
{
private:
	OmnObjHashItr<C, size>		mItr;

public:
	OmnObjMgrItr(const OmnObjHash<C, size> &hash)
		:
	mItr(hash.clone())
	{
	}

	~OmnObjMgrItr();

	void	reset() {mItr.reset();}
	C		crt() {return mItr.crt();}
	C		next() {return mItr.next();}
	bool	hasMore() const {return mItr.hasMore();}
	int		objCount() const {return mItr.objCount();}
};

#endif
