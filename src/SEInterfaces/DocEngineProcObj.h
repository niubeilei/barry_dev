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
// A DocEngineProc is a Jimo that is responsible for processing one 
// datalet related request such as creating, deleting, modifying, 
// or reading datalets on the cubes. That is, all DocEngineProcs run
// on a Datalet Store Cluster.
//
// Modification History:
// 2015/03/31 Created by Ma Yazong
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocEngineProcObj_h
#define Aos_SEInterfaces_DocEngineProcObj_h

#include "SEInterfaces/JimoCallPackage.h"


class AosDocEngineProcObj : virtual public AosJimo
{
public:

public:
	virtual ~AosDocEngineProcObj();

	virtual bool handleJimoCall(
						AosRundata *rdata, 
						AosJimoCall &jimo_call, 
						AosDataletParserObj *parser) = 0;

	virtual bool isValid() const = 0;
};

#endif
