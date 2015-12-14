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
// 	The table contains the following fields:
//  	tablename
//  	name
//  	tagging
//  	description
//  	key_fields
//  	tagging_fname
//  	name_fname
//		desc_fname
//		max_limit
//		no_delete
//		readonly
//		log
// 		stat 
// 		obj_listing
// 		indexed_fields
// 		translators
// 		archive
// 		create_time
// 		mod_time
// 		read_lock
// 		rl_user
// 		rl_time
// 		write_lock
// 		wl_user
// 		wl_time
// 		newobj_lock
// 		nol_user
// 		nol_time
//
// Modification History:
// 02/21/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataMgr/GDDef.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataMgr/GDObj.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "Util1/Wait.h"


const OmnString sgGDTableName = "group_obj_dm";
const OmnString sgInsertFields = 
        "tablename, name, tagging, description, key_fields, tagging_fname, "
		"name_fname, desc_fname, max_limit, no_delete, readonly, log, "
		"stat, obj_listing, indexed_fields, translators, archive, create_time, "
		"mod_time, read_lock, rl_user, rl_time, "
		"write_lock, wl_user, wl_time, "
		"newobj_lock, nol_user, nol_time) values ('";

AosGDDef::AosGDDef()
:
mMaxLimit(0)
{
}


AosGDDef::AosGDDef(const OmnString &tablename)
:
mTablename(tablename),
mMaxLimit(0)
{
}


AosGDDef::~AosGDDef()
{
}


bool
AosGDDef::getGDDef(const OmnString &tablename)
{
	// 
	// It retrieves the groupd data definition from the table.
	// 
	mTablename = tablename;
	return retrieveFromDb();
}


OmnRslt
AosGDDef::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
	int idx = 0;
    mTablename 	 	= record->getStr(idx++, "NoName", rslt);
    mName 			= record->getStr(idx++, "NoName", rslt);
	mTagging		= record->getStr(idx++, "", rslt);
	mDescription	= record->getStr(idx++, "", rslt);
	mKeyFields		= record->getStr(idx++, "", rslt);
	mTaggingFname	= record->getStr(idx++, "", rslt);
	mNameFname		= record->getStr(idx++, "", rslt);
	mDescFname		= record->getStr(idx++, "", rslt);
	mMaxLimit		= record->getInt(idx++, 0, rslt);
	mNoDelete		= record->getStr(idx++, "", rslt);
	mReadOnly		= record->getStr(idx++, "", rslt);
	mLog			= record->getStr(idx++, "", rslt);
	mStat			= record->getStr(idx++, "", rslt);
	mObjListing		= record->getStr(idx++, "", rslt);
	mIndexedFields  = record->getStr(idx++, "", rslt);
	mTranslators	= record->getStr(idx++, "", rslt);
	mArchive		= record->getStr(idx++, "", rslt);
	mCreateTime		= record->getStr(idx++, "", rslt);
	mModTime		= record->getStr(idx++, "", rslt);
	mReadLock		= record->getStr(idx++, "", rslt);
	mRLUser			= record->getStr(idx++, "", rslt);
	mRLTime			= record->getStr(idx++, "", rslt);
	mWriteLock		= record->getStr(idx++, "", rslt);
	mWLUser			= record->getStr(idx++, "", rslt);
	mWLTime			= record->getStr(idx++, "", rslt);
	mNewObjLock		= record->getStr(idx++, "", rslt);
	mNOLUser		= record->getStr(idx++, "", rslt);
	mNOLTime		= record->getStr(idx++, "", rslt);

	return rslt;
}


OmnString
AosGDDef::updateStmt() const
{
    OmnString stmt = "update ";
	stmt << sgGDTableName << " set "
		<< "name='" << mName
		<< "', tagging='" << mTagging
 		<< "', description='" <<  mDescription
 		<< "', key_fields='" << mKeyFields
		<< "', tagging_fname='" << mTaggingFname
		<< "', desc_fname='" << mDescFname
		<< "', name_fname='" << mNameFname
		<< "', max_limit=" << mMaxLimit
		<< ",  no_delete='" << mNoDelete
		<< "', readonly='" << mReadOnly
		<< "', log='" << mLog
		<< "', sta='" << mStat
		<< "', obj_listing='" << mObjListing
		<< "', indexed_fields='" << mIndexedFields
		<< "', translators='" << mTranslators
		<< "', archive='" << mArchive
		<< "', mod_time=now(), '" 
		<< "' where tablename='" << mTablename << "'";
	return stmt;
}


