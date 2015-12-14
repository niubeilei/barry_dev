////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 08/01/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_TaggedData_TaggedData_h
#define Aos_Proggie_TaggedData_TaggedData_h

#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include <list>

class TiXmlNode;

class AosTaggedData: virtual public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString 			mTag;
	OmnString			mId;
	OmnString 			mCreator;
	OmnString 			mCreateTime;
	OmnString			mStatus;

public:
	AosTaggedData();
	~AosTaggedData();

	// OmnObjDb Interface
	virtual OmnRslt     serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnRslt		serializeFromDb();
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	int 	getData(
					const OmnString &tags, 
					const OmnString &names,
					const int start_idx, 
					const int num,
					const OmnString &query,
					const OmnString &order,
					const bool getTotal,
					OmnString &data, 
					AosXmlRc &errcode, 
					OmnString &errmsg);
};

#endif

