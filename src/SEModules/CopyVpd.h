//
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
// 03/28/2011: Created by Phnix
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CopyVpd_h
#define Aos_CopyVpd_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


#define PREFIX "prefix"
#define SUFFIX "suffix"
#define SEQNO  "seqno"

struct AosCopyType
{
	enum E
	{
		eInvalid,

		ePrefix,
		eSuffix,
		eSeqno,

		eMax
	};

	static E toEnum(const OmnString &str)
	{
		if (str == PREFIX) return ePrefix;
		if (str == SUFFIX) return eSuffix;
		if (str == SEQNO) return eSeqno;
		return eInvalid;
	}
};


class AosCopyVpd
{
public:
	static OmnString getNextVpdObjid(
				const OmnString &prefix_type,
				const OmnString &oldObjid,
				OmnString &strAdd,
				const AosRundataPtr &rdata);

	static OmnString attachSeqno(const OmnString &objid);
};

#endif
