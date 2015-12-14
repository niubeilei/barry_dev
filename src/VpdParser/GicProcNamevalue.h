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
#ifndef Omn_VpdParser_GicProcNamevalue_h
#define Omn_VpdParser_GicProcNamevalue_h

#include "VpdParser/GicProc.h"

class AosGicProcNamevalue : public AosGicProc
{
public:
	AosGicProcNamevalue() 
	:
	AosGicProc(AosGicName::eNameValue)
	{}
	virtual ~AosGicProcNamevalue();

	virtual bool procGic(
				const AosBookPtr &book,
				TiXmlElement *obj, 
				TiXmlElement *viewer, 
				TiXmlElement *editor, 
				AosXmlRc &errcode, 
				OmnString &errmsg);

	TiXmlElement * createInputGic(TiXmlElement *namevalue);
};
#endif
