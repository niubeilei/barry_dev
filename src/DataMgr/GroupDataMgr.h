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
// 02/20/2009: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataMgr_GroupDataMgr_h
#define Aos_DataMgr_GroupDataMgr_h

#include "DataMgr/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"

class TiXmlElement;

#include <string.h>


class AosGroupDataMgr: virtual public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		eMaxKeyfields = 100,
		eMaxIndexFields = 100
	};

private:
	char *			mUsername;
	char *			mContents;
	char *			mTablename;
	char *			mAppname;

	AosGDDefPtr		mGroupDef;
	TiXmlElement  *	mXmlobj;

public:
	AosGroupDataMgr();
	~AosGroupDataMgr();

	bool process(TiXmlElement *root, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &responseContents);

	bool addObj(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool modifyObj(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool renameObject(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool deleteObj(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool retrieveObj(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool queryGroup(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool archive(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool createGroup(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool modifyGroup(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool destroyGroup(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg, 
				OmnString &response);

	bool createTable(
				const OmnString &tablename, 
				TiXmlElement *xmlobj,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool retrieveObjList(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool lockObj(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool lockGroup(
				TiXmlElement *node,
				AosXmlRc &errcode,
				OmnString &errmsg, 
				OmnString &response);

	bool canCreateMoreObject(OmnString &reason);

private:
	bool checkTable(struct aos_xml_node *node, 
				OmnString &tablename,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool determineObjid(struct aos_xml_node *node, 
				TiXmlElement *xmlobj,
				OmnString objid,
				const OmnString &tablename,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool preprocess(
				TiXmlElement *node, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
};
#endif

