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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysDefinedValues_SysDefValMgr_h
#define Aos_SysDefinedValues_SysDefValMgr_h

#include "SysDefinedValues/SysDefVal.h"
#include <ext/hash_map>
using namespace std;

class AosSysDefValMgr : virtual public AosSysDefVal
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosSysDefValObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosSysDefValObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	static map_t 	smJimoMap;

public:
	AosSysDefValMgr();
	~AosSysDefValMgr();

	virtual bool resolve(const AosRundataPtr &rdata, 
						const OmnString &val_name, 
						const OmnString &dft, 
						OmnString &value); 

	virtual bool resolveWithMember(const AosRundataPtr &rdata, 
						const OmnString &val_type, 
						const OmnString &val_member, 
						const OmnString &dft, 
						OmnString &value);


private:
	bool init(const AosRundataPtr &rdata);
	bool createDftJimoDocs(const AosRundataPtr &rdata);

	AosSysDefValObjPtr getJimo(
						const AosRundataPtr &rdata, 
						const OmnString &val_name);

	bool createDftJimoDoc(
						const AosRundataPtr &rdata, 
						const OmnString &classname, 
						const OmnString &val_name);
};
#endif



