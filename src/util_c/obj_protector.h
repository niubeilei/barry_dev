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
// 03/15/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_obj_protector_h
#define aos_util_obj_protector_h

#define AOS_OBJ_PROTECTOR_SIZE 5

#deine AOS_DECL_PREFIX_PROTECTOR int __prefix_protector[AOS_OBJ_PROTECTOR_SIZE]

#deine AOS_DECL_POSTFIX_PROTECTOR int __postfix_protector[AOS_OBJ_PROTECTOR_SIZE]

#define AOS_SET_PREFIX_PROTECTOR(v)							\
	int __idx;												\
	for (__idx=0; __idx<AOS_OBJ_PROTECTOR_SIZE; __idx++)	\
	__prefix_protector[__idx] = (v)

#define AOS_SET_POSTFIX_PROTECTOR(v)						\
	int __idx;												\
	for (__idx=0; __idx<AOS_OBJ_PROTECTOR_SIZE; __idx++)	\
	__postfix_protector[__idx] = (v)

#define AOS_CHECK_PREFIX_PROTECTOR(v)						\
	int __idx;												\
	for (__idx=0; __idx<AOS_OBJ_PROTECTOR_SIZE; __idx++)	\
	aos_assert_r(__prefix_protector[__idx] == (v), -1)

#define AOS_CHECK_POSTFIX_PROTECTOR(v)						\
	int __idx;												\
	for (__idx=0; __idx<AOS_OBJ_PROTECTOR_SIZE; __idx++)	\
	aos_assert_r(__postfix_protector[__idx] == (v), -1)

#endif // End of Include

