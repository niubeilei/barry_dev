////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/03/07 Created by Arvin  
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_Jimos_StatTime_h
#define AOS_StatUtil_Jimos_StatTime_h

#include "StatUtil/StatField.h"
#include "StatUtil/Jimos/Ptrs.h"
#include "Rundata/Rundata.h"
class AosStatTime : public OmnRCObject
{
	OmnDefineRCObject;
	private:
		OmnString mTimeFieldName;
		OmnString mTimeFormat;
		OmnString mGrpbyTimeUnit;
	public:
	
		AosStatTime();
		
		~AosStatTime();
		
		virtual bool config(const AosXmlTagPtr &conf);
	
//		virtual OmnString toXmlString();
		
		virtual OmnString getTimeFormat(){return mTimeFormat;}

		virtual OmnString getGrpbyTimeUnit(){return mGrpbyTimeUnit;}
		
		virtual OmnString getTimeFieldName(){return mTimeFieldName;}

		virtual bool setTimeFormat(const OmnString &timeFormat)
		{
			mTimeFormat = timeFormat;
			return true;
		}
		virtual bool setGrpbyTimeUnit(const OmnString &timeUnit)
		{
			mGrpbyTimeUnit = timeUnit;
			return true;
		}

};

#endif
