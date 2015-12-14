////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2012/03/20	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
//#ifndef Aos_ValueSel_ValueSelMath_h
//#define Aos_ValueSel_ValueSelMath_h

#if 0
#include "ValueSel/ValueSel.h"

class AosValueSelMath : public AosValueSel
{
	enum Type
	{
		eInvalid,

		eAdd,
		eSubtract,
		eMultiply,
		eDivide,
		
		eMax
	};

public:
	AosValueSelMath(const bool reg);
	AosValueSelMath(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelMath();

	virtual bool		run(
							AosValueRslt &valueRslt,
							const AosXmlTagPtr &item,
							const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString	getXmlStr(
							const OmnString &tagname, 
							const int level,
							const AosRandomRulePtr &rule, 
							const AosRundataPtr &rdata);

private:
	Type toEnum(const OmnString &opr)
	{
		if (opr == "+") return eAdd;
		if (opr == "-") return eSubtract;
		if (opr == "*") return eMultiply;
		if (opr == "/") return eDivide;
		return eInvalid;
	}
};
#endif

