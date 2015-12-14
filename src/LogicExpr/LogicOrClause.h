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
// 07/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_LogicExpr_LogicOrClause_h
#define Omn_LogicExpr_LogicOrClause_h

#include "LogicExpr/Ptrs.h"
#include "LogicExpr/LogicTerm.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosLogicOrClause : public AosLogicTerm 
{
	OmnDefineRCObject;

private:
	OmnDynArray<AosLogicTermPtr>	mTerms;

public:
	AosLogicOrClause();
	virtual ~AosLogicOrClause();

	virtual bool	config(const OmnXmlItemPtr &def);
	virtual bool	evaluate(bool &rslt, 
							 OmnString &errmsg,
							 const AosLogicObjPtr &obj) const;
};

#endif

