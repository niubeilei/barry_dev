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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_Interface_h
#define Aos_Jimo_Interface_h

#include "Jimo/Ptrs.h"
#include "Jimo/MethodDef.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <hash_map>
using namespace std;

class AosInterface : public OmnRCObject
{
	OmnDefineRCObject;

	typedef hash_map<const OmnString, AosMethodDef, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosMethodDef, Omn_Str_hash, compare_str>::iterator itr_t;

private:
	OmnMutexPtr	mLock;
	map_t		mMethods;
	OmnString	mDef;

public:
	AosInterface();
	~AosInterface();

	virtual AosInterfacePtr clone(AosRundata *rdata) const;
	virtual OmnString toString() const;

	virtual bool retrieveMethods(
							AosRundata *rdata, 
							AosJimo *jimo);

	virtual bool config(	AosRundata *rdata, 
							const AosXmlTagPtr &worker_doc);

	virtual void * getMethod(AosRundata *rdata, 
							const char *name, 
							AosMethodId::E &method_id);

	virtual bool serializeTo(
							AosRundata *rdata, 
							AosBuff *buff);

	virtual bool serializeFrom(
							AosRundata *rdata, 
							AosBuff *buff);
};

#endif

