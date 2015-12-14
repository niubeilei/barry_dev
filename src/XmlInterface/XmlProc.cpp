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
// 04/17/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#if 0
#include "XmlInterface/XmlProc.h"

#include "alarm_c/alarm.h"
#include "Book/BookMgr.h"
#include "DataMgr/GroupDataMgr.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "DataStore/GenericDbObj.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "TinyXml/TinyXml.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "Util/File.h"
#include "Util/a_string.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "VpdParser/VpdParser.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Charset.h"
#include "XmlInterface/Template.h"
#include "XmlInterface/Dialog.h"
#include "XmlInterface/Rvg.h"

static OmnString sgDataDir = ".";
static OmnString sgWorkingDir;
static AosVpdParser sgVpdParser;
static AosBookMgr sgBookMgr;

AosXmlProc::AosXmlProc()
{
}


AosXmlProc::~AosXmlProc()
{
}


bool 
AosXmlProc::setDataDir(const OmnString &path)
{
	sgDataDir = path;
	return true;
}


bool 
AosXmlProc::setWorkingDir(const OmnString &path)
{
	sgWorkingDir = path;
	return true;
}


// 
// Description:
// 1. Open the file 'filename'
// 2. Parse the XML document 
// 3. Process the XML document
//
// Return:
// If success, 
// 	a. If nothing needs to return, it returns 0
// 	b. Otherwise, it returns the transaction ID
//
// If failed, it returns a transaction ID
//
// Returned data are encoded in the following format and stored in a transaction
// in the database. 
// 	<status>code</status>
// 	<errmsg>the error message</errmsg>
//
int
AosXmlProc::process(const OmnString &filename)
{
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString response;
	mFilename = filename;
	OmnString fn;
	OmnString errmsg;
	OmnString responseContents;

    if (filename.data()[0] == '/')
	{
		fn = filename;
	}
	else
	{
		fn << sgDataDir << "/" << filename;
	}

	if (!OmnFile::fileExist(fn.data()))
	{
		OmnAlarm << "File not found: " << fn << enderr;
		errmsg = "File not found!";
		return wrapup(errcode, responseContents, 0, errmsg);
	}

	/*
	node = aos_xml_node_create_from_file(filename.data(), 1);
	if (!node)
	{
		OmnString contents; 
		OmnFile f(filename, OmnFile::eReadOnly);
		f.readAll(contents);
		OmnAlarm << "Failed to parse the contents: " << contents << enderr;
		response = "<status>";
		response << eAosXmlInt_SyntaxError << "</status><errmsg>" 
			<< AosXmlInt_GetErrmsg(eAosXmlInt_SyntaxError) << "</errmsg>";
		goto wrapup;
	}

	aos_assert_g(node->mf->get_str_by_attr((aos_field_t *)node, 
		"name", "operation", &operation) == 1, wrapup);
	*/
	TiXmlDocument request(fn.data());
	request.LoadFile();
	TiXmlNode *root = request.FirstChild();
	TiXmlElement *elem = root->ToElement();
	const char *dmid = root->getChildTextByAttr("name", "dmid", "");
	if (!elem)
	{
		errmsg = "Invalid request: ";
		ostringstream oss(ostringstream::out);
		oss << *root;
		errmsg << oss.str();
		return wrapup(errcode, responseContents, root, errmsg);
	}

	if (strcmp(dmid, "group_obj") == 0)
	{
		AosGroupDataMgr dm;
		dm.process(elem, errcode, errmsg, responseContents);
	}
	else
	{
		const char *operation = root->getChildTextByAttr("name", "operation", "");
		if (strcmp(operation, "saveObject") == 0)
		{
			modifyObj(root, errcode, errmsg);
		}
		else
		{
			errcode = eAosXmlInt_IncorrectOperation;
			errmsg = "Incorrect Data Manager ID: ";
			errmsg << dmid;
		}
	}

/*	else if (strcmp(operation, "createObject") == 0) 
	{
		AosGroupDataMgr dm;
		dm.createObject(node, errcode, errmsg);
	}
	else if (strcmp(operation, "delete") == 0) 
	{
		delObj(node, errcode, errmsg);
	}
	else if (strcmp(operation, "retrieve") == 0) 
	{
		readObj(node, errcode, errmsg);
	}
	else if (strcmp(operation, "run_rvg") == 0)
	{
		AosRvg_Run(node, errcode, errmsg);
	}
	else if (strcmp(operation, "saveDbRecord") == 0)
	{
		saveDbRecord(node, errcode, errmsg);
	}
	else if (strcmp(operation, "serverProc") == 0)
	{
		if (serverProc(node, errcode, errmsg)) errcode = eAosXmlInt_Ok;
	}
	else
	{
		errcode = eAosXmlInt_IncorrectOperation;
		errmsg = "Incorrect operation: ";
		errmsg << operation;
	}
	*/
	return wrapup(errcode, responseContents, root, errmsg);
}


