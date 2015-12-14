///////////////////////////////////////////////////////////////////////////
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
// 08/25/2010: Created by John Huang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicUpLoader.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "GICs/GIC.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicUpLoader();

AosGicUpLoader::AosGicUpLoader(const bool flag)
:
AosGic(AOSGIC_UPLOADER, AosGicType::eUpLoader, flag)
{
}


AosGicUpLoader::~AosGicUpLoader()
{
}


bool	
AosGicUpLoader::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	int gic_layer	= vpd->getAttrInt("gic_layer",1);
	int uld_maxsize = vpd->getAttrInt("uld_maxsize",100000);
	int uld_cntrd 	= vpd->getAttrInt("uld_cntrd",16);
	OmnString uld_opr 		= 	vpd->getAttrStr("uld_opr","uploadImg"); 
	OmnString uld_folder 	= 	vpd->getAttrStr("uld_folder","tmp"); 
	OmnString uld_fnbind 	= 	vpd->getAttrStr("uld_fnbind",""); 
	OmnString uld_pctnrs 	= 	vpd->getAttrStr("uld_pctnrs",""); 
	OmnString uld_tags 		= 	vpd->getAttrStr("uld_tags","notags"); 
	OmnString zky_crtor	 	= 	vpd->getAttrStr("zky_crtor","guest");
	OmnString zky_target 	= 	vpd->getAttrStr("zky_target","");
	OmnString zky_file_map_vpd =    vpd->getAttrStr("zky_file_map_vpd","tmp_file");
	OmnString gic_ename =    vpd->getAttrStr("gic_ename","");
	OmnString gic_uploadjsp =    vpd->getAttrStr("gic_uploadjsp","");
	
	OmnString str;
	str	<<	","
		<< 	"gic_layer:"	<< 	gic_layer 	<<	","
		<< 	"uld_cntrd:" 	<< 	uld_cntrd 	<<	","
		<< 	"uld_maxsize:" 	<< 	uld_maxsize <<	","
		<< 	"uld_opr:'" 	<< 	uld_opr 	<<	"',"
		<<	"uld_folder:'"	<<	uld_folder 	<<	"'," 
		<<	"uld_fnbind:'"	<<	uld_fnbind 	<<	"'," 
		<<	"uld_pctnrs:'"	<<	uld_pctnrs 	<<	"'," 
		<<	"uld_tags:'"	<<	uld_tags	<<	"'," 
		<<	"zky_target:'"	<<	zky_target	<<	"'," 
		<<  "zky_file_map_vpd:'" << zky_file_map_vpd << "',"
		<<  "gic_ename:'" << gic_ename << "',"
		<<  "gic_uploadjsp:'" << gic_uploadjsp << "',"
		<< 	"zky_crtor:'" 	<< 	zky_crtor	<<	"'";
	code.mJson << str;
	return true;
}
