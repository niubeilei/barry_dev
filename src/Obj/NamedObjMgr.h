////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NamedObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_NamedObjMgr_h
#define Omn_ObjMgr_NamedObjMgr_h

#include "Obj/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxPList.h"


class OmnNamedObjMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	OmnNamedObjMgr();
	virtual ~OmnNamedObjMgr();

	int		getObjCount() const {return mObjects.entries();}
	void	removeAll() {mObjects.clear();}

	OmnNamedObjPtr getObj(const OmnString &objName, const bool removeFlag = false)
	{
		// 
		// Retrieve the object from the hash table
		//
		return mObjects.get(objName, removeFlag);
	}

	bool addObj(const OmnNamedObjPtr &obj, 
				const bool overrideFlag, 
				const bool reportError);

	bool removeObj(const OmnString &objName)
	{
		return mObjects.get(objId, true);
	}

	bool housekeeping();
};

#endif
