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
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_Semantics_h
#define Aos_Semantics_Semantics_h

#ifndef AOS_SEMANTICS

#define aos_decl_loop_safety_guard(guard)
#define aos_rule_loop_safety_guard(guard, max)
#define aos_rule_loop_safety_guard_b(guard, max)
#define aos_rule_loop_safety_guard_r(guard, max, ret)
#define AosSemanDecl(decl)
#define AosSemanSet(var, value)

#else

#define AosSemanDecl(decl) decl
#define AosSemanSet(var, value) var = value

#ifndef aos_decl_loop_safety_guard
#define aos_decl_loop_safety_guard(xxx)					\
		int xxx = 0;
#endif

#define aos_sr_loop_safety_guard(guard, max) 			\
			if ((guard)++ > (max))						\
			{											\
				AosRaiseError("Failed safety guard");	\
				return;									\
			}

#define aos_sr_loop_safety_guard_b(guard, max) 			\
			if ((guard)++ > (max))						\
			{											\
				AosRaiseError("Failed safety guard");	\
				break;									\
			}

#define aos_sr_loop_safety_guard_r(guard, max, ret) 	\
			if ((guard)++ > (max))						\
			{											\
				AosRaiseError("Failed safety guard");	\
				return (ret);							\
			}

#endif

#endif
