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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelQuery_h
#define Aos_ValueSel_ValueSelQuery_h

#include "ValueSel/ValueSel.h"

class AosValueSelQuery : public AosValueSel
{
public:

	enum
	{
		eMaxConds = 20,
		eDftPageSize = 100
	};
	
	struct AosQueryCond
	{
		public:
		OmnString		mCtnrObjid;
		OmnString		mAttrName;
		OmnString		mValue;
		AosOpr			mOpr;
		bool			mReverse;
		bool			mOrder;
		
		public:
		AosQueryCond(){}
		~AosQueryCond(){}
		void reset()
		{
			mCtnrObjid = "";
			mAttrName = "";
			mValue = "";
			mOpr = eAosOpr_Invalid;
			mReverse = false;
			mOrder = false;
		}
	};

public:
	AosValueSelQuery(const bool reg);
	AosValueSelQuery(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelQuery();

	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata);
	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);
};
#endif

