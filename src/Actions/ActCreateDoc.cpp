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
#include "Actions/ActCreateDoc.h"

#include "ValueSel/ValueSel.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEModules/ObjMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;

#if 0
AosActCreateDoc::AosActCreateDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATEDOC, AosActionType::eCreateDoc, flag)
{
}

AosActCreateDoc::AosActCreateDoc(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_CREATEDOC, AosActionType::eCreateDoc, false)
{
	if(!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}

AosActCreateDoc::~AosActCreateDoc()
{
}

bool	
AosActCreateDoc::config(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	return true;
}

AosActionObjPtr
AosActCreateDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateDoc(def, rdata);
	}
	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

bool
AosActCreateDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function creates a doc based on the smartdoc configuration.
	// If successful, the created doc is set to 'smartdoc' (by calling
	// smartdoc->appendCreatedDoc(...);
	// sdoc:Action Doc
	// Action Doc format
	// <action zky_actid=AOSACTID_CREATEDOC  zky_createdoc="true"
	//	zky_xpath="xxx" zky_doctmpl="xxx" zky_data_type="XmlDoc" zky_value_type="const">
	//	<zky_doc_selector zky_docselector_type="receiveddoc"></zky_doc_selector>
	//	<objid_tmpl>
	//		<entry type="1|2|...">xxx</entry>
	//		<entry type="1|2|...">xxx</entry>
	//		...
	//	</objid_tmpl>
	//	<attrs>
	//		<xpath>
	//			<name zky_value_type="rand"/>
	//			<name></name>	
	//			....
	//		</xpath>
	//		<value>
	//			<name></name>
	//			....
	//		</value>
	//	</attrs>
	//	<attrs>
	//		....
	//	</attrs>
	//	...
	// </action>
	//
	//AOSTAG_XPATH(zky_xpath): it identifies a portion of the input doc
	//AOSTAG_DOCTMPL(zky_doctmpl): it is an objid that identifies a doc template.
	//If both zky_xpath and zky_doctmpl are specified, the input doc is not null, 
	//and the doc path identifies a portion of the doc, zky_doctmpl is ignored
	
	aos_assert_r(rdata, false);
	aos_assert_r(sdoc, false);

	AosXmlTagPtr logdoc;
	if (sdoc->getAttrStr(AOSTAG_VALUE_TYPE) !="")
	{
		AosValueRslt valueRslt;
		AosValueSelObj::getValueStatic(valueRslt, sdoc, rdata);
		// The data type cannot be XML DOC
		if (!valueRslt.isNull())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}
		
		if (!valueRslt.isXmlDoc())
		{
			rdata->setError() << "Value not is an XML doc";
			return false;
		}
		logdoc = valueRslt.getXmlValue(rdata.getPtr()); 	
	} 

	if (!logdoc)
	{
		OmnString xml ="<xml>";
		xml << "</xml>";
		AosXmlParser parser2;
		logdoc = parser2.parse(xml, "" AosMemoryCheckerArgs);
	}
	aos_assert_r(logdoc, false);

	//set log objid
	OmnString objid;
	AosXmlTagPtr objid_tmpl = sdoc->getFirstChild(AOSTAG_OBJIDTMPL);
	if (objid_tmpl)
	{
		if (objid_tmpl->getFirstChild("attrs"))
		{
			bool rslt1 = parseAttrs(objid_tmpl, logdoc, rdata);
			if (!rslt1)
			{
				rdata->setError() << "Missing ctnr attr!" ;
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
	}
	
	bool rslt = parseAttrs(sdoc, logdoc, rdata);
	if (!rslt)
	{
		rdata->setError() << "Missing attr!" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rslt = parseNode(sdoc, logdoc, rdata);
	aos_assert_r(rslt, false);

	//save log doc
	OmnString createdoc = sdoc->getAttrStr("zky_createdoc");
	if (createdoc == "true")
	{
		logdoc->removeAttr(AOSTAG_DOCID);

		bool checkSecurity = sdoc->getAttrBool(AOSTAG_CHECK_ACCESS, true);
		u64 userid = 0;
		if (!checkSecurity)userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));

		AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, logdoc, "", "",
				true, false, false, false, false, true, true);

		if (!checkSecurity)rdata->setUserid(userid);

		if (!newdoc)
		{
			rdata->setError() << "Failed to create the doc : " << logdoc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else
	{
		// Chen Ding, 01/17/2012
		// rdata->appendDoc(logdoc);
	}
	if (!logdoc->isRootTag())
	{
		logdoc = logdoc->clone(AosMemoryCheckerArgsBegin);
	}
	rdata->setCreatedDoc(logdoc, false);
	bool is_set = sdoc->getAttrBool(AOSTAG_SET_CONTENTS, false);
	if (is_set)
	{
		OmnString docstr = "<Contents>";
		docstr << logdoc->toString() << "</Contents>";
		rdata->setResults(docstr);
	}
	return true;
}

bool
AosActCreateDoc::addAttrNode(
				const AosXmlTagPtr &logdoc, 
				const OmnString &path,
				const OmnString &value)
{
	aos_assert_r(logdoc, false);	
	aos_assert_r(path != "", false);
	AosXmlTagPtr doc = logdoc;
	AosXmlTagPtr xml;
	OmnStrParser1 parser(path, "/", false, false);
	OmnString word = parser.nextWord();
	OmnString word1;
	while (word != "") 
	{
		xml = doc->getFirstChild(word);
		word1 = parser.nextWord();
		//add node
		if (!xml && word1!="")  
		{
			doc = doc->addNode1(word);
		}
		//set Attr
		if (word1 =="")
		{
			doc->setAttr(word, value);
		}
		word = word1;
	}
	return true;
}


OmnString
AosActCreateDoc::isAttrAndText(const OmnString &path)
{
	//attr;text;
	aos_assert_r(path != "", "");
	OmnStrParser1 parser(path, "/", false, false);
	OmnString word;
	while ((word = parser.nextWord())!="") 
	{
		if (word == "_#text" || word == "_$text")
			return "text";
	}
	return "attr";
}


bool
AosActCreateDoc::addTextNode(
					const AosXmlTagPtr &logdoc,
					const OmnString &path,
					const OmnString &value)
{
	aos_assert_r(logdoc, false);
	aos_assert_r(path != "", false);
	OmnStrParser1 parser(path, "/", false, false);
	OmnString word;
	OmnString text;
	while ((word = parser.nextWord())!="") 
	{
		if (word == "_#text" || word == "_$text")
			break;
		if (text == "") text << word;
		else text << "/" << word;
	}
	logdoc->setNodeText(text, value, true);
	return true;
}


bool
AosActCreateDoc::parseAttrs(
			const AosXmlTagPtr &sdoc, 
			AosXmlTagPtr &logdoc,
			const AosRundataPtr &rdata)
{
	//parse Attrs 
	aos_assert_r(logdoc, false);
	AosXmlTagPtr attrs = sdoc->getFirstChild("attrs");
	if (!attrs) return true;

	AosXmlTagPtr attr = attrs->getFirstChild("attr");
	while(attr)
	{
		AosXmlTagPtr xpath = attr->getFirstChild("xpath");
		OmnString attrname,value;
		if (xpath)
		{
			AosValueRslt valueRslt;
			if (AosValueSelObj::composeValuesStatic(valueRslt, xpath, rdata))
			{
				if (!valueRslt.isNull())
				{
					rdata->setError() << "Value is invalid";
					return false;
				}

				if (valueRslt.isXmlDoc())
				{
					rdata->setError() << "Value is an XML doc";
					return false;
				}

				attrname = valueRslt.getStr(); 
			}
		}		

		AosXmlTagPtr valuexml = attr->getFirstChild("value");
		if(valuexml)
		{
			AosValueRslt valueRslt;
			if (AosValueSelObj::composeValuesStatic(valueRslt, valuexml, rdata))
			{
				if (!valueRslt.isNull())
				{
					rdata->setError() << "Value is invalid";
					return false;
				}

				if (valueRslt.isXmlDoc())
				{
					rdata->setError() << "Value is an XML doc";
					return false;
				}
				value = valueRslt.getStr();
			}
		}
		if (attrname != "" && value == "")
		{
			if (isAttrAndText(attrname) == "attr")
			{
				// remove Attr
				logdoc->removeAttr(attrname); 
			}
			else
			{
				logdoc->xpathRemoveText(attrname);
			}
		}

		if (attrname != "" && value != "")
		{
			if (isAttrAndText(attrname) == "attr")
			{
				// add new nodes/attributes into the doc
				aos_assert_r(addAttrNode(logdoc, attrname, value), false);
				//logdoc->xpathSetAttr(attrname, value);
			}
			else
			{
				// add new nodes/bodies into the doc
				aos_assert_r(addTextNode(logdoc, attrname, value), false);
				//logdoc->setNodeText(attrname, value, true);
			}
		}
		attr = attrs->getNextChild();
	}
	return true;
}


bool
AosActCreateDoc::parseNode(
			const AosXmlTagPtr &sdoc, 
			AosXmlTagPtr &logdoc,
			const AosRundataPtr &rdata)
{
	// <nodes>
	// <xpath>
	// <zky_valuedef .../>
	// </xpath>
	// <node>
	// <zky_valuedef zky_value_type="const" zky_data_type="xmldoc" zky_copy="true">
	// <zky_docselector zky_type="..." zky_doc_xpath="..."> </zky_docselector>
	// </zky_valuedef>
	// </node>
	// </nodes>
	aos_assert_r(logdoc, false);
	AosXmlTagPtr nodes = sdoc->getFirstChild("nodes");
	if (!nodes) return true;

	AosXmlTagPtr node = nodes->getFirstChild("node");
	aos_assert_r(node, false);
	while(node)
	{
		OmnString path;
		AosXmlTagPtr xpath = node->getFirstChild("xpath");
		if (xpath)
		{
			AosValueRslt valueRslt;
			if (AosValueSelObj::composeValuesStatic(valueRslt, xpath, rdata))
			{
				if (!valueRslt.isNull())
				{
					rdata->setError() << "Value is invalid";
					return false;
				}

				if (valueRslt.isXmlDoc())
				{
					rdata->setError() << "Value is an XML doc";
					return false;
				}

				path = valueRslt.getStr(); 
			}
		}

		AosXmlTagPtr nodexml;
		AosXmlTagPtr nodetag = node->getFirstChild("nodetag");
		if (!nodetag) 
		{
			node = nodes->getNextChild();
			continue;
		}

		AosValueRslt valueRslt;
		nodetag = nodetag->getFirstChild();
		aos_assert_r(nodetag, false);
		if (AosValueSelObj::getValueStatic(valueRslt, nodetag, rdata))
		{
			// The data type cannot be XML DOC
			if (!valueRslt.isNull())
			{
				rdata->setError() << "Value is invalid";
				return false;
			}

			if (!valueRslt.isXmlDoc())
			{
				rdata->setError() << "Value not is an XML doc";
				return false;
			}
			nodexml  = valueRslt.getXmlValue(rdata.getPtr()); 	
		}
		
		if (nodexml)
		{
			if (path == "" )
			{
				logdoc->addNode(nodexml);
			}
			else
			{
				addPathNode(logdoc, path, nodexml);
			}
		}
		node = nodes->getNextChild();
	}
	return true;
}


bool
AosActCreateDoc::addPathNode(
		const AosXmlTagPtr &logdoc,
		const OmnString &path,
		const AosXmlTagPtr &nodexml)
{
	aos_assert_r(logdoc, false);
	aos_assert_r(path != "", false);
	AosXmlTagPtr doc = logdoc;
	OmnStrParser1 parser(path, "/", false, false);
	OmnString word; 
	while ((word = parser.nextWord()) != "")
	{
		//add node
		if (!doc->getFirstChild(word))
		{
			doc = doc->addNode1(word);
		}
	}
	return true;
}

#endif 
