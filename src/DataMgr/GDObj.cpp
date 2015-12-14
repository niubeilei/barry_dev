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
// 	This db object assumes the table:
// 		seqno
//		objid
//		name
//		tagging
//		description
//		creator
//		create_time
//		modifier
//		modify_time
//		xml
//		read_lock
//		rl_user
//		rl_time
//		write_lock
//		wl_user
//		wl_time
//
//  The above are standard fields. 'seqno' is an auto incremented seqno.
//  'tagging' may be retrieved from 'xml', or it may be edited independent
//  of 'xml'. Similarly, 'description' may or may not be retrieved from 'xml'.
//
//  In addition, one or more indexed fields
//  may be added to the table. An indexed field is a field whose value 
//  can be retrieved from the 'xml' field, which is an XML object. It is
//  used for fast query. 
//
//  Indexed fields are defined by member data 'mIndexedFields[...]'. 
//  	mIndexedFields[0]: the path pointing to the attribute in 'xml'
//  	mIndexedFields[1]: the field name; 
//  	mIndexedFields[2]: the path pointing to the attribute in 'xml'
//  	mIndexedFields[3]: the field name; 
//  	...
//  If a field name is not defined in mIndexedFields[i], the field name 
//  will be determined based on idx_<nnn>, where nnn is a sequence number, 
//  starting from 0.
//
// Modification History:
// 02/21/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataMgr/GDObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataMgr/Ptrs.h"
#include "DataMgr/GDDef.h"
#include "DataMgr/GDObj.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "DataStore/DbTrackTable.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "XmlInterface/XmlRc.h"


const int sgNumFields = 16;
const char *sgFieldnames[sgNumFields] = 
{
	"seqno", 
	"objid", 
	"name",
	"tagging", 
	"description",
	"creator",
	"create_time",
	"modifier", 
	"mod_time", 
	"xml", 
	"read_lock", 
	"rl_user", 
	"rl_time", 
	"write_lock", 
	"wl_user", 
	"wl_time"
};

AosGDObj::AosGDObj()
:
mSeqno(0),
mNumIndexedFields(-1),
mXmlDoc(0),
mXmlobj(0)
{
}


AosGDObj::~AosGDObj()
{
	if (mNumIndexedFields > 0) 
		aos_str_split_releasemem(mIndexedFields, mNumIndexedFields);

	if (mXmlDoc) OmnDelete mXmlDoc;
}


bool
AosGDObj::parse(
		const OmnString &tablename,
		const OmnString &contents,
		const AosGDDefPtr &def,
		OmnString &errmsg)
{
	// This function is called to set up the object. This is normally
	// called when it receives a client request to create a new object. 
	// 'contents' should be in the form:
	// 	<xmlobj	tagging="xxx">
	// 		<name>xxx</name>
	// 		<description><![CDATA[xxxx]]></description>
	// 	</xmlobj>
	// 			
	TiXmlDocument thedoc;
	thedoc.Parse(contents);
	TiXmlNode *root = thedoc.FirstChild();
	aos_assert_r(root, false);
	TiXmlElement *xmlobj = root->ToElement();
	aos_assert_r(xmlobj, false);

	// This function construct an instance of this class based on the
	// xml object 'xmlobj'. 
	mTablename = tablename;
	mObjid = "";
	mSeqno = 0;

	OmnString tagging_fname = def->getTaggingFname();
	if (tagging_fname != "") mTagging = xmlobj->Attribute(tagging_fname, "");

	OmnString desc_fname = def->getDescFname();
	if (desc_fname != "") mDescription = xmlobj->getChildText(desc_fname, "");

	OmnString name_fname = def->getNameFname();
	if (name_fname != "") mName = xmlobj->getChildText(name_fname, "");

	mXml = contents;
	mXmlobj = xmlobj;

	aos_assert_r(setIndexedFields(def->getIndexedFields()), false);
	return true;
}


