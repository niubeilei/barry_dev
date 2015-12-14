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
#ifndef Omn_LogicExpr_LogicTerm_h
#define Omn_LogicExpr_LogicTerm_h

#include "LogicExpr/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlParser/Ptrs.h"


class AosLogicTerm : virtual public OmnRCObject
{
public:
	AosLogicTerm();
	virtual ~AosLogicTerm();

	virtual bool	config(const OmnXmlItemPtr &def) = 0;
	virtual bool	evaluate(bool &rslt, 
							 OmnString &errmsg,
							 const AosLogicObjPtr &) const = 0;

	static AosLogicTermPtr	createTerm(const OmnXmlItemPtr &def);
};

#endif

