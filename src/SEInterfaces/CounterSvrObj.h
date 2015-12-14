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
// 03/28/2013 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CounterSvrObj_h
#define Aos_SEInterfaces_CounterSvrObj_h

#include "alarm_c/alarm.h"
#include "CounterUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosCounterSvrObj : virtual public OmnRCObject
{
private:
	static AosCounterSvrObjPtr	smCounterSvr;

public:
	virtual bool addTrans(
			const AosCounterTransPtr &trans, 
			const AosRundataPtr &rdata) = 0;
	virtual bool config(const AosXmlTagPtr &config) = 0;

public:
	static void setCounterSvr(const AosCounterSvrObjPtr &d) {smCounterSvr = d;}
	static AosCounterSvrObjPtr getCounterSvr() {return smCounterSvr;}
};
#endif
