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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_HtmlDataView_h
#define Aos_GICs_HtmlDataView_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "HtmlModules/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"
#include <vector>
#include <boost/regex.hpp>
#include "HtmlModules/HandleImage.h"

using boost::regex;

class AosGicHtmlDataView : public AosGic
{

public:
	AosGicHtmlDataView(const bool flag);
	~AosGicHtmlDataView();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
			
private:
	
	enum{
		eMaxHtmlCodeLength = 100000	
	};

	OmnString
	generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr);


	OmnString
	getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft);
	
	OmnString
	createHtmlCode(const OmnString &temp, const AosXmlTagPtr &record);
	static bool
	isInt(const OmnString &temp);
	OmnString
	handdleStartImg(const OmnString &html, const AosHtmlReqProcPtr &htmlPtr);
//	OmnString
//	changeImgSrc(int height, int width, const OmnString& src);
//	bool processImage(OmnString& htmlcode);
//	void setEndImage(OmnString& htmlcode);
};

#endif
