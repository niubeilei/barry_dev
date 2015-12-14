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
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_DocSelector_h
#define AOS_DocSelector_DocSelector_h

#include "DocSelector/DocSelectorType.h"
#include "Alarm/Alarm.h"
#include "DocSelector/Ptrs.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosDocSelector : public AosDocSelObj
{
	OmnDefineRCObject;

private:
	AosDocSelectorType::E	mType;
	OmnMutexPtr				mLock;

public:
	enum
	{
		eMax = 100
	};

	AosDocSelector(
			const OmnString &name, 
			const AosDocSelectorType::E type, 
			const bool reg);
	~AosDocSelector();

	virtual OmnString getXmlStr(
	 		const OmnString &tagname,
	 		const int level,
	 		const AosRandomRulePtr &rule,
	 		const AosRundataPtr &rdata) = 0;

	AosDocSelectorPtr createSelector(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	static AosXmlTagPtr selectDocStatic(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	static AosXmlTagPtr selectDocStatic(
	 		const AosXmlTagPtr &sdoc, 
	 		const OmnString &name,
	 		const AosRundataPtr &rdata);

	static AosDocSelectorPtr getSelectorStatic(
			const AosDocSelectorType::E type, 
			const AosRundataPtr &rdata);

	AosDocSelectorType::E getType() const
	{
		return mType;
	}

private:
	bool registerSelector(const OmnString &name, const AosDocSelectorPtr &selector);

protected:
	void addHeader( OmnString &str,
					const OmnString &tagname,
					const OmnString &type, 
					const AosRandomRulePtr &rule);
};
#endif

