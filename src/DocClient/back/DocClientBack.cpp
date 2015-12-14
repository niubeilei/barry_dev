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
//  This class is used to create docs. It will:
//
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 09/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
bool
AosDocClient::reserveObjid1(
		const AosRundataPtr &rdata,
		OmnString &objid, 
		u64 &docid,
		const bool resolve, 
		const bool keepDocid) 
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << " = \"" << "reserveobjid" << "\" "
		<< AOSTAG_OBJID << "= \"" << objid << "\" "
		<< AOSTAG_DOCID << "= \"" << docid << "\" "
		<< "resolve" << "= \"" << (resolve?"true":"false") << "\" "
		<< "keepDocid" << "=\"" << (keepDocid?"true":"false") << "\" >"
		<< "</trans>";
	AosXmlTagPtr resp;
	bool rslt = addReq(rdata, docstr, resp, docid);
	aos_assert_rr(rslt, rdata, false);
	
	aos_assert_r(resp, false);
	if (!rdata->isOk())
	{	
		rdata->setError() << resp->getNodeText();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	objid = resp->getAttrStr(AOSTAG_OBJID, "");
	docid = resp->getAttrU64(AOSTAG_DOCID, 0);
	return true;
}
bool
AosDocClient::createObjByTemplate(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosXmlTagPtr &newxml,
			const AosXmlTagPtr &root)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \""<< "createobjbytemplate" << "\" >"
		<< "<newxml>" << newxml->toString() << "</newxml>"
		<< "<root>" << root->toString() << "</root>"
		<< "</trans>";
	
	bool rslt1 = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt1, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}

bool
AosDocClient::createDocSafe3(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			const AosXmlTagPtr &root)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << " = \"" << "createdocsafe" << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "<root>" << root->toString() << "</root>"
		<< "</trans>";
	
	bool rslt1 = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt1, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	//Process resp
	return true;
}

bool
AosDocClient::createDocByTemplate1(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &tmpl)
{
	u64 docid = tmpl->getAttrU64(AOSTAG_DOCID, 0);
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \""<< "createdocbytemplate1"<< " \" >"
		<< "<tmpl>" << tmpl->toString() << "</tmpl>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, 0);
	if (!rdata->isOk())
	{
		return 0;
	}
	return 0;
}


bool
AosDocClient::createRootCtnr(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosXmlTagPtr &doc)
{
	OmnString str ="<trans ";
	str << AOSTAG_TYPE <<"= \"" << "createrootctnr" << "\" " 
		<< AOSTAG_DOCID " = \"" << docid << " \">"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";
	bool rslt = addReq(rdata, str, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	//Process resp
	return true;
}

bool
AosDocClient::modifyObj(
			const AosRundataPtr &rdata,
			const u64 &newdid,
			const AosXmlTagPtr &newxml,
			const AosXmlTagPtr &origdoc,
			const AosXmlTagPtr &root,
			const bool &synobj)
{
	aos_assert_rr(newdid == newxml->getAttrU64(AOSTAG_DOCID, 0), rdata, false);
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \"" << "modifyobj" << "\" "
		<< "synobj" << "=\"" << (synobj?"true":"false") << "\" >"
	    << "<newxml>" << newxml->toString() << "</newxml>"
		<< "<root>" << root->toString() << "</root>"
		<< "<origdoc>" << origdoc->toString() << "</origdoc>"
		<< "</trans>";

	bool rslt = addReq(rdata, docstr, false, newdid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}

bool
AosDocClient::modifyAttrStr(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &attrname,
			const OmnString &newvalue,
			const OmnString &oldvalue,
			const bool value_unique,
			const bool docid_unique,
			const bool exist,
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &doc)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE <<  "=\"" << "modifyattrstr" << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< "attrname" << "=\"" << attrname << "\" "
		<< "newvalue" << "=\"" << newvalue<< "\" "
		<< "oldvalue" << "=\"" << oldvalue<< "\" "
		<< "value_unique " << "= \"" << (value_unique?"true":"false") << "\" "
		<< "docid_unique "<< "= \"" << (docid_unique? "true":"false") << "\" "
		<< "exist" << "=\"" << (exist?"true":"false") << "\" "
		<< "fname = \"" << fname << "\" "
		<< "line = \"" << line << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
	    return false;
	}
	return true;
}
bool
AosDocClient::modifyAttrU64(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &attrname,
			const u64 &newvalue,
			const u64 &oldvalue,
			const bool value_unique,
			const bool docid_unique,
			const bool exist,
			const AosXmlTagPtr &doc)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << " = \"" << "modifyattru64" << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< "attrname" << " = \"" << attrname << "\" "
		<< "newvalue" << "= \"" << newvalue << "\" "
		<< "oldvalue" << " = \"" << oldvalue << "\" "
		<< "value_unique"<< " = \"" << (value_unique?"true":"false") << "\" "
		<< "docid_unique" << " = \"" << (docid_unique?"true":"false") << "\" "
		<< "exist" << "=\"" << (exist?"true":"false") <<">"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}

