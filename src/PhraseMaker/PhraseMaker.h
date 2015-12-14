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
// 2013/05/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_PhraseMaker_PhraseMaker_h
#define Aos_PhraseMaker_PhraseMaker_h

#include "SEInterfaces/PhraseMakerObj.h"


class AosPhraseMaker : virtual public AosPhraseMakerObj
{
public:
	AosPhraseMaker();
	~AosPhraseMaker();

protected:
	bool config( 		const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def);
};

#endif
