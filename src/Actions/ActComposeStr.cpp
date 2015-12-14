////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/16/2011	Created by Tracy Huang
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActComposeStr.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"

#include "ValueSel/ValueSel.h"

using namespace std;

AosActComposeStr::AosActComposeStr(const bool flag)
:
AosSdocAction(AOSACTTYPE_COMPOSESTR, AosActionType::eComposeStr, flag)
{
}

/*
AosActComposeStr::AosActComposeStr(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eComposeStr, false)
{
	if(!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}
*/

AosActComposeStr::~AosActComposeStr()
{
}


AosActionObjPtr
AosActComposeStr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActComposeStr(false);
	}
	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActComposeStr::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function composes a string. 'sdoc' defines how to compose 
	// the string:
	// 	<sdoc ...>
	// 		<substrs>
	// 			<substr .../>
	// 			<substr .../>
	// 			...
	//		</substrs>
	//		<setresult .../>
	//	</sdoc>
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		AosSetError(rdata, "smartdoc_missing_smartdoc") << enderr;
		return false;
	}

	AosXmlTagPtr substrs = sdoc->getFirstChild("substrs");
	if (!substrs)
	{
		AosSetErrorUser(rdata, "actcomposestr_missing_substrs") << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr xmltag = substrs->getFirstChild();
	OmnString results;
	AosValueRslt value;
	OmnString vv;
	while (xmltag)
	{
		bool rslt = AosValueSel::getValueStatic(value, xmltag, rdata);
		aos_assert_r(rslt, false);
		vv = value.getStr();
		xmltag = substrs->getNextChild();
	}
	// rdata->setResults(results);
	rdata->setResults(results);
	return true;
}