bool
AosGDObj::setIndexedFields(const OmnString &indexedFields)
{
	// 'indexedFields' is a string of:
	// 		<attr_path>,<name>,<attr_path>,<name>,...
	// All indexed fields are string type. If <name> is empty, it is:
	// 		"idx_nnn"
	// where 'nnn' is a sequence number, starting from 0.
	// This function parses 'indexedFields'.
	if (mNumIndexedFields > 0) 
		aos_str_split_releasemem(mIndexedFields, mNumIndexedFields);
	mNumIndexedFields = 0;

	if (indexedFields == "") return true;

	mNumIndexedFields = aos_str_split(indexedFields.data(), ',', mIndexedFields, 
		eMaxIndexedFields);

	int idx = 0;
	for (int i=0; i<mNumIndexedFields; i+=2)
	{
		mIndexedValues[idx++] = mXmlobj->getNodeAttribute(mIndexedFields[i], "");
	}

	return true;
}


OmnRslt
AosGDObj::serializeFromRecord(const OmnDbRecordPtr &record)
{
	// This function assumes the record has loaded all the fields. 
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
	int idx = 0;
	mSeqno			= record->getU64(idx++, 0, rslt);
	mObjid			= record->getStr(idx++, "", rslt);
	mTagging		= record->getStr(idx++, "", rslt);
	mDescription	= record->getStr(idx++, "", rslt);
	mCreator		= record->getStr(idx++, "", rslt);
	mCreateTime		= record->getStr(idx++, "", rslt);
	mModifier		= record->getStr(idx++, "", rslt);
	mModTime		= record->getStr(idx++, "", rslt);
	mXml			= record->getStr(idx++, "", rslt);
	mReadLock		= record->getStr(idx++, "", rslt);
	mRLUser			= record->getStr(idx++, "", rslt);
	mRLTime			= record->getStr(idx++, "", rslt);
	mWriteLock		= record->getStr(idx++, "", rslt);
	mWLUser			= record->getStr(idx++, "", rslt);
	mWLTime			= record->getStr(idx++, "", rslt);

	int vidx = 0;
	if (mNumIndexedFields > 0)
	{
		for (int i=0; i<mNumIndexedFields; i+=2)
		{
			mIndexedValues[vidx++] = record->getStr(idx++, "", rslt);
		}
	}

	return rslt;
}


OmnString
AosGDObj::updateStmt() const
{
	OmnAlarm << "Should never call this function!" << enderr;
	return "";
}


OmnString
AosGDObj::removeStmt() const
{
	aos_assert_r(mTablename != "", "");
	aos_assert_r(mSeqno > 0, "");
    OmnString stmt = "delete from ";
	stmt << mTablename << " where seqno=" << mSeqno;
    return stmt;
}


OmnString
AosGDObj::removeAllStmt() const
{
	aos_assert_r(mTablename != "", "");
    OmnString stmt = "delete from ";
	stmt << mTablename;
    return stmt;
}


OmnString
AosGDObj::existStmt() const
{
	aos_assert_r(mTablename != "", "");
	aos_assert_r(mSeqno > 0, "");
    OmnString stmt = "select seqno from ";
	stmt << mTablename << " where seqno=" << mSeqno;
    return stmt;
}


OmnString
AosGDObj::insertStmt() const
{
	OmnAlarm << "Should never call this function!" << enderr;
    return "";
}


bool
AosGDObj::changeRecordForAdd()
{
	// This function assumes all member data are set correctly.
	
	AosDbTrackTable tracktable(AosDbTrackTable::eAdd, mTablename);
	tracktable.startAssemble();
	tracktable.addField("objid", mObjid, false);
	tracktable.addField("name", mName, false);
	tracktable.addField("tagging", mTagging, false);
	tracktable.addField("description", mDescription, true);
	tracktable.addField("creator", mCreator, false);
	tracktable.addField("create_time", mCreateTime, false);
	tracktable.addField("modifier", mModifier, false);
	tracktable.addField("mod_time", mModTime, false);
	tracktable.addField("xml", mXml, true);
	tracktable.addField("read_lock", mReadLock, true);
	tracktable.addField("rl_user", mRLUser, true);
	tracktable.addField("rl_time", mRLTime, true);
	tracktable.addField("write_lock", mReadLock, true);
	tracktable.addField("wl_user", mRLUser, true);
	tracktable.addField("wl_time", mRLTime, true);

	if (mNumIndexedFields > 0)
	{
		int vidx = 0;
		OmnString fname;
		for (int i=0; i<mNumIndexedFields; i+=2)
		{
			if (strlen(mIndexedFields[i+1]) > 0)
			{
				fname = mIndexedFields[i+1];
				tracktable.addField(fname, mIndexedValues[vidx++], true);
			}
		}
	}

	tracktable.endAssemble();
	tracktable.addToDb();
	return true;
}


