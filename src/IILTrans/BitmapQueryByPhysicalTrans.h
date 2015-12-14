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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTrans_BitmapQueryByPhysical_h
#define AOS_IILTrans_BitmapQueryByPhysical_h

/*
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "TransUtil/IILTrans.h"
#include "Util/Opr.h"

#include <vector>
using namespace std;


class AosIILTransBitmapQueryByPhysical : public AosIILTrans
{
		
	int								mPhysicalId;
	vector<OmnString>				mIILNames;
	vector<AosQueryRsltObjPtr>		mBitmapIdLists;
	vector<AosBitmapObjPtr>	mPartialBitmaps;

public:
	AosIILTransBitmapQueryByPhysical(const bool regflag);
	~AosIILTransBitmapQueryByPhysical(){}

	// AosIILTrans interface
	virtual AosIILTransPtr clone();
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata);
	virtual bool initTrans(
					const AosXmlTransPtr &trans, 
					const AosXmlTagPtr &trans_doc,
					const AosRundataPtr &rdata);
	virtual AosIILType getIILType() const;
	virtual u64 getIILID(const AosRundataPtr &rdata);
	virtual bool needCreateIIL() const {return true;}
	virtual int getSerializeSize() const;
};
*/

#endif
