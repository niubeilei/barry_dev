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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlLayoutMgr_Layout_h
#define Aos_HtmlLayoutMgr_Layout_h

#include "HtmlLayoutMgr/HtmlLayoutTypes.h"
#include "HtmlLayoutMgr/Ptrs.h"
#include "HtmlServer/HtmlReqProc.h"
#include "HtmlServer/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "HtmlUtil/HtmlUtil.h"


class AosLayout : virtual public OmnRCObject 
{
	OmnDefineRCObject;

protected:
	AosHtmlLayoutType		mLayoutType;
	AosXmlTagPtr	mVpd;

public:
	AosLayout();
	AosLayout(const AosXmlTagPtr &vpd);
	~AosLayout();

	virtual bool	
	generateLayoutCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight
	) = 0;
	virtual AosHtmlLayoutType	getLayoutType() const {return mLayoutType;}

	bool
	registerLayout(
		const AosHtmlLayoutType layOutType, 
		const AosLayoutPtr &layout);

	static AosLayoutPtr	mLayouts[eAosHtmlLayoutType_Max];

	static int 
	createLayoutVpd(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		AosXmlTagPtr &obj, 
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight);
};

#endif

