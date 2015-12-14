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
#include "SmartDoc/SdocExportData.h"

#include "Debug/Error.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "Rundata/Rundata.h"
#include "QueryClient/QueryClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/WebRequest.h"
#include <dirent.h>                                                                                             
#include <sys/types.h>

AosSdocExportData::AosSdocExportData(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_EXPORTDATA, AosSdocId::eExportData, flag),
mDataFile(NULL)
{
}


AosSdocExportData::~AosSdocExportData()
{
}


bool
AosSdocExportData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// 1. parse the smart doc.
	// <doc zky_totalNum="" >
	// </doc>
	OmnString totalNum;
	if (sdoc)
	{
		totalNum = sdoc->getAttrStr("zky_totalNum");
	}

	// 2. run the query
	OmnString rslt_data;
	AosXmlTagPtr request = rdata->getReceivedDoc();
	AosXmlTagPtr qrydoc = request->getFirstChild("command");
	if (qrydoc)
	{
		qrydoc = qrydoc->getFirstChild();
	}
	aos_assert_rr(qrydoc, rdata, false);
	qrydoc->setAttr("psize", totalNum);
	
	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(qrydoc, rdata);
	if (!query || !query->isGood())
	{
		return false;
	}
	
	query->procPublic(qrydoc, rslt_data, rdata);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(rslt_data, "" AosMemoryCheckerArgs);
	AosXmlTagPtr rsltdata;
	if (!root || !(rsltdata = root->getFirstChild()))
	{
		return false;
	}
	
	// 3. export result to a file.
	//input:  <record attr1='' attr2='' .../>
	//		  title="xxx,yyy,"
	//		  attrs="a,b,c"
	//output: 
	//		xxx,yyy,zzz
	//		attr1,attr2,attr3
	//		attr1,attr2,attr3
	
	OmnString title = request->getChildTextByAttr("name", "zky_title");
	title << "\n";
	OmnString fname;	
	bool rslt = composeFileName(fname);
	if (!rslt)
	{
		return false;
	}
	mDataFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
    aos_assert_rr(mDataFile, rdata, false);   
    mDataFile->append(title);
	AosXmlTagPtr rcd = rsltdata->getFirstChild("record");
	while (rcd)
	{	OmnString data;
		rcd->removeAttr(AOSTAG_DOCID);
		rcd->resetAttrNameValueLoop();
		OmnString aname, avalue;
		bool finished = false;
		while (rcd->nextAttr("", aname, avalue, finished, false, true))
		{
			if (finished)	break;
			checkValue(avalue);
			data << avalue << ",";
		}
		data << "\n";
		mDataFile->append(data);
		rcd = rsltdata->getNextChild();
	}
	mDataFile = 0;
	
	//4. create a doc 
	OmnString docstr;
	docstr << "<object " << AOSTAG_RSC_FNAME << "=\"" << fname << "\" "
		<< AOSTAG_OTYPE << "=\"vpd\""
		<< "></object>";
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!doc)
	{
	 return false;
	}
	rslt = AosDocClientObj::getDocClient()->createDoc1(rdata, 0, "", doc, true, 0, 0, false);
	if (!rslt)
	{
		return false;
	}
    OmnString objid  = doc->getAttrStr(AOSTAG_OBJID);	
	OmnString contents;
	contents << "<Contents zky_docid=\""<< objid << "\" />";
	rdata->setResults(contents);
	return true;
}


bool 
AosSdocExportData::checkValue(OmnString &value)
{
	bool flag = false;
	//" ,
	int pos = 0;
	while (pos < value.length())
	{
		int idx = 0;
		if ((idx =value.indexOf(pos, '"')) != -1)
		{
			value.insert("\"", idx);
			if(!flag) flag = true;
			pos = idx + 1;
		}
		if ((idx =value.indexOf(pos, ',')) != -1)		
		{
			pos = idx;
			if(!flag) flag = true;
		}
		pos++;	
	}
	
	if (flag)
	{
		value.insert("\"", 0);
		value.insert("\"", value.length()-1);
	}
	return true;
}

bool 
AosSdocExportData::composeFileName(OmnString &fname)
{
	/*
	DIR *dir;
	OmnString filepath = "/usr/local/openlaszlo/lps-4.7.2/Server/lps-4.7.2/ExportData";
	if (!(dir = opendir(filepath.data())))                                                            
	{
		mkdir(filepath.data(), 0755);
		dir = opendir(filepath.data());
	}
	if (!dir)
	{
		aos_assert_r(dir, false);
	}
	closedir(dir);
	*/
	
	OmnString filepath = "";
	u64 tm = OmnGetSecond();
	fname << filepath << "/tmp_" << tm << ".csv";
	return true;
}
