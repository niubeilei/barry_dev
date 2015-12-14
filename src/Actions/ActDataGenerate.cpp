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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActDataGenerate.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataGenUtil/DataGenUtil.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "ValueSel/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include <vector>
using namespace std;

AosActDataGenerate::AosActDataGenerate(const bool flag)
:
AosSdocAction(AOSACTTYPE_DATAGENERATE, AosActionType::eDataGenerate, flag)
{
}


/*
AosActDataGenerate::AosActDataGenerate(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eDataGenerate, false)
{
	if(!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}
*/

AosActDataGenerate::~AosActDataGenerate()
{
}

bool	
AosActDataGenerate::config(const AosXmlTagPtr &config)
{
	return true;
}


/*
AosActionObjPtr
AosActDataGenerate::clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosActDataGenerate(def);
	}
	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/


bool
AosActDataGenerate::run(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	vector<AosDataGenUtilPtr> attrname;
	vector<AosDataGenUtilPtr> attrvalue;
	AosXmlTagPtr createddoc = 0;
	int total = sdoc->getAttrInt("zky_totaldoc", 1);
	aos_assert_rr(parse(sdoc, attrname, attrvalue, createddoc, rdata), rdata, false);
	aos_assert_rr(createddoc, rdata, false);
	bool iscreatedoc = sdoc->getAttrBool("zky_createdoc");
	OmnString docstr = "<Contents>";
	for (int i = 0; i< total; i++)
	{
		AosXmlTagPtr datagendoc = createddoc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_rr(datagendoc, rdata, false);
		aos_assert_rr(createData(datagendoc, attrname, attrvalue, rdata), rdata, false);
		if (iscreatedoc)
		{
			datagendoc->removeAttr(AOSTAG_DOCID);
			rdata->setReceivedDoc(datagendoc, true);
			AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, datagendoc, "", "",
					true, false, false, false, false, true, true);
			if (!newdoc)
			{
				rdata->setError() << "Failed to create the doc : " << datagendoc->toString();
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			docstr << "<record "
					<< AOSTAG_OBJID << " =\"" << newdoc->getAttrStr(AOSTAG_OBJID, "") << "\" "
					<< AOSTAG_DOCID << " = \""  << newdoc->getAttrStr(AOSTAG_DOCID, "") << "\" "
					<< "/>";
		}
		else
		{
			if (total == 1)rdata->setCreatedDoc(datagendoc, true);
			docstr << "<record>" << datagendoc->toString() << "</record>";
		}
	}
	docstr << "</Contents>";
	rdata->setResults(docstr);
	return true;
}


bool
AosActDataGenerate::parse(
			const AosXmlTagPtr &config, 
			vector<AosDataGenUtilPtr> &attrname,
			vector<AosDataGenUtilPtr> &attrvalue,
			AosXmlTagPtr &createddoc,
			const AosRundataPtr &rdata)
{
	aos_assert_r(config, false);
	createddoc = parseDoc(config, rdata); 
	aos_assert_rr(createddoc, rdata, false);
	aos_assert_rr(parseAttribute(config, attrname, attrvalue, rdata), rdata, false);
	return true;
}


AosXmlTagPtr 
AosActDataGenerate::parseDoc(
			const AosXmlTagPtr &config,
			const AosRundataPtr &rdata)
{
	AosXmlTagPtr createddoc;
	if (config->getAttrStr(AOSTAG_VALUE_TYPE) !="")
	{
		AosValueRslt valueRslt;
		AosValueSel::getValueStatic(valueRslt, config, rdata);
		// The data type cannot be XML DOC
		if (!valueRslt.isNull())
		{
			rdata->setError() << "Value is invalid";
			return NULL;
		}

		/*
		if (!valueRslt.isXmlDoc())
		{
			rdata->setError() << "Value not is an XML doc";
			return NULL;
		}
		createddoc = valueRslt.getXmlValue(rdata.getPtr());
		*/
	}

	if (!createddoc)
	{
		OmnString xml ="<xml>";
		xml << "</xml>";
		AosXmlParser parser2;
		createddoc  = parser2.parse(xml, "" AosMemoryCheckerArgs);
	}
	return createddoc;
}


bool
AosActDataGenerate::parseAttribute(
			const AosXmlTagPtr &config,
			vector <AosDataGenUtilPtr> &attrname,
			vector <AosDataGenUtilPtr> &attrvalue,
			const AosRundataPtr &rdata)
{
	aos_assert_r(config, false);
	AosXmlTagPtr attrs = config->getFirstChild("attrs");
	if (!attrs) return true;

	AosXmlTagPtr attribute = attrs->getFirstChild();
	while(attribute)
	{
		AosXmlTagPtr xpathxml = attribute->getFirstChild("xpath");
		AosXmlTagPtr valuexml = attribute->getFirstChild("value");
		if (xpathxml && valuexml)
		{
			AosDataGenUtilPtr xpathptr = AosDataGenUtil::getDataGenUtilStatic(
					xpathxml->getFirstChild(), rdata);
			AosDataGenUtilPtr valueptr = AosDataGenUtil::getDataGenUtilStatic(
					valuexml->getFirstChild(), rdata);
			if (xpathptr && valueptr) 
			{
				attrname.push_back(xpathptr);
				attrvalue.push_back(valueptr);
			}
		}
		attribute = attrs->getNextChild();
	}
	aos_assert_rr(attrname.size() == attrvalue.size(), rdata, false);
	return true;
}


bool
AosActDataGenerate::createData(
			const AosXmlTagPtr &createddoc,
			const vector <AosDataGenUtilPtr> &attrname,
			const vector <AosDataGenUtilPtr> &attrvalue,
			const AosRundataPtr &rdata)
{
	if (attrname.size() == 0 || attrvalue.size() == 0) return true;
	aos_assert_rr(attrname.size() == attrvalue.size(), rdata, false);
	AosXmlTagPtr doc = createddoc;
	aos_assert_r(doc, false);
	AosValueRslt valueRslt;
	for (u32 i = 0; i < attrname.size(); i++)
	{
		OmnString aname, avalue; 
		valueRslt.reset();
		if (attrname[i]->nextValue(valueRslt, rdata))
		{
			aname = valueRslt.getStr();
		}

		valueRslt.reset();
		if (attrvalue[i]->nextValue(valueRslt, rdata))
		{
			avalue = valueRslt.getStr();
		}

		if (aname != "" && avalue != "")
		{
			addNode(doc, aname);
			createddoc->xpathSetAttr(aname, avalue);		
		}
	}
	return true;
}


bool
AosActDataGenerate::addNode(
			AosXmlTagPtr doc,
			const OmnString &path)
{
	aos_assert_r(path != "", false);
	OmnStrParser1 parser(path, "/", false, false);
	AosXmlTagPtr xml;
	OmnString word = parser.nextWord();
	aos_assert_r(word != "", false);
	OmnString node = parser.nextWord();

	while (node != "")
	{
		if (word == "_#text") return true;
		xml = doc->getFirstChild(word);
		if (!xml)
		{
			doc = doc->addNode1(word);
		}
		word = node;
		node = parser.nextWord();
	}
	return true;
}


AosActionObjPtr
AosActDataGenerate::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDataGenerate(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


