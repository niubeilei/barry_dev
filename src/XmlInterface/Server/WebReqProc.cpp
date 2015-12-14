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
//
// Modification History:
// 03/24/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Server/WebReqProc.h"

#include "Alarm/Alarm.h"
#include "Book/BookMgr.h"
#include "Book/Container.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataService/DataService.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Proggie/TaggedData/TaggedData.h"
#include "Proggie/ProggieUtil/TcpRequest.h"
#include "SearchEngine/SeXmlParser.h"
#include "SearchEngine/XmlTag.h"
#include "SearchEngine/DocServer.h"
#include "TinyXml/TinyXml.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "TorturerWrappers/ImgConverter.h"
#include "UserMgmt/User.h"
#include "util_c/strutil.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Passwd.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/XmlObj.h"
#include <iostream>
#include <fstream>
#include <sstream>


static OmnMutex sgLock;
static int sgUploadImgSeqno;

static AosBookMgr sgBookMgr;
static AosDataService sgDataService;
static AosImgConverterPtr sgImgConverter;

const int sgObjtableFnamesCt = 13;
static OmnString sgObjtableFnames[sgObjtableFnamesCt]={"dataid", "name", "tnail", 
			"vvpdname", "container", "tags", "xml", "evpdname", 
			"type", "subtype", "creator", "createtime", "updatetime"};

const int sgTagtableFnamesCt = 10;
static OmnString sgTagtableFnames[sgTagtableFnamesCt]={"tag", "dataid", "type", "thumbnail",
			"name", "ordername", "description", "creator", "createtime", "xml"};

static int sgStdTableFnamesCt = 4;
static OmnString sgStdTableFnames[4] = {"name", "keywords", 
			"description", "xml"};



AosWebReqProc::AosWebReqProc()
:
mNumReqs(0)
{
	mLock = OmnNew OmnMutex();
	if (!sgImgConverter) 
	{
		sgImgConverter = OmnNew AosImgConverter();
		AosObjectWrapper::setImgConverter(sgImgConverter);
	}
}


AosWebReqProc::~AosWebReqProc()
{
}


bool			
AosWebReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);

	/*
	const char *ddd = buff->getData();
	char cdata[20000];
	int llen = buff->getDataLength();
	if (llen >= 19999) llen = 19999;
	strcpy(cdata, ddd);
	cout << __FILE__ << ":" << __LINE__ 
		<< "To process request: " << buff->getDataLength() << endl;
	int mmm;
	for (mmm=0; mmm+200<llen; mmm+=200)
	{
		char nnn[201];
		strncpy(nnn, &cdata[mmm], 200);
		cout << nnn << endl;
	}
	cout << &cdata[mmm] << endl;
	*/
	OmnScreen << "To process request: " << req->getData() << ":" << conn->getSock() << endl;


	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;
	char *data = req->getData();

	TiXmlDocument thedoc;
	thedoc.Parse(data);
	TiXmlNode *root = thedoc.FirstChild();
	if (!root)
	{
		sendResp(req, errcode, "Message incorrect", "");
		return false;
	}

	mUsername = root->getChildTextByAttr("name", "username", 0);
	const char *dmid = root->getChildTextByAttr("name", "dmid", 0);
	const char *operation = root->getChildTextByAttr(
			"name", "operation", "");

	OmnTrace << "Operation: " << operation << endl;
	OmnString contents;
	if (!dmid)
	{
		// There is no dmid. Get the operation
		if (strcmp(operation, "getUploadId") == 0) 
		{
			req->sendResponse("chending");
			system("mkdir /tmp/chending");
			system("chmod 777 /tmp/chending");
			return true;
		}
		if (strcmp(operation, "modifyObj") == 0) 
			return modifyObj(req, root);
		if (strcmp(operation, "retrieve") == 0) 
			return retrieveObj(req, root);

		if (strcmp(operation, "delObject") == 0) 
		{
			AosObjectWrapper obj;
			obj.delObj(root, errcode, errmsg);
		}
		else if (strcmp(operation, "retlist_by_sql") == 0) 
			retrieveList(req, root, contents, errcode, errmsg);
		else if (strcmp(operation, "serverCmd") == 0) 
			procServerReq(req, root, contents, errcode, errmsg);
		else if (strcmp(operation, "serverreq") == 0) 
			procServerreq(req, root, contents, errcode, errmsg);
		else if (strcmp(operation, "sendmsg") == 0) 
			sendmsg(req, root, contents, errcode, errmsg);
		else if (strcmp(operation, "regsvr") == 0) 
		{
			regServer(req, root, contents, errcode, errmsg);
			return true;
		}
		else if (strcmp(operation, "retrieve_data") == 0) 
			sgDataService.retrieveData(root, contents, errcode, errmsg);
		else if (strcmp(operation, "retrieve_tagdata") == 0) 
			retrieveTagData(root, contents, errcode, errmsg);
		else
		{
			errmsg = "Unrecognized operation: ";
			errmsg << operation;
			OmnAlarm << errmsg << enderr;
		}
		sendResp(req, errcode, errmsg, contents);
		return true;
	}
	else
	{
		errmsg = "Unrecognized data manager ID: ";
		errmsg << dmid;
	}
	sendResp(req, errcode, errmsg, contents);
	return true;
}


