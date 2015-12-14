////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryDistrMapObj_h
#define Aos_SEInterfaces_QueryDistrMapObj_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosQueryDistrMapObj : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	static AosQueryDistrMapObjPtr	smObject;

public:
	AosQueryDistrMapObj();

	static void setObject(const AosQueryDistrMapObjPtr &o) {smObject = o;}
	static AosQueryDistrMapObjPtr getObject() {return smObject->createObject();}
	virtual AosQueryDistrMapObjPtr createObject() = 0;
	virtual bool setInfo(vector<OmnString> &values,  
						vector<u64> &docids,
						vector<u64> &num_entries) = 0;
	virtual bool clone(const AosQueryDistrMapObjPtr &obj) = 0;
	virtual OmnString toString() = 0;
 	virtual bool    serializeFromXml(const AosXmlTagPtr &xml) = 0;
	virtual bool    reset() = 0;
	virtual bool    getDistrEntry(OmnString &value,u64 &docid,u64 &numdocs) = 0;
};

#endif
