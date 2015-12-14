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
// Monday, December 03, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef  Aos_RVG_ANDRSG_H
#define  Aos_RVG_ANDRSG_H

#include "RVG/ComposeRSG.h"

class AosAndRSG : public AosComposeRSG
{
	OmnDefineRCObject;
private:
	bool	mOrdered;

public:
	AosAndRSG();
	~AosAndRSG();
	virtual bool nextValue(AosValue& value);

	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);
	virtual bool createIncorrectRVGs(AosRVGPtrArray& rvgList);

	void setOrderedFlag(bool flag);
	bool getOrderedFlag();

};

#endif

