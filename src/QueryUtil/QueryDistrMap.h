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
// Modification History:
// 01/10/2014 Created by Shawn
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QueryDistrMap_h
#define Aos_QueryUtil_QueryDistrMap_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "QueryUtil/Ptrs.h"
#include "SEUtil/IILIdx.h"
#include "SEInterfaces/QueryDistrMapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"



class AosQueryDistrMap : public AosQueryDistrMapObj
{
	OmnDefineRCObject;

private:
	vector<OmnString>	mValues;
	vector<u64>			mDocids;
	vector<u64>			mNumEntries;
	u64					mIdx;

public:
	AosQueryDistrMap();
	~AosQueryDistrMap();

	virtual void 	clear();
	virtual bool	serializeToXml(AosXmlTagPtr &xml);
	virtual bool	serializeFromXml(const AosXmlTagPtr &xml);
	virtual bool 	clone(const AosQueryDistrMapObjPtr &obj);
	virtual bool    reset();
	virtual bool    getDistrEntry(OmnString &value,u64 &docid,u64 &numdocs);
	virtual OmnString toString();

	virtual AosQueryDistrMapObjPtr createObject();
	virtual bool setInfo(vector<OmnString> &values,  
						vector<u64> &docids,
						vector<u64> &num_entries);
		
};
#endif



