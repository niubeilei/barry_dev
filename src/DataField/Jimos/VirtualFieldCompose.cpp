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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataField/Jimos/VirtualFieldCompose.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosVirtualFieldCompose_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosVirtualFieldCompose(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosVirtualFieldCompose::AosVirtualFieldCompose(const int version)
:
AosDataField(version)
{
	mIgnoreSerialize = true;
}


AosVirtualFieldCompose::~AosVirtualFieldCompose()
{
}


bool 
AosVirtualFieldCompose::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	// The configuration format is:
	// 	<field name=AOSVIRTUALFIELDNAME_COMPOSE>
	// 		<pre_filter><![CDATA[expression]]></pre_filter>
	// 		<post_filter><![CDATA[expression]]></post_filter>
	// 		<entries>
	// 			<entry need_trim="true|false">	 (default: false)
	// 				<null_default data_type="xxx">xxx</null_default>
	// 				<empty_default data_type="xxx">xxx</empty_default>
	// 				<separator>xxx</separator>
	// 			</entry>
	// 			<entry .../>
	// 			<entry .../>
	// 			...
	// 		</entries>
	// 	</field>
	mFields.clear();
	if (!def)
	{
		AosLogError(rdata, true, AosErrmsgId::eMissingDataFieldConfig) << enderr;
		return false;
	}

	// Sanity Check only
	OmnString name = def->getAttrStr("name");
	if (name == "")
	{
		AosLogError(rdata, true, AosErrmsgId::eJimoNameEmpty)
			<< AosEnumFieldName::eConfig << def << enderr;
		return false;
	}

	// Parse the pre filter
	AosXmlTagPtr tag = def->getFirstChild("pre_filter");
	if (tag)
	{
		// Parser the expression, then set the following member data:
		// AosExprObjPtr		mFilter;
		// AosExprObj*			mFilterRaw;
		OmnNotImplementedYet;
	}

	// Parse the post filter
	tag = def->getFirstChild("post_filter");
	if (tag)
	{
		// Parser the expression, then set the following member data:
		// AosExprObjPtr		mRsltFilter;
		// AosExprObj*			mRsltFilterRaw;
		OmnNotImplementedYet;
	}
	
	// Parse the fields
	AosXmlTagPtr entries = def->getFirstChild("entries");
	if (!entries)
	{
		AosLogUserError(rdata, AosErrmsgId::eVirtualFieldMissingEntries)
			<< AosEnumFieldName::eConfig << def << enderr;
		return false;
	}

	AosXmlTagPtr entry_tag = entries->getFirstChild();
	while (entry_tag)
	{
		// 	<entry need_trim="true|false">	 (default: false)
		// 	<null_default data_type="xxx">xxx</null_default>
		// 		<empty_default data_type="xxx">xxx</empty_default>
		// 		<separator>xxx</separator>
		// 	</entry>
		OmnNotImplementedYet;
		entry_tag = entries->getNextChild();
	}

	if (mFields.size() == 0)
	{
		AosLogUserError(rdata, AosErrmsgId::eVirtualFieldMissingEntries)
			<< AosEnumFieldName::eConfig << def << enderr;
		return false;
	}

	return true;
}


AosJimoPtr
AosVirtualFieldCompose::cloneJimo() const
{
	return OmnNew AosVirtualFieldCompose(*this);
}


bool 
AosVirtualFieldCompose::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosVirtualFieldCompose::getFieldValue(
		AosRundata *rdata, 
		AosDataRecordObj *record, 
		AosValueRslt &value)
{
	value.setNullValue();

	// Check whether it is filtered.
	AosValueRslt vv;
	if (mFilterRaw)
	{
		bool rslt = mFilterRaw->getValue(rdata, record, vv);
		if (!rslt) return false;
		if (!vv.getValueBool())
		{
			// It is filtered. 
			return true;
		}
	}

	// It composes the field values.
	u32 size = mFields.size();
	OmnString value_str;
	for(u32 i=0; i<size; i++)
	{
		bool rslt = record->getFieldValue(mFields[i].mFieldIdx, vv, false, rdata);
		if (!rslt) return false;

		if (vv.isNull())
		{
			if (mFields[i].mNullDefault.isNull()) 
			{
				// It is ignored.
				return true;
			}

			vv = mFields[i].mNullDefault;
		}

		if (mFields[i].mNeedTrim) vv.normalizeWhiteSpace();
		if (vv.isEmpty())
		{
			if (mFields[i].mEmptyDefault.isNull())
			{
				// Empty field is ignored.
				return true;
			}
			vv = mFields[i].mEmptyDefault;
		}

		OmnString ss = vv.getValueStr1();
		value_str << mFields[i].mSep << ss;
	}

	if (mRsltFilterRaw)
	{
		AosValueRslt v_rslt = value_str;
		bool rslt = mRsltFilterRaw->getValue(rdata, v_rslt, vv);
		if (!rslt) return false;
		if (!vv.getValueBool()) 
		{
			// Filtered
			return true;
		}
	}

	value = value_str;
	return true;
}
#endif
