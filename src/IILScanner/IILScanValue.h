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
// 2012/11/12 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILScanner_IILScanValue_h
#define Aos_IILScanner_IILScanValue_h

#include "IILScanner/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include <vector>
using namespace std;

#define AOSIILSCANVALUE_ALL		"all"
#define AOSIILSCANVALUE_AVG		"avg"
#define AOSIILSCANVALUE_FIRST	"first"
#define AOSIILSCANVALUE_LAST	"last"
#define AOSIILSCANVALUE_MAX		"max"
#define AOSIILSCANVALUE_MIN		"min"
#define AOSIILSCANVALUE_SUM		"sum"
#define AOSIILSCANVALUE_INVALID	"invalid"

class AosIILScanValue : virtual public OmnRCObject 
{
	OmnDefineRCObject;
	
public:
	enum E
	{
		eAll,
		eAvg,
		eFirst,
		eLast,
		eMax,
		eMin,
		eSum,

		eInvalid	
	};

private:
	E 		mType;

public:
	AosIILScanValue(
		const E type,
		const bool regist_flag);
	~AosIILScanValue();

	static void	init();
	static bool	registerScanValue(
					const AosIILScanValue::E type,
					AosIILScanValue * scan);

	static E to_enum(const OmnString &type);
	static OmnString to_string(const E type);

	static AosIILScanValuePtr createIILScanValue(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);
	
	virtual AosIILScanValuePtr clone(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata) = 0;
	
	virtual bool scanValue(
					vector<OmnString> &keys,
					vector<u64> &values,
					vector<OmnString> &out_keys,
					vector<u64> &out_values,
					const AosRundataPtr &rdata) = 0;

private:
	virtual bool config(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata) = 0;
};
#endif
