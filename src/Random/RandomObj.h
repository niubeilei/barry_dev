////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomObj.h
// An AosRandom defines a random object generator. The object itself is
// defined through an XML document. This random object is able to generate
// random values such as random integers or strings based on a pattern. 
//
// This is a super class. All random objects are derived from this class. 
//
// Modification History:
// 11/22/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_RandomObj_h
#define Omn_Random_RandomObj_h

#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "RVG/ReturnCode.h"
#include "Util/RCObject.h"
#include "Util/StrParser.h"
#include "aosUtil/Types.h"
#include "XmlParser/XmlItem.h"

class AosGenTable;

class AosRandomObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum RandomObjType 
	{
		eSelector,
		eIntegerSelector, 
		eStringSelector,
		eLenStringSelector
	};

protected:
	OmnString		mName;
	RandomObjType	mType;	
	bool			mIsGood;
	u32				mBase;

public:
	AosRandomObj();
	AosRandomObj(const OmnString &name);
	~AosRandomObj();

	static AosRandomObjPtr	createRandomObj(const OmnString &conf);
	static AosRandomObjPtr	createRandomObj(const OmnXmlItemPtr &conf);
	static bool retrieveName(const OmnXmlItemPtr &conf, OmnString &name);
	static bool retrieveType(const OmnXmlItemPtr &conf, RandomObjType &type);

	bool		isGood() const {return mIsGood;}

	// 
	// Random Generation Functions
	//
	virtual int			nextInt() = 0;
	virtual int			nextInt(AosGenTable &data) = 0;
	virtual int 		nextInt(const OmnString &name) = 0;
	virtual int 		nextInt(const OmnString &name, AosGenTable &data) = 0;
	virtual OmnString	nextStr() = 0;	
	virtual OmnString	nextStr(AosGenTable &data) = 0;	
	virtual OmnString	nextStr(const OmnString &name) = 0;
	virtual OmnString	nextStr(const OmnString &name, AosGenTable &data) = 0;

	virtual OmnString	nextIncorrectStr(
							AosRVGReturnCode &rcode,
							OmnString &errmsg);

	// 
	// Utility Member Functions
	//
	bool expect(OmnStrParser1 &parser, char c, const OmnString &contents, const OmnXmlItemPtr &conf);
	bool getInt(OmnStrParser1 &parser, 
				int64_t &value, 
				const OmnString &message,
				const OmnString &contents,  
				const OmnXmlItemPtr &conf);
	bool getStr(OmnStrParser1 &parser, 
				OmnString &value, 
				const OmnString &message, 
				const OmnString &contents,
				const OmnXmlItemPtr &conf);
	
	bool finalCheck(const OmnXmlItemPtr &conf, const int weightTotal);
	bool checkWeight(int weight, const OmnXmlItemPtr &conf);
	u32	 getBase() const {return mBase;}

};

#endif