bool
AosWebReqProc::retrieveList(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// Retrieve the tablename 
	const char *tname = root->getChildTextByAttr("name", "tablename", 0);
	OmnTrace << "tablename: " << tname << endl;

	const char *fnames = root->getChildTextByAttr("name", "fieldnames", 0);
	OmnTrace << "fieldnames: " << fnames << endl;

	OmnString query = root->getChildTextByAttr("name", "stmt", 0);
	OmnTrace << "query: " << query<< endl;

	const char *idx = root->getChildTextByAttr("name", "index", 0);
	OmnTrace << "idx: " << idx << endl;

	const char *pagesize = root->getChildTextByAttr("name", "pagesize", 0);
	OmnTrace << "pagesize: " << pagesize << endl;

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);

	// If 'idx' == 0, we need to count how many records the query
	// will have.
	OmnRslt rslt;
	u32 total = 0xffffffff;
	if (strcmp(idx, "0") == 0)
	{
		OmnString stmt = "select count(*) from ";
		stmt << tname << " " << query;
		OmnDbRecordPtr record;
		aos_assert_r(store->query(stmt, record), false);
		aos_assert_r(rslt, false);
		total = record->getU32(0, 0, rslt);
	}

	OmnString stmt = "select ";
	stmt << fnames << " from " << tname << " " << query;

	stmt << " limit " << idx << ", " << pagesize;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	aos_assert_r(rslt, false);

	table->reset();
	OmnDbRecordPtr record;
	if (total != 0xffffffff)
	{
		contents = "<Contents total=\"";
		contents << total << "\"";
	}
	else
		contents << "<Contents";
	contents << " num=\"" << table->entries() << "\">";

	// !!!!!!!!!!!!!! In the current implementations, we
	// handle two cases: retrieving 'name' only or 'name, xml'. 
	OmnString ss;
	while (table->hasMore())
	{
		record = table->next();
		ss = record->getStr(0, "", rslt);
		if (record->getNumFields() == 2)
		{
			contents << "<entry name=\"" << ss
				<< "\">" << record->getStr(1, "", rslt)
				<< "</entry>";
		}
		else
		{
			contents << "<entry name=\"" << ss << "\"/>";
		}
	}
	contents << "</Contents>";

	char * data = contents;
	for (int i=0; i<contents.length(); i++)
		cout << data[i];
	cout << "\n";
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::retrieveObj(const AosNetRequestPtr &req, TiXmlNode *root) 
{
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;

	// Retrieve the objid
	const char *objid = root->getChildTextByAttr("name", "objid", 0);
	if (!objid)
	{
		errmsg = "Missing the objid!";
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	const char *ffs = root->getChildTextByAttr("name", "fnames", 0);

	AosObjectWrapper wp;
	OmnString resp;
	if (ffs)
	{
		const char *tname = root->getChildTextByAttr("name", "tname", 0);
		const char *objid_fname = root->getChildTextByAttr(
			"name", "objidfname", 0);
		
		if (!tname || strlen(tname) == 0)
		{
			errmsg = "Missing the table name attribute!";
			OmnAlarm << errmsg << enderr;
			sendResp(req, errcode, errmsg, "");
			return false;
		}

		if (!objid_fname || strlen(objid_fname) == 0)
		{
			errmsg = "Missing the objid_fname attribute!";
			OmnAlarm << errmsg << enderr;
			sendResp(req, errcode, errmsg, "");
			return false;
		}

		if (!wp.getObject(tname, objid_fname, objid, 
			ffs, resp, errmsg))
		{
			sendResp(req, errcode, errmsg, "");
			return false;
		}
	}
	else
	{
		resp = "<Contents>";
		if (!wp.getObject(objid, errmsg))
		{
			OmnAlarm << errmsg << enderr;
			sendResp(req, errcode, errmsg, "");
			return false;
		}
		resp << wp.getXml() << "</Contents>";
	}
	sendResp(req, eAosXmlInt_Ok, "", resp);
	return true;
}


bool
AosWebReqProc::modifyObj(const AosNetRequestPtr &req, TiXmlNode *root) 
{
	// It is to save an object.
	// 'root' should be in the form:
	// 	<tag ...>
	// 		<xmlobj ...>
	// 			<the object to be saved>
	// 		</xmlobj>
	// 		...
	// 	</tag>
	// where 'xmlobj' tag is the object to be saved. 
	//
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;

	TiXmlNode *child = root->FirstChild("xmlobj");
	if (!child)
	{
		// Missing the object to save. This is an error.
		errmsg = "Missing the object to save!";
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	child = child->FirstChild();

	if (!child)
	{
		errmsg = "Object is incorrect!";
		sendResp(req, errcode, errmsg, "");
		return false;
	}
	
	TiXmlElement *childelem = child->ToElement();
	if (!childelem)
	{
		errmsg = "Message not correct!";
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	// Retrieve the objid
	const char *real_objid = childelem->Attribute("objid");
	if (!real_objid)
	{
		ostringstream oss(ostringstream::out);
		oss << *childelem;
		OmnScreen << "The object: " << oss.str().c_str() << endl;
		errmsg = "Missing the objid in the object!";
		OmnAlarm << errmsg << enderr;
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	// Retrieve the dmid
	OmnString dmid = childelem->Attribute("dmid");
	if (dmid.length() > 0)
	{
		// We will process the request based on the dmid
		bool result;
		if (dmid == "form")
		{
			result = sgBookMgr.processReq(childelem, errcode, errmsg);	
		}

		if (result) sendResp(req, eAosXmlInt_Ok, "", "");
		else sendResp(req, errcode, errmsg, "");
		return result;
	}
		
	AosObjectWrapper wp;
	wp.setName(real_objid);
 
	// Object type serves as the table name
	char *substrs[3];
	int num = aos_str_split((char *)real_objid, '_', substrs, 3);
	if (num <= 0)
	{
		errmsg = "Internal error: ";
		errmsg << __FILE__ << ":" << __LINE__;
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	wp.setTablename(substrs[0]);
	wp.setUsername(mUsername);

	aos_str_split_releasemem(substrs, 3);

	OmnString contents;
	if (!wp.modifyObj(childelem, contents, errcode, errmsg)) 
	{
		OmnAlarm << "Failed to save the object: " << errmsg << enderr;
		sendResp(req, errcode, errmsg, "");
		return false;
	}
	errcode = eAosXmlInt_Ok;
	sendResp(req, eAosXmlInt_Ok, "", contents);
	return true;
}


void
AosWebReqProc::sendResp(
		const AosNetRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents)
{
	OmnString resp = "<status error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\">" << errmsg << "</status>";

	if (contents != "" ) resp << contents;

OmnTrace << "Send response: " << resp << endl;
	req->sendResponse(resp);
}


bool
AosWebReqProc::procServerReq(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// The request is identifies by the arguments:

	// 'root' should be in the form:
	// 	<tag ...>
	// 		<command ...>
	// 			<operation opr="xxx" .../>
	// 		</command>
	// 		<objdef .../>
	// 	</tag>
	//
	errcode = eAosXmlInt_General;
	contents = "";

	TiXmlNode *child = root->FirstChild("command");
	if (!child)
	{
		errmsg = "Failed to retrieve the command!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	child = child->FirstChild();
	if (!child)
	{
		errmsg = "Object is incorrect!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	TiXmlElement *childelem = child->ToElement();
	if (!childelem)
	{
		errmsg = "Message not correct!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	const char *opr = childelem->Attribute("opr");
	if (!opr)
	{
		errmsg = "Missing the opr attribute!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	char c1 = opr[0];
	bool processed = false;
	bool rslt;
	switch (c1)
	{
	case 'c':
		 if (strcmp(opr, "ctContainer") == 0)
			 return createContainer(req, childelem, errcode, errmsg);

		 if (strcmp(opr, "createcobj") == 0)
		 {
			 AosObjectWrapper wp;
			 wp.setUsername(mUsername);
			 return wp.createCtnrObj(root, childelem, contents, errcode, errmsg);
		 }
		 break;

	case 'g':
		 if (strcmp(opr, "getFileNames") == 0)
		 {
			 processed = true;
			 rslt = getFileNames(req, childelem, contents, errcode, errmsg);
		 }
		 break;

	case 'l':
		 if (strcmp(opr, "login") == 0)
		 {
			 processed = true;
			 rslt = checkLogin(req, childelem, contents, errcode, errmsg);
		 }
		 break;

	case 'r':
		 if (strcmp(opr, "retlist") == 0 || strcmp(opr, "retmems") == 0)
		 {
			 processed = true;
			 rslt = retrieveList(req, childelem, contents, errcode, errmsg);
		 }
		 else if (strcmp(opr, "retonemem") == 0)
         {
             processed = true;
             rslt = retrieveObj(req, childelem, contents, errcode, errmsg);
         }
		 break;

	case 's':
		 if (strcmp(opr, "sendmail") == 0)
		 {
			 processed = true;
			 AosPasswdSvr passwd;
			 rslt = passwd.sendmail(mUsername, req, childelem, contents, errcode, errmsg);
		 }
		 break;

	case 'x':
		 if (strcmp(opr, "xmldbobj") == 0)
		 {
			 processed = true;
			 AosXmlObj xobj;
			 rslt = xobj.getXmlObj(childelem, contents, errcode, errmsg);
		 }
		 break;
	}
	
	if (!processed)
	{
		errmsg = "Unrecognized server request: ";
    	errmsg << opr;
		OmnAlarm << errmsg << enderr;
		rslt = false;
	}
	return rslt;
}


bool
AosWebReqProc::checkLogin(
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It is to create a container
	// 'childelem' should be in the form:
	// 	<operation opr="login"
	// 		username="xxx"
	// 		password="xxx"/>
	// 	</operation>
	//
	errcode = eAosXmlInt_General;
	
	const char *name = childelem->Attribute("username");
	const char *passwd = childelem->Attribute("password");
	const char *container = childelem->Attribute("container");

cout << __FILE__ << ":" << __LINE__ << ": container: " << container << endl;
	OmnString dataid;
	AosXmlTagPtr xml;
	bool rslt = AosUser::checkLogin(name, passwd, container, "objtable", 
			dataid, xml, errmsg);

	// Add an entry into the tag_table. 
	AosObjectWrapper ow;
	ow.addLoginEntry(name, dataid, xml, contents, rslt, errmsg);
	if (rslt) errcode = eAosXmlInt_Ok;
	return rslt;
}


bool
AosWebReqProc::createContainer(
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It is to create a container
	// 'childelem' should be in the form:
	// 	<operation opr="createContainer"
	// 		name="xxx"
	// 		desc="xxx"
	// 		tags="xxx"
	// 		creator="xxx/>
	// 	</operation>
	//
	errcode = eAosXmlInt_General;
	
	AosContainer container;
	
	OmnString name = childelem->Attribute("name");
	OmnString tags = childelem->Attribute("tags");
	OmnString desc = childelem->Attribute("desc");
	OmnString creator = childelem->Attribute("creator");

	bool rslt = container.createContainer(name, 
		tags, desc, creator, errcode, errmsg);
	aos_assert_r(rslt, false);
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::getFileNames(
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem, 
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	static const int lsDefaultPageSize =50;
	static const OmnString lsDefaultSep = "|^|";

	// It retrieves file names. 'childelem' should be in the form:
	// 	<operation opr="getFileNames'
	// 		sep="xxx"
	// 		recursive="xxx"
	// 		dirnames="xxx"
	// 		psize="xxx"
	// 		start_idx="xxx" (optional)/>
	errcode = eAosXmlInt_General;
	const char *pp = childelem->Attribute("psize");
	if (!pp)
	{
		errmsg = "Missing page size!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	int pagesize = atoi(pp);
	if (pagesize <= 0) pagesize = lsDefaultPageSize;

	pp = childelem->Attribute("start_idx");
	int index = (pp)?atoi(pp):0;
	if (index < 0) index = 0;

	pp = childelem->Attribute("recursive");
	bool recur = (pp)?(pp[0] != '0'):true;

	OmnString sep = childelem->Attribute("sep");
	if (sep == "") sep = lsDefaultSep;

	OmnString dirs = childelem->Attribute("dirnames");

	OmnFile ff;
	std::list<OmnString> thelist;
	int total = ff.getFilesMulti(dirs, thelist, sep, recur);

	// Calculate the number of entries for this request
	int num = thelist.size() - index;
	if (num < 0) num = 0;
	if (num > pagesize) num = pagesize;

	contents = "<Contents total=\"";
	contents << total << "\" num=\"" << num << "\">";

	// Skip the first index-1 entries
	std::list<OmnString>::iterator it;
	if (num > 0)
	{
		int crt_idx = 0;
		int num_procd = 0;
		for (it=thelist.begin(); it != thelist.end(); it++)
		{
			if (crt_idx < index) continue;
			if (num_procd != 0) contents << sep;
			contents << *it;
			num_procd++;
			if (num_procd >= num) break;
		}
	}
	contents << "</Contents>";
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::retrieveTagData(
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// Retrieve the tablename 
	const char *tname = root->getChildTextByAttr("name", "tablename", 0);
	OmnTrace << "tablename: " << tname << endl;

	const char *fnames = root->getChildTextByAttr("name", "fieldnames", 0);
	OmnTrace << "fieldnames: " << fnames << endl;

	const char *query = root->getChildTextByAttr("name", "stmt", 0);
	OmnTrace << "query: " << query<< endl;

	const char *order = root->getChildTextByAttr("name", "order", 0);
	OmnTrace << "order: " << order << endl;

	const char *idx = root->getChildTextByAttr("name", "index", 0);
	OmnTrace << "idx: " << idx << endl;
	int the_idx = (idx)?atoi(idx):0;

	const char *pagesize = root->getChildTextByAttr("name", "pagesize", 0);
	OmnTrace << "pagesize: " << pagesize << endl;
	int psize = (pagesize)?atoi(pagesize):eDefaultPageSize;

	const char *tags = root->getChildTextByAttr("name", "tags", 0);
	OmnTrace << "tags: " << tags << endl;

	AosTaggedData obj;
	obj.getData(tags, fnames, the_idx, psize, 
		   	query, (order)?order:"order by dataid", the_idx == 0, 
	   	 	contents, errcode, errmsg);		   

	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::retrieveList(
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem, 
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	static const int lsDefaultPageSize =50;

	// It retrieves a list of data:
	// 	<operation opr="retlist'
	// 		psize="xxx"
	// 		query="xxx"
	// 		tname="xxx"
	// 		order="xxx"
	// 		dir="xxx"		// either 'asc' or 'des', dft: 'asc'
	// 		fnames="xxx"
	// 		get_total="true"
	// 		start_idx="xxx" (optional)/>
	// 	Refer to 'constructQuery(..)' for information about how 
	// 	'query' should be defined.
	errcode = eAosXmlInt_General;

	const char *tname = childelem->Attribute("tname");
	if (!tname)
	{
		errmsg = "Missing 'tname' field!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString query;
   	if (!constructQuery(childelem->Attribute("query"), query, errmsg))
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString order_fname = childelem->Attribute("order");
	const char *dir = childelem->Attribute("dir");
	mIsAscending = true;
	if (dir && strcmp(dir, "des") == 0) mIsAscending = false;
	const char *pp = childelem->Attribute("psize");
	int pagesize = atoi(pp);
	if (pagesize <= 0) pagesize = lsDefaultPageSize;

	pp = childelem->Attribute("start_idx");
	int start_idx = (pp)?atoi(pp):0;
	if (start_idx < 0) start_idx = 0;

	// Process 'fnames'
	// 'fnames' is defined in the form:
	// 	"fname|$|mapped_name|$|type|$$|..."
	// where 'type' indicates how to pack the data into the response. 
	// Allowed values are:
	// 	1: as an attribute
	// 	2: as a subtag, enclosed by CDATA
	// 	3: as an XML subtag
	const char *ffs = childelem->Attribute("fnames");
	if (!ffs)
	{
		errmsg = "Missing the fnames field!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	bool finished;
	OmnString parts[eMaxFields];
	AosStrSplit slt(ffs, "|$$|", parts, eMaxFields, finished);
	aos_assert_r(slt.entries() > 0, false);

	// Construct the name list for the SQL statement	
	OmnString namelist = "distinct ";
	bool isStdField[eMaxFields];
	OmnString fnames[eMaxFields];
	OmnString mapped_names[eMaxFields];
	char ftypes[eMaxFields];
	bool withXml = false;
	int numFields = slt.entries();
	int numNonStdFields = 0;
	int numQueriedFields = 0;
	for (int i=0; i<slt.entries(); i++)
	{
		OmnString pp[5];
		AosStrSplit ss(parts[i].data(), "|$|", pp, 5, finished);

		OmnString nnn = pp[0];

		// Check whether the field should be retrieved from 
		// the xml object. Below are the fields that can be
		// retrieved from tables directly. Other fields must 
		// be retrieved from xml objects. 
		// 1. For objtable:
		// 		"dataid, name, tnail, vvpdname, container, 
		// 		tags, xml, evpdname".
		//
		// 2. For all other tables:
		// 		"name, keywords, description, xml"
		bool found = isStdFname(tname, nnn);

		fnames[i] = nnn;
		mapped_names[i] = (ss.entries()>1)?pp[1]:nnn;
		ftypes[i] = (ss.entries() > 2)?pp[2].data()[0]:'2';
		isStdField[i] = found;
		if (found) 
		{
			// It is a standard name. If it is 'xml', we need to
			// put it to the end.
			if (nnn == "xml") withXml = true;
			else 
			{
				if (numQueriedFields != 0) namelist << ",";
				numQueriedFields++;
				// if (nnn == "dataid" && strcmp(tname, "tag_table") == 0)
				// 	namelist << "distinct ";
				namelist << nnn;
			}
		}
		else
		{
			numNonStdFields++;
		}
	}

	if (numNonStdFields > 0 || withXml) 
	{
		if (numQueriedFields > 0) namelist << ", ";
		namelist << "xml";
		numQueriedFields++;
	}

	if (numQueriedFields <= 0)
	{
		errmsg = "No field names!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);

	// If 'start_idx' == 0, we need to count how many records the query
	// will have.
	OmnRslt rslt;
	u32 total = 0xffffffff;
	const char *get_total = childelem->Attribute("get_total");
	if (get_total && strcmp(get_total, "true") == 0)
	{
		OmnString stmt = "select count(*) from ";
		stmt << tname << " " << query;
		OmnDbRecordPtr record;
		aos_assert_r(store->query(stmt, record), false);
		aos_assert_r(rslt, false);
		aos_assert_r(record, false);
		total = record->getU32(0, 0, rslt);
	}

	// bool isStdOrder = isStdFname(tname, order_fname);
	OmnString stdOrderFnames, xmlOrderFnames;
	parse_order(tname, order_fname, stdOrderFnames, xmlOrderFnames);

	// Chen Ding, 03/11/2010
	if (xmlOrderFnames.length() > 0 && !withXml)
	{
		if (numQueriedFields > 0) namelist << ", ";
		namelist << "xml";
		numQueriedFields++;
	}

	OmnString stmt = "select ";
	stmt << namelist << " from " << tname << query;
	if (stdOrderFnames.length() > 0)
		stmt << " order by " << stdOrderFnames;

	stmt << " limit " << start_idx << ", " << pagesize;

cout << __FILE__ << ":" << __LINE__ << ": stmt: " << stmt << endl;
	OmnDbTablePtr table;
	rslt = store->query(stmt, table);
	aos_assert_r(rslt, false);

	table->reset();
	OmnDbRecordPtr record;
	contents = "<Contents tname=\"";
	contents << tname << "\" start_idx=\"" << start_idx << "\"";
	if (total != 0xffffffff)
	{
		contents << " total=\"" << total << "\"";
	}
	contents << " num=\"" << table->entries() << "\">";

	// !!!!!!!!!!!!!! In the current implementations, we
	// handle two cases: retrieving 'name' only or 'name, xml'. 
	OmnDynArray<OmnDbRecordPtr, eArrayInitSize, 
		eArrayIncSize, eArrayMaxSize> records;
	OmnDynArray<AosXmlTagPtr, eArrayInitSize, 
		eArrayIncSize, eArrayMaxSize> xmls;
	if (xmlOrderFnames.length() > 0)
	{
		// The query shall be ordered by a non-standard field name.
		procNonStdResults(xmlOrderFnames, numQueriedFields, table, 
			records, xmls);
	}
	else
	{
		bool needXmls = false;
		for (int i=0; i<numFields; i++)
		{
			if (!isStdField[i])
			{
				needXmls = true;
				break;
			}
		}

		AosXmlTagPtr xml;
		while (table->hasMore())
		{
			record = table->next();
			records.append(record);
			if (needXmls)
			{
				OmnString value = record->getStr(numQueriedFields-1, "", rslt);
				AosXmlParser parser;
				xml = parser.parse(value, "");
				if (xml)
				{
					xml = xml->getFirstChild();
					xmls.append(xml);
				}
			}
		}
	}

	OmnString ss;
	for (int i=0; i<records.entries(); i++)
	{
		contents << "<record ";
		record = records[i];
		bool subtag_found = false;

		int fieldIdx = 0;
		AosXmlTagPtr xml;
		OmnString value;
		for (int nn = 0; nn<numFields; nn++)
		{
			if (isStdField[nn])
			{
				value = record->getStr(fieldIdx++, "", rslt);
			}
			else
			{
				xml = xmls[i];
				if (xml)
				{
					int len;
					u8 *vv = xml->getAttr(fnames[nn], len);
					if (vv && len > 0) value.assign((char *)vv, len);
					else value = "";
				}
				else
				{
					value = "";
				}
			}

			switch (ftypes[nn])
			{
			case '1':
			 	 // Attribute
			 	 if (subtag_found)
			 	 {
				 	 errmsg = "Attribute-type fields cannot be after "
					 	 "a subtag-type fields: ";
				 	 errmsg << ffs;
				 	 OmnAlarm << errmsg << enderr;
				 	 return false;
			 	 }
			 	 contents << mapped_names[nn] << "=\"" << value << "\" ";
			 	 break;

			case '2':
			 	 // Escaped subtag
			 	 if (!subtag_found) contents << ">";
			 	 subtag_found = true;
			 	 contents << "<" << mapped_names[nn] 
				 	 << "><![CDATA[" << value
				 	 << "]]></" << mapped_names[nn] << ">";
			 	 break;

			case '3':
			 	 // Non-escaped subtag
			 	 if (!subtag_found) contents << ">";
			 	 subtag_found = true;
			 	 contents << "<" << mapped_names[nn]
				 	 << ">" << value 
				 	 << "</" << mapped_names[nn] << ">";
			 	 break;

			default:
			 	 errmsg = "Unrecognized field type. The ";
			 	 errmsg << nn << "-th field. Fields: " << ffs; 
			 	 OmnAlarm << errmsg << enderr;
			 	 return false;
			}
		}
		if (subtag_found) contents << "</record>";
		else contents << "/>";
	}
	contents << "</Contents>";
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::constructQuery(
		const char *def, 
		OmnString &query, 
		OmnString &errmsg)
{
	// 'def' defines a query, which should be in the form:
	// 	<fname>|$|<opr>|$|<value>|$|<type>|$$|...
	// where '<type>' identifies whether it is string type (1) or 
	// integral type (2). Optionally, a query term can be 
	// just a string.
	query = "";

	OmnString pp[30];
	bool finished;
	AosStrSplit ss(def, "|$$|", pp, 30, finished);
	int idx = 0;
	for (int i=0; i<ss.entries(); i++)
	{
		OmnString qq = pp[i];
		OmnString parts[4];
		AosStrSplit tt(qq.data(), "|$|", parts, 4, finished);

		if (tt.entries() == 1)
		{
			if (idx == 0)
				query = " where ";
			else
				query << " AND ";
			query << qq;
			continue;
		}

		if (tt.entries() != 4)
		{
			errmsg = "Query incorrect: ";
			errmsg << def;
			return false;
		}

		OmnString mm = parts[3];
		if (mm.length() != 1)
		{
			errmsg = "Query incorrect: ";
			errmsg << def;
			return false;
		}
		 
		if (idx == 0) 
			query = " where ";
		else
			query << " AND ";
		idx++;
		if (mm == "1")
		{
			query << parts[0] << parts[1] << "'"
				<< parts[2] << "'";
		}
		else
		{
			query << parts[0] << parts[1] << parts[2];
		}
	}

	return true;
}


bool
AosWebReqProc::retrieveObj(
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem, 
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It retrieves a list of data:
	// 	<operation opr="retlist'
	// 		dataid="xxx"
	// 		fnames="xxx"/>
	errcode = eAosXmlInt_General;

	const char *dataid = childelem->Attribute("dataid");
	aos_assert_r(dataid, false);

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store, false);

	OmnString stmt = "select name, tnail, vvpdname, evpdname from objtable where ";
	stmt << "dataid='" << dataid << "'";

	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	aos_assert_r(rslt, false);
	if (!record)
	{
		// The object not found
		errmsg = "Object not found!";
		return false;
	}

	contents = "<Contents>";
	contents << "<record dataid='" << dataid 
		<< "' name='" << record->getStr(0, "", rslt)
		<< "' tnail='" << record->getStr(1, "", rslt) << "'";

	OmnString str = record->getStr(2, "", rslt);
	if (str != "") contents << " vvpdname='" << str << "'";
	str = record->getStr(3, "", rslt);
	if (str != "") contents << " evpdname='" << str << "'";
	contents << "/>";
	contents << "</Contents>";
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::isStdFname(
		const OmnString &tname, 
		const OmnString &fname)
{
	if (tname == "objtable" || tname == "images")
	{
		for (int k=0; k<sgObjtableFnamesCt; k++)
		{
			if (fname == sgObjtableFnames[k])
			{
				return true;
			}
		}
	}

	if (tname == "vertab")
	{
		if (fname == "dataid" || 
			fname == "origobj" ||
			fname == "xml") return true;
		return false;
	}

	if (strcmp(tname, "tag_table") == 0)
	{
		for (int k=0; k<sgTagtableFnamesCt; k++)
		{
			if (fname == sgTagtableFnames[k])
			{
				return true;
			}
		}
	}
	
	for (int k=0; k<sgStdTableFnamesCt; k++)
	{
		if (fname == sgStdTableFnames[k])
		{
			return true;
		}
	}

	return false;
}


bool
AosWebReqProc::procNonStdResults(
		const OmnString &order_fname,
		const int numFields,
		const OmnDbTablePtr &table, 
		OmnDynArray<OmnDbRecordPtr, eArrayInitSize, 
		eArrayIncSize, eArrayMaxSize> &records,
		OmnDynArray<AosXmlTagPtr, eArrayInitSize, 
		eArrayIncSize, eArrayMaxSize> &xmls)
{
	// The results should be ordered based on a non-standard
	// field name. This function first retrieves the ordered
	// field values for all the matched records. It then 
	// sort the values. It will return two arrays:
	// 		records[] 	all the records
	// 		xmls[]		all the xml objects
	// Both arrays are ordered by this function.
	//
	// IMPORTANT: the caller need to make sure it did retrieved
	// the XML objects, which is the last field in the select 
	// statement.
	//
	// IMPORTANT: currently we do not support multiple field ordering.
	
	// 1. First round, retrieve all the XML objects
	OmnDbRecordPtr record;
	AosXmlParser parser;
	AosXmlTagPtr root;
	records.clear();
	xmls.clear();
	OmnDynArray<OmnString, eArrayInitSize, 
		eArrayIncSize, eArrayMaxSize> values;
	while (table->hasMore())
	{
		record = table->next();
		OmnRslt rslt;
		OmnString value = record->getStr(numFields-1, "", rslt);
		root = parser.parse(value, "");
		if (root)
		{
			AosXmlTagPtr xml = root->getFirstChild();

			// Note that it is possible that there is no xml.
			// If no xml, the searched field value is an empty string.
			if (!xml) value = "";
			else 
			{
				int len;
				u8 *vv = xml->getAttr(order_fname, len);
				if (vv && len > 0) value.assign((char *)vv, len);
				else value = "";
			}

			values.append(value);
			records.append(record);
			xmls.append(root);
		}
	}

	// 2. The first round retrieved all the records, xmls, and
	//    the values and stored them in the arrays. This round
	//    will sort them using 'values'.
	//    All sorting is alphabetic. It can be either ascending 
	//    or descending, which is specified by 'mIsAscending'. 
	//    In the current implementation, we will use bubble sorting.
cout << __FILE__ << ":" << __LINE__ << ": Found values: " << values.entries() << ":" << records.entries() << endl;
	AosXmlTagPtr xml;
	for (int i=0; i<values.entries()-1; i++)
	{
		// Find the smallest entry for position 'i'.
		OmnString vv = values[i];
		for (int k=i+1; k<values.entries(); k++)
		{
			int rslt = strcmp(vv.data(), values[k].data());
			if ((mIsAscending && rslt > 0) || 
				(!mIsAscending && rslt < 0))
			{
				values[i] = values[k];
				values[k] = vv;
				vv = values[i];
				record = records[i];
				records[i] = records[k];
				records[k] = records[i];
				xml = xmls[i];
				xmls[i] = xmls[k];
				xmls[k] = xml;
			}
		}
	}
cout << __FILE__ << ":" << __LINE__ << ": Return values: " << values.entries() << ":" << records.entries() << endl;
	return true;
}


bool
AosWebReqProc::parse_order(
		const OmnString &tname, 
		const OmnString &order_fname, 
		OmnString &stdOrderFnames, 
		OmnString &xmlOrderFnames)
{
	// It is in the form:
	// 		fname [asce|desc], fname [asce|desc] ...
	// The field names can be standard or xml field names. 
	// It puts all the standard field names into 'stdOrderFnames' 
	// (including [asce|desc]), and all xml field names into
	// 'xmlOrderFnames'.
	stdOrderFnames = "";
	xmlOrderFnames = "";
	OmnString parts[30];
	bool finished;
	AosStrSplit split(order_fname.data(), ",", parts, 30, finished);
	for (int i=0; i<split.entries(); i++)
	{
		OmnString entry = parts[i];
		OmnString pp[2];
		AosStrSplit ss(entry.data(), " ", pp, 2, finished);
		OmnString fname = pp[0];
		if (isStdFname(tname, fname))
		{
			if (stdOrderFnames != "") stdOrderFnames << ",";
			stdOrderFnames << entry;
		}
		else
		{
			if (xmlOrderFnames != "") xmlOrderFnames << ",";
			xmlOrderFnames << fname;
		}
	}
cout << __FILE__ << ":" << __LINE__ << ": stdFnames: " << stdOrderFnames << endl;
cout << __FILE__ << ":" << __LINE__ << ": xmlFnames: " << xmlOrderFnames << endl;
	return true;
}


bool
AosWebReqProc::sendmsg(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// 'root' should be in the form:
	// 	<request ...>
	// 		<item name="rcvr">xxx</item>
	// 		<item name="msg">xxx</item>
	// 	</request>
	//
	errcode = eAosXmlInt_General;
	contents = "";

	OmnString sender = root->getChildTextByAttr("name", "sender", 0);
	OmnString receiver = root->getChildTextByAttr("name", "rcvr", 0);
	if (receiver == "")
	{
		errmsg = "Missing receiver!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString msg = root->getChildTextByAttr("name", "msg", 0);
	if (msg == "")
	{
		errmsg = "Missing message to send!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnScreen << "To send message: " << receiver << ":" << msg << endl;

	mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		if (mReqNames[i] == receiver)
		{
			// Found the receiver. 
OmnScreen << "To send message to: " << mReqNames[i] << endl;
			OmnString contents = "<Message><sender><![CDATA[";
			contents << sender << "]]></sender><contents><![CDATA["
				<< msg << "]]></contents></Message>";
			sendResp(mConns[i], eAosXmlInt_Ok, "", contents);
			mLock->unlock();
			errcode = eAosXmlInt_Ok;
			return true;
		}
	}

	errmsg = "Receiver not online!";
	mLock->unlock();
	errcode = eAosXmlInt_Ok;
	contents = "";
	return true;
}


bool
AosWebReqProc::regServer(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// 'root' should be in the form:
	// 	<request ...>
	// 		<item name="requester">xxx</item>
	// 		<item name="evt">xxx</item>
	// 	</request>
	//
	errcode = eAosXmlInt_General;
	contents = "";

	OmnString requester = root->getChildTextByAttr("name", "requester", 0);
	if (requester == "")
	{
		errmsg = "Missing requester!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString evt = root->getChildTextByAttr("name", "evt", 0);
	if (evt == "")
	{
		errmsg = "Missing message to send!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnScreen << "To register event: " << requester << ":" << evt << endl;
	i64 sec = OmnSystemRelativeSec1();
	bool needToNotify = true;
	bool found = false;

	// Search to see whether the user is alread online. 
	// The user is online if there is a record for it 
	// and its time is not expired yet.
	mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		if (mReqNames[i] == requester) 
		{
			if (mConns[i]) mConns[i]->closeConn();
			mConns[i] = req;
			found = true;
			if (mTime[i] + eTimeoutSec > sec)
			{
				// No need to notify
				needToNotify = false;
			}
			mTime[mNumReqs] = sec;
			break;
		}
	}

	OmnScreen << "Need to notify: " << needToNotify << endl;

	if (found)
	{
		if (needToNotify) notifyUsers(requester);
		mLock->unlock();
		errcode = eAosXmlInt_Ok;
		return true;
	}

	// Find the one that has been expired
	for (int i=0; i<mNumReqs; i++)
	{
		if (mTime[i] + eTimeoutSec < sec)
		{
			mReqNames[i] = requester;
			if (mConns[i]) mConns[i]->closeConn();
			mConns[i] = req;
			mTime[mNumReqs] = sec;
OmnScreen << "Registered: " << requester << endl;
			if (needToNotify) notifyUsers(requester);
			mLock->unlock();
			errcode = eAosXmlInt_Ok;
			return true;
		}
	}

	// Did not find it. Added it.
	mReqNames[mNumReqs] = requester;
	mConns[mNumReqs] = req;
	mTime[mNumReqs] = sec;
OmnScreen << "Registered: " << requester << endl;
	mNumReqs++;
	if (needToNotify) notifyUsers(requester);

	mLock->unlock();
	errcode = eAosXmlInt_Ok;
	contents = "";
	return true;
}


bool
AosWebReqProc::procServerreq(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// The request is identifies by the arguments:

	// 'root' should be in the form:
	// 	<tag ...>
	// 	</tag>
	//
	errcode = eAosXmlInt_General;
	contents = "";

	OmnString reqid = root->getChildTextByAttr("name", "reqid", "");

	if (reqid == "getusers")
	{
		return getOnlineUsers(req, root, contents, errcode, errmsg);
	}

	if (reqid == "getdomain")
	{
		return getDomain(req, root, contents, errcode, errmsg);
	}
	else if (reqid == "uploadimg")
	{
		uploadImgReq(req, contents, errcode, errmsg);
	}
	else
	{
		errmsg = "Unrecognized request id!";
		OmnAlarm << errmsg << enderr;
	}
	sendResp(req, errcode, errmsg, contents);
	return false;
}


bool
AosWebReqProc::getOnlineUsers(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// The request is identifies by the arguments:

	// 'root' should be in the form:
	// 	<tag ...>
	// 	</tag>
	//
	errcode = eAosXmlInt_General;
	contents = "";

	OmnString args = root->getChildTextByAttr("name", "args", "");

	contents = "<Users>";
	mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		contents << "<user><![CDATA[" << mReqNames[i] << "]]></user>";
	}
	contents << "</Users>";
	mLock->unlock();

	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::notifyUsers(const OmnString &username)
{
	// The caller should have locked the class.
	OmnString notify = "<status error=\"false\" code=\"200\"/><notify type=\"useronline\"><![CDATA[";
	notify << username << "]]></notify>";
	i64 sec = OmnSystemRelativeSec1();
	OmnScreen << "To notify user: " << mNumReqs << endl;
	for (int i=0; i<mNumReqs; i++)
	{
		OmnScreen << "Check: " << mReqNames[i] << endl;
		if (mReqNames[i] != username) 
		{
		OmnMark;
			if (mTime[i] + eTimeoutSec < sec)
			{
		OmnMark;
				if (mConns[i]) mConns[i]->closeConn();
				mConns[i] = 0;
				mReqNames[i] = "";
			}
			else
			{
		OmnMark;
				OmnScreen << "To send notification: " << mReqNames[i] << endl;
				sendResp(mConns[i], eAosXmlInt_Ok, "", notify);
			}
		}
	}

	return true;
}


bool
AosWebReqProc::getDomain(
		const AosNetRequestPtr &req, 
		TiXmlNode *root,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It retrieves the domain. The domain name is in "args".
	// If 'args' is empty, it is an error. If 'args' is 
	// 'dftdomain', it is the system default domain.

	errcode = eAosXmlInt_General;
	contents = "";

	OmnString domain = root->getChildTextByAttr("name", "args", "");

	if (domain == "")
	{
		errmsg = "Missing the domain name!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString rslt;
	if (domain == "dftdomain")
	{
		rslt = "http://58.211.230.20:8080/lps-4.6.1/images";
		// rslt = "http://192.168.99.98:8080/lps-4.6.1/images";
		// rslt = "http://dev.zykie.com:8080/lps-4.6.1/images";
	}
	else
	{
		errmsg = "Domain name not recognized: ";
		errmsg << domain;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	contents = "<Contents>";
	contents << rslt << "</Contents>";
		
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosWebReqProc::uploadImgReq(
		const AosNetRequestPtr &req, 
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It retrieves the domain. The domain name is in "args".
	// If 'args' is empty, it is an error. If 'args' is 
	// 'dftdomain', it is the system default domain.

	errcode = eAosXmlInt_General;
	contents = "";

	sgLock.lock();
	int seqno = sgUploadImgSeqno++;
	sgLock.unlock();

	OmnString cmd = "mkdir /tmp/";
	cmd << seqno;
	system(cmd);

	contents = "<Contents>";
	contents << "/tmp/" << seqno << "</Contents>";
		
	errcode = eAosXmlInt_Ok;
	return true;
}



