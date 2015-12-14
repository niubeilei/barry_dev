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
//   
//
// Modification History:
// 04/17/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "DataMgr/GroupDataMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataMgr/GDObj.h"
#include "DataMgr/GDDef.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/GenericDbObj.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"
#include "Util/OmnNew.h"


const OmnString sgAosGroupDataMgrTablename = "group_obj_dm";
const OmnString sgAosTablenameSystemId = "tablename_seqno";

AosGroupDataMgr::AosGroupDataMgr()
{
}


AosGroupDataMgr::~AosGroupDataMgr()
{
}


bool
AosGroupDataMgr::process(
		TiXmlElement *node, 
		AosXmlRc &errcode, 
		OmnString &errmsg, 
		OmnString &response)
{
	errcode = eAosXmlInt_General;
	errmsg = "Internal Error!";
	response = "";

	aos_assert_r(node, false);
	const char *operation = node->getChildTextByAttr("name", "operation", "");
	if (strcmp(operation, "create_group") == 0)
	{
		return createGroup(node, errcode, errmsg, response);
	}
	
	if (strcmp(operation, "add_obj") == 0)
	{
		return addObj(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "modify_obj") == 0)
	{
		return modifyObj(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "delete_obj") == 0)
	{
		return deleteObj(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "retrieve_obj") == 0)
	{
		return retrieveObj(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "query_group") == 0)
	{
		return queryGroup(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "archive") == 0)
	{
		return archive(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "modify_group") == 0)
	{
		return modifyGroup(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "destroy_group") == 0)
	{
		return destroyGroup(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "retrieve_objlist") == 0)
	{
		return retrieveObjList(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "lock_group") == 0)
	{
		return lockGroup(node, errcode, errmsg, response);
	}

	if (strcmp(operation, "lock_obj") == 0)
	{
		return lockObj(node, errcode, errmsg, response);
	}

	errmsg = "Unrecognized operation: ";
	errmsg << operation;
	return false;
}