OmnString
AosGDObj::retrieveStmt() const
{
	aos_assert_r(mTablename != "", "");
	aos_assert_r(mSeqno > 0, "");
    OmnString stmt = "select * from ";
	stmt << mTablename << " where seqno=" << mSeqno;
	return stmt; 
}


bool
AosGDObj::retrieveObjBySeqno(
		const OmnString &seqno, 
		const OmnString &tablename)
{
	OmnString stmt = "select * from ";
	stmt << tablename << " where seqno=" << seqno;
	return OmnDbObj::serializeFromDb(stmt);
}


bool
AosGDObj::addObjectToDb()
{
	// It adds a new object into the database. It assumes that the caller
	// has set up all the values as needed. Before adding to database, 
	// it creates an empty record and retrieves the seqno from this 
	// empty record. It then updates this empty record based on this
	// class' contents. 
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	aos_assert_r(store->insertEmptyRecord(mTablename, mSeqno), false);
	aos_assert_r(mSeqno > 0, false);
	mObjid = mTablename;
	mObjid << "_" << mSeqno;

	OmnString stmt = "update ";
	stmt << mTablename << " set "
		<< "objid='" << mObjid
		<< "', name='" << mName
		<< "', tagging='" << mTagging
		<< "', description='" << mDescription
		<< "', creator='" << mCreator
		<< "', create_time=now()"
		<< ", modifier='" << mCreator
		<< "', mod_time=now()"
		<< ", xm='" << mXml;

	OmnString fname;
	int fidx = 0;
	int vidx = 0;
	if (mNumIndexedFields > 0)
	{
		for (int i=0; i<mNumIndexedFields; i+=2)
		{
			if (strlen(mIndexedFields[i+1]) == 0)
			{
				fname = "idx_";
				fname << fidx++;
			}
			else
			{
				fname = mIndexedFields[i+1];
			}
			stmt << "', " << fname << "='" << mIndexedValues[vidx++];
		}
	}

	stmt << "' where seqno=" << mSeqno;
	aos_assert_r(updateToDb(stmt), false);
	changeRecordForAdd();
	return true;
}


bool
AosGDObj::retrieveList(
		TiXmlElement *node, 
		const AosGDDefPtr &groupDef,
		AosXmlRc &errcode, 
		OmnString &errmsg, 
		OmnString &results)
{
	// It retrieves the list of the objects. If the table contains too many objects,
	// it will only retrieve the first x number of entries. Subsequent
	// requests can be used to retrieve the remaining objects.
	//  <request>
	//      <tag name="start_idx">xxx</tag>
	//      <tag name="page_size">xxx</tag>
	//      <tag name="obj_listing">xxx</tag>
	//  </request>

	// 1. Retrieving the starting index
	const char *str = node->getChildTextByAttr("name", "start_idx", 0);
	int start_idx = 0;
	if (str)
	{
		start_idx = atoi(str);
		aos_assert_r(start_idx >= 0, false);
	}

	// 2. Retrieving the page size.
	str = node->getChildTextByAttr("name", "page_size", 0);
	if (!str)
	{
		errmsg = "Missing page size!";
		return false;
	}
	int page_size = atoi(str);
	if (page_size <= 0)
	{
		errmsg = "Incorrect page size: ";
		errmsg << str;
		return false;
	}
	if (page_size >= eMaxPageSize) page_size = eMaxPageSize;

	// 3. Retrieving objlist and order
	const char *objlisting = node->getChildTextByAttr("name", "obj_listing", 0);
	OmnString fieldnames = groupDef->getObjListingFieldnames(objlisting);
	OmnString order = groupDef->getObjListingOrder(objlisting);

	char *fnames[eMaxFieldNames];
	int rc = aos_str_split(fieldnames.data(), ',', fnames, eMaxFieldNames);

	// 4. Construct the query statement
	OmnString stmt = "select ";
	stmt << fieldnames << " from " << mTablename 
		<< " limit " << start_idx << ", " << page_size;
	if (order != "") stmt << " " << order;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	OmnDbTablePtr table;
	OmnRslt rslt = store->query(stmt, table);
	if (!rslt)
	{
		errmsg = "Failed to query the database: ";
		errmsg << stmt;
		aos_str_split_releasemem(fnames, eMaxFieldNames);
		return false;
	}

	OmnDbRecordPtr record;
	table->reset();
	results = "<Records num=\"";
    results	<< table->entries() << "\" ending_idx=\""
		<< start_idx + table->entries();
	if (start_idx == 0)
	{
		// Need to get the total number of records in the table
		AosGDObj theobj;
		int total = theobj.getTotalRecords(mTablename);
		results << "\" total=\"" << total;
	}
    results	<< "\">";
	while (table->hasMore())
	{
		record = table->next();
		results << "<Record objid=\"" << record->getStr(0, "", rslt) << "\">";
		for (int i=1; i<rc; i++)
		{
			results << "<attr name=\"" << fnames[i] << "\"><![CDATA["
				<< record->getStr(i, "", rslt) << "]]></attr>";
		}
		results << "</Record>";
	}
	results << "</Records>";

	aos_str_split_releasemem(fnames, eMaxFieldNames);
	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


int 
AosGDObj::getTotalRecords(const OmnString &tablename)
{
	OmnString stmt = "select count(seqno) from ";
	stmt << tablename;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, -1);
	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	aos_assert_r(rslt, -1);
	aos_assert_r(record, -1);
	int count = record->getInt(0, -1, rslt);
	return count;
}


