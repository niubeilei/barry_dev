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
// 10/22/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/Objid.h"

#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Database/DbRecord.h"
#include "MultiLang/LangDictMgr.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Rundata/Rundata.h"
#include "Security/Ptrs.h"
#include "Security/Session.h"
#include "Security/SessionMgr.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/MetaExtension.h"
#include "SEUtil/SeTypes.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/WebRequest.h"

int AosObjid::smMaxObjidLen = AosObjid::eMaxObjidLen;

// Chen Ding, 2013/06/11
static u8 sgCharMap[128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// [0, 9]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// [10, 19]
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		// [20, 29]
	0, 0, 1, 0, 0, 1, 1, 1, 0, 0,		// [30, 39]	'!(33)', '"(34)', '&(38)', ''(39)'
	1, 1, 1, 1, 0, 1, 1, 1, 1, 1,		// [40, 49] ',(44)'
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		// [50, 59]
	0, 1, 0, 0, 1, 1, 1, 1, 1, 1,		// [60, 69] '<(60)', '>(62)', '?(63)'
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		// [70, 79]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		// [80, 89]
	1, 1, 0, 1, 1, 1, 0, 1, 1, 1,		// [90, 99] '\(92)', '`(96)'
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		// [100, 109]
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,		// [110, 119]
	1, 1, 1, 1, 1, 1, 1, 0				// [120, 127]
};
static OmnMutex sgLock;
static bool sgInited = false;


