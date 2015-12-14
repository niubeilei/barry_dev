////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MngdObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_MngdObjMgr_h
#define Omn_ObjMgr_MngdObjMgr_h

#include "aosUtil/Types.h"
#include "Obj/Ptrs.h"
#include "Obj/MngdObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxPList.h"


class OmnMngdObjMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnSIPL<int64_t, OmnMngdObjPtr>	mObjects;

public:
	OmnMngdObjMgr();
	virtual ~OmnMngdObjMgr();

	int		getObjCount() const {return mObjects.entries();}
	void	removeAll() {mObjects.clear();}

/*
	OmnMngdObjPtr getObj(const int64_t &objId, const bool removeFlag = false)
	{
		// 
		// Retrieve the object from the hash table
		//
		return mObjects.get(objId, removeFlag);
	}
*/

	bool addObj(const OmnMngdObjPtr &obj, 
				const bool overrideFlag, 
				const bool reportError);

/*
	bool removeObj(const int64_t &objId)
	{
		return mObjects.get(objId, true);
	}
*/

	bool housekeeping();
};

#endif
