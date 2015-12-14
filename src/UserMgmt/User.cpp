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
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 3/23/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "UserMgmt/User.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "SEUtil/DocTags.h"
#include "TinyXml/TinyXml.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosUser::AosUser()
:
OmnObject(OmnClassId::eAosUser),
OmnDbObj(OmnClassId::eAosUser)
{
}


AosUser::~AosUser()
{
}


OmnClassId::E	
AosUser::getClassId() const 
{
	return OmnClassId::eAosUser;
}


int
AosUser::objSize() const
{
	return sizeof(*this);
}


void
AosUser::reset()
{
    mObjid = "";
    mUsername = "";
    mPasswd = "";
    mEngname = "";
    mFirstName = "";
    mLastName = "";
    mEmail = "";
    mWorkEmail = "";
    mOtherEmail = "";
    mCell = "";
    mHomePhone = "";
    mWorkPhone = "";
    mAddress = "";
    mCountry = "";
    mState = "";
    mCity = "";
    mZip = "";
    mQQ = "";
    mMSN = "";
    mNickname = "";
    mProfession = "";
    mSex = ' ';
    mBirthday = "";
    mDescription = "";
    mStatus = ' ';
    mXml = "";
}


OmnString
AosUser::toString() const
{
	OmnString str;
    str << "Class AosUser:"
        << "    mUsername:   " << mUsername
        << "    mFirstName:   " << mFirstName
        << "    mLastName:   " << mLastName
        << "    mEmail:   " << mEmail
        << "    mWorkPhone:   " << mWorkPhone
        << "    mHomePhone:   " << mHomePhone
        << "    mDescription:   " << mDescription
        << "    mStatus:   " << mStatus;

	return str;
}


OmnDbObjPtr	
AosUser::clone() const
{
    AosUserPtr obj = OmnNew AosUser();
	obj.setDelFlag(false);

    obj->mObjid = mObjid;
    obj->mUsername = mUsername;
    obj->mPasswd = mPasswd;
    obj->mEngname = mEngname;
    obj->mFirstName = mFirstName;
    obj->mLastName = mLastName;
    obj->mEmail = mEmail;
    obj->mWorkEmail = mWorkEmail;
    obj->mOtherEmail = mOtherEmail;
    obj->mCell = mCell;
    obj->mHomePhone = mHomePhone;
    obj->mWorkPhone = mWorkPhone;
    obj->mAddress = mAddress;
    obj->mCountry = mCountry;
    obj->mState = mState;
    obj->mCity = mCity;
    obj->mZip = mZip;
    obj->mQQ = mQQ;
    obj->mMSN = mMSN;
    obj->mNickname = mNickname;
    obj->mProfession = mProfession;
    obj->mSex = mSex;
    obj->mBirthday = mBirthday;
    obj->mDescription = mDescription;
    obj->mStatus = mStatus;
    obj->mXml = mXml;

	return obj.getPtr();
}


OmnRslt
AosUser::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user where username='";
    stmt << mUsername << "'";
	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUser: " 
			<< toString() << " from db!" << enderr;
	}

	if (!record)
	{
		// 
		// Didn't find the record
		//
		rslt.setErrmsg("Didn't find the object");
		OmnTraceDb << "Didn't find the object: " << toString() << endl;
		return OmnRslt(OmnErrId::eObjectNotFound, "");
	}

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
AosUser::updateStmt() const
{
    OmnString stmt = "update user set ";
    stmt 
        << "username = '" << mUsername
        << "' passwd = '" << mPasswd
        << "' engname = '" << mEngname
        << "' firstname = '" << mFirstName
        << "' lastname = '" << mLastName
        << "' email = '" << mEmail
        << "' workemail = '" << mWorkEmail
        << "' otheremail = '" << mOtherEmail
        << "' cell = '" << mCell
        << "' homephone = '" << mHomePhone
        << "' workphone = '" << mWorkPhone
        << "' address = '" << mAddress
        << "' country = '" << mCountry
        << "' state = '" << mState
        << "' city = '" << mCity
        << "' zip = '" << mZip
        << "' qq = '" << mQQ
        << "' msn = '" << mMSN
        << "' nickname = '" << mNickname
        << "' profession = '" << mProfession
        << "' sex = " << mSex
        << "  birthday = '" << mBirthday
        << "' description = '" << mDescription
        << "' status = " << mStatus
        << " xml = '" << mXml << "' where objid='"
		<< mObjid << "'";

	return stmt;
}


OmnString
AosUser::removeStmt() const
{
    OmnString stmt = "delete from user where ";
    stmt << "username = " << mUsername;
    return stmt;
}


OmnString
AosUser::removeAllStmt() const
{
    return "delete from user";
}


OmnString
AosUser::existStmt() const
{
    OmnString stmt = "select * from user where ";
    stmt << "username = '" << mUsername << "'";
    return stmt;
}


