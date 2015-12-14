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
// 11/27/07: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#ifndef  AOS_RVG_ENUMSEQRSG_H
#define  AOS_RVG_ENUMSEQRSG_H

#include "RVG/EnumRSG.h"
//
// get a sequence of enum string 
//
class AosEnumSeqRSG : public AosEnumRSG
{
	OmnDefineRCObject;
private:
	char				mSeperator;
	u32					mMinNum;
	u32					mMaxNum;
	bool				mUnique;
	bool				mOrdered;
	
public:
	AosEnumSeqRSG();
	~AosEnumSeqRSG();

	virtual bool nextValue(AosValue& value);

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList);

	char getSeparator();
	u32 getMinNum();
	u32 getMaxNum();
	bool getUniqueFlag();
	bool getOrderedFlag();

	void setSeperator(const char sep);
	void setMinNum(const u32 min);
	void setMaxNum(const u32 max);
	void setUniqueFlag(const bool flag);
	void setOrderedFlag(const bool flag);
	
private:
	bool nextUniqueOrdered(AosValue &value, const u32 numRepeats);
	bool nextUniqueNotOrdered(AosValue &value, const u32 numRepeats);
	bool nextMultipleOrdered(AosValue &value, const u32 numRepeats);
	bool nextMultipleNotOrdered(AosValue &value, const u32 numRepeats);

};

#endif   // ----- #ifndef AOS_RVG_ENUMSEQRSG_H -----

