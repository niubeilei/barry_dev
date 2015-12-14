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
// 12/16/2009: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_XMLINTERFACE_PASSWDGEN_H
#define AOS_XMLINTERFACE_PASSWDGEN_H

#include "Proggie/ProggieUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class TiXmlElement;

class AosPasswdSvr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosPasswdSvr();
	~AosPasswdSvr();

	bool 		sendmail(
					const OmnString &username,
					const AosNetRequestPtr &req, 
					TiXmlElement *childelem,
					OmnString &contents,
					AosXmlRc &errcode,
					OmnString &errmsg);
	bool 		readFile();
	OmnString 	generatePwd();
};
#endif
