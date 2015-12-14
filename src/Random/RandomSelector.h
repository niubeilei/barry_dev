////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomSelector.h
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomSelector_h
#define Omn_Random_RandomSelector_h

#include "aosUtil/Types.h"
#include "Random/Ptrs.h"
#include "Random/RandomObj.h"
#include "XmlParser/Ptrs.h"



class AosRandomSelector : public AosRandomObj
{
public:

protected:

public:
	AosRandomSelector();
	AosRandomSelector(const OmnString &name);
	~AosRandomSelector();

	static AosRandomSelectorPtr	create(
				const OmnString &name, 
				const OmnXmlItemPtr &conf); 
	static AosRandomSelectorPtr createRandomInteger(
				const OmnString &name, 
				const OmnXmlItemPtr &conf, 
				const u32 base);

};

#endif

