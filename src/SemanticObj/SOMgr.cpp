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
// 12/09/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticObj/SOMgr.h"

#include "SemanticObj/SemanticObj.h"



AosSOMgr::AosSOMgr()
{
}


AosSOMgr::~AosSOMgr()
{
}


bool	
AosSOMgr::addSO(const AosSemanticObjPtr &so)
{
	return mObjs.add(so);
}


bool	
AosSOMgr::removeSO(const AosSemanticObjPtr &so)
{
	return mObjs.remove(so);
}


