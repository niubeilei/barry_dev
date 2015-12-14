////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomSelectorLenStr.h
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomSelectorLenStr_h
#define Omn_Random_RandomSelectorLenStr_h

#include "aosUtil/Types.h"
#include "Random/Ptrs.h"
#include "Random/RandomObj.h"
#include "XmlParser/Ptrs.h"



class AosRandomSelectorLenStr : public AosRandomObj
{
public:

protected:

public:
	AosRandomSelectorLenStr();
	AosRandomSelectorLenStr(const OmnString &name);
	~AosRandomSelectorLenStr();

	static AosRandomSelectorLenStrPtr	create(
				const OmnString &name, 
				const OmnXmlItemPtr &conf); 
	static AosRandomSelectorLenStrPtr createRandomLenStr(
				const OmnString &name, 
				const int type, 
				const OmnXmlItemPtr &conf, 
				const u32 base);

};

#endif

