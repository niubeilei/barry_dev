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
// It counts the occurences of the sum of some attributes in every <record>, and then
// saves the sum to another doc.
//
// Modification History:
// 12/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCountOccurences.h"

#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
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
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;

const int sgMaxAttrs = 30;


AosActCountOccurences::AosActCountOccurences(const bool flag)
:
AosSdocAction(AOSACTTYPE_COUNT_OCCU, AosActionType::eCountOccur, flag)
{
}


AosActCountOccurences::~AosActCountOccurences()
{
}


bool
AosActCountOccurences::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
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

	// 3. Retrieve the counters
	OmnString counters = sdoc->getAttrStr(AOSTAG_COUNTERS);
	if (counters == "")
	{
		AosSetError(rdata, AOSLT_MISSING_COUNTERS);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 4. Retrieve the pair separator
	OmnString entry_sep = sdoc->getAttrStr(AOSTAG_ENTRY_SEP, "|$$|");
	if (entry_sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_ENTRY_SEPARATOR);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 5. Retrieve the field separator
	OmnString field_sep = sdoc->getAttrStr(AOSTAG_FIELD_SEP, "|$|");
	if (field_sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FIELD_SEPARATOR);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// Counters are in the following format:
	// 	aname|$|value|$|cname|$|type|$$|aname|$|value|$|cname|$|type...
	OmnStrParser1 parser(counters, entry_sep.data());
	OmnString pair;
	vector<OmnString> anames, values, cnames, types;
	vector<OmnString> parts;
	while ((pair = parser.nextWord()) != "" && anames.size() < eMaxFields)
	{
		int nn = AosStrSplit::splitStrBySubstr(pair, field_sep.data(), parts, 4);
		if (nn == 4)
		{
			OmnString nn = parts[0];
			if (nn != "")
			{
				anames.push_back(nn);
				values.push_back(parts[1]);
				cnames.push_back(parts[2]);
				types.push_back(parts[3]);
			}
		}
	}

	if (anames.size() <= 0)
	{
		AosSetError(rdata, AOSLT_MISSING_COUNTERS);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 6.  Do the counting
	int64_t counts[anames.size()];
	for (u32 i=0; i<anames.size(); i++) counts[i] = 0;
	AosXmlTagPtr doc = docs->getFirstChild();
	while (doc)
	{
		for (u32 i=0; i<anames.size(); i++)
		{
			if (anames[i].length() > 0)
			{
				OmnString vv = doc->getAttrStr(anames[i]);
				if (vv == values[i]) counts[i]++;
			}
		}
		doc = docs->getNextChild();
	}

	bool doc_modified = false;
	for (u32 i=0; i<anames.size(); i++)
	{
		// The results can be one of the following format:
		// 	a. Save the value
		// 	b. true (not 0) or false (0)
		// 	c. translated by the smart doc
		if (anames[i] == "") continue;

		if (types[i] == AOSVALUE_SAVE_VALUE)
		{
			// It needs to save the sum to the target doc.
			OmnString ss = target_doc->getAttrStr(cnames[i]);
			if (ss != values[i])
			{
				target_doc->setAttr(cnames[i], values[i]);
				doc_modified = true;
			}
			continue;
		}

		if (types[i] == AOSVALUE_TRUE_FALSE)
		{
			OmnString rslt = (values[i] != "")?"true":"false";
			OmnString rr = target_doc->getAttrStr(cnames[i]);
			if (rr != rslt)
			{
				target_doc->setAttr(cnames[i], values[i]);
				doc_modified = true;
			}
			continue;
		}

		if (types[i] == AOSVALUE_VALUE_MAP)
		{
			// The result is mapped to another value.
			// It uses a value selector to map the value. 
			AosValueRslt vv(values[i]);
			bool rslt = AosValueSel::getValueStatic(vv, sdoc, AOSTAG_VALUE_MAP, rdata);
			if (rslt)
			{
				OmnString rr = target_doc->getAttrStr(cnames[i]);
				if (vv.getStr() != rr)
				{
					rr = vv.getStr();
					target_doc->setAttr(cnames[i], rr);
					doc_modified = true;
				}
			}
			continue;
		}
	}

	if (doc_modified)
	{
		rdata->setArg1(AOSARG_DOC_MODIFIED, "true");
		// AOSMONITORLOG_FINISH(rdata);
	}
	return true;
}


AosActionObjPtr
AosActCountOccurences::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCountOccurences(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


