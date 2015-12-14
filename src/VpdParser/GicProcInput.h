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
// 	Created: 12/14/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_VpdParser_GicProcInput_h
#define Omn_VpdParser_GicProcInput_h

#include "VpdParser/GicProc.h"

class AosGicProcInput : public AosGicProc
{
public:
	AosGicProcInput();
	virtual ~AosGicProcInput();

	virtual bool procGic(
				const AosBookPtr &book,
				TiXmlElement *obj, 
				TiXmlElement *viewer, 
				TiXmlElement *editor, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
};
#endif
