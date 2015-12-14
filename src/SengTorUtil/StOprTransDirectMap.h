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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StOprTransDirectMap_h
#define AosSengTorUtil_StOprTransDirectMap_h

#include "alarm_c/alarm.h"
#include "SengTorUtil/StOprTranslator.h"


class AosStOprTransDirectMap : public AosStOprTranslator
{
private:

public:
	AosStOprTransDirectMap(const bool regflag);
	~AosStOprTransDirectMap();

	virtual OmnString setRandContents(const AosRundataPtr &rdata);
	virtual AosStOprTranslatorPtr clone() const;
	virtual bool pickCommand(OmnString &cmd, 
						OmnString &operation, 
						const AosSengTestThrdPtr &thread);

private:
};
#endif

