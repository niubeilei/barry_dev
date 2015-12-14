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
#include "Rundata/RdataErrEntry.h"

#include "Rundata/Rundata.h"
#include "Rundata/RdataUtil.h"

bool AosRdataErrEntry::smEncodeFilePos = false;


AosRdataErrEntry &
AosRdataErrEntry::setError(
		const char *file, 
		const int line, 
		const OmnString &errmsg_id,
		const OmnString &translated_msg,
		const OmnString &entity_name, 
		const OmnString &entity_errmsg,
		const bool is_user_error,
		AosRundata *rdata)
{
	if (rdata && mErrmsgId != "")
	{
		rdata->setErrorEntry(mIsUserError, createErrmsg(), mFname.data(), mLine);
		reset();
	}

	mErrmsgId = errmsg_id;
	mFname = file;
	mLine = line;
	mIsUserError = is_user_error;
	mTranslated = translated_msg;
	mEntityName = entity_name;
	mEntityErrors = entity_errmsg;
	mRdata = rdata;
	return *this;
}


// Chen Ding, 2014/11/09
AosRdataErrEntry &
AosRdataErrEntry::setError(
		const char *file, 
		const int line, 
		const OmnString &errmsg_id,
		const bool is_user_error,
		AosRundata *rdata)
{
	if (rdata && mErrmsgId != "")
	{
		rdata->setErrorEntry(mIsUserError, createErrmsg(), mFname.data(), mLine);
		reset();
	}

	mErrmsgId = errmsg_id;
	mFname = file;
	mLine = line;
	mIsUserError = is_user_error;
	mTranslated = "";
	mRdata = rdata;
	return *this;
}


AosRdataErrEntry &
AosRdataErrEntry::setError(
		const char *file, 
		const int line, 
		const AosErrmsgId::E errmsg_id,
		const bool is_user_error,
		AosRundata *rdata)
{
	if (rdata && mErrmsgId != "")
	{
		rdata->setErrorEntry(mIsUserError, createErrmsg(), mFname.data(), mLine);
		reset();
	}

	mEnumErrId = errmsg_id;
	mFname = file;
	mLine = line;
	mIsUserError = is_user_error;
	mTranslated = "";
	mRdata = rdata;
	return *this;
}


AosRdataErrEntry & 
AosRdataErrEntry::operator << (const OmnEndError er)
{
	if (mRdata) 
	{
		mRdata->setErrorEntry(mIsUserError, createErrmsg(), mFname.data(), mLine);
	}
	reset();
	return *this;
}


AosRdataErrEntry & 
AosRdataErrEntry::operator << (const AosFN &field_name)
{
	if (mFieldName != "") pushField();

	mFieldName = field_name.fname;
	return *this;
}


AosRdataErrEntry & 
AosRdataErrEntry::operator << (const AosFieldName::E field_name)
{
	if (mFieldName != "") pushField();

	mFieldName = AosFieldName::toStr(field_name);
	return *this;
}


void
AosRdataErrEntry::pushField()
{
	mFieldNames.push_back(mFieldName);

	if (mDetails != "")
	{
		mDetails.replace("<![CDATA[", "", true);
		mDetails.replace("]]>", "", true);
	}

	if (mXmlDetails.size() > 0)
	{
		for (u32 i=0; i<mXmlDetails.size(); i++)
		{
			mDetails << "<xml_" << i << ">" << mXmlDetails[i] 
				<< "</xml_" << i << ">";
		}
	}

	mDetailsVec.push_back(mDetails);
	mDetails = "";
	mXmlDetails.clear(); 
}


OmnString
AosRdataErrEntry::createErrmsg()
{
	// The error message is an XML tag:
	// 	<error fname="xxx"
	// 		line="xxx"
	// 		error_pos="xxx"
	// 		errmsg_id="xxx">
	// 		<errmsg><![CDATA[xxx]]></errmsg>
	// 		<entity entity_name="xxx"><![CDATA[xxx]]></entity>
	// 		<details><![CDATA[xxx]]></details>
	//	</error>	
	
	if (mErrmsgId == "") return "";

	OmnString error = "<error";
	error << " errmsg_id=\"" << mErrmsgId  << "\"";
	error << " errpos=\"" << AosFilePosEncoder::getCode(mFname, mLine) << "\"";
	if (!smEncodeFilePos)
	{
		error << " fname=\"" << mFname << "\"";
		error << " line=\"" << mLine << "\"";
	}
	error << ">";

	if (mTranslated != "")
	{
		error << "<errmsg><![CDATA[" << mTranslated << "]]></errmsg>";
	}

	if (mFieldName != "") pushField();

	// Chen Ding, 2014/11/09
	if (mFieldNames.size() != mDetailsVec.size())
	{
		cout << "********** Alarm ********** " << endl;
	}
	else
	{
		for (u32 i=0; i<mFieldNames.size(); i++)
		{
			AosRdataUtil::normalizeTagname(mFieldNames[i]);
			error << "<" << mFieldNames[i]
				<< "\"><![CDATA[" << mDetailsVec[i] 
				<< "]]></" << mFieldNames[i] << ">";
		}
	}

	if (mEntityName != "")
	{
		error << "<entity entity_name=\"" << mEntityName
			<< "\"><![CDATA[" << mEntityErrors << "]]></entity>";
	}

	error << "</error>";
	return error;
}


AosRdataErrEntry & 
AosRdataErrEntry::operator << (const AosXmlTagPtr &xml)
{
	if (xml)
	{
		mXmlDetails.push_back(xml->toString());
	}
	return *this;
}

