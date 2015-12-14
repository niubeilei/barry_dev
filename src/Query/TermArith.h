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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermArith_h
#define Aos_Query_TermArith_h

#include "Query/TermIILType.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"


class AosTermArith : public AosTermIILType
{

public:
	AosTermArith(const bool regflag);
	AosTermArith(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata);
	AosTermArith(
		const OmnString &lhs, 
		const OmnString &rhs,
		const bool order, 
		const bool reverse, 
		const AosOpr opr,
		const OmnString &ctnr_objid, 
		const AosRundataPtr &rdata);

	AosTermArith(
		const OmnString &iilname,
		const AosOpr opr,
		const OmnString &rhs,
		const bool order,
		const bool reverse, 
		const AosRundataPtr &rdata);		// Ketty 2014/02/14

	virtual void	toString(OmnString &str);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	static bool convertToXml(
				const OmnString &cond_str, 
				const AosXmlTagPtr &conds_tag, 
				const OmnString fields[5],
				const i64 &nn,
				const AosRundataPtr &rdata);

	static bool convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);

	static bool convertEpochCond(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);
	
	// Ketty 2014/02/14	
	virtual bool isStatValueTerm();
	virtual bool isTimeTerm();
	virtual OmnString getFieldName(){ return mFieldname; };
	virtual bool setIILName(const OmnString &iil_name){ mIILName = iil_name; return true;};

	void 		setOpr(const AosOpr opr) { mOpr = opr;}
	//void 		setIILName(const OmnString &iilname) { mIILName=iilname;}
	void 		setValue(const OmnString &value) { mValue = value;}
private:
	bool 	parse(const AosRundataPtr &rdata);
};
#endif

