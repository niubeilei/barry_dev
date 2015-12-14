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
// 2015/08/20 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoProgCheckers_JPSCCheckIndexesAndDocs_h
#define Aos_JimoProgCheckers_JPSCCheckIndexesAndDocs_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Rundata/Rundata.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/JPSyntaxCheckerObj.h"

class AosJPSCCheckIndexesAndDocs :  public AosJPSyntaxCheckerObj
{
	OmnDefineRCObject;

	private:
		typedef map<OmnString,AosJimoLogicObjNewPtr>  map_t;
		typedef map<OmnString,AosJimoLogicObjNewPtr>::iterator  itr_t;
		OmnString 		mErrMsg;
	
	public:
		AosJPSCCheckIndexesAndDocs(const AosRundata* rdata,const int version);

		AosJPSCCheckIndexesAndDocs(const AosJPSCCheckIndexesAndDocs &JPSC);

		~AosJPSCCheckIndexesAndDocs();
		
		//Jimo Interface
		AosJimoPtr cloneJimo() const;

		virtual bool checkSyntax(AosRundata *rdata, AosJimoProgObj *prog);
};

#endif

