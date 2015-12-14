////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjMgr_ObjMgr_h
#define Omn_ObjMgr_ObjMgr_h

#include "Message/Ptrs.h"
#include "Obj/Ptrs.h"
#include "Obj/MgdDbObj.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SmtPList.h"


class OmnDbObjMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultInvalidObjTTL = 60,		// 60 seconds
	};

	OmnString					mName;
	OmnMutex					mLock;
	OmnSmtPList<OmnMgdDbObjPtr>	mHash;
	int							mInvalidObjTTL;

public:
	OmnDbObjMgr();
	virtual ~OmnDbObjMgr();

	int			getObjCount() const {return mHash.entries();}
	void		removeAll() {mHash.clear();}
	OmnMgdDbObjPtr getObj(const OmnMgdDbObjPtr &obj, const bool removeFlag = false);
	bool		objCreated(const OmnMgdDbObjPtr &obj);
	bool		objDeleted(const OmnMgdDbObjPtr &obj);
	bool		housekeeping();
	bool		clear();
	OmnString	getName() const {return mName;}
	bool		procMsg(const OmnMsgPtr &msg);
};

#endif
