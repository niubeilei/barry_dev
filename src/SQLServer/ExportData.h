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
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SQLInterface_ExportData_h
#define Omn_SQLInterface_ExportData_h

#include "Obj/ObjDb.h"
#include "SQLServer/SqlProc.h"
#include "SqlUtil/SqlReqid.h"



class AosExportData : virtual public AosSqlProc, 
						   virtual public OmnDbObj
{

private:

public:
	AosExportData(const bool regflag);
	~AosExportData();

	// ExportData Interface
	virtual bool proc(const AosTransPtr &trans, const AosRundataPtr &rdata);

	// OmnObjDb Interface
	virtual int         objSize() const {return sizeof(*this);}

};
#endif

