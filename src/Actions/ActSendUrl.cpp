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
// This action send one url:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 04/09/2013 Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSendUrl.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
#include <stdio.h>
#include <curl/curl.h>
using namespace std;

AosActSendUrl::AosActSendUrl(const bool flag)
:
AosSdocAction(AOSACTTYPE_SENDURL, AosActionType::eSendUrl, flag)
{
}


AosActSendUrl::~AosActSendUrl()
{
}


bool	
AosActSendUrl::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action send one url . 
	// 	<sdoc AOSTAG_ZKY_TYPE=SENDURL
	// 		zky_send_url="xxx"
	// 		zky_send_method="post/get">
	// 		<parameters>
	// 			<parameter name="xxx" valuefrom="valuesel|rdata.log">
	// 				<valuesel/>
	// 			</parameter>
	// 			...
	// 			<parameter name="xxx" valuefrom="valuesel|rdata.log">
	// 				<valuesel/>
	// 			</parameter>
	// 		</parameters>
	// 	</sdoc>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr pchild = sdoc->getFirstChild("parameters");
	if (!pchild)
	{
		AosSetError(rdata, "failed to get parameters");
		OmnAlarm << rdata->getErrmsg() << "failed to get parameters" << enderr;
		return false;
	}
	OmnString parameters;
	AosXmlTagPtr ele = pchild->getFirstChild();
	while(ele)
	{
		OmnString name = ele->getAttrStr("name", "");
		aos_assert_r(name != "", false);
		OmnString valuefrom = ele->getAttrStr("valuefrom", "valuesel");
		if(valuefrom == "rdata.log")
		{
			parameters << name << "=" << rdata->getLogStr() << "&";
		}
		else
		{
			AosValueRslt value;
			bool rslt = AosValueSel::getValueStatic(value, ele, AOSTAG_VALUE_SELECTOR, rdata);
			aos_assert_r(rslt, false);

			// The data type cannot be XML DOC
			if (!value.isNull())
			{
				AosSetError(rdata, "eValueIncorrect");
				OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
				return false;
			}

			OmnString newvalue = value.getStr();
			parameters << name << "=" << newvalue << "&";
		}

		ele = pchild->getNextChild();
	}
	OmnString url = sdoc->getAttrStr("zky_send_url");
	OmnString method = sdoc->getAttrStr("zky_send_method", "post");


	CURL *curl;
	CURLcode res;
	 
	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);
	 
	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be a https:// URL if that is what should receive the
		   data. */ 
		/* Now specify the POST/GET data */ 
		if(method == "post")
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.data());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parameters.data());
		}
		else
		{
			url << "?" << parameters;
			curl_easy_setopt(curl, CURLOPT_URL, url.data());
		}
	 
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		  	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSendUrl::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSendUrl(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


