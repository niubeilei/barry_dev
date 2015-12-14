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
// 12/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conds_CondSArith_h
#define Aos_Conds_CondSArith_h

#include "Conds/Condition.h"
#include "Random/RandomUtil.h"
#include <vector>

class AosCondSArith : virtual public AosCondition
{
	enum Type
	{
		eInvalid,

		eLT,
		eLE,
		eGT,
		eGE,
		eEqual,
		eNotEqual,

		eMax
	};

public:
	AosCondSArith(const bool reg);
	~AosCondSArith();

	// Condition interface
	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();
	virtual OmnString generateCommand(
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata);

	Type toEnum(const OmnString &opr)
	{
		if (opr == "eq" || opr == "==") return eEqual;
		if (opr == "ne" || opr == "!=") return eNotEqual;
		if (opr == "lt" || opr == "<") return eLT;
		if (opr == "le" || opr == "<=") return eLE;
		if (opr == "gt" || opr == ">") return eGT;
		if (opr == "ge" || opr == ">=") return eGE;

		return eInvalid; 
	}
	// RandomXml interface. This function should not be in this class.
	// Will move to the torturer.
	virtual OmnString getXmlStr(
					const OmnString &tagname,
					const int level,
					const OmnString &then_part,
					const OmnString &else_part,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	static OmnString pickOperator()
	{
		switch (OmnRandom::nextInt1(0, 5))
		{
		case 0: return "==";
		case 1: return "!=";
		case 2: return "<";
		case 3: return "<=";
		case 4: return ">";
		case 5: return ">=";
		}

		return "==";
	}
};
#endif
