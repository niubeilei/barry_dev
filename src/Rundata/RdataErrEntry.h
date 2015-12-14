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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Rundata_RdataErrEntry_h
#define Aos_Rundata_RdataErrEntry_h

#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/FieldName.h"
#include "Util/FilePosEncoder.h"
#include "UtilData/FN.h"

class AosRdataErrEntry 
{
private:
	OmnString			mErrmsgId;
	OmnString			mTranslated;
	OmnString			mEntityErrors;
	vector<OmnString>	mXmlDetails;			
	OmnString			mDetails;			
	OmnString			mEntityName;			
	// AosRundataPtr 		mRdata;		Phil, 2015/10/20
	AosRundata* 		mRdata;			// Phil, 2015/10/20
	OmnString			mFname;
	int					mLine;
	bool				mIsUserError;
	AosErrmsgId::E		mEnumErrId;
	OmnString			mFieldName;		// Chen Ding, 2014/11/09
	vector<OmnString>	mFieldNames;	// Chen Ding, 2014/11/09
	vector<OmnString>	mDetailsVec;	// Chen Ding, 2014/11/09

	static bool smEncodeFilePos;

public:
	AosRdataErrEntry()
	{
	}

	~AosRdataErrEntry() {}

	void reset()
	{
		mErrmsgId = "";
		mTranslated = "";
		mEntityErrors = "";
		mXmlDetails.clear();
		mDetails = "";
		mEntityName = "";
		mRdata = 0;				// Phil, 2015/10/20
		mFname = "";
		mLine = -1;
		mIsUserError = false;
		mEnumErrId = AosErrmsgId::eOk;
		mFieldName = "";
		mFieldNames.clear();
		mDetailsVec.clear();
	}

	AosRdataErrEntry &setError(
			const char *file, 
			const int line, 
			const OmnString &errmsg_id,
			const OmnString &msg,
			const OmnString &name, 
			const OmnString &errmsg,
			const bool is_user_error,
			AosRundata *rdata);

	AosRdataErrEntry &setError(
			const char *file, 
			const int line, 
			const OmnString &errmsg_id,
			const bool is_user_error,
			AosRundata *rdata);

	AosRdataErrEntry &setError(
			const char *file, 
			const int line, 
			const AosErrmsgId::E errmsg_id,
			const bool is_user_error,
			AosRundata *rdata);


	AosRdataErrEntry & operator << (const AosFN &fname);
	AosRdataErrEntry & operator << (const AosFieldName::E field);
	AosRdataErrEntry & operator << (const OmnEndError er);

	AosRdataErrEntry & operator << (const OmnString &errmsg)
	{
		mDetails << errmsg;
		return *this;
	}

	AosRdataErrEntry & operator << (const std::string &errmsg)
	{
		mDetails << errmsg;
		return *this;
	}

	AosRdataErrEntry & operator << (const char *errmsg)
	{
		mDetails << errmsg;
		return *this;
	}

	AosRdataErrEntry & operator << (const u8 *msg)
	{
		mDetails << (const char *)msg;
		return *this;
	}

	AosRdataErrEntry & operator << (const int value)
	{
		mDetails << value;
		return *this;
	}

	AosRdataErrEntry & operator << (const u8 value)
	{
		mDetails << value;
		return *this;
	}

	AosRdataErrEntry & operator << (const u32 value)
	{
		mDetails << value;
		return *this;
	}

	AosRdataErrEntry & operator << (void *ptr)
	{
		char buff[100];
		sprintf(buff, "%lx", (unsigned long)ptr);
		mDetails << buff;
		return *this;
	}

	AosRdataErrEntry & operator << (const u64 &value)
	{
		mDetails << value;
		return *this;
	}

	AosRdataErrEntry & operator << (const int64_t &value)
	{
		mDetails << value;
		return *this;
	}

	AosRdataErrEntry & operator << (const double &value)
	{
		mDetails << value;
		return *this;
	}

	// Chen Ding, 2013/12/06
	AosRdataErrEntry & operator << (const AosXmlTagPtr &xml);

	// OmnString getErrmsg() const {return mErrmsg;}
	// Chen Ding, 2014/11/09
	// bool isOk() const {return mErrmsgId == "";}
	bool isOk() const {return mErrmsgId == "" && mEnumErrId == AosErrmsgId::eOk;}

	static void setEncodeFilePos(const bool b) {smEncodeFilePos = b;}

	OmnString createErrmsg();

private:
	void pushField();
};
#endif

