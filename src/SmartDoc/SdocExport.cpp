#include "SmartDoc/SdocExport.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServerCb.h"
#include "SeReqProc/Download.h"
#include "SeReqProc/Ptrs.h"
#include "SEServer/SeReqProc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/Ptrs.h"
#include "ValueSel/ValueSelQuery.h"
#include "Util/StrParser.h"
#include "QueryClient/QueryClient.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include <dirent.h>
#include <sys/types.h>
// Chen Ding, 05/31/2012
// static	bool sgInitFilePath = false;
#define AOSCTNR_EXPORTCTNR      "ctnr_export"
#define eMaxDocLen 100
#if 0

AosSdocExport::AosSdocExport(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_EXPORT, AosSdocId::eExport, flag)
{
	mFilePath = "Export/";
}

AosSdocExport::~AosSdocExport()
{
}


bool 
AosSdocExport::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//the smart doc should be this form:
	//<sdoc zky_doc_sep=”xxx” zky_field_sep=”xxx”>
	//	<query/>
	//	<fields>
	//		<field xpath=”xxx” name=”xxx” sep=”xxx” dft=”xxx”/>
	//		<field .../>
	//		...	
	//	</fields>
	//</sdoc>
	//For more information, please refer to the sdoc which named "sdoc_export"
	if(!sdoc)
	{
		rdata->setError() << "Failed retrieving the smart doc";
		return false;
	}

	AosXmlTagPtr fields = sdoc->getFirstChild("fields");
	if (!fields)
	{
		rdata->setError() << "Missing fields tag";
		return false;
	}

	mHeader.clear();
	mHeaderBd.clear();
	AosXmlTagPtr field = fields->getFirstChild();
	while(field)
	{
		OmnString header = field->getAttrStr(AOSTAG_HEADER);
		if (header == "")
		{
			rdata->setError() << "header should not be empty";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mHeader.push_back(header);	
		OmnString header_bd = field->getNodeText();
		if (header_bd == "")
		{
			rdata->setError() << "header data bind should not be empty";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mHeaderBd.push_back(header_bd);
		field = fields->getNextChild();
	}

	OmnString rslt_data;
	AosXmlTagPtr qry_xml = sdoc->getFirstChild("query");
	if (!qry_xml)
	{
		rdata->setError() << "Failed retrieving query";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr query_xml = qry_xml->clone(AosMemoryCheckerArgsBegin);

	//write to the file
	OmnString fname = "";
	OmnString objid = "";
	bool rslt = composeFileName(fname, objid);
	if (!rslt)
	{
		rdata->setError() << "fail to create file";
		return false;
	}
	OmnString filename = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	filename << "/" << mFilePath << fname;
	OmnFilePtr mDataFile = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(mDataFile && mDataFile->isGood(), false);
	AosQueryRsltObjPtr qRslt = queryData(query_xml, rdata);
	if(!qRslt)
	{
		rdata->setError() << "fail to query the data";
		return false;
	}
	/*
	OmnString ctnr_objid = query_xml->getAttrStr("zky_ctnr_objid");
	if (ctnr_objid == "")
	{
		rdata->setError() << "Missing container objid";
		return false;
	}

	vector<OmnString> objids;
	int num = AosQuery::getSelf()->retrieveContainerMembers(ctnr_objid, 
			objids, 10000, 0, false, rdata);
	if (num == 0)
	{
		rdata->setError() << "no elements in container";
		return false;
	}
*/
	//rslt = exportToFile(objids, mDataFile, rdata);
	rslt = exportToFile(qRslt, mDataFile, rdata);
	if(!rslt)
	{
		rdata->setError() << "fail to export data";
		return false;
	}

	// Now we should create a file to describe the excel file.
	// the file should contain two attributes: zky_export_dir,
	// zky_export_filename.
	/*
	OmnString fileobjid = sdoc->getAttrStr(AOSTAG_FILEOBJID);
	if (fileobjid == "")
	{
		rdata->setError() << "fail to get file objid";
		return false;
	}
	*/
	OmnString filestr;
	filestr << "<exportfile "
		<< AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_RSCDIR << "=\"" << mFilePath << "\" "
		<< AOSTAG_RSC_FNAME << "=\"" << fname << "\"" 
		<< AOSTAG_PARENTC << "=\"" << AOSCTNR_EXPORTCTNR << "\" />";
	
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(filestr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);

	//rslt = docclient->createDocSafe1(rdata, filestr, "", "",
	//		true, false, false, false, false, false);
	AosXmlTagPtr newdoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, doc, "", "",
			        true, false, false, false, false, true, true);
	if (!rslt)
	{
		rdata->setError() << "fail to create file about the export file";
		return false;
	}

	/*
	// to download the export file.
	OmnString docstr = "<request>";
	docstr << "<item name='args'><![CDATA[objid=" << objid << "]]></item>"
		<< "</request>";
	AosXmlTagPtr request = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(request, false);
	rdata->setRequestRoot(request);
	AosDownloadPtr dwld = OmnNew AosDownload(false);
	aos_assert_r(dwld, false);
	rslt = dwld->proc(rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to download the file";
		return false;
	}
	*/
	OmnString respstr = "<Contents ";
	respstr << AOSTAG_OBJID << "=\"" << objid << "\" />";
	rdata->setResults(respstr);
	return true;
}


AosQueryRsltObjPtr
AosSdocExport::queryData(
		AosXmlTagPtr &qry_xml,
		const AosRundataPtr &rdata)
{
	// This funciton query the data from our system,
	// we use AosValueSelQuery to query data.
	AosValueRslt vRslt;
	// Chen Ding, 05/31/2012
	// AosValueSelQueryPtr vQry = OmnNew AosValueSelQuery(false);
	AosValueSelObjPtr vQry = AosValueSelObj::getValueSelStatic(AosValueSelType::eQuery, rdata);
	aos_assert_r(vQry, 0);
	aos_assert_r(vQry->run(vRslt, qry_xml, rdata), 0);

	AosQueryRsltObjPtr qRslt = vRslt.getQueryRslt(rdata.getPtr());
	if (!qRslt)
	{
		rdata->setError() << "Failed to query data";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	return qRslt;
}


/*
bool
AosSdocExport::exportToFile(
		const vector<OmnString> &objids,
		OmnFilePtr &mDataFile,
		const AosRundataPtr &rdata)
		*/
bool
AosSdocExport::exportToFile(
		const AosQueryRsltObjPtr &qrslt,
		OmnFilePtr &mDataFile,
		const AosRundataPtr &rdata)
{
	//export data
	aos_assert_r(mDataFile, false);
	
	// export the  column name.
	for (int i=0; i<(int)mHeader.size(); i++)
	{
		mDataFile->append(mHeader[i]);
		mDataFile->append("\t");
	}
	mDataFile->append("\n");
	
	/*
	for (int i=0; i< objids.size(); i++)
	{
		AosXmlTagPtr doc = docclient->getDocByObjid(objids[i], rdata);
		if (!doc)
		{
			rdata->setError() << "Failed to get doc: " << objids[i]; 
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		//export contents to file.
		for (size_t j=0; j<mColumns.size(); j++)
		{
	   		OmnString value = doc->getAttrStr(mColumns[j]);
	        mDataFile->append(value);
	        mDataFile->append("\t");
	    }
	    mDataFile->append("\n");
	}
*/
	u64 docid;
	bool finished;
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	qrslt->nextDocid(docid, finished, rdata);
	while(docid && !finished)
	{
		// Retrieve doc.
		AosXmlTagPtr doc = docclient->getDocByDocid(docid, rdata);
		if (!doc)
		{
			rdata->setError() << "Failed to get doc: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	
		for (int j=0; j<(int)mHeaderBd.size(); j++)
		{
			OmnString value = doc->getAttrStr(mHeaderBd[j]);
			mDataFile->append(value);
			mDataFile->append("\t");
		}
		mDataFile->append("\n");
		qrslt->nextDocid(docid, finished, rdata);
	}
	return true;
}



bool
AosSdocExport::composeFileName(OmnString &fname, OmnString &objid)
{
	DIR *dir;
	OmnString fdir = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	fdir << "/" << mFilePath;
	if (!(dir = opendir(fdir.data())))
	{
		mkdir(fdir.data(), 0755);
		dir = opendir(fdir.data());
	}
	if (!dir)
	{
		aos_assert_r(dir, false);
	}
	closedir(dir);

	u64 tm = OmnGetSecond();
	OmnString filename;
	filename << "tmp_" << tm << ".txt";
	objid = filename;
	fname = filename;
    return true;
}

#endif
