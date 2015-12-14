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
// 02/19/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocSendMail.h"

#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SEUtilServer/UserDocMgr.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#if 0

AosSdocSendMail::AosSdocSendMail(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_SENDMAIL, AosSdocId::eSendMail, flag)
{
}


AosSdocSendMail::~AosSdocSendMail()
{
}


bool
AosSdocSendMail::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<smartdoc zky_sdoctp="sendemail" zky_otype="zky_smtdoc">
	//	<sender>
	//		<name zky_value_type="const"><![CDATA[xxxx]]></name>
	//		...
	//	</sender>
	//	<senderpasswd>
	//		<name zky_value_type="const"><![CDATA[xxxx]]></name>
	//	</senderpasswd>
	//	<subject>
	//		<name zky_value_type="const"><![CDATA[xxxxxx]]></name>
	//		<name zky_value_type="attr" zky_xpath="xxxx">
	//		<zky_doc_selector zky_docselector_type="xxx">
	//		</zky_doc_selector>
	//		</name>
	//		...
	//	</subject>
	//	<contents>
	//		<name zky_value_type="const"><![CDATA[xxxxxx]]></name>
	//		<name zky_value_type="attr" zky_xpath="xxxx">
	//		<zky_doc_selector zky_docselector_type="xxx">
	//		</zky_doc_selector>
	//		</name>
	//		<name zky_value_type="const"><![CDATA[xxxxx]]><name>
	//		...
	//	</contents>
	//</smartdoc>
	//Zky2742 Linda 2011/02/19
	OmnString substr, constr, senderstr, passwdstr;
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mSdoc = sdoc;
	
	AosXmlTagPtr sender = sdoc->getFirstChild("sender");
	if (sender)
	{
		//rdata->setError() << "Missing Sender!";
		//OmnAlarm << rdata->getErrmsg() << enderr;
		//return false;
		AosValueRslt valueRslt;
		if (AosValueSel::getValueStatic(valueRslt, sender, rdata))
		{
			if (!valueRslt.isValid())
			{
				rdata->setError() << "Value is invalid";
		   		 return false;
			}

			if (valueRslt.isXmlDoc())
			{
				rdata->setError() << "Value is an XML doc";
				return false;
			}
			senderstr = valueRslt.getValueStr1();
		}
		if (senderstr!="")
		{
			AosXmlTagPtr senderpasswd = sdoc->getFirstChild("senderpasswd"); 
			if (!senderpasswd)
			{
				rdata->setError() << "Missing Sender Passwd!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}

			AosValueRslt valueRslt;
			if (AosValueSel::getValueStatic(valueRslt, senderpasswd, rdata))
			{
				if (!valueRslt.isValid())
				{
					rdata->setError() << "Value is invalid";
		   			 return false;
				}

				if (valueRslt.isXmlDoc())
				{
					rdata->setError() << "Value is an XML doc";
					return false;
				}
				passwdstr = valueRslt.getValueStr1();
			}

			if (passwdstr == "")
			{
				rdata->setError() << "Missing Sender Passwd!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
	}

	AosXmlTagPtr subject = sdoc->getFirstChild("subject");
	if (!subject)
	{
		rdata->setError() << "Missing Subject!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt valueRslt;
	if (AosValueSel::getValueStatic(valueRslt, subject, rdata))
	{
		if (!valueRslt.isValid())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}
		substr = valueRslt.getValueStr1();
	}

	AosXmlTagPtr contents = sdoc->getFirstChild("contents");
	if (!contents)
	{
		rdata->setError() << "Missing Contents!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt valueRslt1;
	if (AosValueSel::getValueStatic(valueRslt1, contents, rdata))
	{
		if (!valueRslt1.isValid())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt1.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}
		constr = valueRslt1.getValueStr1();
	}

	AosXmlTagPtr userobj = rdata->getSourceDoc();
	if (!userobj)
	{
		rdata->setError() << "Missing Userobj!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString email = userobj->getAttrStr(AOSTAG_EMAIL);
	if (email == "")
	{
		rdata->setError() << "Missing Email Address!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString docstr ="<sendemail ";
	docstr<< "smailaddr=\"" << senderstr <<"\" "
		<< "senderpasswd =\"" << passwdstr <<"\" "
		<< "rmailaddr =\"" << email <<"\" "
		<< "subject =\"" << substr <<"\" "
		<< "contents =\"" << constr <<"\" />";

	rdata->setResults(docstr);
	rdata->setOk();
	return true;
}

#endif 
