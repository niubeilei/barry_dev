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
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IndexMgr/IndexMgr.h"

#include "API/AosApi.h"
#include "DocTrans/IndexMgrModifyDocTrans.h"
#include "IndexMgr/Ptrs.h"
#include "IndexMgr/ParalIIL.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "SEInterfaces/QueryTermObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEUtil/IILName.h"
#include "WordMgr/WordIdHash.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosIndexMgr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosIndexMgr(version);
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



AosIndexMgr::AosIndexMgr(const int version)
:
AosIndexMgrObj(version),
mLock(OmnNew OmnMutex()),
mCheckFreq(eDftCheckFreq)
{
}


AosIndexMgr::~AosIndexMgr()
{
}


bool
AosIndexMgr::createEntry(
		const AosRundataPtr &rdata, 
		const OmnString iilname, 
		const OmnString &table_name, 
		const OmnString &field_name, 
		const OmnString &index_type)
{
	OmnString docstr = "<index ";
	docstr 
		<< AOSTAG_IILNAME << "=\"" << iilname << "\" "
		<< AOSTAG_TABLENAME << "=\"" << table_name << "\" "
		<< AOSTAG_FIELD_NAME << "=\"" << field_name << "\" "
		<< AOSTAG_INDEX_TYPE << "=\"" << index_type << "\"/>";
	AosXmlTagPtr doc = AosStr2Xml(rdata, docstr  AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);
	return createEntry(rdata, doc);
}


bool 
AosIndexMgr::createEntry(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// Index Definition Docs are indexed by full field names and by IIL names.
	// This function creates the doc. After that, it tries to add the 
	// full field name to the full field name IIL and the iil name 
	// to the iil name IIL. If any of them fails, it means either the
	// full field name is not unique or the iil name is not unique.
	// It will fail the operation.
	//
	// Check whether it is already there
	OmnString field_name = def->getAttrStr(AOSTAG_FIELD_NAME);
	OmnString table_name = def->getAttrStr(AOSTAG_TABLENAME);
	OmnString iilname = def->getAttrStr(AOSTAG_IILNAME);
	OmnString index_type = def->getAttrStr(AOSTAG_INDEX_TYPE);
	if (iilname == "")
	{
		AosSetErrorUser(rdata, "indexmgr_missing_iilname") << def << enderr;
		return false;
	}

	if (field_name == "")
	{
		AosSetErrorUser(rdata, "indexmgr_missing_field_name") << def << enderr;
		return false;
	}

	if (index_type == "")
	{
		AosSetErrorUser(rdata, "indexmgr_missing_index_type") << def << enderr;
		return false;
	}

	if (index_type != AOSINDEXTYPE_NORM && index_type != AOSINDEXTYPE_PARAL_IIL)
	{
		AosSetErrorUser(rdata, "indexmgr_invalid_index_type") << def << enderr;
		return false;
	}

	OmnString key = getKey(table_name, field_name);
	mLock->lock();
	keyitr_t itr = mKey2IILNameMap.find(key);
	if (itr != mKey2IILNameMap.end())
	{
		// It already exists. 
		mLock->unlock();
		AosSetErrorUser(rdata, "indexmgr_index_already_defined") << enderr;
		logIndexAlreadyExist(rdata, field_name, iilname);
		return true;
	}
	mLock->unlock();

	// It does not exist
	def->setAttr(AOSTAG_PARENTC, AOSCTNR_INDEX_CONTAINER);
	def->setAttr(AOSTAG_OTYPE, AOSOTYPE_INDEX_DEF_DOC);
	def->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	AosXmlTagPtr doc = AosCreateDoc(def, rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata, "indexmgr_failed_creating_index_doc") << enderr;
		logIndexAlreadyExist(rdata, field_name, iilname);
		return true;
	}

	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid > 0, false);

	// Add full field name index entry
	if (!addFieldnameIndexEntry(rdata, table_name, field_name, docid))
	{
		AosSetErrorUser(rdata, "indexmgr_fieldname_not_unique") << enderr;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		AosDeleteDocByDocid(docid, rdata);
		return false;
	}

	// Add IIL name index entry
	if (!addIILNameIndexEntry(rdata, iilname, docid))
	{
		AosSetErrorUser(rdata, "indexmgr_iilname_not_unique") << enderr;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		removeFieldnameIndexEntry(rdata, table_name, field_name, docid);
		AosDeleteDocByDocid(docid, rdata);
		return false;
	}

	return true;
}


