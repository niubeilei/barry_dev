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
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericVerbs_GenericVerbMgr_h
#define Aos_GenericVerbs_GenericVerbMgr_h

#include "SEInterfaces/GenericVerbMgrObj.h"
#include "Thread/Ptrs.h"
#include <hash_map>
using namespace std;

class AosGenericVerbMgr : public AosGenericVerbMgrObj
{
	OmnDefineRCObject;

protected:
	typedef hash_map<const OmnString, AosGenericVerbObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosGenericVerbObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr				mLock;
	map_t					mVerbs;
	vector<AosXmlTagPtr>	mJimoDocs;

public:
	AosGenericVerbMgr(const int version);
	AosGenericVerbMgr(const AosGenericVerbMgr &rhs);
	~AosGenericVerbMgr();

	virtual AosJimoPtr cloneJimo() const;
	virtual bool addGenericVerb(AosRundata *rdata, const AosGenericVerbObjPtr &verb);
	virtual bool proc( AosRundata *rdata, 
						const OmnString &verb,
						const OmnString &obj_type,
						const OmnString &obj_name, 
						const OmnString &jsonstr);

private:
	AosGenericVerbObjPtr createVerb(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &jimo_doc);
	bool init();
	bool createVerbs(AosRundata *rdata);
	bool createGenericVerb(AosRundata *rdata, const OmnString &doc_str);
};
#endif

