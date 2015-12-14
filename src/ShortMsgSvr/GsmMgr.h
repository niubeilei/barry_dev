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
// 05/19/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgSvr_GsmMgr_h
#define Aos_ShortMsgSvr_GsmMgr_h

#include "Rundata/Rundata.h"
#include "ShortMsgUtil/Ptrs.h"
#include "ShortMsgUtil/GsmModem.h"
#include <map>
using namespace std;


class AosGsmMgr
{
	OmnDefineRCObject;
public:
	typedef	map<OmnString, AosGsmModemPtr>	  				 AosGsmModemMap;
	typedef map<OmnString, AosGsmModemPtr>::iterator		 AosGsmModemMapItr;	
	typedef pair<OmnString, AosGsmModemPtr>					 AosGsmModemPair;	

private:
	AosGsmModemMap				mGsmModemMap;
	AosGsmModemPtr				mDftGsmModem;

public:
	AosGsmMgr(const AosXmlTagPtr &config);
	~AosGsmMgr();
	
	AosGsmModemPtr				getGsmModem(const OmnString   &key);
	void						close();

private:
	bool			init(const AosXmlTagPtr &config);
};
#endif
