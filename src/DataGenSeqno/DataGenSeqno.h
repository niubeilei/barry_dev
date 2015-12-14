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
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 11/02/2011: Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataGenSeqno_DataGenSeqno_h
#define Aos_DataGenSeqno_DataGenSeqno_h

#include "DataGenSeqno/DataGenSeqnoType.h"
#include "DataGenSeqno/Ptrs.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "Util/ValueRslt.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"



class AosDataGenSeqno : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		    eMax = 100
	};
private:
	AosDataGenSeqnoType::E 	mType;
	OmnMutexPtr             mLock;
public:

    AosDataGenSeqno(
			const OmnString &name,
			const AosDataGenSeqnoType::E type,
			const bool reg);

    ~AosDataGenSeqno();

	virtual bool createSeqno(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata) = 0;

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata) = 0;

	static 	bool createSeqnoStatic(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	bool modifyDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);

	AosDataGenSeqnoType::E getType() const
	{
	    return mType;
	}

	OmnString	getAttrStrValue(
			const AosXmlTagPtr &sdoc, 
			const OmnString &tagname, 
			const OmnString &dft,
			const AosRundataPtr &rdata);

	u64		getAttrU64Value(
			const AosXmlTagPtr &sdoc, 
			const OmnString &tagname, 
			const u64 &dft,
			const AosRundataPtr &rdata);

private:
	bool registerseqno(const OmnString&, const AosDataGenSeqnoPtr &seqno);
};
#endif

