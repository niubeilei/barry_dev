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
// 2015/04/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoProg_JimoProgScript_h
#define Aos_JimoProg_JimoProgScript_h

#include "JQLStatement/Ptrs.h"
#include "JimoProg/JimoProg.h"

class AosJimoProgScript : public AosJimoProg
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, vector<AosJqlStatementPtr>, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, vector<AosJqlStatementPtr>, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t			mSections;
	OmnString		mCode;

public:
	AosJimoProgScript(const int version);
	~AosJimoProgScript();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// JimoProg Interface
	virtual ProgType getProgType() const {return eJimoScript;}
	virtual OmnString getCode() const {return mCode;}
	virtual OmnString generateCode(AosRundata *rdata);

	virtual bool appendStatement(AosRundata *rdata, 
								const OmnString &section_name,
								const AosJqlStatementPtr &statement);

};
#endif

