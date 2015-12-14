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
// 05/10/2012 Created by Chen Ding
// 2012/11/12 Moved From IILUtil By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILScanner_IILValueType_h
#define Aos_IILScanner_IILValueType_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"


#define AOSIILVALUETYPE_COMPOSE				"compose"
#define AOSIILVALUETYPE_INVALID				"invalid"
#define AOSIILVALUETYPE_KEY					"key"
#define AOSIILVALUETYPE_POSTFIX				"postfix"
#define AOSIILVALUETYPE_PREFIX				"prefix"
#define AOSIILVALUETYPE_SUBSTRING			"substr"
#define AOSIILVALUETYPE_VALUE				"value"
#define AOSIILVALUETYPE_VALUESEL			"valuesel"

class AosValueRslt;
class AosIILSelector;

class AosIILValueType
{
public:
	enum E
	{
		eInvalid,

		eKey,
		eValue,
		ePrefix,
		ePostfix,
		eSubstr,
		eCompose,
		eValueSel,

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	static E toEnum(const OmnString &name);
	static OmnString toString(const E code);
	static bool getValue(
					const AosIILSelector &selector,
					AosValueRslt &result,
					const char *key, 
					const int key_len,
					const u64 &docid, 
					const AosIILScannerObjPtr &scanner,
					const AosRundataPtr &rdata);
};
#endif