AosXmlTagPtr
AosIndexMgr::getIndexDefDoc(
		const AosRundataPtr &rdata,
		const OmnString &table_name, 
		const OmnString &field_name)
{
	// 'full_fieldname' may contain table name.
	OmnString key = getKey(table_name, field_name);

	u64 docid = 0;
	OmnString iilname;
	mLock->lock();
	keyitr_t itr = mKey2IILNameMap.find(key);
	if (itr == mKey2IILNameMap.end())
	{
		// Did not find it.
		mLock->unlock();
		OmnString name = AosIILName::composeIndexMgrIILName();                        
		aos_assert_rr(name != "", rdata, 0);
		bool rslt = AosIILClientObj::getDocidStatic(rdata, name, key, docid);
		aos_assert_rr(rslt, rdata, 0);
		if (docid == 0)
		{
			// There is no index doc for [tablename, fieldname].
			return 0;
		}

		AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
		aos_assert_rr(doc, rdata, 0);
		iilname = doc->getAttrStr(AOSTAG_IILNAME);
		aos_assert_rr(iilname != "", rdata, 0);

		mLock->lock();
		mKey2IILNameMap[key] = iilname;
		mIILName2DocidMap[iilname] = docid;
		mDocid2DocMap[docid] = doc;
		mLock->unlock();
		return doc;
	}
	
	iilname = itr->second;
	mLock->unlock();
	return getIndexDefDoc(rdata, iilname);
}


AosXmlTagPtr
AosIndexMgr::getIndexDefDoc(
		const AosRundataPtr &rdata,
		const OmnString &iilname)
{
	// 'full_fieldname' may contain table name.
	u64 docid = 0;
	mLock->lock();
	dociditr_t itr = mIILName2DocidMap.find(iilname);
	if (itr == mIILName2DocidMap.end())
	{
		// Does not exist. 
		mLock->unlock();
		OmnString name = AosIILName::composeIILNameIndexIILName();                        
		aos_assert_rr(name != "", rdata, 0);
		bool rslt = AosIILClientObj::getDocidStatic(rdata, name, iilname, docid);
		if (!rslt)
		{
			rslt = AosIILClientObj::getDocidStatic(rdata, name, iilname, docid);
		}
		aos_assert_rr(rslt, rdata, 0);
		if (docid == 0)
		{
			return 0;
		}

		mLock->lock();
		mIILName2DocidMap[iilname] = docid;
	}
	else
	{
		docid = itr->second;
	}

	// It is local.
	AosXmlTagPtr doc = getIndexDefDocLocked(rdata, docid);
	mLock->unlock();
	aos_assert_rr(doc, rdata, 0);
	return doc;
}


bool 
AosIndexMgr::removeEntry(
		const AosRundataPtr &rdata, 
		const OmnString &table_name,
		const OmnString &field_name)
{
	AosXmlTagPtr doc = getIndexDefDoc(rdata, table_name, field_name);
	if (!doc)
	{
		AosSetErrorUser(rdata, "indexmgr_failed_get_doc")
			<< "Table name: " << table_name
			<< "Field name: " << field_name << enderr;
		return false;
	}

	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	if (docid == 0)
	{
		AosSetErrorUser(rdata, "indexmgr_faild_get_doc") << enderr;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}
//	if (!isLocal(docid)) return removeEntryRemote(rdata, docid, table_name, field_name);

	// It is local.
	mLock->lock();
	OmnString iilname = doc->getAttrStr(AOSTAG_IILNAME);
	if (iilname == "")
	{
		mLock->unlock();
		AosSetErrorUser(rdata, "indexmgr_iilname_empty")
			<< "Table name: " << table_name
			<< "Field name: " << field_name << enderr;
		return false;
	}

	OmnString key = getKey(table_name, field_name);
	mKey2IILNameMap.erase(key);
	mIILName2DocidMap.erase(iilname);
	mDocid2DocMap.erase(docid);
	mLock->unlock();

	bool rslt = AosDocClientObj::removeDocByDocidStatic(rdata, docid);
	aos_assert_rr(rslt, rdata, false);

	removeFieldnameIndexEntry(rdata, table_name, field_name, docid);
	removeIILNameIndexEntry(rdata, iilname, docid);
	return true;
}


