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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActSeqno_h
#define Aos_SdocAction_ActSeqno_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>

class AosActSeqno : virtual public AosSdocAction
{
public:
	enum PrefixMethod
	{
		eInvalid, 

		eSeqnoOnly,
		eWithPrefix,

		eMax
	};

private:

public:
	AosActSeqno(const bool flag);
	~AosActSeqno();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

	static PrefixMethod toPrefixEnum(const OmnString &name);
	static bool getSeqno(
			OmnString &seqnoStr,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata, 
			const int seq_num=0);

private:
	static bool createSeqnoOnly(
			const AosXmlTagPtr &sdoc,
			OmnString &seqnoStr,
			const AosRundataPtr &rdata,
			const int seq_num);

	static bool createSeqnoWithPrefix(
			const AosXmlTagPtr &sdoc,
			OmnString &seqnoStr,
			const AosRundataPtr &rdata,
			const int seq_num);

	static bool retrieveSeqno(
			const AosXmlTagPtr &sdoc,
			u64	  &seqno,
			const AosRundataPtr &rdata,
			const int seq_num);
};
#endif