int
AosXmlProc::wrapup(
		const AosXmlRc errcode, 
		const OmnString &responseContents,
		TiXmlNode *node,
		OmnString &errmsg)
{
	OmnString response;

	if (errcode > 200)
	{
		response = "<status error=\"true\" msg=\""; 
		response << errmsg << "\">" << errcode << "</status>";

		if (node)
		{
			ostringstream oss(ostringstream::out);
			oss << *node;
			// It failed processing the request. 
			OmnAlarm << "Failed processing the request: \n" 
				<< "    Errcode: " << errcode << "\n"
				<< "    Errmsg: " << AosXmlInt_GetErrmsg(errcode) 
				<< ". " << errmsg<< "\n"
				<< "    Request: " << oss.str() << enderr;
		}
		else
		{
			OmnAlarm << "Failed processing the request: \n"
				<< "    Errcode: " << errcode << "\n"
				<< "    Errmsg: " << errmsg << enderr;
		}
	}
	else
	{
		response = "";
		if (responseContents.length() > 0)
		{
			response = "<status>200</status>";
			response << "<contents>" << responseContents << "</contents>";
		}
	}

	if (errcode == 200) return 0;

	u32 transId;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	store->storeTransData(response, transId);

	// if (objtype) aos_free(objtype);
	// if (objid) aos_free(objid);
	// if (subobj_id) aos_free(subobj_id);
	// if (contents) aos_free(contents);
	// if (operation) aos_free(operation);
	return transId;
}


bool
AosXmlProc::modifyObj(
		TiXmlNode *node,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	aos_assert_r(node, false);

	const char *objtype = node->getChildTextByAttr("name", "objtype", 0);

	if (!objtype)
	{
		// It is to save a generic object. Need to retrieve everything 
		// from the contents
		TiXmlNode *child = node->firstChildByAttr("name", "xmlobj");

		// Chen Ding, 10/24/2008
		// Need to handle the case when the contents are empty. This is because
		// the function 'modifyObject(...)' assumes the object Id is contained
		// in 'contents'. If 'contents' is not empty, this is true. Object ID
		// should actually be retrieved from 'objname' attribute.
		const char *objid = node->getChildTextByAttr("name", "objname", "");
		return modifyObject(objid, child, errcode, errmsg);
	}

	/*
	if (strcmp(objtype, "rvg") == 0)
	{
		return AosRvg_Modify(node, errcode, errmsg);
	}

	if (strcmp(objtype, "charset") == 0)
	{
		return AosCharset_Modify(node, errcode, errmsg);
	}

	if (strcmp(objtype, "template") == 0)
	{
		return AosTemplate_Modify(node, errcode, errmsg);
	}

	if (strcmp(objtype, "dialog") == 0)
	{
		return AosDialog_Modify(node, errcode, errmsg);
	}
	*/

	OmnAlarm << "Incorrect object type: " << objtype << enderr;

	errcode = eAosXmlInt_IncorrectObjType;
	errmsg = "";
	OmnAlarm << "Unrecognized objtype: " << objtype << enderr;
	return true;
}


