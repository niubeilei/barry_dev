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
#ifndef Aos_GICs_gic_h
#define Aos_GICs_gic_h

#include "GICs/GicTypes.h"
#include "GICs/Ptrs.h"
#include "HtmlServer/HtmlReqProc.h"
#include "HtmlServer/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "SEUtil/Ptrs.h"
#include "HtmlUtil/HtmlUtil.h"
#include "SEUtil/XmlRandObj.h"


class AosGic : virtual public OmnRCObject , virtual public AosRandXmlObj 
{
	OmnDefineRCObject;
public:
	
	static AosGicPtr	mGics[AosGicType::eMax];

protected:
	AosGicType::E	mGicType;
	AosXmlTagPtr	mVpd;
	OmnString		mName;

public:
	AosGic(const OmnString &name, const AosGicType::E type, const bool flag);
	AosGic(const AosXmlTagPtr &vpd);
	~AosGic();

	virtual bool generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code
	) = 0;

	virtual bool getFramerCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code
	){return true;};

	// Felicia, 09/24/2011
	// virtual bool retrieveContainers(
	// 			   	const AosHtmlReqProcPtr &htmlPtr,
	// 			    AosXmlTagPtr &vpd,
	// 				const AosXmlTagPtr &obj,
	// 				const OmnString &parentid,
	// 				AosHtmlCode &code);

	AosGicType::E	getGicType() const {return mGicType;}
	OmnString		getGicName() const {return mName;}

	bool registerGic(
			const OmnString &name,
			const AosGicPtr &gic, 
			OmnString &errmsg);

	static bool createGic(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentId,
		AosHtmlCode &code,
		const int p_width,
		const int p_height);

	static AosGicPtr getGic(const AosGicType::E type);
	OmnString getXmlStr(
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata) {return "";}
};

#endif

