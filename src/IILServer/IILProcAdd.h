////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILServer_IILProcAdd_h
#define Aos_IILServer_IILProcAdd_h

//#include "IILServer/IILProc.h"
#include "TransUil/TransProc.h"
#include "TransUil/TransReqId.h"
#include "TransPest/Ptrs.h"

class AosIILProcAdd : public AosTransProc 
{
public:
	AosIILProcAdd(const bool reg_flag);
	~AosIILProcAdd();

	virtual bool doTrans(const AosTransPtr &trans);
	AosTransReqId::E  getReqOpr(){ return AosTransReqId::eIILAddDoc;};
};
#endif

