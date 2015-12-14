////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Import is import the contents of excel.
// The excel should be some format:
// 1. The excel should have header, and the header shuold not be empty.
// 2. Excel should start at the first colomn.
//
// Modification History:
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocImport.h"

#include "Actions/SdocAction.h"
#include "ValueSel/ValueSel.h"
#include "DbQuery/Query.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEBase/SeUtil.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/DynArray.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "Util/CodeConvertion.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/Ptrs.h"
#include <dirent.h>
#include <sys/types.h>
#include <iconv.h>

const OmnString sgXmlHeader = "<?xml version=\"1.0\"?>";
const OmnString sgExcelHeader = "<?mso-application progid=\"Excel.Sheet\"?>";
static bool	sgStartThreadFlag		= false;
static OmnMutexPtr sgCreateDocLock = OmnNew OmnMutex();
static OmnCondVarPtr sgCreateDocCondVar = OmnNew OmnCondVar();

static OmnMutexPtr sgSplitXmlLock = OmnNew OmnMutex();
static OmnCondVarPtr sgSplitXmlCondVar = OmnNew OmnCondVar();
AosSdocImport::AosSdocImport(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_IMPORT, AosSdocId::eImport, flag),
mNumReqs(0)
{
}

AosSdocImport::~AosSdocImport()
{
}


void
AosSdocImport::startThread()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mCreateDocThread = OmnNew OmnThread(thisPtr, "CreateDocid", eCreateDocId, true, true, __FILE__, __LINE__);
	mCreateDocThread->start();
	
	mSplitXmlThread = OmnNew OmnThread(thisPtr, "SplitXmlID", eSplitXmlId, true, true, __FILE__, __LINE__);
	mSplitXmlThread->start();
}



