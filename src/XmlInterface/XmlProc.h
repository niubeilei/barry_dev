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
#ifndef Aos_XmlInterface_XmlProc_h
#define Aos_XmlInterface_XmlProc_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"


#include <string.h>

struct aos_xml_node;
class TiXmlNode;

class AosXmlProc: virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnString 	mFilename;

public:
	AosXmlProc();
	~AosXmlProc();

	static bool setDataDir(const OmnString &path);
	static bool setWorkingDir(const OmnString &path);
	int	process(const OmnString &filename);

	int wrapup(
				const AosXmlRc errcode, 
				const OmnString &responseContents,
				TiXmlNode *node,
				OmnString &errmsg);

	bool modifyObj(
				TiXmlNode *node,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool createObject(
				struct aos_xml_node *node,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool delObj(
				struct aos_xml_node *node,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool readObj(
				struct aos_xml_node *node,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool modifyObject(
				const char * const objid,
				TiXmlNode *child,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool saveDbRecord(
				struct aos_xml_node *node,
				AosXmlRc &errcode,
				OmnString &errmsg);

	bool serverProc(
				TiXmlNode *node,
				AosXmlRc &errcode,
				OmnString &errmsg);
};
#endif

