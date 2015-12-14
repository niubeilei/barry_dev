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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryVars_QueryVarMgr_h
#define Aos_QueryVars_QueryVarMgr_h

#include "QueryVars/Ptrs.h"
#include "QueryVars/QueryVar.h"
#include <ext/hash_map>
using namespace std;

class AosQueryVarMgr : virtual public AosQueryVar
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosQueryVarPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosQueryVarPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	static map_t 	smJimoMap;

public:
	AosQueryVarMgr();
	~AosQueryVarMgr();

	virtual bool pickJimo(const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);

	virtual bool eval(  const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def, 
						AosValueRslt &value);

	virtual AosJimoPtr cloneJimo() const;

private:
	bool init(const AosRundataPtr &rdata);
	bool createDftJimoDocs(const AosRundataPtr &rdata);

	AosQueryVarObjPtr getJimo(
						const AosRundataPtr &rdata, 
						const OmnString &val_name);

	bool createDftJimoDoc(
						const AosRundataPtr &rdata, 
						const OmnString &classname, 
						const OmnString &val_name);
};
#endif



