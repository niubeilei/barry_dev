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
// 2013/05/29 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DynJimos_JimoRandomNumGen_JimoRandomNumGen_h
#define Aos_DynJimos_JimoRandomNumGen_JimoRandomNumGen_h

#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"

class AosJimoRandomNumGen : public AosJimo
{
public:
	class pharseType
	{
	public:
		OmnString mType;
		OmnString mRangeType;
		int mWeight;
	};
	class mcRange : public pharseType 
	{
	public:
		int mMin;
		int mMax;
	}mRange;
	class mcSequence : public pharseType
	{
	public:
		int mStart;
		int mEnd;
		int mStep;
	}mSequence;
	class mcEnum : public pharseType
	{
	}mEnum;
	class mcRpick : public pharseType
	{
	}mRpick;

	map<OmnString, pharseType> mapRange;
	u32 mPharseNum;


public:
	AosJimoRandomNumGen(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &sdoc);
	~AosJimoRandomNumGen();

	// Jimo Interface
	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff) const;
	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);
	virtual AosJimoPtr clone(
							const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc) const;
	virtual bool run(		const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc);
	virtual OmnString toString() const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const OmnString &interface_objid) const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const int interface_id) const;

private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
	bool runWithSmartdoc(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc);
	int dataPhrase(OmnString type);
	bool setRangeAttr(AosXmlTagPtr &range, OmnString type);
};
#endif
