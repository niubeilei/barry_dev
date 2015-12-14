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
// 2014/8/20 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AggrFuncs_AggrFuncMgr_h
#define Aos_AggrFuncs_AggrFuncMgr_h

#include "SEInterfaces/AggrFuncObj.h"
#include "SEInterfaces/AggrFuncMgrObj.h"
#include <ext/hash_map>
using namespace std;

class AosAggrFuncMgr : virtual public AosAggrFuncMgrObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosAggrFuncObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosAggrFuncObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	static map_t 	smJimoMap;

public:
	AosAggrFuncMgr(const int version);
	~AosAggrFuncMgr();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual AosAggrFuncObjPtr pickAggrFunc(
				const AosRundataPtr &rdata,
				const OmnString &func_name);
	
	virtual bool isFuncValid(
				const AosRundataPtr &rdata,
				const OmnString &func_name);

private:
	bool init(const AosRundataPtr &rdata);
	bool createDftJimos(const AosRundataPtr &rdata);

	AosAggrFuncObjPtr getJimo(
				const AosRundataPtr &rdata, 
				const OmnString &val_name);

	bool createDftJimo(
				const AosRundataPtr &rdata, 
				const OmnString &classname, 
				const OmnString &val_name);
	AosXmlTagPtr createDftJimoDoc(
				const AosRundataPtr &rdata, 
				const OmnString &classname, 
				const OmnString &val_name);
};
#endif



