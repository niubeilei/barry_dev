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
// 07/16/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlLayoutMgr_AutoLayout_h
#define Aos_HtmlLayoutMgr_AutoLayout_h

#include "HtmlLayoutMgr/Layout.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosAutoLayout : public AosLayout
{

public:
	AosAutoLayout();
	~AosAutoLayout();

	virtual bool	
	generateLayoutCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		AosXmlTagPtr &obj, 
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight);
};


#endif

