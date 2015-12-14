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
#ifndef Aos_GenericVerbs_GenericVerb_h
#define Aos_GenericVerbs_GenericVerb_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/GenericVerbObj.h"
#include "Thread/Ptrs.h"


class AosGenericVerb : public AosGenericVerbObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr	mLock;
	map_t		mObjects;
	OmnString	mVerb;

public:
	AosGenericVerb(const int version);
	AosGenericVerb(const AosGenericVerb &rhs);
	~AosGenericVerb();

	virtual OmnString getVerbName() const {return mVerb;}
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config( 	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &jimo_doc);

	virtual bool proc( 	AosRundata *rdata, 
					const OmnString &obj_type,
					const OmnString &obj_name, 
					const OmnString &jsonstr);

};
#endif

