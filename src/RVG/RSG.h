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
// 	the class relationship:
//        AosRSG
//           |- AosComposeRSG
//           |- AosCharsetRSG
//           |- AosEnumRSG
//			 	|- AosEnumSeqRSG
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RVG_RSG_H
#define AOS_RVG_RSG_H

#include "RVG/RVG.h"

//
// Random String Generator base class
//
class AosRSG : public AosRVG
{
protected:

public:
	AosRSG();
	virtual ~AosRSG();

};

typedef std::list<AosRSGPtr> AosRSGPtrList;

#endif
