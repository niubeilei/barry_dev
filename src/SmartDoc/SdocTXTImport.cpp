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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocTXTImport.h"

#include "Actions/SdocAction.h"
#include "ValueSel/ValueSel.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEBase/SeUtil.h"
#include "SearchEngine/DocServerCb.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "Util/CodeConvertion.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <dirent.h>
#include <sys/types.h>
#include <iconv.h>

#define eMaxDocLen 1000000
AosSdocTXTImport::AosSdocTXTImport(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_TXTIMPORT, AosSdocId::eTXTImport, flag)
{
}

AosSdocTXTImport::~AosSdocTXTImport()
{
}

bool 
AosSdocTXTImport::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<sdoc insertto="cdata" zky_container=”xxx” zky_doc_sep=”xxx” zky_field_sep=”xxx”>
	//	<fields>
	//		<field type=”attr|text” name=”xxx”/>
	//		<field type=”attr|text” name=”xxx”/>
	//		...	
	//	</fields>
	//	<actions>
	//		...
	//	<actions>
	//</sdoc>
	
	if(!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	//get respose from rdata  2011-06-28, By Tracy
	AosXmlTagPtr req = rdata->getRequestRoot();

	AosXmlTagPtr rootchild = req->getFirstChild();	//request
	if (!rootchild)
	{
		OmnAlarm << "Request incorrect!" << enderr;
		rdata->setError() << "Request incorrect!" ;
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	AosXmlTagPtr fdoc = rootchild->getFirstChild("objdef");
	if (fdoc)
	{
		fdoc = fdoc->getFirstChild();
	}

	OmnString fobjid = fdoc->getAttrStr("fobjid");;
	if (fobjid != "")
	{
		fdoc = AosDocClientObj::getDocClient()->getDocByObjid(fobjid, rdata);
		if (fdoc->isRootTag()) fdoc = fdoc->getFirstChild();
	}

	if (!fdoc)
	{
		rdata->setError() << "Missing Obj Def!";
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	OmnString fname = fdoc->getAttrStr("fname");
	OmnString filedir = fdoc->getAttrStr(AOSCONFIG_FILEDIR);

	if (fname == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FILENAME);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	OmnString insertto = sdoc->getAttrStr("insertto");
	OmnString container = sdoc->getAttrStr("zky_container");
	if (container == "")
	{
		AosSetError(rdata, AOSLT_MISSING_CONTAINER);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	OmnString doc_sep = sdoc->getAttrStr("zky_doc_sep", "\r\n");
	OmnString field_sep = sdoc->getAttrStr("zky_field_sep", ",");

	OmnString encode_flag = sdoc->getAttrStr("zky_encodeflag", "false");
	OmnString codefrom = sdoc->getAttrStr("zky_encodefrom", "GBK");
	OmnString codeto = sdoc->getAttrStr("zky_encodeto", "UTF-8");
	bool ispublic = sdoc->getAttrBool(AOSTAG_ISPUBLIC, false);
	
	if (doc_sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_DOC_SEPARATOR);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	if (field_sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FIELD_SEPARATOR);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	AosXmlTagPtr fields = sdoc->getFirstChild("fields");
	if (!fields)
	{
		AosSetError(rdata, AOSLT_MISSING_FIELDS);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	AosXmlTagPtr field = fields->getFirstChild();
	vector<AosColumn> columns;
	while (field)
	{
		AosColumn col;

		col.type = field->getAttrStr("type","");
		col.attrname = field->getAttrStr("name","");
		columns.push_back(col);
		field = fields->getNextChild();
	}

	OmnString fpath;
	fpath << filedir << "/" << fname;
	OmnString dir = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	OmnString fdir;
	if (dir[0] == '/')
	{
		dir << "/" << fpath;
		fdir = dir;
	}
	else
	{
		fdir = OmnApp::getAppBaseDir();
		fdir << "/" << dir << "/" << fpath;
	}
	OmnFile ff(fdir, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!ff.isGood())
	{
		AosSetError(rdata, AOSLT_FILE_NOT_FOUND);
		OmnAlarm << rdata->getErrmsg() << ": " << fdir << enderr;
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
//create doc
	OmnString buff;
	bool finished;
	OmnString doc;
	doc << "<doc zky_pctrs=\"" << container << "\"></doc>";
	AosXmlParser parser;
	AosXmlTagPtr docxml = parser.parse(doc, "" AosMemoryCheckerArgs);
	aos_assert_r(docxml, false);
	OmnString records;
	int total = 0;
	while (1)
	{
		buff = "";
		bool rslt = ff.readUntil(buff, doc_sep.data(), eMaxDocLen, false, finished);
		if (!rslt)
		{
			AosSetError(rdata, AOSLT_FAILED_READ_FILE);
			OmnAlarm << rdata->getErrmsg() << ": " << fdir << enderr;
			return false;
		}

		if (buff.length() <= 0)
		{
			rdata->setOk();
			break;
		}

		if (encode_flag == "true")
		{
			CodeConvertion newtsp(codefrom.data(), codeto.data());

			OmnConnBuff buff1(buff.length()*3);
			int len = newtsp.translate((char *)buff.data(), buff.length(), buff1.getData(), buff.length()*3);
			aos_assert_r(len != -1, false);
			buff.assign(buff1.getData(), strlen(buff1.getData()));
		}
		AosXmlTagPtr recordtag;
		createRecord(sdoc, columns, buff, recordtag, rdata);
		total++;
		if(insertto != "cdata")
		{
			docxml->addNode(recordtag);
		}
		else
		{
			records << recordtag->toString();
		}
	}
	if(insertto == "cdata")
	{
		docxml->setText(records, true);
		docxml->setAttr("total_records", total);
		docxml->setAttr("insertto", "cdata");
	}
	bool r = createXml(sdoc, docxml, ispublic, rdata);
	aos_assert_r(r, false);
	OmnString rslts = "<Contents>";
	rslts << "<rslt zky_objid=\"" << docxml->getAttrStr(AOSTAG_OBJID) << "\" />";
	rslts << "</Contents>";
	rdata->setResults(rslts);
	return true;
}

bool
AosSdocTXTImport::createRecord(
		const AosXmlTagPtr &sdoc,
		vector<AosColumn> &columns,
		const OmnString &buff,
		AosXmlTagPtr &recordtag,
		const AosRundataPtr &rdata)
{
	// 1. Create record 
	OmnString record;
	OmnString text = "";
	record << "<record />";
	AosXmlParser parser;
	recordtag = parser.parse(record, "" AosMemoryCheckerArgs);
	aos_assert_r(recordtag, false);

	OmnStrParser1 strparser(buff);
	OmnString entry;
	int idx = 0;
	for (u32 i = 0; i < columns.size(); ++i)
	{
	    strparser.nextExcelEntry(idx, entry);
		entry.removeWhiteSpaces();
		OmnString attrname = columns[i].attrname;
		if(attrname == "")
		{
			continue;
		}

		if (entry != "")
		{
			recordtag->xpathSetAttr(attrname, entry);
		}
	}
	return true;
}

bool
AosSdocTXTImport::createXml(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &doc,
		const bool ispublic,
		const AosRundataPtr &rdata)
{

	rdata->setCreatedDoc(doc, true);
	// 1. Run actions
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (actions)
	{
		AosSdocAction::runActions(actions, rdata);
	}
	
	// 2. AosDocServer::getSelf()->createDoc1(...)
	AosXmlTagPtr doc_created = AosDocClientObj::getDocClient()->createDocSafe3(
			rdata, doc, rdata->getCid(), "", ispublic, true, false, false, true, 
			true, true);

	if (!doc_created) return false;
	rdata->setOk();
	return true;
}

