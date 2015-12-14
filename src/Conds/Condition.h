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
// This is a utility to select docs.
//
// Modification History:
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Conds_Condition_h
#define AOS_Conds_Condition_h

#include "Alarm/Alarm.h"
#include "Conds/Ptrs.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/XmlRandObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosCondition : public AosConditionObj 
{
private:
	OmnDefineRCObject;


public:
	AosCondition(
			const OmnString &name, 
			const AosCondType::E type, 
			const bool reg);
	~AosCondition();

	// Condition Interface
	virtual bool evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) = 0;
	virtual bool evalCond(const AosRundataPtr &rdata);
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	// virtual AosConditionObjPtr clone(const AosRundataPtr &rdata) {return clone();}
	virtual OmnString getXmlStr(
					const OmnString &tagname,
					const int level,
					const OmnString &then_part,
					const OmnString &else_part,
					const AosRandomRulePtr &rule,
	 				const AosRundataPtr &rdata);
	virtual OmnString generateCommand(
					const AosXmlTagPtr &sdoc, 
					const AosRundataPtr &rdata);

	//ken 2012/05/08
	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata){ return true; };
	virtual AosConditionObjPtr getCondition(const AosXmlTagPtr &conf, const AosRundataPtr &);

	static bool initConditions();
	static AosConditionObjPtr getCondition(const AosCondType::E type, const AosRundataPtr &);
	static bool evalCondStatic(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	static bool evalCondsStatic(
					const AosXmlTagPtr &def, 
					const int max, 
					const AosRundataPtr &rdata);
	static bool evalCondStatic(
			const AosXmlTagPtr &sdoc, 
			const OmnString &name,
			const AosRundataPtr &rdata);


	static OmnString getRandXml(
			const OmnString &tagname,
			AosRandomRulePtr &rule, 
			const int idx, 
			const AosRundataPtr &rdata);

	OmnString getXmlStr(
			const OmnString &tagname,
			const OmnString &then_part,
			const OmnString &else_part,
			const OmnString &type,
			const AosRandomRulePtr &rule, 
			const AosRundataPtr &rdata);

	OmnString getTagname(const OmnString &tagname, const AosRandomRulePtr &rule);
	void addHeader(
			OmnString &str,
			const OmnString &tagname,
			const OmnString &type,
			const AosRandomRulePtr &rule);
	void addThenElse(
			OmnString &str,
			const OmnString &then_part,
			const OmnString &else_part, 
			const OmnString &tagname);

private:
	bool registerCondition(const OmnString &name, const AosConditionObjPtr &selector);
};
#endif