bool
AosIndexMgr::resolveIndex(
		const AosRundataPtr &rdata, 
		const OmnString &table_name,
		const OmnString &field_name, 
		OmnString &iilname)
{
	// 1. Check whether it is defined in the map.
	AosXmlTagPtr doc = getIndexDefDoc(rdata, table_name, field_name);
	if (!doc)
	{
		// There is no definition for it.
		return getDefaultIILName(rdata, table_name, field_name, iilname);
	}

	iilname = doc->getAttrStr(AOSTAG_IILNAME);
	aos_assert_rr(iilname != "", rdata, false);
	return true;
}


bool 
AosIndexMgr::modifyEntry(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// Check whether it is already there
	OmnString field_name = def->getAttrStr(AOSTAG_FIELD_NAME, "");
	OmnString table_name = def->getAttrStr(AOSTAG_TABLENAME, "");
	OmnString iil_name = def->getAttrStr(AOSTAG_IILNAME, "");
	aos_assert_rr(iil_name != "", rdata, false);
	aos_assert_rr(field_name != "" || table_name != "", rdata, false);
	
	mLock->lock();
	u64 docid = 0;
	dociditr_t itr = mIILName2DocidMap.find(iil_name);
	if (itr == mIILName2DocidMap.end())
	{
		// Does not exist.
		mLock->unlock();
		docid =	getIILNameIndexEntry(rdata, iil_name);

		mLock->lock();

		if (docid == 0)
		{
			mLock->unlock();
			return createEntry(rdata, def);
		}
		mIILName2DocidMap[iil_name] = docid;
	}
	else
	{
		docid = itr->second;
	}
/*	
	if (!isLocal(docid)) 
	{
		mLock->unlock();
		return modifyEntryRemote(rdata, docid, def);
	}
*/	
	AosXmlTagPtr doc = getIndexDefDocLocked(rdata, docid);
	aos_assert_rl(doc, mLock, false);

	u64 old_docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rl(old_docid > 0, mLock, false);

	docid = def->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rl(docid > 0, mLock, false);

	if (old_docid != docid)
	{
		mLock->unlock();
		AosSetErrorUser(rdata, "indexmgr_index_already_defined") << enderr;
		rdata->setReturnCode(AosReturnCode::eInternalError);
		return false;
	}

	OmnString old_field_name = doc->getAttrStr(AOSTAG_FIELD_NAME, "");
	OmnString old_table_name = doc->getAttrStr(AOSTAG_TABLENAME, "");
	OmnString old_iil_name = doc->getAttrStr(AOSTAG_IILNAME, "");
	aos_assert_rr(old_field_name != "", rdata, false);
	aos_assert_rr(old_table_name != "", rdata, false);
	aos_assert_rr(old_iil_name != "", rdata, false);

	// If either the tablename or field name are modified, it needs
	// to remove the index entry and add a new index entry.
	if (old_field_name != field_name || old_table_name != table_name)
	{
		// Note that if more than one user is doing this, the following
		// operations may fail. Since this happens very rarely, it is
		// treated as a KNOWN BUG!!!!!!!!!!!!!!!
		removeFieldnameIndexEntry(rdata, old_table_name, old_field_name, docid);
		addFieldnameIndexEntry(rdata, table_name, field_name, docid);
	}

	// If the iilname changed, it needs to modify the IIL index entry
	if (old_iil_name != iil_name)
	{
		// Note that if more than one user is doing this, the following
		// operations may fail. Since this happens very rarely, it is
		// treated as a KNOWN BUG!!!!!!!!!!!!!!!
		removeIILNameIndexEntry(rdata, old_iil_name, docid);
		addIILNameIndexEntry(rdata, iil_name, docid);
	}

	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_rl(doc_client, mLock, 0);

	mLock->unlock();
	bool rslt = doc_client->modifyObj(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


u64
AosIndexMgr::getIILNameIndexEntry(
		const AosRundataPtr &rdata, 
		const OmnString &iilname)
{
	OmnString name = AosIILName::composeIILNameIndexIILName();                        
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, 0);
	bool isunique;
	u64 docid;
	bool rslt =	iilclient->getDocid(name, iilname, eAosOpr_eq, false, docid, isunique, rdata);
	aos_assert_rr(rslt, rdata, 0);
	aos_assert_rr(isunique, rdata, 0);
	return docid;
}


bool
AosIndexMgr::addFieldnameIndexEntry(
		const AosRundataPtr &rdata, 
		const OmnString &table_name, 
		const OmnString &field_name, 
		const u64 docid)
{
	OmnString key = getKey(table_name, field_name);
	OmnString iilname = AosIILName::composeIndexMgrIILName();                        
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, 0);
	return iilclient->addStrValueDoc(iilname, key, docid, true, true, rdata);
}


