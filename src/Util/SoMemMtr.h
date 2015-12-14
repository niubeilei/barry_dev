////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SoMemMtr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SoMemMgr_h
#define Omn_Util_SoMemmgr_h

#include "Database/Ptrs.h"
// #include "iptables/RouteRule.h"
#include "Obj/ObjSys.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include <vector>

class OmnSoMemMtr : virtual public OmnSysObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eIsGood,
		eSnapshotId,
		eLocId,
		eFileName,
		eLine,
		eNewNum,
		eDelNum,
		eMemSize
	};
	bool 			mIsGood;
	int				mSnapshotId;
	int				mLocId;
	OmnString		mFileName;
	int 			mLine;
	int				mNewNum;
	int				mDelNum;
	int 			mMemSize;

private:
	OmnString	sqlFields() const;

public:
	OmnSoMemMtr();
	~OmnSoMemMtr();
	
	virtual int			objSize() const;
	virtual OmnString	toString() const;
	virtual OmnClassId::E	getClassId() const {return OmnClassId::eOmnSoMemMtr;}

    virtual bool	    serializeTo(OmnSerialTo &s) const;
    virtual bool	    serializeFrom(OmnSerialFrom &s);

    virtual bool        serializeToDb(const int snapshotId) ;
	virtual OmnRslt		serializeFromDb(const OmnDbRecordPtr &record);

	OmnString 			SqlFields();
	OmnString			sqlStmtInsert() const;

	void	set(const char *file, 
				const int line , 
				const int mLocId,
				const int mNewNum,
				const int mDelNum,
				const size_t size);
	bool  retrieve(const int locId);
};
#endif

