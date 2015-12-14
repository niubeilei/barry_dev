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
// 01/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StAccess_h
#define AosSengTorUtil_StAccess_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
//#include "SecUtil/AccessType.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/HashMap.h"
#include "XmlUtil/Ptrs.h"


class AosStAccess : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef AosHashMap<OmnString, AosStAccessPtr, Omn_Str_hash, compare_str, 1000> Hash_t;
	typedef AosHashMap<OmnString, AosStAccessPtr, Omn_Str_hash, compare_str, 1000>::iterator HashItr_t;

	static Hash_t 		smAccessMap;
	OmnString			mName;
	AosStAccessPtr		mPrev;
	AosStAccessPtr		mNext;

public:
	AosStAccess(const OmnString &name, const bool flag);
	~AosStAccess() {}

	virtual bool checkAccessByOpr(
					const AosXmlTagPtr &def,
					const AosStDocPtr &local_doc,
					bool &granted, 
					bool &denied,
					const AosSengTestThrdPtr &thread) = 0;
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const AosSengTestThrdPtr &thread) = 0;

	static bool checkAccessByOprStatic(
					const AosXmlTagPtr &def,
					const AosStDocPtr &local_doc,
					bool &granted, 
					bool &denied,
					const AosSengTestThrdPtr &thread);

	AosStAccessPtr	getNext() const {return mNext;}
	AosStAccessPtr	getPrev() const {return mPrev;}
	static OmnString randAccessXmlStr(
					const OmnString &tagname, 
					const AosSengTestThrdPtr &thread);

private:
	bool registerAccess(const AosStAccessPtr &access);
};
#endif