bool
AosGDObj::lockForReading(const OmnString &username)
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

	return lockObj(stmt);
}


bool
AosGDObj::lockForWriting(const OmnString &username)
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

	return lockObj(stmt);
}


bool
AosGDObj::unlockReading(const OmnString &username, OmnString &errmsg)
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
AosGDObj::unlockWriting(const OmnString &username, OmnString &errmsg)
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
AosGDObj::lockForReadWrite(const OmnString &username)
{
	// IMPORTANT: it assumes the caller has just retrieved the
	// locking information from the database. 
	OmnString stmt = "update ";
	if (mWriteLock == "locked" && mWLUser == username &&
		mReadLock == "locked" && mRLUser == username)
	{
		// Already locked by this user
		return true;
	}

	if (mWriteLock == "locked" && mWLUser == username)
	{
		// Write lock was locked by this user. Need to get the read lock
		stmt << mTablename << " set read_lock='locked', rl_username='"
			<< username << "', rl_time=now() where seqno=" << mSeqno
			<< " and read_lock=''";
	}
	else if (mReadLock == "locked" && mRLUser == username)
	{
		// Read lock was locked by this user. Need to get the write lock
		stmt << mTablename << " set write_lock='locked', wl_username='"
			<< username << "', wl_time=now() where seqno=" << mSeqno
			<< " and write_lock=''";
	}
	else
	{
		// None of the locks were owned by this user. 
		stmt << mTablename << " set write_lock='locked', wl_username='"
			<< username << "', wl_time=now(), read_lock='locked', rl_username='"
			<< username << "', rl_time=now() where seqno=" << mSeqno
			<< " and write_lock='' and read_lock=''";
	}
	return lockObj(stmt);
}


bool
AosGDObj::lockObj(const OmnString &stmt)
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


