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
// 07/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_LogicExpr_LogicExpr_h
#define Omn_LogicExpr_LogicExpr_h

#include "LogicExpr/Ptrs.h"
#include "LogicExpr/LogicTerm.h"
#include "LogicExpr/LogicExprUtil.h"
#include "TorturerConds/Util.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosLogicExpr : public AosLogicTerm
{
	OmnDefineRCObject;

private:
	AosDataType		mDataType;
	AosOperandType  mLeftType;
	OmnString		mLeftOperand;
	AosTortCondOpr	mOperator;
	AosOperandType  mRightType;
	OmnString		mRightOperand;

public:
	AosLogicExpr();
	virtual ~AosLogicExpr();

	virtual bool 	config(const OmnXmlItemPtr &def);
	virtual bool	evaluate(bool &rslt, 
							 OmnString &errmsg,
							 const AosLogicObjPtr &obj) const;

};

#endif

