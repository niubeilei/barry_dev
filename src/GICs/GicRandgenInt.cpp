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
// 26/08/2010: Created by ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicRandgenInt.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicRandgenInt();

AosGicRandgenInt::AosGicRandgenInt(const bool flag)
:
AosGic(AOSGIC_RANDGENINT, AosGicType::eRandgenInt, flag)
{
}


AosGicRandgenInt::~AosGicRandgenInt()
{
}


bool	
AosGicRandgenInt::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString mebname = vpd->getAttrStr("meb_name");
   	OmnString rangesStr = "[";
   	AosXmlTagPtr range = vpd->getFirstChild();
   	while(range)
   	{
   	   	rangesStr << "{min:" << range->getAttrStr("min") << ", max:" << range->getAttrStr("max") << ", weight:" << range->getAttrStr("weight") << "}";
	   	range = vpd->getNextChild();
	   	if(range)
	   	{
	   	    rangesStr << ",";
	   	}
   	}
   	rangesStr << "]";

   	code.mJson << ",rangesStr: \"" << rangesStr << "\","
	   		   << "hasNoContent:true";
	if(mebname != "")
	{
		code.mJson << ",name:\"" << mebname << "\"";
	}
   
	return true;
}
