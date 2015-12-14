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
// 07/26/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlCheckboxGrp_h
#define Aos_GICs_GicHtmlCheckboxGrp_h
			
#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicHtmlCheckboxGrp : public AosGic
{
	OmnDefineRCObject;

public:
	AosGicHtmlCheckboxGrp(const bool flag);
	~AosGicHtmlCheckboxGrp();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
                  
    bool
	doLayout(
	    const AosXmlTagPtr &vpd,
		AosHtmlCode &code);
    bool	
    createJsonCode(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code);
	OmnString 
	setStyle(const OmnString &style);
	OmnString getXml(const AosRandomRulePtr &rule);
private:
	AosXmlTagPtr        mData;
	OmnString           mDataSource;
	OmnString           mLabel;
	OmnString           mValue;
	OmnString           mTip;
	OmnString           mFontStyle;
	OmnString           mFontSize;
	OmnString           mColor;
	OmnString           mObj;
	OmnString           mDefCheckbox;
	OmnString           mOther;
	int                 mCsp;
	int                 mRsp;
};

#endif

