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
// 2011/02/18: Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlMenu_h
#define Aos_GICs_GicHtmlMenu_h

#include "GICs/GIC.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <iostream>
#include <boost/regex.hpp>
using namespace boost ;
using namespace std ;



class AosGicHtmlMenu : public AosGic
{
	OmnDefineRCObject ;

public:
	AosGicHtmlMenu(const bool flag);
	~AosGicHtmlMenu();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
		
private:
	OmnString ParseDate(AosXmlTagPtr &cont,OmnString &id, bool flage ,OmnString &cla);
	OmnString replace(OmnString &cont);
	string& replaceTow(string &cont);
};

#endif