bool 
AosSdocImport::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// The smartdoc should be this format:
	//<sdoc zky_container=”xxx” zky_doc_sep=”xxx” zky_field_sep=”xxx” create_method="singlerow|mutirow">
	//	<actions>
	//		...
	//	<actions>
	//</sdoc>
	//
	// The data should be this format:
	// <Table ss:ExpandedColumnCount="4" ss:ExpandedRowCount="4" x:FullColumns="1" x:FullRows="1" ss:DefaultColumnWidth="54" ss:DefaultRowHeight="14.25">
	// <Row>
	// <Cell>
	// <Data ss:Type="String">员工号</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">姓名</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">性别</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">年龄</Data>
	// </Cell>
	// </Row>
	// <Row>
	// <Cell>
	// <Data ss:Type="Number">1</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">张三</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">男</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="Number">26</Data>
	// </Cell>
	// </Row>
	// <Row>
	// <Cell>
	// <Data ss:Type="Number">2</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">李四</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">男</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="Number">27</Data>
	// </Cell>
	// </Row>
	// <Row>
	// <Cell>
	// <Data ss:Type="Number">3</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">王五</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="String">女</Data>
	// </Cell>
	// <Cell>
	// <Data ss:Type="Number">23</Data>
	// </Cell>
	// </Row>
	// </Table>
	if (!sgStartThreadFlag) 
	{
		
		startThread();
		sgStartThreadFlag = true;
	}
	

	if(!sdoc)
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	

	// 1. retrieve the header and sysbd from the sdoc.
	bool rslt = initHeader(sdoc, rdata);
	if (!rslt)
	{
		rdata->setError() << "The header configuration is wrong";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rslt = initSysBd(sdoc, rdata);
	if (!rslt)
	{
		rdata->setError() << "The sysbd configuration is wrong";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mPageSize = sdoc->getAttrInt(AOSTAG_PAGESIZE, 1000);
	mCreateMethod = sdoc->getAttrStr("create_method", "singlerow");
	
	// 2. get the upload doc, retrieve the excel.
	OmnString objid = sdoc->getAttrStr(AOSTAG_FILEOBJID);
	if (objid == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FILEOBJID);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	AosXmlTagPtr filedoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!filedoc)
	{
		AosSetError(rdata, AOSLT_DOC_NOT_FOUND);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	OmnString filename = filedoc->getAttrStr("zky_rscfn");
	if (filename == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FILENAME);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	OmnString filedir = filedoc->getAttrStr("zky_rscdir");
	
	// 3. retrieve the docs's container.
	OmnString container = sdoc->getAttrStr("zky_container");
	if (container == "")
	{
		AosSetError(rdata, AOSLT_MISSING_CONTAINER);
	    // AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 4. retrieve the actions.
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	
	// 6.if doc is exist, what do?
	bool cover = sdoc->getAttrBool("zky_cover", false);
	
	// 7. if cover is true, define the the flag.
	OmnString identify = sdoc->getAttrStr("zky_identify", "");
	
	OmnString tname = sdoc->getAttrStr("zky_tname", "Sheet1");

	aos_assert_r(getExcel(cover, identify, 
				container, tname, actions, filedir, filename, rdata), false);

	OmnString docstr = "<Contents>create docs ......</Contents>";
	rdata->setResults(docstr);
	rdata->setOk();
	return true;
}


bool	
AosSdocImport::getExcel(
		const bool &cover,
		const OmnString &identify,
		const OmnString &container,
		const OmnString &tname,
		const AosXmlTagPtr &actions,
		OmnString &filedir,
		OmnString &filename, 
		const AosRundataPtr &rdata)
{
	// This funciton convert the file of excel to format of xml.
	if (filename == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FILENAME);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	int len = filedir.length();
	if (len>0)
	{
		if (filedir.data()[len-1] != '/') 
			filedir << "/";
	}
	filedir << filename;
	OmnString dirname = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");

	OmnString fname = OmnApp::getAppBaseDir(); 
	fname << dirname << "/" << filedir;
	OmnFile f(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!f.isGood())
	{
		AosSetError(rdata, AOSLT_FAILED_READ_FILE);
		OmnAlarm << rdata->getErrmsg() << ". Filename: " << filedir << enderr;
		return false;
	}
	
	int length = f.getLength();
	OmnConnBuffPtr docBuff = OmnNew OmnConnBuff(length);
	int readn = f.readToBuff(0, length, docBuff->getData());
	if (readn < 0)
	{
		AosSetError(rdata, AOSLT_FAILED_READ_FILE);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	docBuff->setDataLength(readn);
	
	if (readn > eSplitXmlLevel)
	{
		return addSplitRequest(actions, cover, identify, tname, container, docBuff,rdata);
	}
	
	aos_assert_r(splitXml(docBuff, actions, cover, identify, tname, container, rdata), false);
	return true;
}


bool
AosSdocImport::splitXml(
		const OmnConnBuffPtr &buff,
		const AosXmlTagPtr &actions,
		const bool &cover,
		const OmnString &identify,
		const OmnString &tname,
		const OmnString &container,
		const AosRundataPtr &rdata)
{
	// This function splits the xml into smaller ones. 
	// 'buff' is in the format:
	//	 <Worksheet ss:Name="xxx">
	// 		<Table ...>
	// 			<Row>
	// 				...
	// 			</Row>
	// 			<Row>
	// 				...
	// 			</Row>
	// 			...
	// 		</Table>
	//	</Worksheet>
	// 1. Read the contents of 'Worksheet'.
	// 2. Read the start tag ("<Table>")
	// 3. Read the next subtag (i.e., "<Row>") until the page is full
	
	// 1. Read the contents of 'WorkSheet'.
	char *data = buff->getData();
	aos_assert_r(data, false);
	int64_t data_len = buff->getDataLength();
	//AosBuffPtr worksheet_buff = OmnNew AosBuff(data_len, 100 AosMemoryCheckerArgs);
	//felicia, 2012/09/26
	AosBuffPtr worksheet_buff = OmnNew AosBuff(data_len AosMemoryCheckerArgs);
	aos_assert_r(worksheet_buff, false);
	OmnString partten = "<Worksheet ss:Name=\"";
	partten << tname << "\"";
	char *worksheet_start = strstr(data, partten.data());
	char *worksheet_end = 0;
	if (worksheet_start)
	{
		worksheet_end = strstr(worksheet_start, "</Worksheet>");
		aos_assert_r(worksheet_end, false);
		worksheet_buff->setBuff(worksheet_start, worksheet_end-worksheet_start+13);
	}
	else
	{
		OmnAlarm << "Invalid error" << enderr;
		return false;
	}

	data_len = worksheet_buff->dataLen();

	// Read the start tag "<Table .....>".
	//AosBuffPtr table_buff = OmnNew AosBuff(100, 50 AosMemoryCheckerArgs);
	//felicia, 2012/09/26
	AosBuffPtr table_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	aos_assert_r(table_buff, false);
	char *table_start = strstr(worksheet_start, "<Table");
	char *table_end = 0;
	if (table_start)
	{
		table_end = strstr(table_start, ">");
		aos_assert_r(table_end, false);
	}
	aos_assert_r(table_start, false);
	table_buff->setBuff(table_start, table_end-table_start+1);

	// The xml format is:
	// <Table ...>
	//   <Row>......</Row>
	//   <Row>......</Row>
	//   <Row>......</Row>
	//   ......
	// </Table>
	// The start position is table_end, read the pagesize '<Row>' to construct 
	// xml, the xml should be this format:
	// <Table .....>
	// 	 <Row>......</Row>
	// 	   ...pagesize...
	// </Table>
	//
	//
	// check the header of excel.
	char *ctnt_start = strstr(table_end, "<Row");
	char *ctnt_end = 0;
	if (ctnt_start)
	{
		ctnt_end = strstr(ctnt_start, "</Row>");
		aos_assert_r(ctnt_end, false);
		
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(ctnt_start, ctnt_end-ctnt_start+6, "" AosMemoryCheckerArgs);
		if (!xml)
		{
			OmnAlarm << "Inter error"  << enderr;
			return false;
		}
		if (xml->isRootTag()) xml = xml->getFirstChild();
		aos_assert_r(checkHeader(xml, rdata), false);
	}
	else
	{
		OmnAlarm << "Ivalid excel" << enderr;
		return false;
	}

	// create doc.
	if(mCreateMethod == "mutirow")
	{
		OmnString mutirow = "<table>";
		ctnt_start = strstr(ctnt_end, "<Row");
		OmnString identify_value;
		while (ctnt_start && (ctnt_start-worksheet_start < data_len))
		{
			ctnt_end = strstr(ctnt_start, "</Row>");
			aos_assert_r(ctnt_end, false);
			AosXmlTagPtr rowxml;
			aos_assert_r(createRowNode(ctnt_start, ctnt_end-ctnt_start+6, rowxml, identify, identify_value, rdata), false);
			aos_assert_r(rowxml, false);
			mutirow << rowxml->toString();
			ctnt_start = strstr(ctnt_end+6, "<Row");
		}
		mutirow << "</table>";
		AosXmlParser tableParser;
		AosXmlTagPtr tablexml = tableParser.parse(mutirow AosMemoryCheckerArgs);
		aos_assert_r(createTable(tablexml, actions, cover, identify, identify_value, container, rdata), false);
	}
	else
	{
		ctnt_start = strstr(ctnt_end, "<Row");
		while (ctnt_start && (ctnt_start-worksheet_start < data_len))
		{
			ctnt_end = strstr(ctnt_start, "</Row>");
			aos_assert_r(ctnt_end, false);
			aos_assert_r(createXml(ctnt_start, ctnt_end-ctnt_start+6, 
					actions, cover, identify, container, rdata), false);
			ctnt_start = strstr(ctnt_end+6, "<Row");
		}
	}

	return true;
}


bool
AosSdocImport::createTable(
		AosXmlTagPtr doc,
		const AosXmlTagPtr &actions,
		const bool &cover,
		const OmnString identify,
		OmnString &identify_value,
		const OmnString &container,
		const AosRundataPtr &rdata)
{
	bool modify = false;
	if (cover && identify != "")
	{
		bool is_unique = false;
		u64 docid = AosQuery::getSelf()->getMember(container, 
			identify, identify_value, is_unique, rdata);
		if (docid > 0)
		{
			AosXmlTagPtr doc1 = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
			aos_assert_r(doc1, 0);
			doc = doc1->clone(AosMemoryCheckerArgsBegin);
		}	
		modify = true;
	}
	else
	{
		doc->xpathSetAttr(AOSTAG_PARENTC, container, true);
	}
	doc->xpathSetAttr(AOSTAG_PUBLIC_DOC, "true", true);
	doc->xpathSetAttr(AOSTAG_CTNR_PUBLIC, "true", true);
	AosSysBdMapItr iter = mSysBdMap.begin();
	for (iter = mSysBdMap.begin(); iter != mSysBdMap.end(); iter ++)
	{
		aos_assert_r(iter->first != "" && iter->second != "", 0);
		doc->xpathSetAttr(iter->first, iter->second, true);
	}
	aos_assert_r(doc, false);
	
	// 3. add request to create doc.
	aos_assert_r(addRequest(doc, actions, modify, rdata), false);
	return true;
}

bool
AosSdocImport::createRowNode(
		const char *start,
		const int &len,
		AosXmlTagPtr &doc,
		const OmnString identify,
		OmnString &identify_value,
		const AosRundataPtr &rdata)
{
	// 'start' should be this format:
	// <Row..>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   ......
	// </Row>
	aos_assert_r(start, false);
	
	// 1. construct the xml.
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(start, len, "" AosMemoryCheckerArgs);
	if (!xml)
	{
		OmnAlarm << "Inter error"  << enderr;
		return false;
	}
	if (xml->isRootTag()) xml = xml->getFirstChild();
	
	// 2. Based on xml, construct the doc we needed.
	map<OmnString, OmnString>attrs;
	aos_assert_r(initAttrs(identify, identify_value, xml, attrs, rdata), false);
	OmnString rowstr ="<row />";
	AosXmlParser rowparser;
	doc = rowparser.parse(rowstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	map<OmnString, OmnString>::iterator attriter = attrs.begin();
	for (attriter = attrs.begin(); attriter != attrs.end(); attriter ++)
	{
		if(attriter->first != "zky_public_doc" && attriter->first != "zky_public_ctnr")
		{
			doc->xpathSetAttr(attriter->first, attriter->second, true);
		}
	}
	return true;
}

bool
AosSdocImport::createXml(
		const char *start,
		const int &len,
		const AosXmlTagPtr &actions,
		const bool &cover,
		const OmnString identify,
		const OmnString &container,
		const AosRundataPtr &rdata)
{
	// 'start' should be this format:
	// <Row..>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   <Cell>xxxx</Cell>
	//   ......
	// </Row>
	aos_assert_r(start, false);
	
	// 1. construct the xml.
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(start, len, "" AosMemoryCheckerArgs);
	if (!xml)
	{
		OmnAlarm << "Inter error"  << enderr;
		return false;
	}
	if (xml->isRootTag()) xml = xml->getFirstChild();
	
	// 2. Based on xml, construct the doc we needed.
	OmnString identify_value;
	map<OmnString, OmnString>attrs;
	aos_assert_r(initAttrs(identify, identify_value, xml, attrs, rdata), false);
	bool modify = false;
	AosXmlTagPtr doc;
	if (cover && identify != "" && identify_value != "")
	{
		doc = createCoverDoc(modify, container, identify, identify_value, attrs, rdata);
	}
	else
	{
		doc = createNewDoc(container, attrs);
	}
	aos_assert_r(doc, false);
	
	// 3. add request to create doc.
	aos_assert_r(addRequest(doc, actions, modify, rdata), false);
	
	return true;
}

bool
AosSdocImport::checkHeader(
		const AosXmlTagPtr &row, 
		const AosRundataPtr &rdata)
{
	// This function check whether the header of excel is right.
	// the xml should be this format:
	// <Row ...>
	// 	 <Cell>xxxx</Cell>
	// 	 <Cell>xxxx</Cell>
	// 	 <Cell>xxxx</Cell>
	// 	 <Cell>xxxx</Cell>
	// 	 .....
	// </Row>
	// Every data of cell should match the header user config.
	aos_assert_r(row, false);
	
	AosXmlTagPtr column = row->getFirstChild();
	aos_assert_r(column, false);
	
	int i = 0;
	while (column)
	{
		AosXmlTagPtr data = column->getFirstChild();
		if (!data)
		{
			rdata->setError() << "The excel format is error";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		OmnString ctnt = data->getNodeText();
		ctnt.removeWhiteSpaces();
		if ((size_t)i < mColumnVect.size() && ctnt != mColumnVect[i])
		{
			OmnAlarm << "The smartdoc is error" << enderr;
			return false;
		}

		i ++;
		column = row->getNextChild();
	}
	return true;
}


bool
AosSdocImport::createDoc(
		const AosXmlTagPtr &xml,
		const AosXmlTagPtr &actions,
		const bool &modify,
		const AosRundataPtr &rdata)
{
	// This function create docs, this docs's container is param container.
	// The xml should be this format:
	// <Row ...>
	// 		<Cell>xxxx</Cell>
	// 		........
	// </Row>
	aos_assert_r(xml, false);
	
	if (actions)
	{
		rdata->setWorkingDoc(xml, true);
		bool rslt = AosSdocAction::runActions(actions, rdata);
		if (!rslt)
		{
			rdata->setError() << "Failed to run actions";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	aos_assert_r(createSysDoc(modify, xml, rdata), false);

	return true;
}


bool
AosSdocImport::createSysDoc(
		const bool &modify,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	if (modify)
	{
		bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "true", false);
		if (!rslt)
		{
			rdata->setError() << "fail to create file";
			return false;
		}
	}
	else
	{
		AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, doc, "", "",
								true, false, false, false, false, true, true);
		if (!newdoc)
		{
			rdata->setError() << "fail to create file";
			return false;
		}
	}

	return true;
}


AosXmlTagPtr
AosSdocImport::createNewDoc(
		const OmnString &container,
		map<OmnString, OmnString> &attrs)
{
	aos_assert_r(container != "", 0);
	
	AosSysBdMapItr iter = mSysBdMap.begin();
	map<OmnString, OmnString>::iterator attriter = attrs.begin();
	
	OmnString docstr ="<doc />";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	
	doc->xpathSetAttr(AOSTAG_PARENTC, container, true);
	
	for (iter = mSysBdMap.begin(); iter != mSysBdMap.end(); iter ++)
	{
		aos_assert_r(iter->first != "" && iter->second != "", 0);
		doc->xpathSetAttr(iter->first, iter->second, true);
	}
	for (attriter = attrs.begin(); attriter != attrs.end(); attriter ++)
	{
		doc->xpathSetAttr(attriter->first, attriter->second, true);
	}
	return doc;
}


AosXmlTagPtr
AosSdocImport::createCoverDoc(
		bool &modify,
		const OmnString &container,
		const OmnString &identify,
		const OmnString &identify_value,
		map<OmnString, OmnString> &attrs,
		const AosRundataPtr &rdata)
{
	// This function create doc, if the doc exist, recover the doc.
	bool is_unique = false;
	AosXmlTagPtr doc;
	AosSysBdMapItr iter = mSysBdMap.begin();
	map<OmnString, OmnString>::iterator attriter = attrs.begin();

	u64 docid = AosQuery::getSelf()->getMember(container, 
			identify, identify_value, is_unique, rdata);
	if (docid > 0)
	{
		AosXmlTagPtr doc1 = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		aos_assert_r(doc1, 0);
		doc = doc1->clone(AosMemoryCheckerArgsBegin);
		for (iter = mSysBdMap.begin(); iter != mSysBdMap.end(); iter ++)
		{
			aos_assert_r(iter->first != "" && iter->second != "", 0);
			doc->xpathSetAttr(iter->first, iter->second, true);
		}
		for (attriter = attrs.begin(); attriter != attrs.end(); attriter ++)
		{
			doc->xpathSetAttr(attriter->first, attriter->second, true);
		}
		modify = true;
	}	
	else
	{
		OmnString docstr ="<doc />";
		AosXmlParser parser;
		doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
		aos_assert_r(doc, 0);
		
		doc->xpathSetAttr(AOSTAG_PARENTC, container, true);
		for (iter = mSysBdMap.begin(); iter != mSysBdMap.end(); iter ++)
		{
			aos_assert_r(iter->first != "" && iter->second != "", 0);
			//docstr << iter->first << "=\"" << iter->second << "\" ";
			doc->xpathSetAttr(iter->first, iter->second, true);
		}
		for (attriter = attrs.begin(); attriter != attrs.end(); attriter ++)
		{
			//docstr << attriter->first << "=\"" << attriter->second << "\" ";
			doc->xpathSetAttr(attriter->first, attriter->second, true);
		}
	}
	return doc;
}


bool
AosSdocImport::initAttrs(
		const OmnString &identify,
		OmnString &identify_value,
		const AosXmlTagPtr &row,
		map<OmnString, OmnString> &attrs,
		const AosRundataPtr &rdata)
{
	aos_assert_r(row, false);
	attrs.clear();
	
	int j = 0, k = 1, m = 1;
	map<OmnString, OmnString>::iterator itr;
	attrs["zky_public_doc"] = "true";
	attrs["zky_public_ctnr"] = "true";
	
	AosXmlTagPtr column = row->getFirstChild();
	aos_assert_r(column, false);
	while (column)
	{
		// if index > 0, we will fill the attribute, but the value is empty.
		int index = column->getAttrInt("ss:Index", 0);
		if (index>0)
		{
			for (j=k; j<index; j++)
			{
				if ((size_t)j < mColumnVect.size())
				{
					OmnString attr;
					itr = mHeader2AttrMap.find(mColumnVect[j-1]);
					if (itr != mHeader2AttrMap.end()) attr = itr->second;
					attrs[attr] = "";
				}
				m ++;
			}
			k = index;
		}
		
		AosXmlTagPtr data = column->getFirstChild();
		OmnString ctnt;
		if (!data)
		{
			ctnt = "";
		}
		else
		{
			ctnt = data->getNodeText();
		}
		ctnt.removeWhiteSpaces();

		OmnString attr;
		if ((size_t)m < mColumnVect.size()+1)
		{
			itr = mHeader2AttrMap.find(mColumnVect[m-1]);
			if (itr != mHeader2AttrMap.end()) 
			{
				attr = itr->second;
				if (attr == identify)
				{
					identify_value = ctnt;
				}
			}
			else
			{
				rdata->setError() << "No attribute defined by: " << itr->second;
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			attrs[attr] = ctnt;
		}
		else
		{
			rdata->setError() << "Inter error: " << m << " : " << mColumnVect.size();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		m ++;
		k ++;
		column = row->getNextChild();
	}
	return true;
}


bool
AosSdocImport::isInvalidSysBd(const OmnString &sysbd)
{
	aos_assert_r(sysbd != "", false);
	switch (sysbd.data()[4])
	{
		case 'd':
			if (sysbd == "zky_docid") return true;
			break;
		
		case 'o':
			if (sysbd == "zky_objid") return true;
			break;
		
		default:
			return false;
	}
	return false;
}


bool
AosSdocImport::initSysBd(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	AosXmlTagPtr sysbd_tag = sdoc->getFirstChild("sysbd");
	if (!sysbd_tag) return true;
	AosXmlTagPtr child = sysbd_tag->getFirstChild();
	if (!child) return true;

	mSysBdMap.clear();
	while (child)
	{
		OmnString sysbdname = child->getAttrStr("zky_sys_name");
		if (sysbdname == "")
		{
			rdata->setError() << "Missing header";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		if (isInvalidSysBd(sysbdname))
		{
			child = sysbd_tag->getNextChild();
			continue;
		}
		OmnString sysbdvalue = child->getNodeText();
		if (sysbdvalue == "")
		{
			rdata->setError() << "Missing header";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mSysBdMap[sysbdname] = sysbdvalue;
		child = sysbd_tag->getNextChild();
	}
	return true;	
}


bool
AosSdocImport::initHeader(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	AosXmlTagPtr header = sdoc->getFirstChild(AOSTAG_HEADER);
	if (!header)
	{
		rdata->setError() << "Missing header";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr child = header->getFirstChild();
	if (!child)
	{
		rdata->setError() << "Missing header";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mColumnVect.clear();
	mHeader2AttrMap.clear();
	while (child)
	{
		OmnString hname = child->getAttrStr(AOSTAG_HEADER);
		if (hname == "")
		{
			rdata->setError() << "Missing header";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		OmnString attrname = child->getNodeText();
		if (attrname == "")
		{
			rdata->setError() << "Missing header";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mColumnVect.push_back(hname);
		mHeader2AttrMap[hname] = attrname;
		child = header->getNextChild();
	}
	
	return true;
}



bool
AosSdocImport::threadFunc(OmnThrdStatus::E &state,const OmnThreadPtr &thread)
{
    while(state == OmnThrdStatus::eActive)
    {
		if (thread->getLogicId() == eCreateDocId)
		{
			return createDoc(state, thread);
		}
		if (thread->getLogicId() == eSplitXmlId)
		{
			return splitXml(state, thread);
		}
	}
	return true;
}

bool
AosSdocImport::splitXml(OmnThrdStatus::E &state,const OmnThreadPtr &thread)
{
	sgSplitXmlLock->lock();
	if (mSplitQueue.empty())
	{
		// There is no more cached entries. Start a timer to
		// wake this thread up later.
		sgSplitXmlCondVar->wait(sgSplitXmlLock);
		sgSplitXmlLock->unlock();
		return true;
	}
	
	AosSplit split = mSplitQueue.front();
	mSplitQueue.pop();
	sgSplitXmlLock->unlock();
	splitXml(split);	
	return true;

}


bool
AosSdocImport::createDoc(OmnThrdStatus::E &state,const OmnThreadPtr &thread)
{
	sgCreateDocLock->lock();
	if (mNumReqs == 0)
	{
		// There is no more cached entries. Start a timer to
		// wake this thread up later.
		sgCreateDocCondVar->wait(sgCreateDocLock);
		sgCreateDocLock->unlock();
		return true;
	}
	if (mQueue.empty())
	{
		OmnAlarm << "Inter Error" << enderr;
		sgCreateDocLock->unlock();
		return true;
	}
	
	AosImport import = mQueue.front();
	mNumReqs --;
	mQueue.pop();
	sgCreateDocLock->unlock();
	createDoc(import);	
	return true;
}


bool
AosSdocImport::createDoc(const AosImport &import)
{
	AosXmlTagPtr xml= import.mXml;
	AosXmlTagPtr actions = import.mActions;
	bool modify = import.mModify;
	AosRundataPtr rdata = import.mRundata;
	
	bool rslt = createDoc(xml, actions, modify, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	return true;
}


bool
AosSdocImport::splitXml(const AosSplit &split)
{
	AosXmlTagPtr actions = split.mActions;
	bool cover = split.mCover;
	OmnString identify = split.mIdentify;
	OmnString tname = split.mTableName;
	OmnString container = split.mContainer;
	OmnConnBuffPtr buff = split.mBuff;
	AosRundataPtr rdata = split.mRundata;
	aos_assert_r(splitXml(buff, actions, cover, identify, tname, container, rdata), false);
	return true;
}


bool
AosSdocImport::addRequest(
		const AosXmlTagPtr &xml,
		const AosXmlTagPtr &actions,
		const bool &modify,
		const AosRundataPtr &rdata)
{
	AosImport import(xml, actions, modify, rdata);
	sgCreateDocLock->lock();
	mNumReqs ++;
	mQueue.push(import);
	sgCreateDocCondVar->signal();
	sgCreateDocLock->unlock();
	return true;
}


bool
AosSdocImport::addSplitRequest(
		const AosXmlTagPtr &actions,
		const bool &cover,
		const OmnString &identify,
		const OmnString &tname,
		const OmnString &container,
		const OmnConnBuffPtr &docBuff,
		const AosRundataPtr &rdata)
{
	AosSplit split(actions, cover, tname, identify, container, docBuff, rdata);
	sgSplitXmlLock->lock();
	mSplitQueue.push(split);
	sgSplitXmlCondVar->signal();
	sgSplitXmlLock->unlock();
	return true;
}



bool
AosSdocImport::signal(const int threadLogicId)
{
	return true;
}


bool
AosSdocImport::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
