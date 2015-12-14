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
#ifndef Aos_GenericObjs_SystemGenObj_h
#define Aos_GenericObjs_SystemGenObj_h

#include "SEInterfaces/GenericObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"


class AosSystemGenObj : public AosGenericObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr	mLock;
	OmnString	mObjType;
	map_t		mMethods;

public:
	AosSystemGenObj(const int version);
	~AosSystemGenObj();

	virtual OmnString getObjType() const {return mObjType;}
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config( AosRundata *rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual bool execute( AosRundata *rdata, 
						const OmnString &verb,
						const OmnString &obj_name, 
						const OmnString &jsonstr);
	//arvin 2015/4/22
	virtual OmnString getObjType(AosRundata *rdata);
	
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	virtual bool setJimoName(const OmnString &name);

	virtual OmnString getJimoName() const;


	

};
#endif

