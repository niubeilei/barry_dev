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
// RandomSentence is a component class. It randomly gets the cli based on 
// the xml random selection model.  
//
// Modification History:
// 12/07/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomSentence_h
#define Omn_Random_RandomSentence_h

#include "aosUtil/Types.h"
#include "Random/RandomObj.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosRandomSentence : public AosRandomObj
{
	OmnDefineRCObject;

public:
	enum
	{
	    eMaxItems = 100
	};

private:
	int					mItems;
	AosRandomObj		mRandomObj[eMax];

public:
	AosRandomSentence();
	AosRandomSentence(const OmnString &name, const OmnXmlItemPtr &conf);
	~AosRandomSentence();

	virtual int 		nextInt();	
	virtual int 		nextInt(AosGenericDataContainer &data);	
	virtual int 		nextInt(const OmnString &name);	
	virtual int 		nextInt(const OmnString &name, AosGenericDataContainer &data);	
	virtual OmnString	nextStr();
	virtual OmnString	nextStr(const OmnString &name);
	virtual OmnString	nextStr(AosGenericDataContainer &data);
	virtual OmnString	nextStr(const OmnString &name, AosGenericDataContainer &data);

	static AosRandomSentencePtr create(const OmnString &name, const OmnXmlItemPtr &conf);

private:

};

#endif

