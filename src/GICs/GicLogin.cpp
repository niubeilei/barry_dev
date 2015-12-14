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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicLogin.h"
#include "HtmlServer/HtmlUtil.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicLogin();

AosGicLogin::AosGicLogin(const bool flag)
:
AosGic(AOSGIC_LOGIN, AosGicType::eLogin, flag)
{
}


AosGicLogin::~AosGicLogin()
{
}


bool	
AosGicLogin::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code

	OmnString usergrp = vpd->getAttrStr("gic_usergrp");
	OmnString loginbgc = vpd->getAttrStr("gic_loginbgc");
	OmnString loginfsize = vpd->getAttrStr("gic_loginfsize");
	OmnString loginfcolor = vpd->getAttrStr("gic_loginfcolor");
	OmnString registerbgc = vpd->getAttrStr("gic_registerbgc");
	OmnString registerfsize = vpd->getAttrStr("gic_registerfsize");
	OmnString registerfcolor = vpd->getAttrStr("gic_registerfcolor");
	OmnString gic_registervpd = vpd->getAttrStr("gic_registervpd");
	OmnString rgstbflag = vpd->getAttrStr("rgstbflag", "false");
	OmnString align_type = vpd->getAttrStr("align_type", "horizontal");
	OmnString str;
	str 
		<< ",gic_registervpd:\'" << gic_registervpd <<"\'"
		<< ",gic_loginbgc:\'" << loginbgc <<"\'"
		<< ",gic_loginfsize:\'" << loginfsize <<"\'"
		<< ",gic_loginfcolor:\'" << loginfcolor <<"\'"
		<< ",gic_registerbgc:\'" << registerbgc <<"\'"
		<< ",gic_registerfsize:\'" << registerfsize <<"\'"
		<< ",gic_registerfcolor:\'" << registerfcolor <<"\'"
		<< ",align_type:\'" << align_type <<"\'"
		<< ",rgstbflag:" << rgstbflag <<""
		<< ",gic_usergrp:\'" << usergrp <<"\'";

	code.mHtml <<"<div class=\"signin\">登 录</div><div class=\"register\">注 册</div><div class=\"drop\" style=\"background-color: #38E4FF\"><label class=\"login_label\" for=\"name\">用户名:</label><input class=\"login_input\" type=\"text\" name=\"uname\" /><label class=\"login_label\" for=\"password\">密 码:</label><input class=\"login_input\" type=\"password\" name=\"passwd\" /><input type=\"button\" class=\"submit\" value=\"Sign In\" name=\"submit\" /></div>";
	code.mJson << str;
	return true;
}