OmnString
AosUser::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUser 
	// into the database.
    //
    OmnString stmt = "insert into user ("
        "objid, "
        "username, "
		"passwd, "
		"engname, "
        "firstname, "
        "lastname, "
        "email, "
        "workemail, "
        "otheremail, "
        "cell, "
        "homephone, "
        "workphone, "
        "address, "
        "country, "
        "state, "
        "city, "
        "zip, "
        "qq, "
        "msn, "
        "nickname, "
        "profession, "
        "sex, "
        "birthday, "
        "description, "
        "status, "
        "xml) values (";

	char sex = mSex;
	if (mSex != 'm' && mSex != 'f' && mSex != 'u') sex = 'u';
	char status = mStatus;
	if (mStatus == 0) status = 'A';

    stmt<< "'"
    	<< mObjid << "', '"
    	<< mUsername << "', '"
    	<< mPasswd << "', '"
    	<< mEngname << "', '"
    	<< mFirstName << "', '"
    	<< mLastName << "', '"
    	<< mEmail << "', '"
    	<< mWorkEmail << "', '"
    	<< mOtherEmail << "', '"
    	<< mCell << "', '"
    	<< mHomePhone << "', '"
    	<< mWorkPhone << "', '"
    	<< mAddress << "', '"
    	<< mCountry << "', '"
    	<< mState << "', '"
    	<< mCity << "', '"
    	<< mZip << "', '"
    	<< mQQ << "', '"
    	<< mMSN << "', '"
    	<< mNickname << "', '"
    	<< mProfession << "', '"
    	<< sex << "', '"
    	<< mBirthday << "', '"
    	<< mDescription << "', '"
    	<< status << "', '"
    	<< mXml << "')";

    return stmt;
}


OmnRslt
AosUser::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);

    mObjid = 		record->getStr(0, "", rslt);
    mUsername = 	record->getStr(1, "", rslt);
    mPasswd = 		record->getStr(2, "", rslt);
    mEngname = 		record->getStr(3, "", rslt);
    mFirstName = 	record->getStr(4, "", rslt);
    mLastName = 	record->getStr(5, "", rslt);
    mEmail = 		record->getStr(6, "", rslt);
    mWorkEmail = 	record->getStr(7, "", rslt);
    mOtherEmail = 	record->getStr(8, "", rslt);
    mCell = 		record->getStr(9, "", rslt);
    mHomePhone = 	record->getStr(10, "", rslt);
    mWorkPhone = 	record->getStr(11, "", rslt);
    mAddress = 		record->getStr(12, "", rslt);
    mCountry = 		record->getStr(13, "", rslt);
    mState = 		record->getStr(14, "", rslt);
    mCity = 		record->getStr(15, "", rslt);
    mZip = 			record->getStr(16, "", rslt);
    mQQ = 			record->getStr(17, "", rslt);
    mMSN = 			record->getStr(18, "", rslt);
    mNickname = 	record->getStr(19, "", rslt);
    mProfession = 	record->getStr(20, "", rslt);
    mSex = 			record->getChar(21, ' ', rslt);
    mBirthday = 	record->getStr(22, "", rslt);
    mDescription = 	record->getStr(23, "", rslt);
    mStatus = 		record->getChar(24, ' ', rslt);
    mXml = 			record->getStr(25, "", rslt);
	return rslt;
}