bool
AosGroupDataMgr::createGroup(
		TiXmlElement *node, 
		AosXmlRc &errcode, 
		OmnString &errmsg, 
		OmnString &response)
{
	// The function creates an instance of Group Data Manager. If successful, 
	// it will add a record in the group management table and return true.
	// Otherwise, it returns false, the errcode and errmsg contains the 
	// error information. 
	//
	// The group configuration information is contained in the xml object, 
	// which should be in the form:
	// 	<node ...>
	// 		<xmlobj 
	// 			tagging="xxx"
	// 			key_fields="xxx"
	// 			tagging_fname="xxx"
	// 			desc_fname="xxx"
	// 			max_limit="xxx"
	// 			no_delete="xxx"
	// 			read_only="xxx"
	// 			log="xxx"
	// 			stat="xxx"
	// 			archive="xxx">
	// 			<description><![CDATA[xxx]]></description>
	// 			<obj_listing><![CDATA[xxx]]></obj_listing>
	// 			<translators><![CDATA[xxx]]></translators>
	// 		</xmlobj>
	// For a complete description of the attributes, please refer to the 
	// document.
	
	// Retrieve the object definition
	aos_assert_r(preprocess(node, errcode, errmsg), false);

	// Determine tablename. The tablename may be explicitly specified
	// in the xml object, or to be determined by the system. 
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);

	// Determine table name
	u64 seqno;
	OmnString tablename = "table_";
	if (!store->getDbId(sgAosTablenameSystemId, seqno))
	{
		errmsg = "Failed to retrieve table seqno";
		return false;
	}
	tablename << seqno;

	// Retrieve the <xmlobj> node
	TiXmlNode *xmlobj = node->firstChildByAttr("name", "xmlobj");
	if (!xmlobj)
	{
		errmsg = "Missing the <xmlobj> node";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	TiXmlNode *child = xmlobj->FirstChild();
	if (!child)
	{
		errmsg = "The xmlobj does not have a child!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	TiXmlElement *xmlobj_elem = child->ToElement();
	if (!xmlobj_elem)
	{
		errmsg = "The xmlobj is incorrect!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

cout << "The obj: " << *xmlobj_elem << endl;

	// Create the table
	aos_assert_r(createTable(tablename, xmlobj_elem, errcode, errmsg), false);

	// Ready to insert a record to "groupdata_mgr" table. 
	// 	seqno 		auto_increment
	// 	tablename	text, primary key
	AosGDDef dbobj;
	if (!dbobj.createGroupRecord(tablename, xmlobj_elem, errmsg)) return false;

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool
AosGroupDataMgr::addObj(
		TiXmlElement *node, 
		AosXmlRc &errcode, 
		OmnString &errmsg, 
		OmnString &response)
{
	// It creates an object. 'node' shall be in the form:
	// 	<formcomponent name="contents">the contents of the object</formcomponent>
	// 	<formcomponent name="tablename">...
	// 	<formcomponent name="user">...
	//
	// 1. Check whether the table can create more objects,
	// 2. Check whether the object exists,
	// 2. Add the object
	
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Group not defined";
		return false;
	}

	// 1. Check whether it can add more objects
	if (!canCreateMoreObject(errmsg)) return false;

	// 2. Lock for writing
	if (!mGroupDef->lockForNewObj(mUsername, errmsg)) return false;

	AosGDObj theobj;
	if (!theobj.parse(mTablename, mContents, mGroupDef, errmsg)) 
	{
		OmnString tt;
		mGroupDef->unlockNewObj(mUsername, tt);
		return false;
	}
	theobj.setCreator(mUsername);
	theobj.setModifier(mUsername);

	if (!theobj.addObjectToDb())
	{
		OmnAlarm << "Failed to create object" << enderr;
		errmsg = "Failed to create object";
		OmnString tt;
		mGroupDef->unlockNewObj(mUsername, tt);
		return false;
	}

	mGroupDef->unlockNewObj(mUsername, errmsg);
	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool
AosGroupDataMgr::createTable(
		const OmnString &tablename, 
		TiXmlElement *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// First check whether there are indexed fields
	const char *indexed_fields = node->Attribute("indexed_fields");

	AosGDObj theobj;
	OmnString stmt = theobj.getCreateTableStmt(tablename, indexed_fields, errmsg);
	if (stmt == "") return false;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	bool rslt = store->createTable(stmt);
	if (!rslt)
	{
		errmsg = "Failed to create the table: ";
		errmsg << tablename;
		return false;
	}
	return true;
}


bool
AosGroupDataMgr::canCreateMoreObject(OmnString &reason)
{
	// There are several factors that may block creating new objects:
	// 1. The requester does not have the right to do so
	// 2. The data manager max limit is reached
	// 3. The data manager is locked from creating new objects
	// 
	// IMPORTANT: if this function returns true, the table is locked 
	// for creating new object. The caller of this function is responsible
	// for unlocking the table before returning.
	//
	aos_assert_r(mUsername != "", false);
	aos_assert_r(mGroupDef, false);

	// 1. Check whether the requester has the right
	// (TBD).
	
	// 2. Check whether the table has reached its maximum
	if (mGroupDef->maxReached())
	{
		reason = "The table cannot create more objects because the maximum ";
		reason << "has been reached: " << mGroupDef->getMaxLimit();
		return false;
	}

	// 3. Check whether it is locked.
	if (mGroupDef->isLockedForNewObj(mUsername))
	{
		reason = "Table is locked from creating new object. User name: ";
		reason << mGroupDef->getNOLUser() << ". Locking time: "
			<< mGroupDef->getNOLTime();
		return false;
	}

	return true;
}


bool
AosGroupDataMgr::preprocess(
		TiXmlElement *node, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// node is an xml object that has the following format:
	// 	<xmlobj>
	// 		<formcomponent name="tablename">...
	// 		<formcomponent name="username">...
	// 		<formcomponent name="appname">...
	// 		...
	// 	</xmlobj>
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Internal error";
	aos_assert_r(node, false);

	// Retrieve username
	mUsername = (char *)node->getChildTextByAttr("name", "username", 0); 
	if (!mUsername)
	{
		errmsg = "Missing the username!";
		return false;
	}

	// Retrieve tablename 
	mTablename = (char *)node->getChildTextByAttr("name", "tablename", 0);
	mGroupDef = 0;
	if (mTablename)
	{
		mGroupDef = OmnNew AosGDDef();
		if (!mGroupDef->getGDDef(mTablename))
		{
			errmsg = "Failed to retrieve the group definition: ";
			errmsg << mTablename;
			mGroupDef = 0;
			return false;
		}
	}

	// Retrieve appname
	mAppname = (char *)node->getChildTextByAttr("name", "appname", 0);
	if (!mAppname)
	{
		errmsg = "Missing the appname!";
		return false;
	}

	return true;
}


bool
AosGroupDataMgr::modifyObj(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It modifies an object. The object to be modified is defined in 'node'.
	// 1. Check whether the table is locked for writing.
	// 2. Check whether the table is read only.
	// 3. Check whether the user has the right to modify the object.
	// 4. Check whether the object exists. 
	// 5. Check whether the object is locked from modifying 
	// 6. Lock for writing, modify, and unlock.
	
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	aos_assert_r(mUsername, false);

	if (!mTablename)
	{
		errmsg = "Missing tablename!";
		return false;
	}

	if (!mGroupDef)
	{
		errmsg = "Failed to retrieve the group information!";
		return false;
	}

	// 1. Check whether the table is locked for writing
	if (mGroupDef->isLockedForWriting(mUsername))
	{
		errmsg = "Table is locked for writing. Locked by: ";
		errmsg << mGroupDef->getWLUser()
			<< ", and locked at: " << mGroupDef->getWLTime();
		return false;
	}

	// 2. Check whether the table is read only
	if (mGroupDef->isReadOnly())
	{
		errmsg = "Table is read only!";
		return false;
	}

	// 3. Check whether the user has the right
	// (TBD).
	
	// 4. Check whether the object exists. Object is identified by seqno
	OmnString seqno = node->getChildTextByAttr("name", "seqno", 0);
	if (!seqno)
	{
		errmsg = "Failed to retrieve the seqno!";
		return false;
	}

	AosGDObj theobj;
	if (!theobj.retrieveObjBySeqno(seqno, mTablename))
	{
		errmsg = "Object does not exist: ";
		errmsg << seqno;
		return false;
	}

	// Check whether the object is locked for writing
	if (theobj.isLockedForWriting(mUsername))
	{
		errmsg = "Object is locked from modifying. Locked by: ";
		errmsg << theobj.getWLUser() << ", and lock time: "
			<< theobj.getWLTime();
		return false;
	}

	if (!theobj.lockForWriting(mUsername))
	{
		errmsg = "Failed to lock for writing!";
		return false;
	}

	// Determine objid;
	OmnString objid = mTablename;
	objid << "_" << seqno;

	// Retrieve the new object
	OmnString contents = node->getChildTextByAttr("name", "newobj", "");
	if (contents == "")
	{
		errmsg = "Failed to retrieve the object to be updated!";
		OmnString tt;
		theobj.unlockWriting(mUsername, tt);
		return false;
	}

	if (!theobj.modifyObject(mUsername, objid, contents, errmsg))
	{
		errmsg = "Failed to update the object!";
		return false;
	}

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::deleteObj(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It deletes an object:
	// 1. Check whether the table is locked for writing.
	// 2. Check whether the table is read only.
	// 3. Check whether the user has the right to delete objects.
	// 4. Check whether the object exists. 
	// 5. Check whether the object is locked from modifying 
	// 6. Lock for writing, delete.
	
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Missing group information!";
		return false;
	}

	// 1. Check whether the table is locked for writing
	if (mGroupDef->isLockedForWriting(mUsername))
	{
		errmsg = "Table is locked for writing. Locked by: ";
		errmsg << mGroupDef->getWLUser()
			<< ", and locked at: " << mGroupDef->getWLTime();
		return false;
	}

	// 2. Check whether the table is read only
	if (mGroupDef->isReadOnly())
	{
		errmsg = "Table is read only!";
		return false;
	}

	// 3. Check whether the table allows deletion
	if (!mGroupDef->allowDelete(mUsername))
	{
		errmsg = "Table does not allow deleting objects";
		return false;
	}

	// 3. Check whether the user has the right
	// (TBD).
	
	// 4. Check whether object exists
	OmnString seqno = node->getChildTextByAttr("name", "seqno", 0);
	if (seqno == "")
	{
		errmsg = "Failed to retrieve the seqno";
		return false;
	}

	AosGDObj theobj;
	if (!theobj.getObjBySeqno(seqno, errmsg))
	{
		errmsg = "Object does not exist: ";
		errmsg << seqno;
		return false;
	}

	// 5. Check whether the object is locked for writing
	if (theobj.lockForWriting(mUsername))
	{
		errmsg = "Failed to lock the object. Locked by: ";
		errmsg << theobj.getWLUser() << ", and lock time: "
			<< theobj.getWLTime();
		return false;
	}

	if (!theobj.deleteObjBySeqno(seqno))
	{
		errmsg = "Failed to delete the object!";
		return false;
	}

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::retrieveObj(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It retrieves an object:
	// 1. Check whether the table is locked for reading.
	// 2. Check whether the user has the right to read objects.
	// 3. Check whether the object exists. 
	// 4. Retrieve the object.
	// 5. Check whether the object is locked from reading 
	// 6. Translate if needed.
	
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Failed to retrieve group information!";
		return false;
	}

	// 1. Check whether the table is locked for reading 
	if (mGroupDef->isLockedForReading(mUsername))
	{
		errmsg = "Table is locked for reading. Locked by: ";
		errmsg << mGroupDef->getRLUser()
			<< ", and locked at: " << mGroupDef->getRLTime();
		return false;
	}

	// 2. Check whether the user has the right
	// (TBD).
	
	// 3. Check whether the object exists.
	OmnString seqno = node->getChildTextByAttr("name", "seqno", "");
	if (seqno == "")
	{
		errmsg = "Object does not have a seqno";
		return false;
	}

	// 4. Retrieve the object
	AosGDObj theobj;
	if (!theobj.retrieveObjBySeqno(seqno, mTablename)) 
	{
		errmsg = "Failed to retrieve the object: ";
		errmsg << seqno;
		return false;
	}

	// 5. Check whether it is locked for reading
	if (theobj.isLockedForReading(mUsername))
	{
		errmsg = "Object is locked from reading, locked by: ";
		errmsg << theobj.getRLUser() << ", and locked at: "
			<< theobj.getRLTime();
		return false;
	}

	// 6. Check the translator
	const char *translator = node->getChildTextByAttr("name", "translator", 0);
	TiXmlElement *xmlobj = theobj.getXmlobj();
	if (!xmlobj)
	{
		errmsg = "Failed to reconstruct the object!";
		return false;
	}

	if (translator)
	{
		// Translator is a series of paths separated by ','. 
		std::list<OmnString> trans;
	    if (!mGroupDef->getTranslator(translator, trans));
		{
			errmsg = "Failed to retrieve the translator: ";
			errmsg << translator;
			return false;
		}

		std::list<OmnString>::iterator iter;
		for (iter=trans.begin(); iter != trans.end(); iter++)
		{
			xmlobj->removeAttribute(*iter);
		}
	}

	if (!theobj.constructResponse(xmlobj, response))
	{
		errmsg = "Failed to construct the response!";
		return false;
	}

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}

 
bool 
AosGroupDataMgr::queryGroup(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It queries the table. 
	errmsg = "Querying Group Data not implemented yet!";
	return false;
}


bool 
AosGroupDataMgr::archive(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It queries the table. 
	errmsg = "Archive Group Data not implemented yet!";
	return false;
}


bool 
AosGroupDataMgr::modifyGroup(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It modifies a group:
	// 1. Check whether the group exists
	// 2. Check whether the group is locked for writing.
	// 3. Check whether the user has the right to modify the object.
	// 4. Lock for writing, modify, and unlock.
	
	aos_assert_r(preprocess(node, errcode, errmsg), false);

	// 1. Check whether the group exists
	if (!mGroupDef)
	{
		errmsg = "Missing the group definition!";
		return false;
	}

	// 2. Check whether the group is locked for writing
	if (mGroupDef->isLockedForWriting(mUsername))
	{
		errmsg = "Table is locked for writing. Locked by: ";
		errmsg << mGroupDef->getWLUser()
			<< ", and locked at: " << mGroupDef->getWLTime();
		return false;
	}

	// 3. Check whether the user has the right
	// (TBD).
	
	AosGDDef group;
	const char *newgroup = node->getChildTextByAttr("name", "newgroup", 0);
	if (!newgroup)
	{
		errmsg = "Missing the new group";
		return false;
	}

	if (!group.lockForWriting(mUsername, errmsg)) return false;
	if (!group.modifyGroup(mTablename, newgroup, true, errmsg))
	{
		errmsg = "Failed to update the group data!";
		group.unlockWriting(mUsername, errmsg);
		return false;
	}

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::destroyGroup(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It destroys a group. 
	// 1. Check whether the group exists
	// 2. Check whether the group is locked for writing
	// 3. Check whether the user has the right to destroy the group
	// 4. Lock, destroy
	// 5. Destroy the group table.
	
	// 1. Check whether the group exists
	//
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Failed to retrieve the group definition!";
		return false;
	}

	// 1. Check the group exists
	if (!mGroupDef->existInDb())
	{
		errmsg = "Group does not exist!";
		return false;
	}

	// 2. Check whether the group is locked for writing
	if (mGroupDef->isLockedForWriting(mUsername))
	{
		errmsg = "Group is locked for writing. Locked by: ";
		errmsg << mGroupDef->getWLUser()
			<< ", and locked at: " << mGroupDef->getWLTime();
		return false;
	}

	// 3. Check whether the user has the right
	// (TBD).
	
	// 4. Lock and destroy
	if (!mGroupDef->lockForWriting(mUsername, errmsg)) return false;

	if (!mGroupDef->removeFromDb(mUsername))
	{
		mGroupDef->unlockWriting(mUsername, errmsg);
		return false;
	}

	// 5. Destroy the group table
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	if (!store->dropTable(mGroupDef->getTablename()))
	{
		errmsg = "Failed to drop the table!";
		return false;
	}

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::retrieveObjList(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It retrieves object list. If the table contains too many objects,
	// it will only retrieve the first x number of entries. Subsequent 
	// requests can be used to retrieve the remaining objects. 
	// 	<request>
	// 		<tag name="starting_idx">xxx</tag>
	// 		<tag name="page_size">xxx</tag>
	//		<tag name="order">xxx</tag>
	//	</request>
	//
	// If "starting_idx" is missing, it is the initial request. 
	// If "page_size" is missing, it retrieves the entire table.
	// If it is initial request, it will return the total number of records. 
	//
	// 1. Check whether the group exists
	// 2. Check whether the group is locked for reading
	// 3. Check whether the user has the right to retrieve the list
	// 4. Retrieve
	
	// 1. Check whether the group exists
	//
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	
	if (!mGroupDef)
	{
		errmsg = "Failed to retrieve the group definition!";
		return false;
	}

	// 2. Check whether the group is locked for reading 
	if (mGroupDef->isLockedForReading(mUsername))
	{
		errmsg = "Group is locked for reading. Locked by: ";
		errmsg << mGroupDef->getRLUser()
			<< ", and locked at: " << mGroupDef->getRLTime();
		return false;
	}

	// 3. Check whether the user has the right
	// (TBD).
	
	AosGDObj theobj;
	if (!theobj.retrieveList(node, mGroupDef, errcode, errmsg, response)) 
		return false;

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::lockObj(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It performs one of the following:
	// 1. Lock/unlock for writing
	// 2. Lock/unlock for reading
	// 3. Lock/unlock for creating new objects
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Missing group definition!";
		return false;
	}

	const char *seqno = node->getChildTextByAttr("name", "seqno", 0);
	if (!seqno)
	{
		errmsg = "Missing seqno!";
		return false;
	}

	AosGDObj theobj;
	if (!theobj.retrieveObjBySeqno(seqno, mTablename))
	{
		errmsg = "Failed to retrieve the object!";
		return false;
	}

	if (!theobj.lockObj(node, mUsername, errmsg)) return false;

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}


bool 
AosGroupDataMgr::lockGroup(
		TiXmlElement *node,
		AosXmlRc &errcode,
		OmnString &errmsg, 
		OmnString &response)
{
	// It performs one of the following:
	// 1. Lock/unlock for writing
	// 2. Lock/unlock for reading
	// 3. Lock/unlock for creating new objects
	aos_assert_r(preprocess(node, errcode, errmsg), false);
	if (!mGroupDef)
	{
		errmsg = "Missing group definition!";
		return false;
	}

	if (!mGroupDef->lockGroup(node, mUsername, errmsg)) return false;

	errcode = eAosXmlInt_Ok;
	errmsg = "";
	return true;
}