bool
AosDocClient::modifyAttrPriv(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &attrname,
			const OmnString &newvalue,
			const OmnString &oldvalue,
			const bool value_unique,
			const bool docid_unique,
			const bool exist,
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &doc)
{
	return modifyAttrStr(rdata, docid, attrname, newvalue, 
			oldvalue, value_unique, docid_unique, exist, fname, line, doc);
}

bool
AosDocClient::modifyAttrPriv(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &attrname,
			const u64 &newvalue,
			const u64 &oldvalue,
			const bool value_unique,
			const bool docid_unique,
			const bool exist,
			const AosXmlTagPtr &doc)
{
	return modifyAttrU64(rdata, docid, attrname, newvalue, oldvalue,
				value_unique, docid_unique, exist, doc);
}

bool
AosDocClient::isDocDeleted(const AosRundataPtr &rdata, const u64 &docid, bool &result)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "isdocdeleted" << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" />";
	AosXmlTagPtr resp;
	bool rslt = addReq(rdata, docstr, resp, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	result = (resp->getAttrStr("result") == "true");
	return true;	
}

bool
AosDocClient::incrementValue(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &attrname,
			const OmnString &newvalue,
			const OmnString &oldvalue,
			const bool value_unique,
			const bool docid_unique,
			const bool exist,
			const OmnString &fname,
			const int line,
			const AosXmlTagPtr &doc)
{
	return modifyAttrStr(rdata, docid, attrname, newvalue, 
			oldvalue, value_unique, docid_unique, exist, fname, line, doc);
}
bool
AosDocClient::deleteObj(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosXmlTagPtr &origdocroot,
			const OmnString &appname)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \"" << "deleteobj" << "\" "
		<< "appname" << "=\"" << appname << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" >"
		<< "<origdocroot>" <<origdocroot->toString() << "</origdocroot>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}
bool
AosDocClient::addRemoveMemberRequest(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const OmnString &ctnr_objid)
{
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "addremovememberreq" << "\" "
		<< "ctnr_objid" << "=\"" << ctnr_objid << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}

bool
AosDocClient::addAddMemberRequest(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const OmnString &ctnr_objid,
			const u64 &userid)
{

	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "addaddmemberreq\" "
		<< "ctnr_objid" << "=\"" << ctnr_objid << "\" "
		<< AOSTAG_USERID << "=\"" <<userid << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";
	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}
	return true;
}

bool
AosDocClient::saveToFile(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosXmlTagPtr &doc)
{
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << "savetofile" <<"\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";

	bool rslt = addReq(rdata, docstr, false, docid);
	aos_assert_rr(rslt, rdata, false);
	if (!rdata->isOk())
	{
		return false;
	}

	return true;
}
#endif

