////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/14/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/DataStructsUtil.h"

#include "API/AosApi.h"
#include "DataStructs/StructProc.h"
#include "DocClient/DocidMgr.h"
#include "SEInterfaces/StructProcObj.h"


int 
AosDataStructsUtil::getVirtualIdByStatid(
		const u64 &stat_id,
		const int docs_per)
{
	u32 groupid = stat_id / docs_per;
	int vid = groupid % AosGetNumCubes();
	return vid;
}


int 
AosDataStructsUtil::getVirtualIdByGroupid(const u64 &groupid)
{
	int vid = groupid % AosGetNumCubes();
	return vid;
}


AosXmlTagPtr
AosDataStructsUtil::pickStructProcJimoDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	OmnString type = def->getAttrStr("type", "");
	aos_assert_r(type != "", 0);

	if (type == "count") type = AOSSTRUCTPROC_SUM;
	if (type == "dist_count") type = AOSSTRUCTPROC_SUM;	// Ketty 2014/06/02
	if (type == "dist_count_new") type = AOSSTRUCTPROC_SUM;	

	OmnString classname = AosStructProc::getJimoClassname(type);
	aos_assert_r(classname != "", 0);

	OmnString objid = "datastructproc_";
	objid << type << "_jimodoc_v0";
	aos_assert_r(objid != "", 0);

	OmnString jimo_config = "<jimo ";
	jimo_config << "zky_classname=\"" << classname << "\" "
		<< "jimo_name=\"jimo_structproc\" " 
		<< "jimo_type=\"jimo_structproc\" " 
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_JIMO << "\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" " 
		<< "current_version=\"0\">"
		<< "<versions>"
		<< "<ver_0>libStructProcJimos.so</ver_0>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata.getPtr(), jimo_config AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, 0);
	return jimo_doc;
}



u64 
AosDataStructsUtil::nextSysDocid(
		const AosRundataPtr &rdata,
		const int vid)
{
	OmnString objid;
	u64 did =  AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
//OmnScreen << "NextSysDocid; docid:" << did << ";" << endl;
	return did;
}
