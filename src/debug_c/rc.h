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
// 12/15/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_debug_rc_h
#define aos_debug_rc_h

typedef enum
{
	eAosRc_ElemExist,
	eAosRc_FailedAddEntry,
	eAosRc_Fatal,
	eAosRc_KeyLenTooLong,
	eAosRc_MemoryCliErr,
	eAosRc_MemoryCatcherErr,
	eAosRc_NotInit,
	eAosRc_NullPtr,
	eAosRc_ObjDictFailed,
	eAosRc_OutOfMemory,
	eAosRc_ProgramError,
	eAosRc_RuleEvalFalse,
	eAosRc_RunOutOfMemory,
	eAosRc_SemanticOperatorFailed,
	eAosRc_StrUtilErr,
	eAosRc_TooManyElem,
	eAosRc_TypeInconsistent,
	eAosRc_TypeMismatch,
	eAosRc_VirtualFuncCalled,
	eAosRc_ZeroSizeObject,
}aos_retcode_e;

#endif

