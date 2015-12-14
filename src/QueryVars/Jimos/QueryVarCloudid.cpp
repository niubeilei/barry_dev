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
#include "QueryVars/Jimos/QueryVarCloudid.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosQueryVarCloudid_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQueryVarCloudid(version);
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


AosQueryVarCloudid::AosQueryVarCloudid(const int version)
:
AosQueryVar("login_info", version)
{
}


AosQueryVarCloudid::~AosQueryVarCloudid()
{
}


bool
AosQueryVarCloudid::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosQueryVarCloudid::eval(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		AosValueRslt &value)
{
	// The 'def' format:
	// 	<var var_name="cloudid"
	// 		prefix="xxx"
	// 		member_name="$cloudid"
	// 		default_policy="AOSVALUE_USEDEFAULT | AOSVALUE_IGNORE | AOSVALUE_AS_IS"
	// 		<default>xxx</default>
	// 	</var>

	OmnString prefix = def->getAttrStr("prefix");
	if (prefix == "")
	{
		AosSetError(rdata, "queryvarpostfix_missing_value") << def << enderr;
		return false;
	}

	OmnString cid = rdata->getCid();
	if (cid != "")
	{
		prefix << "." << cid;
		value.setStr(prefix);
		return true;
	}

	// The user does not have a cloud id.
	return procDefault(rdata, def, value);
}


AosJimoPtr
AosQueryVarCloudid::cloneJimo()  const
{
	try
	{
		return OmnNew AosQueryVarCloudid(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


/*
void 
AosQrUtil::procQueryVars(
			OmnString &value, 
			const OmnString &dftproc, 
			const OmnString &dftvalue, 
			bool &ignore,
			const AosRundataPtr &rdata)
{
	ignore = false;
	int len = value.length();
	const char *data = value.data();
	//if (len == 24 && value == "logininfo/zky_cloudid__a")
	bool exist;

	if (len > 8 && strncmp(data, "clipvar/", 8) == 0)
	{
		// This means the data is configured to listen to the 
		// clipvar. Will ignore it.
		value = "";
		goto dft_proc;
	}

	// Check the form: var.$cloudid
	if (value != "")
	{
		int idx = value.findSubString(AOSVALUE_CLOUDID_VAR, 0);
		if (idx != -1)
		{
			int strlen = len - 9;
			OmnString substr = value.substr(strlen, len - 1);
			if (len > 9 && strcmp(substr.data(), AOSVALUE_CLOUDID_VAR) == 0)
			{
				}
			}
		}
	}	
	goto dft_proc;

dft_proc:

}
*/