bool
AosIndexMgr::addIILNameIndexEntry(
		const AosRundataPtr &rdata, 
		const OmnString &iil_name, 
		const u64 docid)
{
	OmnString name = AosIILName::composeIILNameIndexIILName();                        
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, 0);
	return iilclient->addStrValueDoc(name, iil_name, docid, true, true, rdata);
}


bool
AosIndexMgr::isGlobalFieldname(
		const OmnString &fieldname, 
		OmnString &new_name)
{
	// Global names are denoted as "::addr"
	if (fieldname.length() < 3) return false;
	if (fieldname.data()[0] == ':' && fieldname.data()[1] == ':') 
	{
		const char *data = fieldname.data();
		int len = fieldname.length() - 2;
		new_name.assign(&data[2], len);
		return true;
	}

	return false;
}


bool
AosIndexMgr::getDefaultIILName(
		const AosRundataPtr &rdata, 
		const OmnString &table_name, 
		const OmnString &field_name,
		OmnString &iilname)
{
	OmnString attr_name = "";
	if (isGlobalFieldname(field_name, attr_name))
	{
		aos_assert_r(attr_name != "", false);
		iilname = AosIILName::composeAttrIILName(attr_name);
		return true;
	}

	iilname = AosIILName::composeContainerAttrIILName(table_name, field_name);
	return true;
}


bool
AosIndexMgr::logIndexAlreadyExist(
		const AosRundataPtr &rdata, 
		const OmnString &field_name, 
		const OmnString &iilname)
{
	/* ??????
	AosDatalet datalet;

	datalet.addCreateTime();
	datalet << "log_name" << AOSLOGNAME_INDEX_ALREADY_EXIST
		<< "field_name" << field_name
		<< "iilname" << iilname
		<< "requester" << rdata->getUserId();
	AosAddSysLog(rdata, datalet);
	*/
	return true;
}


AosParalIILPtr
AosIndexMgr::getParalIIL(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &index_doc)
{
	try
	{
		AosParalIILPtr iil = OmnNew AosParalIIL();
		if (!iil->config(rdata, index_doc)) 
		{
			AosSetError(rdata, "indexmgr_failed_config") 
				<< index_doc << enderr;
			return 0;
		}
		return iil;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosIndexMgr::removeFieldnameIndexEntry(
		const AosRundataPtr &rdata, 
		const OmnString &table_name, 
		const OmnString &field_name, 
		const u64 docid)
{
	OmnString key = getKey(table_name, field_name);
	OmnString index_iilname = AosIILName::composeIndexMgrIILName();                        
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, 0);
	return iilclient->removeValueDoc(index_iilname, key, docid, rdata);
}


bool
AosIndexMgr::removeIILNameIndexEntry(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const u64 docid)
{
	OmnString name = AosIILName::composeIILNameIndexIILName();                        
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, 0);
	return iilclient->removeValueDoc(name, iilname, docid, rdata);
}


OmnString
AosIndexMgr::getKey(const OmnString &tablename, const OmnString &fieldname)
{
	if (tablename == "") return fieldname;
	OmnString key = tablename;
	key << "." << fieldname;
	return key;
}


