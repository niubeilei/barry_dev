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
#ifndef Omn_VpdParser_GicProc_h
#define Omn_VpdParser_GicProc_h

#include "Book/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "VpdParser/GicName.h"
#include "VpdParser/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>

class TiXmlElement;

class AosGicProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosGicName::E	mGicId;

public:
	AosGicProc(const AosGicName::E id);
	virtual ~AosGicProc() {}

	AosGicName::E	getGicId() const {return mGicId;}

	virtual bool procGic(
				const AosBookPtr &book,
				TiXmlElement *obj, 
				TiXmlElement *viewer, 
				TiXmlElement *editor, 
				AosXmlRc &errcode, 
				OmnString &errmsg) = 0;

	virtual AosGicProcNamevaluePtr	toNamevalue() const;
};
#endif
