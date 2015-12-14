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
#ifndef Aos_SEInterfaces_DataletParserObj_h
#define Aos_SEInterfaces_DataletParserObj_h

#include "Jimo/Jimo.h"


class AosDataletParserObj : virtual public AosJimo
{
public:

public:
	virtual ~AosDataletParserObj();

	virtual bool isValid() const = 0;
	virtual AosXmlTagPtr parse(AosRundata *rdata, const OmnString &doc) = 0;
};

#endif