bool
AosXmlProc::delObj(aos_xml_node_t *node, AosXmlRc &errcode, OmnString &errmsg)
{
	return true;
}


bool
AosXmlProc::readObj(aos_xml_node_t *node, AosXmlRc &errcode, OmnString &errmsg)
{
	/*
	if (objtype == "rvg") return AosRvg_ReadRvg(objid, contents, errcode, errmsg);

	OmnAlarm << "Incorrect object type: " << objtype << enderr;
	errcode = eAosXmlInt_IncorrectObjType;
	errmsg = "";
	*/
	aos_not_implemented_yet;
	return true;
}


/*
// It modifies the object based on 'contents'. If 'contents' is null, it
// means the object contents are empty. It should not be treated as an error.
bool
AosXmlProc::modifyObject(
		const char * const objid,
		TiXmlNode *child,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	aos_assert_r(objid, false);
	AosObjectWrapper wp;
	wp.setName(objid);
	wp.setXml(child);

	// Object type serves as the table name
	char *substrs[3];
	aos_assert_r(aos_str_split((char *)objid, '_', substrs, 3) >= 0, false);
	wp.setTablename(substrs[0]);

	aos_assert_r(wp.saveObj(errcode, errmsg), false);
	errcode = eAosXmlInt_Ok;
	aos_str_split_releasemem(substrs, 3);
	return true;
}
*/

bool
AosXmlProc::saveDbRecord(
		aos_xml_node_t *node,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	/*
	aos_assert_r(node, false);

	char *contents = 0;
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, 
		"name", "contents", &contents) == 1, false);

	aos_assert_r(contents, false);

	// 'contents' is in the form:
	// 	<Record objid="the_obj_id" num_attrs="nnn">
	// 		<attr name="attr_name">the_contents_of_the_attribute</attr>
	// 		<attr name="attr_name">the_contents_of_the_attribute</attr>
	// 		...
	//  </Record>
	//  The first <attr> is always for the objid.
	//
	//  In the current implementation, we only handles the 'xml' attribute, 
	//  which should be the definition of the object.
	
	char *objid = 0;
	return true;
	*/
	return false;
}


bool
AosXmlProc::serverProc(
		TiXmlNode *node,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	errcode = eAosXmlInt_SyntaxError;
	aos_assert_r(node, false);

	// It requests a server processing. The actual processing type is indicated
	// by the field 'flag'
	const char *flag = node->getChildTextByAttr("name", "flag", 0); 
	const char *args = node->getChildTextByAttr("name", "arguments", 0);

	if (!flag)
	{
		errmsg = "Received a ServerProc operation, looking for the flag"
			" but found none";
		errcode = eAosXmlInt_SyntaxError;
		OmnAlarm << errmsg << enderr;
		return true;
	}

	TiXmlNode *child = node->firstChildByAttr("name", "xmlobj");
	if (!child)
	{
		errmsg = "Missing the child definition";
		return false;
	}

	TiXmlElement *xmlobj = child->ToElement();
	aos_assert_r(xmlobj, false);

	if (strcmp(flag, "CreateBkpage") == 0)
	{
		return sgVpdParser.createBkpage(xmlobj, errcode, errmsg);
	}

	if (strcmp(flag, "CreateBook") == 0)
	{
		return sgBookMgr.createBook(xmlobj, errcode, errmsg);
	}

	if (strcmp(flag, "RemoveBook") == 0)
	{
		return sgBookMgr.removeBook(xmlobj, errcode, errmsg);
	}

	if (strcmp(flag, "CreateBkHomepage") == 0)
	{
		ostringstream oss(ostringstream::out);
		oss << *xmlobj;
		OmnString contents(oss.str().c_str());
		return sgBookMgr.createHomepage(contents, args, errcode, errmsg);
	}

	OmnAlarm << "Incorrect flag: " << flag << enderr;

	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Incorrect flag: ";
	errmsg << flag;
	OmnAlarm << errmsg << enderr;
	return true;
}
#endif
