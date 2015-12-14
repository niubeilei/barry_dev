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
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 8/18/2010: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_ClickRate_h
#define Aos_UserMgmt_ClickRate_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/StrParser.h"

class AosClickRate : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:


public:
    AosClickRate();
    ~AosClickRate();

	bool dayClickRate( const AosXmlTagPtr &doc, const u64 &docid);
	bool monthClickRate( const AosXmlTagPtr &doc, const u64 &docid);
	bool weekClickRate( const AosXmlTagPtr &doc, const u64 &docid);
	bool yearClickRate( const AosXmlTagPtr &doc, const u64 &docid);
	u64 week();
	int IsRound(u64 year);

private:

};
#endif

