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
// This action assumes a doc:
// 	<doc ...>
// 		<record .../>
// 		<record .../>
// 		...
// 	</doc>
// It calculates the sum of some attributes in every <record>, and then
// saves the sum to another doc.
//
// Modification History:
// 12/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCalcSum.h"

#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ValueDefs.h"
#include "SEBase/SeUtil.h"
#include "Security/SessionMgr.h"
#include "Security/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/DynArray.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;

const int sgMaxAttrs = 30;


AosActCalcSum::AosActCalcSum(const bool flag)
:
AosSdocAction(AOSACTTYPE_CALCSUM, AosActionType::eCalcSum, flag)
{
}


AosActCalcSum::~AosActCalcSum()
{
}


bool
AosActCalcSum::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// 'rdata' args:
	// 		AOSARGNAME_DOC_MODIFIED: indicate whether the doc was modified.
	// 		AOSARGNAME_CALC_VALUE: the calculated value
	//
	rdata->setArg1(AOSARG_DOC_MODIFIED, "false");

	if (!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
		OmnAlarm << rdata->getErrmsg() << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 1. Retrieve the target doc
	AosXmlTagPtr target_doc = AosRunDocSelector(rdata, sdoc, AOSTAG_TARGET_DOC);
	if (!target_doc)
	{
		AosSetError(rdata, AOSLT_MISSING_TARGET_DOC);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 2. Retrieve the docs to process
	AosXmlTagPtr docs = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCS_TO_PROCESS);
	if (!docs)
	{
		AosSetError(rdata, AOSLT_MISSING_DOCS_TO_PROCESS);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 3. Retrieve the name of the attribute whose values are calculated
	OmnString value_anames = sdoc->getAttrStr(AOSTAG_VALUE_ANAMES);
	if (value_anames == "")
	{
		AosSetError(rdata, AOSLT_MISSING_VALUE_ANAME);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 4. Retrieve the separator
	OmnString sep = sdoc->getAttrStr(AOSTAG_SEPARATOR, ",");
	if (sep == "") sep = ",";

	// value_anames is a comma delimited string, containing one or more
	// attribute names.
	vector<OmnString> anames;
	int nn = AosStrSplit::splitStrBySubstr(value_anames, sep.data(), anames, sgMaxAttrs);
	aos_assert_rr(nn > 0, rdata, false);
	if (anames[0] == "")
	{
		AosSetError(rdata, AOSLT_MISSING_VALUE_ANAME);
		OmnAlarm << rdata->getErrmsg() << ": " << value_anames << enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 5. Retrieve the name of the attribute in which the sum is stored.
	OmnString stored_aname = sdoc->getAttrStr(AOSTAG_STORED_ANAME);
	if (stored_aname == "")
	{
		AosSetError(rdata, AOSLT_MISSING_STORED_ANAME);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 6. Calculate the sum
	AosXmlTagPtr doc = docs->getFirstChild();
	int64_t sum = 0;
	while (doc)
	{
		for (u32 i=0; i<anames.size(); i++)
		{
			if (anames[i].length() > 0)
			{
				int64_t vv = doc->getAttrInt64(anames[i], 0);
				sum += vv;
			}
		}
		doc = docs->getNextChild();
	}

	// 7. The results can be one of the following format:
	// 	a. Save the value
	// 	b. true (not 0) or false (0)
	// 	c. translated by the smart doc
	OmnString result_type = sdoc->getAttrStr(AOSTAG_RESULT_TYPE, AOSVALUE_SAVE_VALUE);
	if (result_type == AOSVALUE_SAVE_VALUE)
	{
		// It needs to save the sum to the target doc.
		int64_t dft_value = sdoc->getAttrInt64(AOSTAG_DEFAULT_VALUE, 0);
		int64_t ss = target_doc->getAttrInt64(stored_aname, dft_value);
		if (ss == sum)
		{
			// Nothing changed. Do nothing
			// AOSMONITORLOG_FINISH(rdata);
			rdata->setArg1(AOSARG_DOC_MODIFIED, "false");
			return true;
		}

		// Modify the doc
		target_doc->setAttr(stored_aname, sum);
		rdata->setArg1(AOSARG_DOC_MODIFIED, "true");
		rdata->setArg1(AOSARG_CALC_VALUE, sum);
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	if (result_type == AOSVALUE_TRUE_FALSE)
	{
		OmnString rslt = (sum)?"true":"false";
		OmnString rr = target_doc->getAttrStr(stored_aname, "false");
		if (rr == rslt)
		{
			// Nothing changed. Do nothing.
			// AOSMONITORLOG_FINISH(rdata);
			rdata->setArg1(AOSARG_DOC_MODIFIED, "false");
			return true;
		}

		// Value changed. Need to modify the doc. 
		target_doc->setAttr(stored_aname, rslt);
		rdata->setArg1(AOSARG_DOC_MODIFIED, "true");
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	if (result_type == AOSVALUE_VALUE_MAP)
	{
		// The result is mapped to another value.
		// It uses a value selector to map the value. 
		OmnString dd;
		dd << sum;
		AosValueRslt vv(dd);
		bool rslt = AosValueSel::getValueStatic(vv, sdoc, AOSTAG_VALUE_MAP, rdata);
		if (!rslt)
		{
			// AOSMONITORLOG_FINISH(rdata);
			return false;
		}

		OmnString rr = target_doc->getAttrStr(stored_aname);
		if (vv.getStr() == rr)
		{
			// Value not changed. Do nothing.
			// AOSMONITORLOG_FINISH(rdata);
			rdata->setArg1(AOSARG_DOC_MODIFIED, "false");
			return true;
		}

		// Value changed. Need to modify the doc. 
		OmnString actual_value = vv.getStr();
		target_doc->setAttr(stored_aname, actual_value);
		rdata->setArg1(AOSARG_DOC_MODIFIED, actual_value);
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	AosSetError(rdata, "invalid_result_type:") << result_type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	// AOSMONITORLOG_FINISH(rdata);
	return false;
}


AosActionObjPtr
AosActCalcSum::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCalcSum(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