bool 
AosIndexMgr::getIILNames(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query_req, 
		const OmnString &iilname, 
		vector<OmnString> &iilnames)
{
	AosXmlTagPtr index_doc = getIndexDefDoc(rdata, iilname);
	if (!index_doc)
	{
		AosSetErrorUser(rdata, "indexmgr_index_not_defined") << iilname << enderr;
		return false;
	}

	OmnString index_type = index_doc->getAttrStr(AOSTAG_INDEX_TYPE);
	if (index_type == AOSINDEXTYPE_NORM)
	{
		iilnames.clear();
		iilnames.push_back(index_doc->getAttrStr(AOSTAG_IILNAME));
		return true;
	}

	if (index_type == AOSINDEXTYPE_PARAL_IIL)
	{
		AosParalIILPtr iil = getParalIIL(rdata, index_doc);
		if (!iil)
		{
			AosSetErrorUser(rdata, "indexmgr_iil_not_found") << iilname << enderr;
			return false;
		}

		return iil->getIILNames(rdata, query_req, iilnames);
	}

	AosSetError(rdata, "indexmgr_invalid_index_type") << index_doc << enderr;
	return false;
}


bool 
AosIndexMgr::isParalIIL(
		const AosRundataPtr &rdata, 
		const OmnString &iilname)
{
	AosXmlTagPtr index_doc = getIndexDefDoc(rdata, iilname);
	if (!index_doc) return false;
	OmnString type = index_doc->getAttrStr(AOSTAG_INDEX_TYPE);
	return (type == AOSINDEXTYPE_PARAL_IIL);
}


AosJimoPtr
AosIndexMgr::cloneJimo() const
{
	return OmnNew AosIndexMgr(*this);
}


bool 
AosIndexMgr::convertIILName(
		const AosRundataPtr &rdata, 
		const int epoch_day, 
		const OmnString &iilname, 
		bool &converted,
		OmnString &new_iilname)
{
	// If it is not parallel IIL, nothing will be done.
	// Otherwise, it maps [iilname+epoch_day] to a new
	// IIL name. 
	mLock->lock();
	bitr_t iil_itr = mIILStatusMap.find(iilname);
	if (iil_itr != mIILStatusMap.end())
	{
		Status iil_type = iil_itr->second;
		switch (iil_type)
		{
		case eNormIIL:
			 converted = false;
			 mLock->unlock();
			 return true;

		case eParalIIL:
			 {
			 	 // It is a parallel IIL.
			 	 OmnString key = iilname;
			 	 key << "_" << epoch_day;
			 	 keyitr_t itr = mIILNameMap.find(key);
			 	 if (itr != mIILNameMap.end())
			 	 {
				 	 new_iilname = itr->second;
				 	 mLock->unlock();
				 	 converted = true;
				 	 return true;
				 }
			 }
			 break;

		case eInvalidParalIIL:
			 // It is a parallel IIL but it is incorrect.
			 converted = false;
			 mLock->unlock();
			 return true;

		default:
			 AosSetError(rdata, "internal_error") << enderr;
			 mLock->unlock();
			 converted = false;
			 return false;
		}
	}

	mLock->unlock();
	AosXmlTagPtr index_doc = getIndexDefDoc(rdata, iilname);
	mLock->lock();
	if (!index_doc)
	{
		// It is a normal IIL. 
		mIILStatusMap[iilname] = eNormIIL;
		converted = false;
		mLock->unlock();
		return true;
	}

	OmnString index_type = index_doc->getAttrStr(AOSTAG_INDEX_TYPE);
	if (index_type == AOSINDEXTYPE_NORM)
	{
		converted = false;
		mIILStatusMap[iilname] = eNormIIL;
		mLock->unlock();
		return true;
	}

	if (index_type == AOSINDEXTYPE_PARAL_IIL)
	{
		AosParalIILPtr iil;
		iilitr_t itr = mParalIILMap.find(iilname);
		if (itr == mParalIILMap.end())
		{
			AosXmlTagPtr doc = index_doc->clone(AosMemoryCheckerArgsBegin);
			iil = getParalIIL(rdata, doc);
			if (!iil)
			{
				mIILStatusMap[iilname] = eInvalidParalIIL;
				converted = false;
				mLock->unlock();
				AosSetEntityError(rdata, "indexmgr_paral_iil_not_defined", "Index", "")
					<< iilname << enderr;
				return false;
			}
			mParalIILMap[iilname] = iil;
		}
		else
		{
			iil = itr->second;
		}

		int level, period;
		bool need_create = false;
		if (!iil->getIILName(rdata, iilname, epoch_day, 
					new_iilname, level, period, need_create)) 
		{
			mIILStatusMap[iilname] = eInvalidParalIIL;
			converted = false;
			mLock->unlock();
			return false;
		}

		mIILStatusMap[iilname] = eParalIIL;
		OmnString key = iilname;
		key << "_" << epoch_day;
		mIILNameMap[key] = new_iilname;
		if (need_create)
		{
			aos_assert_rl(level>=0, mLock, false);
			mLock->unlock();
			bool rslt = addPeriod(rdata, index_doc->getAttrU64(AOSTAG_DOCID, 0), level, period); 
			converted = true;
			aos_assert_rr(rslt, rdata, false);
			return true;
		}
		converted = true;
		mLock->unlock();
		return true;
	}

	converted = false;
	mIILStatusMap[iilname] = eNormIIL;
	mLock->unlock();
	AosSetError(rdata, "indexmgr_internal_error") << enderr;
	return false;
}


