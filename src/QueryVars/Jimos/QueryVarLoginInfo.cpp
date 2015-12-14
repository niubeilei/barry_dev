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
// Modification History:
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryVars/Jimos/QueryVarLoginInfo.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosQueryVarLoginInfo_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQueryVarLoginInfo(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosQueryVarLoginInfo::AosQueryVarLoginInfo(const int version)
:
AosQueryVar("login_info", version)
{
}


AosQueryVarLoginInfo::~AosQueryVarLoginInfo()
{
}


bool
AosQueryVarLoginInfo::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosQueryVarLoginInfo::eval(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		AosValueRslt &value)
{
	// The 'def' format:
	// 	<var var_type="literal|var|func"
	// 		var_name="xxx"
	// 		member_name="xxx"
	// 		default_policy="AOSVALUE_USEDEFAULT | AOSVALUE_IGNORE | AOSVALUE_AS_IS">
	// 		<default>xxx</default>
	// 	</var>
	AosXmlTagPtr logindoc = AosGetDocByCloudid(rdata->getCid(), rdata);
	if (!logindoc)
	{
		AosSetEntityError(rdata, "sysdefvallogininfo_logindoc_not_found", "QueryVar", "")
			<< enderr;
		return false;
	}

	OmnString member_name = def->getAttrStr("member_name");
	if (member_name == "")
	{
		AosSetEntityError(rdata, "sysdefvallogininfo_invalid_name", "QueryVar", "")
			<< enderr;
		return false;
	}

	value.setStr(logindoc->xpathQuery(member_name));

	if (value != "" && value != AOS_NULL_STRING) return true;

	return procDefault(rdata, def, value);
}


AosJimoPtr
AosQueryVarLoginInfo::cloneJimo()  const
{
	try
	{
		return OmnNew AosQueryVarLoginInfo(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

