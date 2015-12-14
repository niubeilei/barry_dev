////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NatTable.h
// Description:
//	It stores NAT devices.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_NatTable_h
#define Omn_DataServer_NatTable_h

#include "SysObj/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/HashAddrPort.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnNatTableSingleton,
						OmnNatTable,
						OmnNatTableSelf,
						OmnSingletonObjId::eNatTable, 
						"NatTable");

class OmnIpAddr;

class OmnNatTable	:	public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnHashAddrPort<OmnSoNatPtr, 0x3ff>	mHash;
	OmnMutexPtr							mLock;

public:
	OmnNatTable();
	~OmnNatTable();

	bool		start() {return true;}
	bool		stop() {return true;}
	bool		config(const OmnXmlParserPtr &parser) {return true;}

	bool		addNat(const int bubbleId, const OmnIpAddr &addr);
	bool		removeNat(const int bubbleId, const OmnIpAddr &addr);
	bool		isNat(const int bubbleId, const OmnIpAddr &addr);
	OmnSoNatPtr	getNat(const int bubbleId, const OmnIpAddr &ipAddr);

	virtual OmnString       getSysObjName() const {return "OmnNatTable";}
	virtual OmnSingletonObjId::E  getSysObjId() const 
			{return OmnSingletonObjId::eNatTable;}
	virtual OmnRslt         config(const OmnSysObjPtr &def);

};
#endif
