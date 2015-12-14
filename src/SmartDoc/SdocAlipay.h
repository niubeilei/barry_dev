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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocAlipay_h
#define Aos_SmartDoc_SdocAlipay_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Security/Ptrs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>

class AosSdocAlipay : public AosSmartDoc
{

public:
	AosSdocAlipay(const bool flag);
	~AosSdocAlipay();

	static OmnString smAlipayUrl;
	static OmnString smAlipayId;
	static OmnString smAlipayKey;
	static OmnString smSignType;
	static OmnString smCharset;
	static OmnString smReturnJsp;
	static OmnString smNotifyJsp;

	virtual bool config();

	bool		login(const AosXmlTagPtr &xmldoc);
	bool		pay(
					const AosXmlTagPtr &doc,
					const OmnString &serviceType,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	
	bool		alipayCb(
					const AosXmlTagPtr &obj, 
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	bool		payCb(
					const AosXmlTagPtr &obj,
					const AosXmlTagPtr &oldDoc,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	void		parmFilter(map<OmnString, OmnString> &parm);
	OmnString	buildSign(map<OmnString, OmnString> &parm);
	OmnString	linkParmToStr(map<OmnString, OmnString> &parm);
	OmnString   procParm(map<OmnString, OmnString> &parm, u64 docid);


	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocAlipay(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	OmnString 	getValue(const AosXmlTagPtr &configDoc, 
						const OmnString &configPath, 
						const OmnString &defaultPath,
						const AosXmlTagPtr &fromDoc)
		{
			OmnString attrPath = configDoc->getAttrStr(configPath);
			OmnString value = fromDoc->getAttrStr(attrPath);
			if(value == "" && defaultPath != "")
			{
				value = configDoc->getAttrStr(defaultPath);	
			}
			return value;
		};

	bool	payDualFuncPriv(
				map<OmnString, OmnString> &parm,
				const AosXmlTagPtr &config,
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata);

	bool	payDirectPriv(
				map<OmnString, OmnString> &parm,
				const AosXmlTagPtr &config,
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata);

};

#endif