OmnString
AosGDDef::removeStmt() const
{
    OmnString stmt = "delete from ";
	stmt << sgGDTableName << " where tablename='" << mTablename << "'";
    return stmt;
}


OmnString
AosGDDef::removeAllStmt() const
{
    OmnString stmt = "delete from ";
	stmt << sgGDTableName;
    return stmt;
}


OmnString
AosGDDef::existStmt() const
{
    OmnString stmt = "select tablename from ";
	stmt << sgGDTableName << " where tablename='" << mTablename << "'";
    return stmt;
}


OmnString
AosGDDef::insertStmt() const
{
    OmnString stmt = "insert into ";
	stmt << sgGDTableName << " (" << sgInsertFields
    	<< mTablename << "', '"
    	<< mName << "', '"
		<< mTagging << "', '"
		<< mDescription << "', '"
        << mKeyFields << "', '"
		<< mTaggingFname << "', '"
		<< mNameFname << "', '"
		<< mDescFname << "',"
		<< mMaxLimit << ", '"
		<< mNoDelete << "', '"
		<< mReadOnly << "', '"
		<< mLog << "', '"
		<< mStat << "', '"
		<< mObjListing << "', '"
		<< mIndexedFields << "', '"
		<< mTranslators << "', '"
		<< mArchive << "', now(), now(), '"
		<< mReadLock << "', '"
		<< mRLUser << "', now(), '"
		<< mWriteLock << "', '"
		<< mWLUser << "', now(), '"
		<< mNewObjLock << "', '"
		<< mNOLUser << "', now())";

    return stmt;
}


OmnString
AosGDDef::retrieveStmt() const
{
    OmnString stmt = "select * from ";
	stmt << sgGDTableName << " where tablename='" << mTablename << "'";
	return stmt; 
}


bool
AosGDDef::lockForWriting(const OmnString &username, OmnString &errmsg)
{
	// IMPORTANT: it assumes the caller has just retrieved the
	// locking information from the database.
	if (mWriteLock == "locked" && mWLUser == username)
	{
		// Already locked by this user
		return true;
	}
	
	OmnString stmt = "update ";
	stmt << mTablename << " set write_lock='locked', wl_username="
			<< username << ", wl_time=now() where seqno=" << mSeqno
			<< " and write_lock=''";
		
	return lockGroup(stmt);
}


bool
AosGDDef::lockForReading(const OmnString &username, OmnString &errmsg)
{
	// IMPORTANT: it assumes the caller has just retrieved the 
	// locking information from the database. 
	if (mReadLock == "locked" && mRLUser == username)
	{
		// Already locked by this user. 
		return true;
	}

	OmnString stmt = "update ";
	stmt << mTablename << " set read_lock='locked', rl_username="
		<< username << ", rl_time=now() where seqno=" << mSeqno
		<< " and read_lock=''";

	return lockGroup(stmt);
}


bool
AosGDDef::lockForNewObj(const OmnString &username, OmnString &errmsg)
{
	// IMPORTANT: it assumes the caller has just retrieved the 
	// locking information from the database. 
	if (mNewObjLock == "locked" && mNOLUser == username)
	{
		// Already locked by this user. 
		return true;
	}

	OmnString stmt = "update ";
	stmt << mTablename << " set newobj_lock='locked', nol_username="
		<< username << ", nol_time=now() where seqno=" << mSeqno
		<< " and newobj_lock=''";

	return lockGroup(stmt);
}


bool
AosGDDef::unlockReading(const OmnString &username, OmnString &errmsg)
{
	// It assumes that the caller just read the locking information 
	// from database. If the lock was not locked at all, do nothing.
	// If the lock was locked by this user, unlock it. If the lock
	// was locked by someone else, it is an error.
	if (mReadLock == "") return true;

	if (mRLUser != username)
	{
		// The lock was locked by someone else
		errmsg = "Lock was locked by someone else: ";
		errmsg << mRLUser << ", locked at: " << mRLTime;
		return false;
	}

	OmnString stmt = "update ";
	stmt << mTablename << " set read_lock='' where seqno="
		<< mSeqno << " and read_lock='locked' and rl_user='"
		<< username << "'";
	return updateToDb(stmt);
}


