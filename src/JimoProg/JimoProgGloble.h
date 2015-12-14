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
// Modification History:
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoProg_JimoProgGloble_h
#define AOS_JimoProg_JimoProgGloble_h

#include "JimoProg/JimoProg.h"

class AosJimoProgGloble : public AosJimoProg
{

public:
	AosJimoProgGloble();

	~AosJimoProgGloble();

	// JimoProg Interface
	virtual AosJimoProgObjPtr createJimoProg(
								const AosXmlTagPtr &def,
								AosRundata *rdata);

//    virtual bool run(AosRundata *rdata);
	
	virtual bool saveLogicDoc(
					AosRundata *rdata, 
					const OmnString &objid, 
					const OmnString &conf);
	
	virtual AosXmlTagPtr getLogicDoc(
					AosRundata *rdata, 
					const OmnString &objid);

};
#endif

