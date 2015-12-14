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
// 01/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_LangDictObj_h
#define Aos_SEInterfaces_LangDictObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"

#ifndef AOSTERM 
#define AOSTERM(id, rdata) AosLangDictObj::getTermStatic((id), (rdata))
#endif

#ifndef AOSTERM_U
#define AOSTERM_U(id, rdata) AosLangDictObj::getTermStatic((id), (rdata))
#endif

class AosLangDictObj : virtual public OmnRCObject
{
private:
	static AosLangDictObjPtr smDictionary;

public:
	virtual OmnString getTerm(  const OmnString &id, AosRundata *rdata) = 0;

	static AosLangDictObjPtr getDictionary() {return smDictionary;}
	static void setDictionary(const AosLangDictObjPtr &d) {smDictionary = d;}
	static OmnString getTermStatic(const OmnString &id, AosRundata *rdata)
	{
		if (!smDictionary) return id;
		return smDictionary->getTerm(id, rdata);
	}
};
#endif
