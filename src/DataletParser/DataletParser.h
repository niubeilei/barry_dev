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
// A DataletParser is a Jimo that is responsible for processing one 
// datalet related request such as creating, deleting, modifying, 
// or reading datalets on the cubes. That is, all DataletParsers run
// on a Datalet Store Cluster.
//
// Modification History:
// 2015/06/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletParser_DataletParser_h
#define Aos_DataletParser_DataletParser_h

#include "SEInterfaces/DataletParserObj.h"


class AosDataletParser : public AosDataletParserObj
{
public:
	virtual ~AosDataletParser();
};

#endif
