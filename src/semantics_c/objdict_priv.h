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
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_objdict_priv_h
#define aos_semantics_objdict_priv_h

#define AOS_OBJDICT_CHECK_INIT 								\
	if (!sg_objdict_init_flag) {							\
		aos_alarm("Object Dictionary not initialized yet");	\
		return 0;											\
	}

#endif // End of Include

