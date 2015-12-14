////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_AccessRcd_h
#define Aos_XmlUtil_AccessRcd_h

#include "Rundata/Ptrs.h"
#include "SEBase/SecOpr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosAccessRcd : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosXmlTagPtr	mAcdDoc;
	bool			mParentFlag;

public:
	AosAccessRcd(const AosXmlTagPtr &acd_doc, 
				const AosRundataPtr &rdata);
	~AosAccessRcd() {}

	static bool isAccessRecord(const AosXmlTagPtr &doc);
	AosXmlTagPtr getAccess(
			const OmnString &opr_id, 
			const AosRundataPtr &rdata);
	OmnString getAttrStr(const OmnString &name, const OmnString &dft = "");
	OmnString getHomeVpd();
	u64 getOwnerDocid() const;
	u64 getAttrU64(const OmnString &name, const u64 &dft);
	AosXmlTagPtr getDoc() const;
	bool checkAccessRecord(const AosRundataPtr &rdata);
	OmnString getParentContainerObjid() const;
	OmnString getStatemachineState() const;
	u64 getDocid() const;
	bool modifyStatemachineState(const OmnString &state, const AosRundataPtr &rdata);
};
#endif

