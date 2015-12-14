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
#include "QueryVars/Jimos/QueryVarClipvar.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosQueryVarClipvar_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQueryVarClipvar(version);
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


AosQueryVarClipvar::AosQueryVarClipvar(const int version)
:
AosQueryVar("login_info", version)
{
}


AosQueryVarClipvar::~AosQueryVarClipvar()
{
}


bool
AosQueryVarClipvar::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosQueryVarClipvar::eval(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		AosValueRslt &value)
{
	// The 'def' format:
	// 	<var var_name="clipvar"
	// 		member_name="xxx"
	// 		default_policy="AOSVALUE_USEDEFAULT | AOSVALUE_IGNORE | AOSVALUE_AS_IS"
	// 		<default>xxx</default>
	// 	</var>
	OmnString member_name = def->getAttrStr("member_name");
	if (member_name == "")
	{
		AosSetEntityError(rdata, "sysdefvallogininfo_invalid_name", "QueryVar", "")
			<< enderr;
		return false;
	}

	// Currently, clipvars are not supported.
	OmnNotImplementedYet;
	value.reset();
	return false;
}


AosJimoPtr
AosQueryVarClipvar::cloneJimo()  const
{
	try
	{
		return OmnNew AosQueryVarClipvar(*this);
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
				OmnString cid = rdata->getCid();
				if (cid != "")
				{
					value.setLength(len - 9);
					value << "." << cid;
				}
			}
		}
	}	
	goto dft_proc;

dft_proc:

}
*/
