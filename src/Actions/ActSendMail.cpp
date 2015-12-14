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
#include "Actions/ActSendMail.h"

#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Python/Pyemail.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#if 0

AosActSendMail::AosActSendMail(const bool flag)
:
AosSdocAction(AOSACTTYPE_SENDMAIL, AosActionType::eSendMail, flag)
{
}


AosActSendMail::~AosActSendMail()
{
}


bool
AosActSendMail::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<smartdoc type="sendemail" zky_otype="zky_smtdoc">
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
	//	<receiver>
	//		<zky_value ...>
	//	</receiver>
	//</smartdoc>
	
	OmnString substr, constr, senderstr, passwdstr;
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr sender = sdoc->getFirstChild("sender");
	if (sender)
	{
		AosValueRslt valueRslt;
		if (AosValueSel::getValueStatic(valueRslt, sender, rdata))
		{
			if (!valueRslt.isNull())
			{
				rdata->setError() << "Value is invalid";
		   		 return false;
			}

			if (valueRslt.isXmlDoc())
			{
				rdata->setError() << "Value is an XML doc";
				return false;
			}
			senderstr = valueRslt.getStr();
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
				if (!valueRslt.isNull())
				{
					rdata->setError() << "Value is invalid";
		   			 return false;
				}

				if (valueRslt.isXmlDoc())
				{
					rdata->setError() << "Value is an XML doc";
					return false;
				}
				passwdstr = valueRslt.getStr();
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
		if (!valueRslt.isNull())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}
		substr = valueRslt.getStr();
	}

	AosXmlTagPtr contents = sdoc->getFirstChild("contents");
	if (contents)
	{
		AosValueRslt valueRslt1;
		if (AosValueSel::getValueStatic(valueRslt1, contents, rdata))
		{
			if (!valueRslt1.isNull())
			{
				rdata->setError() << "Value is invalid";
				return false;
			}

			if (valueRslt1.isXmlDoc())
			{
				rdata->setError() << "Value is an XML doc";
				return false;
			}
			constr = valueRslt1.getStr();
		}
	}

	OmnString email;
	AosXmlTagPtr receiver = sdoc->getFirstChild("receiver");
	if (!receiver)
	{
		rdata->setError() << "Missing Subject!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt valueRslt2;
	if (AosValueSel::getValueStatic(valueRslt2, receiver, rdata))
	{
		if (!valueRslt2.isNull())
		{
			rdata->setError() << "Value is invalid";
			return false;
		}

		if (valueRslt2.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return false;
		}
		email = valueRslt2.getStr();
	}

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

	if (sdoc->getAttrBool("zky_send_flag", false))
	{
		AosXmlParser parser;
		AosXmlTagPtr emailxml = parser.parse(docstr, "" AosMemoryCheckerArgs);
		AosPyemail::getSelf()->sendmail(emailxml, rdata);
	}
	else
	{
		rdata->setResults(docstr);
	}
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSendMail::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSendMail(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


#endif