bool
AosGDDef::unlockWriting(const OmnString &username, OmnString &errmsg)
{
	// It assumes that the caller just read the locking information 
	// from database. If the lock was not locked at all, do nothing.
	// If the lock was locked by this user, unlock it. If the lock
	// was locked by someone else, it is an error.
	if (mWriteLock == "") return true;

	if (mWLUser != username)
	{
		// The lock was locked by someone else
		errmsg = "Write lock was locked by someone else: ";
		errmsg << mWLUser << ", locked at: " << mWLTime;
		return false;
	}

	OmnString stmt = "update ";
	stmt << mTablename << " set write_lock='' where seqno="
		<< mSeqno << " and write_lock='locked' and wl_user='"
		<< username << "'";
	return updateToDb(stmt);
}


bool
AosGDDef::unlockNewObj(const OmnString &username, OmnString &errmsg)
{
	// It assumes that the caller just read the locking information 
	// from database. If the lock was not locked at all, do nothing.
	// If the lock was locked by this user, unlock it. If the lock
	// was locked by someone else, it is an error.
	if (mNewObjLock == "") return true;

	if (mNOLUser != username)
	{
		// The lock was locked by someone else
		errmsg = "New Object lock was locked by someone else: ";
		errmsg << mNOLUser << ", locked at: " << mNOLTime;
		return false;
	}

	OmnString stmt = "update ";
	stmt << mTablename << " set newobj_lock='' where seqno="
		<< mSeqno << " and newobj_lock='locked' and nol_user='"
		<< username << "'";
	return updateToDb(stmt);
}


bool
AosGDDef::isLockedForReading(const OmnString &username)
{
	return mReadLock == "locked" && mRLUser == username;
}


bool
AosGDDef::isLockedForWriting(const OmnString &username)
{
	return mWriteLock == "locked" && mWLUser == username;
}


bool
AosGDDef::isLockedForNewObj(const OmnString &username)
{
	return mNewObjLock == "locked" && mNOLUser == username;
}


bool
AosGDDef::modifyGroup(
		const OmnString &tablename,
		const OmnString &contents, 
		const bool unlock_write,
		OmnString &errmsg)
{
	// It constructs an instance of this class based on 'node', which 
	// should be in the form:
	// 	<xmlobj>
	// 		<mod fname="xxx" old="xxx" new="xxx"/>
	// 		<mod fname="xxx" old="xxx" new="xxx"/>
	// 		...
	// 	</xmlobj>
	// where 'fname' identifies the field to be modified, 'old' hold
	// the old contents based on which the changes were made, and 'new'
	// hold the modified contents for the field. 
	//
	// Important: if a field is modified, the modifications are based 
	// on the contents the user last retrieved it. It is possible that 
	// the contents may be modified by someone else. This function checks
	// whether it happens. If yes, it will indicate the error and the
	// operation is aborted. 
	//
	// The caller should have locked the record for writing. If 'unlock_write'
	// is true, it will unlock the record.
	
	if (!getGDDef(tablename))
	{
		errmsg = "Failed to retrieve the group definition: ";
		errmsg << tablename;
		return false;
	}	
	
    TiXmlDocument thedoc;
    thedoc.Parse(contents);
    TiXmlNode *root = thedoc.FirstChild();
    aos_assert_r(root, false);
	TiXmlNode *child = 0;
	bool is_first = true;
	OmnString stmt = "update ";
	stmt << sgGDTableName << " set ";
	while ((child = root->IterateChildren(child)))
	{
		TiXmlElement *elem = child->ToElement();
		if (!elem)
		{
			errmsg = "Failed to convert the node to element!";
			OmnAlarm << errmsg << ": " << contents << enderr;
			return false;
		}

		const char *fname = elem->Attribute("fname");
		const char *old_contents = elem->Attribute("old");
		const char *new_contents = elem->Attribute("new");

		if (!fname)
		{
			errmsg = "Failed to retrieve field name!";
			OmnAlarm << errmsg << ": " << contents << enderr;
			return false;
		}

		if (strcmp(fname, "tablename") == 0)
		{
			errmsg = "Tablename cannot be modified!";
			return false;
		}

		if (strcmp(fname, "write_lock") == 0 ||
			strcmp(fname, "wl_user") == 0)
		{
			// These are not allowed. Ignore.
			continue;
		}

		if (!old_contents)
		{
			errmsg = "Failed to retrieve the old contents!";
			OmnAlarm << errmsg << ": " << contents << enderr;
			return false;
		}

		if (!new_contents)
		{
			errmsg = "Failed to retrieve the new contents!";
			OmnAlarm << errmsg << ": " << contents << enderr;
			return false;
		}

		if (!checkValue(fname, old_contents))
		{
			errmsg = "The contents for: ";
			errmsg << fname << " were changed since the time the contents "
				<< "were last retrieved! You may reload the contents and "
				<< "modify the contents again!";
			return false;
		}

		if (!is_first) stmt << ",";
		is_first = false;
		if (strcmp(fname, "max_limit") == 0)
		{
			stmt << fname << "=" << new_contents;
		}
		else
		{
			stmt << fname << "='" << new_contents << "'";
		}
	}	

	if (is_first)
	{
		// This means it contains no modified fields
		errmsg = "The request does not contain any modified fields!";
		OmnAlarm << errmsg << " " << contents << enderr;
		return false;
	}

	if (unlock_write) stmt << ", write_lock='unlock', wl_user=''";

	stmt << " where tablename='" << tablename << "'";

	if (!updateToDb(stmt))
	{
		errmsg = "Failed to update the database!";
		return false;
	}

	return true;
}


