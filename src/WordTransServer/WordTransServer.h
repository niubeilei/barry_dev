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
// 2011/07/15: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_WordTransServer_WordTransServer_h
#define Aos_WordTransServer_WordTransServer_h 

#include "Rundata/Ptrs.h"
#include "TransServer/TransProc.h"
#include "TransServer/Ptrs.h"


class AosWordTransServer : public AosTransProc
{
	OmnDefineRCObject;

public:
	AosWordTransServer();
	~AosWordTransServer();
	
	static bool		config(const AosXmlTagPtr &config);

	virtual bool 	proc(const AosXmlTagPtr &data, const AosRundataPtr &rdata);
	
	virtual void	setTransServer(const AosTransServerPtr &server){}

private:

	bool			getIILID(const AosXmlTagPtr &req, const AosRundataPtr &rdata);
	
	u64				createIIL(const AosXmlTagPtr &req, const AosRundataPtr &rdata);
};
#endif
