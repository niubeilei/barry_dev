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
// 2015/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IDOServer_IDOServer_h
#define Aos_IDOServer_IDOServer_h

#include "IDO/Ptrs.h"
#include "SEInterfaces/IDOServerObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include <ext/hash_map>

class AosIDOServer : public AosIDOServerObj 
{
	OmnDefineRCObject;

private:
	typedef std::hash_map<const OmnString, AosIDOPtr, Omn_Str_hash, compare_str> map1_t;
	typedef std::hash_map<const OmnString, AosIDOPtr, Omn_Str_hash, compare_str>::iterator itr1_t;

	typedef std::hash_map<const u64, AosIDOPtr, u64_hash, u64_cmp> map2_t;
	typedef std::hash_map<const u64, AosIDOPtr, u64_hash, u64_cmp>::iterator itr2_t;

	map1_t		mNameMap;
	map2_t		mDocidMap;
	OmnMutex *	mLock;

public:
	AosIDOServer(const int version);
	virtual ~AosIDOServer();

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const {return true;}
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual OmnString getObjType(AosRundata *rdata);

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					bool &parsed, 
					AosJqlStatementPtr &statement);

private:
	bool createIDOByName(AosRundata *rdata, AosJimoCall &jimo_call);
	bool getIDOByDocid(AosRundata *rdata, AosJimoCall &jimo_call);
	bool runIDOByName(AosRundata *rdata, AosJimoCall &jimo_call);
};

#endif
