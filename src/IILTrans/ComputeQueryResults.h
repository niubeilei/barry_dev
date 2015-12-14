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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_ComputeQueryResults_h
#define AOS_IILTrans_ComputeQueryResults_h

#include "TransUtil/IILTrans.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Opr.h"


class AosIILTransComputeQueryResults : public AosIILTrans
{
	AosQueryContextObjPtr	mQueryContext;

public:
	AosIILTransComputeQueryResults(const bool regflag);
	~AosIILTransComputeQueryResults(){}

	// AosIILTrans interface
	virtual AosIILTransPtr clone();
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata);
	virtual bool initTrans(
					const AosXmlTransPtr &trans, 
					const AosXmlTagPtr &trans_doc,
					const AosRundataPtr &rdata);
	virtual u64 getIILID(const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual int getSerializeSize() const;
};

#endif