bool
AosGDObj::constructResponse(
		TiXmlElement *xmlobj, 
		OmnString &response)
{
	// Response should be in the form:
	// 	<field name="seqno">xxx</field>
	//	<field name="objid">
	//	<field name="name">
	//	<field name="tagging">
	//	<field name="description"><![CDATA[...]]>
	//	<field name="creator">
	//	<field name="create_time">
	//	<field name="modifier">
	//	<field name="modify_time">
	//	<field name="xml"<![CDATA[...]]>
	//	<field name="read_lock">
	//	<field name="rl_user">
	//	<field name="rl_time">
	//	<field name="write_lock">
	//	<field name="wl_user">
	//	<field name="wl_time">
	//	<field name="the indexed field">...
	//	...
	ostringstream oss(ostringstream::out);
	oss << *xmlobj;
	response = "<field name=\"seqno\">";
	response << mSeqno 
		<< "</field><field name=\"objid\">" << mObjid 
		<< "</field><field name=\"name\">" << mName
		<< "</field><field name=\"tagging\">" << mTagging
		<< "</field><field name=\"description\"><![CDATA" << mTagging << "]]>"
		<< "</field><field name=\"creator\">" << mCreator
		<< "</field><field name=\"create_time\">" << mCreateTime
		<< "</field><field name=\"modifier\">" << mModifier
		<< "</field><field name=\"modify_time\">" << mModTime
		<< "</field><field name=\"xml\"><![CDATA" << oss.str() << "]]>"
		<< "</field><field name=\"read_lock\">" << mReadLock
		<< "</field><field name=\"rl_user\">" << mRLUser
		<< "</field><field name=\"rl_time\">" << mRLTime
		<< "</field><field name=\"write_lock\">" << mWriteLock
		<< "</field><field name=\"wl_user\">" << mWLUser
		<< "</field><field name=\"wl_time\">" << mWLTime << "</field>";

	if (mNumIndexedFields > 0)
	{
		int idx = 0;
		for (int i=0; i<mNumIndexedFields; i+=2)
		{
			if (strlen(mIndexedFields[i+1]) > 0)
			{
				response << "<field name=\"" << mIndexedFields[i+1]
					<< "\"><![CDATA[" << mIndexedValues[idx] << "]]></field>";
			}
			idx++;
		}
	}

	return true;
}


bool
AosGDObj::getObjBySeqno(const OmnString &seqno, OmnString &errmsg)
{
	OmnString stmt = "select read_lock, rl_user, rl_time, write_lock, wl_user, "
		"wl_time where seqno=";
	stmt << seqno;
	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record); 
	if (!rslt) return false;
	mReadLock = record->getStr(0, "", rslt);
	mRLUser = record->getStr(1, "", rslt);
	mRLTime = record->getStr(2, "", rslt);
	mWriteLock = record->getStr(3, "", rslt);
	mWLUser = record->getStr(4, "", rslt);
	mWLTime = record->getStr(5, "", rslt);
	return true;
}

	
bool
AosGDObj::lockObj(
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

	if (strcmp(type, "lock_reading") == 0) return lockForReading(username);
	if (strcmp(type, "lock_writing") == 0) return lockForWriting(username);
	if (strcmp(type, "lock_both") == 0) return lockForReadWrite(username);
	if (strcmp(type, "unlock_reading") == 0) return unlockReading(username, errmsg);
	if (strcmp(type, "unlock_writing") == 0) return unlockWriting(username, errmsg);
	if (strcmp(type, "unlock_both") == 0) 
		return unlockReading(username, errmsg) && 
			   unlockWriting(username, errmsg);

	errmsg = "Unrecognized locking type: ";
	errmsg << type;
	return false;
}


bool
AosGDObj::modifyObject(
		const OmnString &username,
		const OmnString &contents, 
		const bool unlock_write,
		OmnString &errmsg)
{
	// It modifies an object based on 'contents'. This function assumes the
	// object was just retrieved from db and it is locked by this user for
	// modifying, and the caller has locked the object for modification.
	//
	// 'contents' should be in the following format:
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
	// the contents might be modified by someone else. This function checks
	// for this. If yes, it will indicate the error and the
	// operation is aborted. 
	//
	
	if (mWriteLock != "locked" || mWLUser != username)
	{
		errmsg = "Record was not locked for modification!";
		return false;
	}

    TiXmlDocument thedoc;
    thedoc.Parse(contents);
    TiXmlNode *root = thedoc.FirstChild();
    aos_assert_r(root, false);
	TiXmlNode *child = 0;
	bool is_first = true;
	OmnString stmt = "update ";
	stmt << mTablename << " set ";
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
			strcmp(fname, "wl_user") == 0 ||
			strcmp(fname, "seqno") == 0 ||
			strcmp(fname, "objid") == 0)
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
		stmt << fname << "='" << new_contents << "'";
	}	

	if (is_first)
	{
		// This means it contains no modified fields
		errmsg = "The request does not contain any modified fields!";
		OmnAlarm << errmsg << " " << contents << enderr;
		return false;
	}

	if (unlock_write) stmt << ", write_lock='unlock', wl_user=''";

	stmt << " where tablename='" << mTablename << "'";

	if (!updateToDb(stmt))
	{
		errmsg = "Failed to update the database!";
		return false;
	}

	return true;
}


