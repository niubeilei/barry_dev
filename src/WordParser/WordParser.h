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
// 09/18/2010	Created by James 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_scws_WordParser_h
#define AOS_scws_WordParser_h

#include <scws/scws.h>
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/WordParserObj.h"
#include "Util/String.h"
#include "Util/SPtr.h"



// Chen Ding, 2014/12/06
// class AosWordParser : virtual public OmnRCObject
class AosWordParser : public AosWordParserObj
{
	OmnDefineRCObject;

private:
	scws_res_t		mRes;
	scws_res_t		mCrt;
	OmnString		mCrtStr;
	bool	        mFirst;
	bool			mFinished;
	scws_t			mScws;	

public:
	AosWordParser();
	~AosWordParser();
	// static bool init(const AosXmlTagPtr &config);
	bool	setSrc(const OmnString &src);
	bool	setSrc(const char* src, int start, int len);
	bool	nextWord(OmnString &wordbuff);
	bool	nextWord(char* wordbuff, int &len);
};

#endif
