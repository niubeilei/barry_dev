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
// Created: 2013/09/23 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_JQLExpr_Ptrs_h
#define Omn_JQLExpr_Ptrs_h

#include "Util/SPtr.h"
OmnPtrDecl(AosWordParser, AosWordParserPtr);
OmnPtrDecl(AosExpr, AosExprPtr)
OmnPtrDecl(AosExprArith, AosExprArithPtr)
OmnPtrDecl(AosExprAtomic, AosExprAtomicPtr)
OmnPtrDecl(AosExprBitArith, AosExprBitArithPtr)
OmnPtrDecl(AosExprComparison, AosExprComparisonPtr)
OmnPtrDecl(AosExprContain, AosExprContainPtr)
OmnPtrDecl(AosExprDouble, AosExprDoublePtr)
OmnPtrDecl(AosExprFieldName, AosExprFieldNamePtr)
OmnPtrDecl(AosExprGenFunc, AosExprGenFuncPtr)
OmnPtrDecl(AosExprGetDocid, AosExprGetDocidPtr)
OmnPtrDecl(AosExprIn, AosExprInPtr)
OmnPtrDecl(AosExprNotIn, AosExprNotInPtr)
OmnPtrDecl(AosExprLike, AosExprLikePtr)
OmnPtrDecl(AosExprLogic, AosExprLogicPtr)
OmnPtrDecl(AosExprMemberOpt, AosExprMemberOptPtr)
OmnPtrDecl(AosExprNumber, AosExprNumberPtr)
OmnPtrDecl(AosExprString, AosExprStringPtr)
OmnPtrDecl(AosExprNameValue, AosExprNameValuePtr)
OmnPtrDecl(AosExprBrackets, AosExprBracketsPtr)
OmnPtrDecl(AosExprCase, AosExprCasePtr)
OmnPtrDecl(AosExprSearchCase, AosExprSearchCasePtr)
OmnPtrDecl(AosExprSimpleCase, AosExprSimpleCasePtr)
OmnPtrDecl(AosExprUserVar, AosExprUserVarPtr)

#endif