bool
AosGDObj::checkValue(const OmnString &fname, const OmnString &value)
{
	// This function checks whether the field named 'fname' holds the same
	// value as 'value'. 
	if (fname == "tagging") 		return mTagging == value;
	if (fname == "name") 			return mName == value;
	if (fname == "description") 	return mDescription == value;
	if (fname == "creator") 		return mCreator == value;
	if (fname == "create_time") 	return mCreateTime == value;
	if (fname == "modifier") 		return mModifier == value;
	if (fname == "mod_time") 		return mModTime == value;
	if (fname == "xml") 			return mXml== value;

	OmnAlarm << "Incorrect field name: " << fname << enderr;
	return false;
}


bool	
AosGDObj::isLockedForReading(const OmnString &username) const
{
	// If the lock is not locked or locked by 'username', it is considered
	// not locked. Otherwise, it is locked. 
	return (mReadLock == "locked" && mRLUser != username);
}


bool	
AosGDObj::isLockedForWriting(const OmnString &username) const
{
	return (mWriteLock == "locked" && mWLUser != username);
}


TiXmlElement *	
AosGDObj::getXmlobj()
{
	// It assumes the xml has been retrieved from database. It reconstruct
	// the object. 

	if (!mXmlDoc) mXmlDoc = OmnNew TiXmlDocument();
	aos_assert_r(mXmlDoc, 0);

	mXmlDoc->Parse(mXml.data());
	TiXmlNode *root = mXmlDoc->FirstChild();
	aos_assert_r(root, 0);
	mXmlobj = root->ToElement();
	aos_assert_r(mXmlobj, 0);
	return mXmlobj;
}


bool	
AosGDObj::deleteObjBySeqno(const OmnString &seqno)
{
	OmnString stmt = "delete from ";
	stmt << mTablename << " where seqno=" << seqno;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	OmnRslt rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);
	return true;
}


OmnString
AosGDObj::getCreateTableStmt(
		const OmnString &tablename, 
		const char *indexed_fields,
		OmnString &errmsg)
{
	OmnString stmt = "create table ";
	stmt << tablename << " ("
	     << "    seqno bigint auto_increment not null primary key,"
	     << "    objid varchar(64),"
	     << "    name varchar(64),"
	     << "    tagging text,"
	     << "    description text,"
	     << "    creator varchar(32),"
	     << "    create_time datetime,"
	     << "    modifier varchar(32),"
	     << "    mod_time datetime,"
	     << "    xml text,"
	     << "    read_lock varchar(10),"
	     << "    rl_user varchar(32),"
	     << "    rl_time datetime,"
	     << "    write_lock varchar(10),"
	     << "    wl_user varchar(32),"
	     << "    wl_time datetime";

	if (indexed_fields)
	{
        // There are indexed fields. All indexed fields are "text" type.
        char *fields[eMaxIndexedFields];
        int rc = aos_str_split(indexed_fields, ',', fields, eMaxIndexedFields);
        aos_assert_r(rc > 0, false);
        for (int i=0; i<rc; i++)
        {
			// Check whether it is a valid field name. A valid field
			// name must contain only letters, digits, and underscores, 
			// and it cannot be a digit string. 
			if (aos_str_is_varstr(fields[i]) != 1)
			{
				errmsg = "Field name is incorrect: ";
				errmsg << fields[i];
				OmnAlarm << errmsg << enderr;
				return false;
			}

			// Check whether it clides with reserved names.
			for (int j=0; j<sgNumFields; j++)
			{
				if (strcmp(fields[i], sgFieldnames[j]) == 0)
				{
					errmsg = "Indexed field name cannot be ";
					errmsg << sgFieldnames[j] << ", which is "
						<< "reserved for internal usage!";
					return "";
				}
			}

			// Check whether it clides with the previous indexed names.
			for (int j=0; j<i; j++)
			{
				if (strcmp(fields[j], fields[i]) == 0)
				{
					errmsg = "Field names are reused: ";
					errmsg << fields[j] << ". The field names are: "
						<< indexed_fields;
					return "";
				}
			}

			stmt << ", " << fields[i] << " text";
		}

		aos_str_split_releasemem(fields, eMaxIndexedFields);
	}
	
	stmt << ")";
	return stmt;
}


