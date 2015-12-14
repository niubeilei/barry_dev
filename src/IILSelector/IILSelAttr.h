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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILSelector_IILSelAttr_h
#define Aos_IILSelector_IILSelAttr_h

#incluce "IILSelector/IILSelector.h"



class AosIILSelAttr : virtual public AosIILSelector
{

public:
	AosIILSelAttr(const bool regflag);

	virtual OmnString getIILName(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

