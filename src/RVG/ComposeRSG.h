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
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RVG_ComposeRSG_H
#define AOS_RVG_ComposeRSG_H

#include "RVG/RSG.h"

enum AosRSGComposeType
{
	eRSGComposeType_INVALID = -1,
	eRSGComposeType_AND = 0,
	eRSGComposeType_OR
	
};

//
// composed Random String Generator 
//
class AosComposeRSG : public AosRSG
{
protected:
	AosRVGPtrList mBRVGList; 

public:
	AosComposeRSG();
	virtual ~AosComposeRSG();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	
	// 
	// get, add, delete BRSG
	//
	void getBRSGList(AosRVGPtrList &rvgList);
	void pushBRSG(const AosRVGPtr& rvg);
	AosRVGPtr popBRSG();
	void clearBRSGList();

protected:
	u32 getTotalComposeWeight(const AosRVGPtrList& rvgList);

};

#endif
