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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocCreateOrder_h
#define Aos_SmartDoc_SdocCreateOrder_h

#include "Actions/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/SdocAction.h"

#include <vector>

#define AOS_DOCID_PLUS_SEQNO 			'b'
#define AOS_OBJID_PLUS_SEQNO 			'c'
#define AOS_CID_PLUS_TIME_PLUS_SEQNO	'd'

class AosSdocCreateOrder : public AosSmartDoc
{
private:
	enum E
	{
		eInvalid,

		eDocidPlusSeqno,
		eObjidPlusSeqno,
		eCidPlusTimePlusSeqno,

		eMax
	};

public:
	AosSdocCreateOrder(const bool flag);
	~AosSdocCreateOrder();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocCreateOrder(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool getSeqno(
			const AosXmlTagPtr &sdoc,
			u64 &seqno,
			const AosRundataPtr &rdata);

	static E toEnum(const char name)
	{
		if (name == AOS_DOCID_PLUS_SEQNO) return eDocidPlusSeqno;
		if (name == AOS_OBJID_PLUS_SEQNO) return eObjidPlusSeqno;
		if (name == AOS_CID_PLUS_TIME_PLUS_SEQNO) return eCidPlusTimePlusSeqno;
		return eInvalid;
	}

	bool	createOrderNum(
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	OmnString createObjid(
			const OmnString &objid_type, 
			const AosXmlTagPtr &order,
			const AosXmlTagPtr &sdoc, 
			const AosXmlTagPtr &item,
			AosRundataPtr &rdata);
};
#endif