bool
AosGDDef::checkValue(const OmnString &fname, const OmnString &value)
{
	// This function checks whether the field named 'fname' holds the same
	// value as 'value'. 
	if (fname == "tablename") 		return mTablename == value;
	if (fname == "name") 			return mName == value;
	if (fname == "tagging") 		return mTagging == value;
	if (fname == "description") 	return mDescription == value;
	if (fname == "key_fields") 		return mKeyFields == value;
	if (fname == "tagging_fname") 	return mTaggingFname == value;
	if (fname == "name_fname") 		return mNameFname == value;
	if (fname == "desc_fname") 		return mDescFname == value;
	if (fname == "no_delete") 		return mNoDelete == value;
	if (fname == "readonly") 		return mReadOnly == value;
	if (fname == "log") 			return mLog == value;
	if (fname == "stat ") 			return mStat == value;
	if (fname == "obj_listing") 	return mObjListing == value;
	if (fname == "indexed_fields") 	return mIndexedFields == value;
	if (fname == "translators") 	return mTranslators == value;
	if (fname == "archive") 		return mArchive == value;
	if (fname == "create_time") 	return mCreateTime == value;
	if (fname == "mod_time") 		return mModTime == value;

	if (fname == "max_limit") 		
	{
		OmnString ss;
		ss << mMaxLimit;
		return ss == value;
	}

	OmnAlarm << "Incorrect field name: " << fname << enderr;
	return false;
}


bool
AosGDDef::createGroupRecord(
		const OmnString &tablename,
		TiXmlElement *node, 
		OmnString &errmsg)
{
	OmnString stmt = "insert into ";
	stmt << sgGDTableName << " (" << sgInsertFields
		<< tablename << "', '" 
		<< node->Attribute("name") << "', '"
		<< node->Attribute("tagging") << "', '"
		<< node->Attribute("description") << "', '"
        << node->Attribute("key_fields") << "', '"
		<< node->Attribute("tagging_fname") << "', '"
		<< node->Attribute("name_fname") << "', '"
		<< node->Attribute("desc_fname") << "', ";

	const char *maxlimit = node->Attribute("max_limit");
    if (!maxlimit || strcmp(maxlimit, "") == 0) stmt << "0, '";
	else stmt << maxlimit << ", '";

	stmt << node->Attribute("no_delete") << "', '"
		<< node->Attribute("read_only") << "', '"
		<< node->Attribute("log") << "', '"
		<< node->Attribute("stat") << "', '"
		<< node->Attribute("obj_listing") << "', '"
		<< node->Attribute("indexed_fields") << "', '"
		<< node->Attribute("translators") << "', '"
		<< node->Attribute("archive") 
		<< "', now(), now(),"
		<< "'', '', now(), "
		<< "'', '', now(), "
		<< "'', '', now())";

	OmnTrace << "To insert into table: " << stmt << endl;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	OmnRslt rslt = store->runSQL(stmt);
	if (!rslt)
	{
		errmsg = "Failed to insert the group into the database!";
		return false;
	}

	return true;
}