bool
AosObjid::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	AosXmlTagPtr conf = OmnApp::getAppConfig();
	if (!conf)
	{
		sgInited = true;
		sgLock.unlock();
		return true;
	}

	AosXmlTagPtr tag = conf->getFirstChild("objid_mgr");
	if (!tag)
	{
		sgInited = true;
		sgLock.unlock();
		return true;
	}

	// Configure the max objid length
	smMaxObjidLen = tag->getAttrInt("max_objid_len", eMaxObjidLen);
	if (smMaxObjidLen < eMinObjidLen)
	{
		OmnAlarm << "Max objid length too small: " << tag->toString() << enderr;
		smMaxObjidLen = eMaxObjidLen;
	}

	// Configure the objid charset:
	// 	<objid_mgr max_objid_len="xxx">
	// 		<objid_charset>ddd,ddd,...</objid_charset>
	// 	</objid_mgr>
	AosXmlTagPtr map = tag->getFirstChild("objid_charset");
	if (map)
	{
		OmnString str = map->getNodeText();
		if (str != "")
		{
			vector<OmnString> values;
			AosSplitStr(str, ", ", values, 128);
			memset(sgCharMap, 1, sizeof(sgCharMap));

			for (u32 i=0; i<values.size(); i++)
			{
				int vv = atoi(values[i].data());
				if (vv >= 0 && vv < 128)
				{
					sgCharMap[vv] = 0;
				}
			}
		}
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


OmnString
AosObjid::createNewObjid(
		const AosXmlTagPtr &objtemp,
		const OmnString &dft,
		const u64 &docid,
		bool &changed,
		const AosRundataPtr &rdata)
{
	// This function creates a new objid based on the template
	// 'objtemp', which is in the following format:
	//		<template>
	//			<entry type="1|2|...">xxx</entry>
	//			<entry type="1|2|...">xxx</entry>
	//			...
	//		</template>
	// If 'objtemp' is empty, it will return 'objid_' + docid.
	if (!objtemp)
	{
		OmnString objid = "objid_";
		objid << docid;
		return objid;
	}

	changed = false;
	OmnString objid;
	AosXmlTagPtr entry = objtemp->getFirstChild();
	while (entry)
	{
		OmnString type = entry->getAttrStr("type");
		if (type == AOSOBJTEMPTYPE_CONSTANT)
		{
			objid << entry->getNodeText();
		}
		else if (type == AOSOBJTEMPTYPE_SEQNO)
		{
			// It is a sequence number
			OmnString seqno = entry->getNodeText();
			objid << seqno;
			seqno.increment();
			entry->setText(seqno, false);
			changed = true;
		}
		else if (type == AOSOBJTEMPTYPE_YEAR)
		{
			objid << AosGetSystemYear();
		}
		else if (type == AOSOBJTEMPTYPE_MONTH)
		{
			objid << AosGetSystemMonth();
		}
		else if (type == AOSOBJTEMPTYPE_DAYOFMONTH)
		{
			objid << AosGetSystemDayOfMonth();
		}
		else if (type == AOSOBJTEMPTYPE_DAY)
		{
			objid << AosGetSystemDayOfMonth();
		}
		else
		{
			objid << "Unknown";
			OmnAlarm << "Unknown type: " << type << ". Data: "
				<< objtemp->getData() << enderr;
			return "";
		}

		entry = objtemp->getNextChild();
	}

	bool rslt = isValidObjid(objid, rdata);
	aos_assert_r(rslt, NULL);
	return objid;
}


bool
AosObjid::procObjid(
		OmnString &objid,
		const OmnString &cid,
		OmnString &errmsg)
{
	// It checks whether the objid is in the form:
	// 		xxx.$yyy
	// If yes, '$yyy' may be replaced based on the contents of '$yyy':
	// 		private		cid from session
	int len = objid.length();
	if (len <= 0)
	{
		errmsg = "Objid is empty!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	aos_assert_r(len > 0, false);
	const char *data = objid.data();

	int idx = len-1;
	while (idx > 0 && data[idx] != AosMetaExtension::eIdChar) idx--;

	if (idx <= 0 || idx == len-1 || data[idx-1] != AosMetaExtension::eSepChar) return true;

	// It is in the form:
	// 		xxx.$yyy
	AosMetaExtension::E ext = AosMetaExtension::toEnum(&data[idx+1]);
	switch (ext)
	{
	case AosMetaExtension::eCloudid:
		 if (cid == "")
		 {
			 errmsg = "Missing cloud id: ";
			 errmsg << objid;
			 OmnAlarm << errmsg << enderr;
			 return false;
		 }

		 objid.setLength(idx);
		 objid << cid;
		 return true;

	default:
		 break;
	}

	// Meta extension not recognized. Currently, not treated as errors.
	// It is simply ignored.
	return true;
}


bool
AosObjid::separateObjidTmpl(
		const OmnString &objid,
		OmnString &real_objid,
		OmnString &template_objid,
		const OmnString &cid,
		OmnString &errmsg)
{
	// 'objid' should be in the form:
	// 		<real_objid>.${<template_prefix>:template_objid}
	// This function extracts the real_objid and template_objid.
	int len = objid.length();
	aos_assert_r(len > 0, false);
	const char *data = objid.data();

	// 1. Find the first '.' backward
	int idx = len-1;
	bool found = false;
	while (idx > 2)
	{
		if (data[idx] == '{' &&
		    data[idx-1] == AosMetaExtension::eIdChar &&
			data[idx-2] == AosMetaExtension::eSepChar)
		{
			found = true;
			break;
		}

		idx--;
	}

	if (!found)
	{
		errmsg = "Objid syntax error";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosMetaExtension::E type = AosMetaExtension::toEnum(&data[idx+1]);
	aos_assert_r(type > AosMetaExtension::eInvalid && type < AosMetaExtension::eMax, false);

	if (type != AosMetaExtension::eCreateByTemplate)
	{
		errmsg = "Internal error";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// It is eCreateByTemplate. Now 'idx' points to '$'.
	// 'objid' should end with '}'. If not, it is an error.
	aos_assert_r(data[len-1] == '}', false);
	int startidx = idx+AosMetaExtension::getTemplatePrefixLen()+1;
	int objidlen = len - startidx - 1;
	template_objid.assign(&data[startidx], objidlen);
	real_objid.assign(data, idx-2);
	if (cid != "") procObjid(real_objid, cid, errmsg);
	return true;	
}


bool
AosObjid::isValidObjid(const OmnString &objid, const AosRundataPtr &rdata)
{
	// Objid should be no longer than smMaxObjidLen
	// and should not contain the following characters:
	// 	new line,
	// 	<
	// 	>
	//
	// Chen Ding, 2013/06/11
	// Objids should satisfy the following:
	// 	1. Length: [1, smMaxObjidLen]
	// 	2. Any character in it should:
	// 		a. [32, 126] but the following are not allowed:
	// 			'!', '"', '&', ''', ',', '<', '>', '?', '\',
	//			'`', 'DEL (127)'.
	//		b. [128, 255]
	const int len = objid.length();
	if (objid == "")
	{
		AosSetErrorUser(rdata, "objid_empty") << enderr;
		return false;
	}
	
	if (len > smMaxObjidLen)
	{
		AosSetError(rdata, "objid_too_long") << ": " << objid;
		return false;
	}

	const u8 *data = (const u8 *)objid.data();
	u8 c;
	for (int i=0; i<len; i++)
	{
		c = data[i];
		if (c < 128 && !sgCharMap[c])
		{
			AosSetErrorUser(rdata, "objid_invalid") << ": " << objid;
			return false;
		}
	}

	rdata->setOk();
	return true;
}


bool
AosObjid::checkObjid(
		const OmnString &objid,
		AosObjidType::E &objid_type,
		const AosRundataPtr &rdata)
{
	// 1. Objids should not be too long.
	// 2. Objids should contain only the characters defined in the map
	// 3. If an objid contains dots, if the length of the substr that
	//    follows the last dot is no less than the cid minimum length,
	//    it must be a cid. Otherwise, the dot is changed to '_'.
	objid_type = AosObjidType::eInvalid;
	int len = objid.length();
	if (len <= 0)
	{
		AosSetErrorUser(rdata, "objid_is_empty") << enderr;
		return false;
	}

	if (len > smMaxObjidLen)
	{
		AosSetErrorUser(rdata, "objid_too_long")
			<< smMaxObjidLen << ". " << objid << enderr;
		return false;
	}

	const char *data = objid.data();
	if (data[0] == '_' && data[1] == 'z' && data[2] == 'y')
	{
		objid_type = AosObjidType::toEnum(objid);
	}
	else
	{
		objid_type = AosObjidType::eNormal;
	}
	return true;
}


bool
AosObjid::addNumToObjid(
		OmnString &objid,
		const AosRundataPtr &rdata)
{
	if (!sgInited) init();

	aos_assert_rr(objid != "", rdata, false);
	OmnString prefix, cid;
	AosObjid::decomposeObjid(objid, prefix, cid);

	if (cid == "")
	{
		objid = prefix;
		objid << "(1)";
	}
	else
	{
		objid = AosObjid::compose(prefix, 1, cid);
	}
	return true;
}


OmnString
AosObjid::getObjidByJQLDocName(
		const JQLTypes::DocType type,
		const OmnString name)
{
	aos_assert_r(name != "", "");

	OmnString objid = "";
	switch (type)
	{
	case JQLTypes::eTableDoc:
		 objid << AOSZTG_JQL_TABLENAME_IIL << "_" << name;
		 break;

	case JQLTypes::eIndexDoc:
		 objid << AOSZTG_INDEXMGR_IIL << "_" << name;
		 break;

	case JQLTypes::eSchemaDoc:
		 objid << AOSZTG_SCHEMA_IIL << "_" << name;
		 break;

	case JQLTypes::eDatasetDoc:
		 objid << AOSZTG_DATASET_IIL << "_" << name;
		 break;

	case JQLTypes::eJobDoc:
		 objid << AOSZTG_JOB_IIL << "_" << name;
		 break;

	case JQLTypes::eTaskDoc:
		 objid << AOSZTG_TASK_IIL << "_" << name;
		 break;

	case JQLTypes::eDataProcDoc:
		 objid << AOSZTG_DATA_PROC_IIL << "_" << name;
		 break;

	case JQLTypes::eDataScannerDoc:
		 objid << AOSZTG_DATA_SCANNER_IIL << "_" << name;
		 break;

	case JQLTypes::eDataConnectorDoc:
		 objid << AOSZTG_DATA_CONNECTOR_IIL << "_" << name;
		 break;

	case JQLTypes::eDataRecordDoc:
		 objid << AOSZTG_DATA_RECORD_IIL << "_" << name;
		 break;
	
	case JQLTypes::eDataFieldDoc:
		 objid << AOSZTG_DATA_FIELD_IIL << "_" << name;
		 break;

	case JQLTypes::eDatabaseDoc:
		 objid << AOSZTG_DATABASE_IIL << "_" << name;
		 break;

	case JQLTypes::eMap:
		 objid << AOSZTG_MAP_IIL << "_" << name;
		 break;

	case JQLTypes::eVirtualField:
		 objid << AOSZTG_VIRTUAL_FIELD_IIL << "_" << name;
		 break;

	case JQLTypes::eServiceDoc:
		 objid << AOSZTG_SERVICE_IIL << "_" << name;
		 break;

	case JQLTypes::eStatisticsDoc:
		 objid << AOSZTG_STATIIL << "_" << name;
		 break;
	
	case JQLTypes::eSequenceDoc:
		 objid << AOSZTG_SEQUENCE_DOC << "_" << name;
		 break;

	case JQLTypes::eJimoLogicDoc:
		 objid << AOSZTG_JIMOLOGIC_DOC << "_" << name;
		 break;

	case JQLTypes::eDict:
		 objid << AOSZTG_DICT_DOC << "_" << name;
		 break;

	default:
		 objid = "";
		 break;
	}

	//eDocInvalid,
	//eDocMax
	aos_assert_r(objid != "", "");
	return objid;
}


OmnString
AosObjid::getObjidPrefixByJQLDocType(const JQLTypes::DocType type)
{
	OmnString objid = "";
	switch (type)
	{
	case JQLTypes::eTableDoc: objid = AOSZTG_JQL_TABLENAME_IIL; break;
	case JQLTypes::eIndexDoc: objid = AOSZTG_INDEXMGR_IIL; break;
	case JQLTypes::eSchemaDoc: objid = AOSZTG_SCHEMA_IIL; break;
	case JQLTypes::eDatasetDoc: objid = AOSZTG_DATASET_IIL; break;
	case JQLTypes::eJobDoc: objid = AOSZTG_JOB_IIL; break;
	case JQLTypes::eTaskDoc: objid = AOSZTG_TASK_IIL; break;
	case JQLTypes::eDataProcDoc: objid = AOSZTG_DATA_PROC_IIL; break;
	case JQLTypes::eDataScannerDoc: objid = AOSZTG_DATA_SCANNER_IIL; break;
	case JQLTypes::eDataConnectorDoc: objid = AOSZTG_DATA_CONNECTOR_IIL; break;
	case JQLTypes::eDataRecordDoc: objid = AOSZTG_DATA_RECORD_IIL; break;
	case JQLTypes::eDataFieldDoc: objid = AOSZTG_DATA_FIELD_IIL; break;
	case JQLTypes::eDatabaseDoc: objid = AOSZTG_DATABASE_IIL; break;
	case JQLTypes::eMap: objid = AOSZTG_MAP_IIL; break;
	case JQLTypes::eVirtualField: objid = AOSZTG_VIRTUAL_FIELD_IIL; break;
	case JQLTypes::eServiceDoc: objid = AOSZTG_SERVICE_IIL; break;
	case JQLTypes::eStatisticsDoc: objid = AOSZTG_STATIIL; break;
	case JQLTypes::eSequenceDoc: objid = AOSZTG_SEQUENCE_DOC; break;
	case JQLTypes::eDict: objid = AOSZTG_DICT_DOC; break;
	default: break;
	}

	aos_assert_r(objid != "", "");
	return objid;
}