AosUserPtr
AosUser::retrieveFromDb(const OmnString &username)
{
	AosUserPtr obj = OmnNew AosUser();
	obj->mUsername = username;
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


bool
AosUser::checkLogin(const OmnString &name, 
		const OmnString &passwd, 
		OmnString &errmsg)
{
	OmnString stmt;
	stmt << "select passwd, status from user where username='"
		<< name << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnTrace << "To retrieve record: " << stmt << endl;
	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	if (!rslt)
	{
		OmnAlarm << "Failed the database operation!" << enderr;
		errmsg = "Database error!";
		return false;
	}

	if (!record)
	{
		errmsg = "User not found!";
		return false;
	}

	OmnString pwd = record->getStr(0, "", rslt);
	char status = record->getChar(1, ' ', rslt);

	if (pwd != passwd)
	{
		errmsg = "Failed the authentication!";
		return false;
	}

	if (status != 'A')
	{
		errmsg = "User account not active!";
		return false;
	}

	return true;
}


bool
AosUser::checkLogin(const OmnString &name, 
		const OmnString &passwd, 
		const OmnString &container,
		const OmnString &tname,
		OmnString &dataid,
		AosXmlTagPtr &xml,
		OmnString &errmsg)
{
	// This function assumes the user information is stored
	// in 'tname'. All users are grouped into 'containers'. 
	// The table has a field 'container' that identifies
	// the container in which a user belongs to. The user
	// information is stored in its xml object that assumes
	// the following:
	// 	<user ...
	// 		acnt_status="xxx"
	// 		username="xxx"
	// 		password="xxx"
	// 		...>
	// 	</user>
	// 
	// This is a temporary solution. 
	//
	xml = getUserRcd(name, container, dataid, errmsg);
	if (!xml)
	{
		errmsg = "User not found!";
		return false;
	}

	OmnString status = xml->getAttrStr("acnt_status", "");
	if (status != AOSUSRSTATUS_ACTIVE)
	{
		errmsg = "Account Invalid: ";
		errmsg << status;
		return false;
	}
	
	OmnString pwd = xml->getAttrStr("password", "");
	if (pwd != passwd)
	{
		errmsg = "Password mismatch!";
		return false;
	}

	return true;
}


AosXmlTagPtr
AosUser::getUserRcd(
		const OmnString &name, 
		const OmnString &ctnr, 
		OmnString &dataid, 
		OmnString &errmsg)
{
	OmnString container = ctnr;
	if (container == "") container = "objtable_a000001PD";
	OmnString origStmt = "select xml, dataid from objtable ";
	origStmt << " where container ='"
		<< container << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();

	OmnDbTablePtr table;
	OmnDbRecordPtr record;
	int startIdx = 0;
	int psize = 100;
	AosXmlTagPtr xml;
	while (1)
	{
		OmnString stmt = origStmt;
		stmt << " limit " << startIdx << ", " << psize;
		startIdx += psize;

		OmnRslt rslt = store->query(stmt, table);
		if (!rslt || !table)
		{
			OmnAlarm << "Failed the database operation!" << enderr;
			errmsg = "Database error!";
			return NULL;
		}

		if (table->entries() <= 0)
		{
			errmsg = "User not found!";
			return NULL;
		}

		table->reset();
		while (table->hasMore())
		{
			record = table->next();
			OmnString value = record->getStr(0, "", rslt);
			AosXmlParser parser;
			xml = parser.parse(value, "" AosMemoryCheckerArgs);
			if (!xml) continue;
			xml= xml->getFirstChild();
			if (!xml) continue;

			int unameLen = 0;
			u8 *uname = xml->getAttr("username", unameLen);
			if (uname && strncmp((char *)uname, name.data(), unameLen) == 0)
			{
				// Found the user record and password is good.
				dataid = record->getStr(1, "", rslt);
				return xml;
			}
		}
	}

	return 0;
}


bool 
AosUser::createUser(
			TiXmlElement *userdef,
			AosXmlRc &errcode, 
			OmnString &errmsg)
{
	// This function creates a new user based on 'userdef', which 
	// should be in the form: 
	// 	<user .../>
	errcode = eAosXmlInt_General;

	// Check whether the user already exists
	mUsername = userdef->Attribute("username");
	if (mUsername == "")
	{
		errmsg = "Missing username!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (existInDb())
	{
		errmsg = "User already exist: ";
		errmsg << mUsername;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	mObjid = "user_";
	mObjid << mUsername;

	// Retrieve all the contents and save the object. 
    mPasswd = 		userdef->Attribute("passwd");
    mEngname = 		userdef->Attribute("engname");
    mFirstName = 	userdef->Attribute("firstname");
    mLastName = 	userdef->Attribute("lastname");
    mEmail = 		userdef->Attribute("email");
    mWorkEmail = 	userdef->Attribute("workemail");
    mOtherEmail = 	userdef->Attribute("otheremail");
    mCell = 		userdef->Attribute("cell");
    mHomePhone = 	userdef->Attribute("homephone");
    mWorkPhone = 	userdef->Attribute("workphone");
    mAddress = 		userdef->Attribute("address");
    mCountry = 		userdef->Attribute("country");
    mState = 		userdef->Attribute("state");
    mCity = 		userdef->Attribute("city");
    mZip = 			userdef->Attribute("zip");
    mQQ = 			userdef->Attribute("qq");
    mMSN = 			userdef->Attribute("msn");
    mNickname = 	userdef->Attribute("nickname");
    mProfession = 	userdef->Attribute("profession");
    mBirthday = 	userdef->Attribute("birthday");
    mDescription = 	userdef->Attribute("desc");

	const char *tt = userdef->Attribute("sex");
	if (tt) mSex = tt[0];

	tt = userdef->Attribute("status");
	if (tt) mStatus = tt[0];
	else mStatus = 'A';

	ostringstream oss(ostringstream::out);
	oss << *userdef;
	mXml = oss.str().c_str();

	if (!saveToDb())
	{
		errmsg = "Failed to create the user!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	errcode = eAosXmlInt_Ok;
	return true;
}


OmnString
AosUser::getUserRealname(
		const OmnString &name, 
		const OmnString &container)
{
	OmnString dataid;
	OmnString errmsg;
	AosXmlTagPtr xml = getUserRcd(name, container, dataid, errmsg);
	if (!xml) return "";
	return xml->getAttrStr("__name", "");
}

