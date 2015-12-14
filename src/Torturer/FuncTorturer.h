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
// 01/04/2008: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturer_FuncTorturer_h
#define Aos_Torturer_FuncTorturer_h

#include "Torturer/Torturer.h"
#include "Torturer/FuncCmd.h"
#include <list>

class AosFuncTorturer : public AosTorturer
{
protected:
	static AosFuncCmdPtrArray mRunTimeFuncs;

	std::string mRegistStmt;
	std::string mIncludes;

public:
	AosFuncTorturer();
	AosFuncTorturer(const std::string& torturerName, const std::string& productName);
	virtual ~AosFuncTorturer();
	virtual bool start();
	

	bool 	genCode(const bool force);

	//
	// Add the objects of the generated classes
	// each class one object, they will be added 
	// into the module they belongs to.
	//
	static bool	addRunTimeFunc(const AosFuncCmdPtr& func);

protected:
	virtual bool init();

private:
	bool 	createMain(const std::string& torDir, const bool force);
	bool 	createMakefile(const std::string& torDir, const bool force) const;


};

#endif

