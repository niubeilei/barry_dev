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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSel_h
#define Aos_ValueSel_ValueSel_h

#include "MultiLang/LangTermIds.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/XmlRandObj.h"
#include "SEInterfaces/ValueSelObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DataTypes.h"
#include "Util/HashUtil.h"
#include "ValueSel/VsTypes.h"
#include "ValueSel/Ptrs.h"
#include "XmlInterface/XmlRc.h"

class AosValueSel : public AosValueSelObj
{
	OmnDefineRCObject;
public:
	enum
	{
		eDftLevel = 5,
	};

public:
	AosValueSel(
			const OmnString &name, 
			const AosValueSelType::E type, 
			const bool reg);
	~AosValueSel();

	// AosValueSel interface
	// Chen Ding, 2013/02/21
	// virtual bool run(
	// 				AosValueRslt &valueRslt,
	// 				const AosXmlTagPtr &item, 
	// 				const AosRundataPtr &rdata) = 0;

	virtual bool run(
					AosValueRslt &value, 
					const char *record, 
					const int record_len, 
					const AosRundataPtr &rdata);

	virtual bool run(AosValueRslt &valueRslt, const AosRundataPtr &rdata);

	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);

	// static bool getValueStatic(
	// 				AosValueRslt &valueRslt,
	// 				const AosXmlTagPtr &sdoc, 
	// 				const AosRundataPtr &rdata);

	static bool getValueForCounterVarStatic(
					const OmnString &str,
					OmnString &cvar,
					const AosRundataPtr &rdata);


	static OmnString getRandXmlStrStatic(
					const OmnString &tagname, 
					const int level,
					AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);

	AosValueSelType::E getType() const {return mType;}
	static OmnString getTagnameStatic(
					const OmnString &tagname, 
					const AosRandomRulePtr &rule);	
	void addHeader(
					OmnString &str,
					const OmnString &tagname,
					const OmnString &type,
					const AosRandomRulePtr &rule);

public:
	bool setValueRange(const AosValueRslt &start, const AosValueSel &end);

	// Chen Ding, 2013/02/21
	static bool init();
};
#endif

