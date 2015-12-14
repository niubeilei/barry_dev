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
#if 0
#ifndef Aos_SmartDoc_SMDMgr_h
#define Aos_SmartDoc_SMDMgr_h

#include "Rundata/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SdocTypes.h"
#include "Actions/SdocAction.h"
#include "Actions/ActAddAttr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "SmartDoc/SdocTypes.h"
#include <vector>
#include "Actions/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"


class AosSMDMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosSMDMgr();
	~AosSMDMgr();

	static bool procSmartdocs(
				const OmnString &sdoc_objids, 
				const AosRundataPtr &rdata);
	static bool procSmartdoc(
				const u64 &sdocid, 
				const AosRundataPtr &rdata);
	static bool runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	static bool procSmartdocStatic(
	 			const OmnString &siteid,
	 			const AosWebRequestPtr &req, 
	 			const AosXmlTagPtr &root, 
	 			const AosXmlTagPtr &userobj, 
	 			const OmnString &sdoc_objid, 
	 			OmnString &contents, 
	 			AosXmlRc &errcode, 
	 			OmnString &errmsg);
};
#endif
#endif
