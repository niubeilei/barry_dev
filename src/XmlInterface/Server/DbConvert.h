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
// 11/11/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_XmlInterface_Server_DbConv_h
#define AOS_XmlInterface_Server_DbConv_h

#include "Util/String.h"
#include "XmlInterface/XmlRc.h"


class TiXmlElement;

class AosConvertDb : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosConvertDb();
	~AosConvertDb();

	bool 	changeAttrName(
				TiXmlElement *cmd,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool 	changeAllAttrName(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &xmlfname,
				const OmnString &idfname, 
				const OmnString &aname, 
				const OmnString &newname, 
				const OmnString &sep,
				OmnString &errmsg);

	bool 	changeAttrName(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &xmlfname,
				const OmnString &idfname, 
				const OmnString &aname, 
				const OmnString &newname, 
				const OmnString &sep,
				OmnString &errmsg);

	bool 	dumpXml(
				const OmnString &tname, 
				const OmnString &idname,
				const OmnString &dataid);
	bool 	changeAllAttrValues(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &aname, 
				const OmnString &avalue, 
				const OmnString &newvalue, 
				OmnString &errmsg);
	bool 	changeAttrValue(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &aname, 
				const OmnString &avalue, 
				const OmnString &newvalue, 
				OmnString &errmsg);
	bool 	removeAttrs(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &aname, 
				const OmnString &avalue, 
				OmnString &errmsg);
	bool 	removeAttr(
				const OmnString &tname, 
				const OmnString &query,
				const OmnString &aname, 
				const OmnString &avalue, 
				OmnString &errmsg);
	bool 	removeDupeAttrs(
				const OmnString &tname, 
				const OmnString &query,
				OmnString &errmsg);
	bool 	removeDupeAttr(
				const OmnString &tname, 
				const OmnString &query,
				OmnString &errmsg);
	static OmnString 	getIdfname(const OmnString &tname);
};

#endif
