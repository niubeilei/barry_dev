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
// 11/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerAction_LogicObj_h
#define Omn_TorturerAction_LogicObj_h

class OmnString;

enum AosOperandType
{
};


class AosLogicObj
{
   
public:	
	virtual bool    getValue(const AosOperandType type,
							 const OmnString &name,
							 OmnString &value,
							 OmnString &errmsg);
};

#endif