AosXmlTagPtr
AosIndexMgr::getIndexDefDocLocked(
		const AosRundataPtr &rdata, 
		const u64 docid)
{
	// Index def docs are cached by 'docid'. This function
	// checks whether the doc is cached. If yes, it returns
	// the doc. Otheriwise, it retrieves the doc and adds
	// the doc to the cache.
	docitr_t itr = mDocid2DocMap.find(docid);
	if (itr != mDocid2DocMap.end())
	{
		// OmnScreen << "Get doc: " << docid << ", " << itr->second.getPtr() << endl;
		return itr->second;
	}

	mLock->unlock();
	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
	aos_assert_rr(doc, rdata, 0);
	mLock->lock();

	// check whether the doc has been retrieved by another
	// thread.
	itr = mDocid2DocMap.find(docid);
	if (itr == mDocid2DocMap.end())
	{
		mDocid2DocMap[docid] = doc;
		return doc;
	}

	return itr->second;
}


bool
AosIndexMgr::addPeriod(
		const AosRundataPtr &rdata, 
		const u64 docid,
		const int level, 
		const int period)
{
	// This function adds a period to the index doc 'docid'. 
	// Modifications to index docs are managed by the doc's
	// master only. Index doc masters are determined by docids.
	// If it is not local, it will send a request to the remote
	// server to do it. If it is local, it needs to lock
	// and process.
	aos_assert_rr(docid > 0, rdata, false);
	AosTransPtr trans = OmnNew AosIndexMgrModifyDocTrans(docid, level, period, false, true);	
	bool rslt = AosSendTrans(rdata, trans); 
	aos_assert_rr(rslt, rdata, false);
	rdata->setOk();
	return true;
}


bool 
AosIndexMgr::checkMerge()
{
	OmnNotImplementedYet;
	return false;
}

bool
AosIndexMgr::isLocal(const u64 docid)
{
	// NotImlementedYet;
	return true;
}


AosXmlTagPtr
AosIndexMgr::getIndexDefDocFromRemote(
		const AosRundataPtr &rdata, 
		const u64 docid,
		const OmnString &table_name, 
		const OmnString &field_name)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosIndexMgr::modifyEntryRemote(
		const AosRundataPtr &rdata, 
		const u64 docid, 
		const AosXmlTagPtr &doc)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIndexMgr::removeEntryRemote(
		const AosRundataPtr &rdata, 
		const u64 docid, 
		const OmnString &table_name, 
		const OmnString &field_name)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIndexMgr::addPeriodRemote(
		const AosRundataPtr &rdata, 
		const u64 docid, 
		const int level, 
		const int period)
{
	OmnNotImplementedYet;
	return false;
}
		

