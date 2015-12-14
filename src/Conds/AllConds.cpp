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
// 11/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/AllConds.h"


#include "Conds/CondAnd.h"
#include "Conds/CondSArith.h"
#include "Conds/CondIn.h"
#include "Conds/CondOr.h"
#include "Conds/CondTrue.h"
#include "Conds/CondFalse.h"
#include "Conds/CondUserDomains.h"
#include "Conds/CondUserGroups.h"
#include "Conds/CondUserRoles.h"
#include "Conds/CondValueListing.h"
#include "Conds/CondDocExist.h"
#include "Conds/CondCharStrMap.h"
#include "Conds/CondCustomize001.h"		// Ken Lee, 2012/05/30	
#include "Conds/CondIILMap.h"			// Ken Lee, 2013/01/26
#include "Conds/CondEmpty.h"			// Ken Lee, 2013/01/26
#include "Conds/CondRegExp.h" 		    // Andy Zhang, 2013/07/26 
#include "Conds/CondExpr.h" 		    // Andy Zhang, 2013/12/17

#include "XmlUtil/XmlTag.h"

AosConditionObjPtr	sgConditions[AosCondType::eMax+1];
static bool isInited = false;

bool AosInitConditions()
{
	if(isInited) return true;
	isInited = true;

	// Chen Ding, 08/12/2012
	// static AosCondAnd 			sgCondAnd(true);
	// static AosCondIn 			sgCondIn(true);
	// static AosCondOr 			sgCondOr(true);
	// static AosCondSArith 		sgCondSArith(true);
	// static AosCondTrue			sgCondTrue(true);
	// static AosCondFalse			sgCondFalse(true);
	// static AosCondUserDomains	sgCondUserDomains(true);
	// static AosCondUserGroups	sgCondUserGroups(true);
	// static AosCondUserRoles		sgCondUserRoles(true);
	// static AosCondValueListing	sgCondValueListing(true);
	// static AosCondDocExist		sgCondDocExist(true);
	// static AosCondCharStrMap	sgCondCharStrMap(true);
	
	static AosConditionObjPtr 	sgCondAnd = OmnNew AosCondAnd(true);
	static AosConditionObjPtr 	sgCondIn = OmnNew AosCondIn(true);
	static AosConditionObjPtr 	sgCondOr = OmnNew AosCondOr(true);
	static AosConditionObjPtr 	sgCondSArith = OmnNew AosCondSArith(true);
	static AosConditionObjPtr	sgCondTrue = OmnNew AosCondTrue(true);
	static AosConditionObjPtr	sgCondFalse = OmnNew AosCondFalse(true);
	static AosConditionObjPtr	sgCondUserDomains = OmnNew AosCondUserDomains(true);
	static AosConditionObjPtr	sgCondUserGroups = OmnNew AosCondUserGroups(true);
	static AosConditionObjPtr	sgCondUserRoles = OmnNew AosCondUserRoles(true);
	static AosConditionObjPtr	sgCondValueListing = OmnNew AosCondValueListing(true);
	static AosConditionObjPtr	sgCondDocExist = OmnNew AosCondDocExist(true);
	static AosConditionObjPtr	sgCondCharStrMap = OmnNew AosCondCharStrMap(true);
	static AosConditionObjPtr	sgCondCustomize001 = OmnNew AosCondCustomize001(true);	// Ken Lee, 2012/05/30
	static AosConditionObjPtr	sgIILMap = OmnNew AosCondIILMap(true);	// Ken Lee, 2013/01/26
	static AosConditionObjPtr	sgEmpty = OmnNew AosCondEmpty(true);	// Ken Lee, 2013/01/26
	static AosConditionObjPtr	sgRegExp = OmnNew AosCondRegExp(true);	// Andy Zhang, 2013/07/26  
	static AosConditionObjPtr	sgExpr = OmnNew AosCondExpr(true);		// Andy Zhang, 2013/12/17

	// Verifying all conditions were registered
	for (int i=AosCondType::eInvalid+1; i<AosCondType::eMax; i++)
	{
		OmnString name = AosCondType::toStr((AosCondType::E)i);
		aos_assert_r(name != AOSCONDTYPE_INVALID, false);
		aos_assert_r(AosCondType::toEnum(name) == i, false);
	}
	return true;
}