bool
AosGDDef::lockGroup(
		TiXmlElement *node, 
		const OmnString &username,
		OmnString &errmsg)
{
	// It does one of the following:
	// 1. Lock/unlock for writing
	// 2. Lock/unlock for reading
	// 3. Lock/unlock for new object
	//
	// IMPORTANT: it assumes the caller has just retrieved the locking 
	// information from db for the record. 
	const char *type = node->getChildTextByAttr("name", "lock_type", 0);
	if (!type)
	{
		errmsg = "Missing lock type!";
		return false;
	}

	if (strcmp(type, "lock_reading") == 0) return lockForReading(username, errmsg);
	if (strcmp(type, "lock_writing") == 0) return lockForWriting(username, errmsg);
	if (strcmp(type, "lock_newobj") == 0)  return lockForNewObj(username, errmsg);
	if (strcmp(type, "unlock_reading") == 0) return unlockReading(username, errmsg);
	if (strcmp(type, "unlock_writing") == 0) return unlockWriting(username, errmsg);
	if (strcmp(type, "unlock_newobj") == 0) return unlockNewObj(username, errmsg);

	errmsg = "Unrecognized locking type: ";
	errmsg << type;
	return false;
}


bool
AosGDDef::lockGroup(const OmnString &stmt)
{
	// It will try to lock the record if it failed
	int tries = 0;
	while (tries < eMaxLockTries)
	{
		if (updateToDb(stmt))
		{
			// Successfully locked the record
			return true;
		}

		// Failed to update the record, which means that it is locked
		// by someone else. Will wait a little bit and try it again
		OmnWaitSelf->wait(0, eWaitInterval);
		tries++;
	}

	return false;
}


int
AosGDDef::getTranslator(const OmnString &id, std::list<OmnString> &trans) const
{
	// A translator is a list of paths, each of which points to an attribute, 
	// or a tag's text, or a tag. It is used to remove the pointed when 
	// an xmlobj is retrieved. 
	//
	// Translators are retrieved in mTranslators. It is in the format:
	// 	<translators>
	// 		<translator id="xxx">
	// 			<path value="xxx"/>	
	// 			<path value="xxx"/>	
	// 			...
	// 		</translator>
	// 		...
	// 	</translator>
	//
	TiXmlDocument thedoc;
	thedoc.Parse(mTranslators);
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlNode *child = root->getImmediateChildByAttr("id", id.data());
	if (!child)
	{
		// Did not find the translator
		trans.clear();
		return 0;
	}

	TiXmlNode *nn = 0;
	while ((nn = child->IterateChildren(nn)))
	{
		TiXmlElement *elem = nn->ToElement();
		if (elem)
		{
			const char *vv = elem->Attribute("value");
			if (vv) trans.push_back(vv);
		}
	}

	return trans.size();
}


bool		
AosGDDef::allowDelete(const OmnString &username) const
{
	// If the table:
	// 	1. is not locked for writing,
	// 	2. 'no_delete' is not 'true', 
	// 	3. is not readonly
	// we can delete objects.
	return !(mWriteLock == "lock" && mWLUser != username ||
			 mNoDelete == "true" ||
			 mReadOnly == "true");
}


bool
AosGDDef::maxReached() const
{
	if (mMaxLimit <= 0) return false;

	AosGDObj theobj;
	int num = theobj.getTotalRecords(mTablename);
	if (mMaxLimit >= num) return true;
	return false;
}


OmnString	
AosGDDef::getObjListingFieldnames(const OmnString &id)
{
	// Object listing is used to determine how to query and list objects. 
	// Object listings are defined as:
	// 	<objlistings>
	// 		<objlisting id="xxx" fieldnames="xxx,xxx,..." order="xxx"/>
	// 		<objlisting id="xxx" fieldnames="xxx,xxx,..." order="xxx"/>
	// 		...
	// 	</objlistings>
	TiXmlDocument thedoc;
	thedoc.Parse(mObjListing);
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlNode *listing = root->getImmediateChildByAttr("id", id);
	if (!listing) return "";

	TiXmlElement *elem = listing->ToElement();
	if (!elem) return "";
	return elem->Attribute("fieldnames");
}


OmnString	
AosGDDef::getObjListingOrder(const OmnString &id)
{
	// Refer to the comments in 'getObjListingFieldnames(...)'
	TiXmlDocument thedoc;
	thedoc.Parse(mObjListing);
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlNode *listing = root->getImmediateChildByAttr("id", id);
	if (!listing) return "";

	TiXmlElement *elem = listing->ToElement();
	if (!elem) return "";
	return elem->Attribute("order");
}

