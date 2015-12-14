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
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticAsserts_SemanticAssertIds_h
#define Aos_SemanticAsserts_SemanticAssertIds_h


extern std::string AosSAID_toStr(const AosSA_ID code);
extern AosSA_ID    AosSAID_toEnum(const std::string &name);

enum AosSA_ID
{
	eAosSAID_PointToObjectType, 

	eAosSAID_MaxEntry
};

#endif

