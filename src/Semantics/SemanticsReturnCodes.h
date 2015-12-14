////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 11/28/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_SemanticsReturnCode_h
#define Aos_Semantics_SemanticsReturnCode_h

enum AosSemanticsReturnCode
{
	eAosRc_RuleEvaluateTrue,
	eAosRc_RuleEvaluateFalse,
	eAosRc_RuleEvaluateContinue,	// Failed but continue the next one
	eAosRc_RuleEvaluateStop,		// Failed and rule evaluation should stop
	eAosRc_RuleEvaluateAbortCall,	// Failed and should abort the function call
	eAosRc_RuleEvaluateAbortApp,	// Failed and should abort the application (exit)
};

#endif
