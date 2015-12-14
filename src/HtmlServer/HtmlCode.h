////////////////////////////////////////////////////////////////////////////

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
// 08/21/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlServer_HtmlCode_h
#define Aos_HtmlServer_HtmlCode_h

#include "Alarm/Alarm.h"
#include "SEUtil/DocTags.h"
#include "Util/String.h"



struct AosHtmlCode
{
	enum Operation
	{
		eInvalid,

		eGenerateCode,
		eGetContainer,

		eMax
	};

	OmnString 	mHtml;
	OmnString 	mJson;
	OmnString 	mJs;
	OmnString 	mCss;
	OmnString 	mFlash;
	OmnString 	mXml;
	int 	  	mCounter;
	Operation	mOpr;
	OmnString	mCrtCaller;
	OmnString	mCrtVpdObjid;
	OmnString gic_width;	// for test. Ketty 2011/09/23
	OmnString gic_height;

	AosHtmlCode()
	:
	mCounter(-1),
	mOpr(eGenerateCode)
	{
	}

	void append(AosHtmlCode &code)
	{
		mHtml << code.mHtml;
		mJson << code.mJson;
		mJs << code.mJs;
		mCss << code.mCss;
		mFlash << code.mFlash;
		mXml << code.mXml;
	};
	void reset()
	{
		mHtml = "";
		mJson = "";
		mJs = "";
		mCss = "";
		mFlash = "";
		mXml = "";
		mCounter = -1;
		gic_width = "";
		gic_height = "";
	};

	~ AosHtmlCode()
	{
	}

	Operation getOperation() const {return mOpr;}
	void setRetrieveContainers() {mOpr = eGetContainer;}
	void setCrtVpdObjid(const OmnString &objid) {mCrtVpdObjid = objid;}
	void setCrtCaller(const OmnString &caller) {mCrtCaller = caller;}
	void addContainer(const OmnString &container)
	{
		// Containers are collected through 'mXml', which is the form:
		// 	<containers>
		// 		<container AOSTAG_COMPID="xxx" 
		// 				AOSTAG_CONTAINER="xxx"
		// 				AOSTAG_VPD_OBJID="xxx"/>
		if (mXml == "")
		{
			mXml = "<containers>";
		}

		mXml << "<container ";
		if (mCrtCaller == "")
		{
			OmnAlarm << "Missing current caller" << enderr;
		}
		else
		{
			mXml << AOSTAG_COMPID << "=\"" << mCrtCaller << "\" ";
		}

		if (container == "")
		{
			OmnAlarm << "Container empty" << enderr;
		}
		else
		{
			mXml << AOSTAG_CONTAINER << "=\"" << container << "\" ";
		}

		if (mCrtVpdObjid == "")
		{
			OmnAlarm << "VPD objid empty" << enderr;
		}
		else
		{
			mXml << AOSTAG_VPD_OBJID << "=\"" << mCrtVpdObjid << "\"";
		}

		mXml << "/>";
	}

	void finishRetrievingContainers()
	{
		if (mXml != "")
		{
			mXml << "</containers>";
		}
	}
};
#endif
