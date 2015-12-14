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
// 05/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/ObjectWrapper.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Book/Tag.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Random/RandomUtil.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Containers.h"
#include "SEUtil/IdGen.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "TinyXml/TinyXml.h"
#include "TorturerWrappers/ImgConverter.h"
#include "TorturerWrappers/StdCtnrId.h"
#include "UserMgmt/User.h"
#include "Util/File.h"
#include "Util/StrSplit.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/Server/MainUtil.h"
#include "XmlInterface/Server/XintUtil.h"



AosImgConverterPtr AosObjectWrapper::mImgConverter;
static AosIdGenPtr sgIdGen;
static AosIdGenPtr sgVersionIdGen;
static AosIdGenPtr sgImagesTableIdGen;
static AosIdGenPtr sgImagesIdGen;
static u32 sgTmpDirSeqno = 100;

AosObjectWrapper::AosObjectWrapper()
{
	if (mImgConverter) 
	{
		mImageDir = AosGetOpenLzHome();
		mImageDir << "/images/";
	}
	
	mDirname = getImageDir();
}


AosObjectWrapper::~AosObjectWrapper()
{
}


OmnString
AosObjectWrapper::getImageDir()
{
	return "a2/";
}


bool
AosObjectWrapper::getObject(
		const OmnString &tname,
		const OmnString &objid_fname,
		const OmnString &objid, 
		const char *ffs,
		OmnString &contents,
		OmnString &errmsg)
{
	OmnString stmt = "select ";

	// Retrieve the field names definition. It should be in the form:
	// 	"fname|$|mapped_name|$|type|$$|..."
	const int max_fields = 50;
	OmnString fnames[max_fields];
	OmnString mapped_names[max_fields];
	char ftypes[max_fields];

	char *fparts[50];
	char *pps[10];
	int num = aos_sstr_split(ffs, "|$$|", fparts, 50);
	aos_assert_r(num > 0, false);
	for (int i=0; i<num; i++)
	{
		int nn = aos_sstr_split(fparts[i], "|$|", pps, 10);
		aos_assert_r(nn == 3, false);
		if (i != 0) stmt << ", ";
		stmt << pps[0];

		fnames[i] = pps[0];
		mapped_names[i] = pps[1];
		ftypes[i] = pps[2][0];
		aos_str_split_releasemem(pps, nn);
	}
	aos_str_split_releasemem(fparts, num);

	stmt << " from " << tname << " where " 
		<< objid_fname << " = \"" << objid << "\"";

	OmnTrace << "To retrieve object: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		errmsg = "Failed to retrieve Object: ";
		errmsg << stmt << " from db!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!record)
	{
		errmsg = "Object not found: ";
		errmsg << stmt;
		return false;
	}

	contents = "<Contents tname=\"";
	contents << tname << "\" objid=\"" << objid << "\" ";
	int subtag_found = false;
	for (int i=0; i<num; i++)
	{
		switch (ftypes[i])
		{
		case '1':
			 // Attribute
			 if (subtag_found)
			 {
				 errmsg = "Attribute-type fields cannot be after "
					 "a subtag-type fields: ";
				 errmsg << ffs;
				 OmnAlarm << errmsg << enderr;
				 return false;
			 }
			 contents << fnames[i] << "=\"" << record->getStr(i, "", rslt)
				 << "\" ";
			 break;

		case '2':
			 // Escaped subtag
			 if (!subtag_found) contents << ">";
			 subtag_found = true;
			 contents << "<" << fnames[i] 
				 << "><![CDATA[" << record->getStr(i, "", rslt)
				 << "]]></" << fnames[i] << ">";
			 break;

		case '3':
			 // Non-escaped subtag
			 if (!subtag_found) contents << ">";
			 subtag_found = true;
			 contents << "<" << fnames[i]
				 << ">" << record->getStr(i, "", rslt) 
				 << "</" << fnames[i] << ">";
			 break;

		default:
			 errmsg = "Unrecognized field type. The ";
			 errmsg << i << "-th field. Fields: " << ffs; 
			 OmnAlarm << errmsg << enderr;
			 return false;
		}
	}
	contents << "</Contents>";

	if (tname == "objtable" || tname == "images")
	{
		// Need to update the read count
		// aos_assert_r(updateCounter(AOSTAG_CNTCM, objid, 1, errmsg), false);
		aos_assert_r(updateCounter("zky_ctnrs", objid, 1, errmsg), false);
	}
	return true;
}


// 
// Description:
// This member function retrieves the named Object. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosObjectWrapper::getObject(const OmnString &name, OmnString &errmsg)
{
	// 
	// It retrieves the RVG identified by "name" from the database.
	// 
	char *parts[2];
	int num = aos_str_split(name.data(), '_', parts, 2);
	if (num <= 1)
	{
		errmsg = "Invalid objid: ";
		errmsg << name;
		return false;
	}

	mTablename = parts[0];
    OmnString stmt;
	if (mTablename == "objtable" || mTablename == "images")
	{
		stmt = "select dataid, tags, xml from ";
		stmt << mTablename << " where dataid='" << name << "'";
	}
	else if (mTablename == "vertab")
	{
		stmt = "select dataid, origobj, xml from ";
		stmt << mTablename << " where dataid='" << name << "'";
	}
	else
	{
 		stmt = "select name, keywords, xml from ";
		stmt << mTablename << " where name='" << name << "'";
	}
	aos_str_split_releasemem(parts, 2);
	
	OmnTrace << "To retrieve Object: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Object: " 
			<< stmt << " from db!" << enderr;
		return false;
	}
	if (!record) 
	{
		errmsg = "Object not found: ";
		errmsg << name;
		return false;
	}
	return serializeFromRecord(record);
}


OmnRslt
AosObjectWrapper::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
   	mName 	   	= record->getStr(0, "NoName", rslt);
   	mTags 		= record->getStr(1, "", rslt);
   	mXml       	= record->getStr(2, "", rslt);

OmnTrace << "xml: " << mXml << endl;

	return rslt;
}


OmnRslt
AosObjectWrapper::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from ";
	stmt << mTablename << " where "
    	<< "name='" << mName << "'";

	
	OmnTrace << "To retrieve Object: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Object: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple Objects with the same name: " 
			<< mName << enderr;
		return false;
	}

	table->reset();
	OmnDbRecordPtr record = table->next();
	aos_assert_r(record, false);

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
AosObjectWrapper::updateStmt() const
{
    OmnString stmt = "update ";
	stmt << mTablename << " set ";
    stmt 
        << "xml = '" << mXml
		<< "', description='" << mDesc
		<< "' where name='" << mName << "'";

	return stmt;
}


OmnString
AosObjectWrapper::removeStmt() const
{
    OmnString stmt = "delete from ";
	stmt << mTablename << " where ";
    stmt << "name = '" << mName << "'";

    return stmt;
}


OmnString
AosObjectWrapper::removeAllStmt() const
{
    OmnString stmt = "delete from ";
	stmt << mTablename;

    return stmt;
}


OmnString
AosObjectWrapper::existStmt() const
{
    OmnString stmt = "select * from ";
	stmt << mTablename << " where ";
    stmt << "name = '" << mName << "'";
    return stmt;
}



OmnString
AosObjectWrapper::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into ";
	stmt << mTablename << " ("
        "name, "
		"keywords, "
		"description, "
        "xml) values ('";

	OmnTrace << "Statement: " << stmt << endl;

    stmt << mName << "', '"
		 << mTags << "', '"
		 << mDesc << "', '"
         << mXml << "')";
    return stmt;
}


bool
AosObjectWrapper::objectExist()
{
	OmnString errmsg;
	return getObject(mName, errmsg);
}


bool
AosObjectWrapper::objectExist(const OmnString &name)
{
	AosObjectWrapper wp;
	OmnString errmsg;
	return wp.getObject(name, errmsg);
}


bool 
AosObjectWrapper::modifyObj(
		TiXmlElement *objdef,
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	static OmnStoreMgr *lsStoreMgr = OmnStoreMgr::getSelf();

	errcode = eAosXmlInt_General;
	mXmlObj = objdef;
	if (!objectExist(mName))
	{
		errmsg = "To modify object but not found!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	// Remove all the tags for the object 'mName'
	OmnString stmt = "delete from tag_table where dataid='";
	stmt << mName << "'";
	OmnDataStorePtr store = lsStoreMgr->getStore();
	store->runSQL(stmt);

	// If the object contains 'dirname' attribute, we will
	// use it. 
	OmnString dname = mXmlObj->Attribute("dirname");
	if (dname != "") 
	{
		mDirname = dname;
		mDirname << "/";
	}

	OmnString dfttags = mXmlObj->Attribute("dft_tags");
	mTags = mXmlObj->Attribute("__tags");
	OmnString tags1 = mXmlObj->Attribute("__tags1");
	if (mTags == "") mTags = tags1;
	else if (tags1 != "") mTags << "," << tags1;
	if (dfttags != "")
	{
		if (mTags == "") mTags = dfttags;
		else mTags << "," << dfttags;
	}
	checkTags(mTags); 
	mTags.escapeSymbal('\'', '\'');
	mDesc = mXmlObj->Attribute("desc");
	mDesc.escapeSymbal('\'', '\'');
	mShortname = mXmlObj->Attribute("__name");
	mShortname.escapeSymbal('\'', '\'');
	mThumbnail = mXmlObj->Attribute("__tnail");
	mThumbnail.escapeSymbal('\'', '\'');
	mObjType = mXmlObj->Attribute("__otype");
	mObjType.escapeSymbal('\'', '\'');
	mContainer = mXmlObj->Attribute("__pctnrs");
	mContainer.escapeSymbal('\'', '\'');
	mVVPD = mXmlObj->Attribute("vvpd");
	mVVPD.escapeSymbal('\'', '\'');
	mEVPD = mXmlObj->Attribute("evpd");
	mEVPD.escapeSymbal('\'', '\'');
	mSubtype = mXmlObj->Attribute("__stype");
	mSubtype.escapeSymbal('\'', '\'');

	mOrderName = mXmlObj->Attribute("__ofname");

	// Update the read counter
	OmnString cc = mXmlObj->Attribute("zky_cntmd");
	int readcnt = 0;
	if (cc != "")
	{
		readcnt = atoi(cc.data());
		if (readcnt < 0) readcnt = 0;
	}
	readcnt++;
	cc = "";
	cc << readcnt;
	mXmlObj->SetAttribute("zky_cntmd", cc);

	// Set the version
	OmnString vers = mXmlObj->Attribute(AOSTAG_VERSION);
	OmnString old_version;
	if (vers == "")
	{
		old_version = "1.0";
		vers = "1.1";
	}
	else
	{
		old_version = vers;
		vers = incrementVersion(vers);
	}
	mXmlObj->SetAttribute(AOSTAG_VERSION, vers);

	ostringstream oss(ostringstream::out);
	oss << *objdef;
	mXml = oss.str().c_str();
	mXml.escapeSymbal('\'', '\'');
	
	if (mTablename == "objtable" || mTablename == "images")
	{
		addVersionObj("dataid", old_version);
		modifyTable(old_version);
	}
	else 
	{
		addVersionObj("name", old_version);
		OmnRslt rslt = updateToDb();
		if (!rslt)
		{
			OmnAlarm << "Failed to update the object: " 
				<< rslt.getErrmsg() << enderr;
			errmsg = "Failed to update to database";
			return false;
		}
	}

	OmnString ordervalue = mXmlObj->Attribute(mOrderName);
	if (mTags != "")
	{
		AosTag tagobj(mObjType, mName, mTags,
			mShortname, mThumbnail, ordervalue, mDesc, mUser, mXml);
		tagobj.addEntries();
	}

	if (mThumbnail != "")
	{
		contents = "<results thumbnail=\"";
		contents << mThumbnail << "\"/>";
	}

OmnTrace << "contents: " << contents << endl;
	errcode = eAosXmlInt_Ok;
	return true;
}


OmnString
AosObjectWrapper::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}


void
AosObjectWrapper::setXml(TiXmlNode *child)
{
	ostringstream oss(ostringstream::out);
	oss << *child;
	mXml = oss.str().c_str();
	mXml.escapeSymbal('\'', '\'');
}


// This function creates an object. If successful, upon return, 
// it will set:
//		contents = "<results objid='xxx'/>"
// This is important because the requester did not know the 
// objid yet. 
bool
AosObjectWrapper::createCtnrObj(
		TiXmlNode *root,
		TiXmlElement *cmd,
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// This function creates an object. If the object already exists, 
	// it is an error. But if the table is 'objtable', it will always
	// create a new object (since there is no primary key in objtable).
	//
	// 'root' should be in the form:
	// 	<tag ...>
	// 		<objdef>
	// 			<obj __tags="xxx"
	// 				desc="xxx"
	// 				__name="xxx"
	//				__tnail="xxx"
	// 				__ofname="xxx"
	//				vvpd="xxx"
	//				evpd="xxx">
	// 				...
	// 			</obj>
	// 		</objdef>
	// 	</tag>
	// 	where <objdef ...> is the object to be created. 
	static OmnStoreMgr *lsStoreMgr = OmnStoreMgr::getSelf();
	errcode = eAosXmlInt_General;

	mCreator = root->getChildTextByAttr("name", "username", "");
	mSiteId = root->getChildTextByAttr("name", "siteid", "");
	mOperator = cmd->Attribute("subopr");
	TiXmlNode *objdef = root->FirstChild("objdef");
	if (!objdef)
	{
		errmsg = "Missing object!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	objdef = objdef->FirstChild();
	if (!objdef)
	{
		errmsg = "Object definition is incorrect!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	TiXmlElement *objdef_elem = objdef->ToElement();
	if (!objdef_elem)
	{
		errmsg = "Object is incorrect!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	mXmlObj = objdef_elem;

	// If the object contains 'dirname' attribute, we will
	// use it. 
	OmnString dname = objdef_elem->Attribute("dirname");
	if (dname != "") 
	{
		mDirname = dname;
		mDirname << "/";
	}

	mTablename = cmd->Attribute("ctnid");
	if (mOperator == "comment")
	{
		return createComment(cmd, contents, errmsg, errcode);
	}

	if (mOperator == "uploadImg")
	{
		// Check whether it is to upload a tar file
		OmnString fname = objdef_elem->Attribute("fname");
		if (fname.length() >= 5)
		{
			const char *data = fname.data();
			OmnString ext = &data[fname.length() - 3];
			ext.toLower();
			if (ext == "rar")
			{
				return createImages("rar", contents, errcode, errmsg);
			}

			if (ext == "zip")
			{
				OmnNotImplementedYet;
				return false;
			}
		}
	}

	// See whether the object already contains 'objid'. If
	// yes, use that. 
	OmnString objid = objdef_elem->Attribute("objid");
	if (objid != "")
	{
		OmnString pp[2];
		bool finished;
		AosStrSplit parts(objid.data(), "_", pp, 2, finished);
		if (parts.entries() != 2)
		{
			errmsg = "ObjectID incorrect: ";
			errmsg << objid;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		mTablename = pp[0];
		mName = objid;
		mName.escapeSymbal('\'', '\'');
	}
	else
	{
		if (mTablename == "")
		{
			errmsg = "Missing the container ID!";
			OmnAlarm << errmsg << enderr;
			return false;
		}

		if (mTablename == "objtable")
		{
			if (!sgIdGen) sgIdGen = OmnNew AosIdGen("objtableid");
			OmnString id = sgIdGen->getNextId();
			if (mOperator == "create_ctnr")
				objid = "objtable_a";
			else
				objid = "objtable_b";
			objid << id;	
			mName = objid;
			mName.escapeSymbal('\'', '\'');
		}
		else if (mTablename == "images")
		{
			if (!sgImagesTableIdGen) sgImagesTableIdGen = 
				OmnNew AosIdGen("imagestableid");
			OmnString id = sgImagesTableIdGen->getNextId();
			objid="images_";
			objid << id;	
			mName = objid;
		}
		else
		{
			// Determine the objid
			OmnDataStorePtr store = lsStoreMgr->getStore();
			u64 seqno = store->getSystemSeqno();
			if (seqno == 0)
			{
				errmsg = "Failed to retrieve the system seqno!";
				OmnAlarm << errmsg << enderr;
				return false;
			}

			char buff[100];
			sprintf(buff, "%s_%012llu", mTablename.data(), seqno);
			mName = buff;
			mName.escapeSymbal('\'', '\'');
			objid = buff;
		}
	
		// Set the objid
		objdef_elem->SetAttribute("objid", objid);
	}

	OmnString dfttags = objdef_elem->Attribute("dft_tags");
	mTags = objdef_elem->Attribute("__tags");
	OmnString tags1 = objdef_elem->Attribute("__tags1");
	if (tags1 != "")
	{
		if (mTags != "") mTags << ",";
		mTags << tags1;
	}

	if (dfttags != "")
	{
		if (mTags != "") mTags << ",";
		mTags << dfttags;
	}
	checkTags(mTags); 
	mTags.escapeSymbal('\'', '\'');
	mDesc = objdef_elem->Attribute("desc");
	mDesc.escapeSymbal('\'', '\'');
	mShortname = objdef_elem->Attribute("__name");
	mShortname.escapeSymbal('\'', '\'');
	mSubtype = objdef_elem->Attribute("__stype");
	mSubtype.escapeSymbal('\'', '\'');
	mContainer.escapeSymbal('\'', '\'');
	mThumbnail = objdef_elem->Attribute("__tnail");
	mThumbnail.escapeSymbal('\'', '\'');

	// Retrieve the parent container, which can be defined
	// either in 'cmd' (as 'container') or in objdef_elem
	// (as '__pctnrs')
	mContainer = cmd->Attribute("container");
	if (mContainer == "") 
	{
		mContainer = objdef_elem->Attribute("__pctnrs");
		mContainer.escapeSymbal('\'', '\'');
	}
	else
	{
		mContainer.escapeSymbal('\'', '\'');
		objdef_elem->SetAttribute("__pctnrs", mContainer.data());
	}

	mVVPD = cmd->Attribute("vvpd");
	if (mVVPD == "") mVVPD = objdef_elem->Attribute("vvpd");
	mVVPD.escapeSymbal('\'', '\'');
	mEVPD = cmd->Attribute("evpd");
	if (mEVPD == "") mEVPD = objdef_elem->Attribute("evpd");
	mEVPD.escapeSymbal('\'', '\'');
	mObjType = objdef_elem->Attribute("__otype");
	mOrderName = objdef_elem->Attribute("__ofname");

	// If the container is not empty, need to add a tag:
	// 		ztg_pctnr_<dataid>
	// This can be used to query all the data that is in 
	// the container. 
	aos_assert_r(addDataidTag(AOSZTG ICOMMENTED, mContainer, errmsg, 
		"Invalid Container"), false);

	if (mOperator == "uploadImg")
	{
		// It is to upload image. 
		if (!uploadImage("", objdef_elem, errcode, errmsg)) return false;
	}

	// Set the contents
	objdef_elem->RemoveAttribute("fname");

	mXmlObj = objdef_elem;

	// Save the object
	if (mOperator == "create_ctnr")
	{
		if (mContainer == "")
		{
			errmsg = "Missing the parent container!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
		aos_assert_r(saveTable("objtable", errmsg), false);
	}
	else if (mTablename == "objtable")
	{
		aos_assert_r(saveTable("objtable", errmsg), false);
	}
	else if (mTablename == "images")
	{
		aos_assert_r(processAiFile(errmsg), false);
		aos_assert_r(saveTable("images", errmsg), false);
	}
	else
	{
		// It is to create an object in tables that assumes 
		// 'mName' is the primary key. Need to check whether
		// the object already exists. 
		OmnString stmt = "select name from ";
		stmt << mTablename << " where name='" << mName << "'";
		OmnDbRecordPtr record;
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		if (!store->query(stmt, record))
		{
			errmsg = "Failed the databse operatin!";
			return false;
		}

		if (record)
		{
			errmsg = "Object already exist: ";
			errmsg << mName;
			errcode = eAosXmlInt_ObjectExist;
			return false;
		}

		mXmlObj->SetAttribute(AOS_COUNT_READ, "1");
		mXmlObj->SetAttribute(AOS_COUNT_MOD, "1");
		mXmlObj->SetAttribute(AOS_COUNT_UP, "0");
		mXmlObj->SetAttribute(AOS_COUNT_DOWN, "0");
		mXmlObj->SetAttribute(AOS_COUNT_CMT, "0");
		mXmlObj->SetAttribute(AOS_COUNT_RECOM, "0");
		mXmlObj->SetAttribute(AOS_COUNT_COLECT, "0");

		ostringstream oss(ostringstream::out);
		oss << *mXmlObj;
		mXml = oss.str().c_str();
		mXml.escapeSymbal('\'', '\'');

		OmnRslt rslt = addToDb();
		if (!rslt)
		{
			errmsg = "Failed to create the object (db error)";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}

	if (mTags != "")
	{
		OmnString ordervalue = mXmlObj->Attribute(mOrderName);
		AosTag tagobj(mObjType, mName, mTags,
			mShortname, mThumbnail, ordervalue, mDesc, mUser, mXml);
		tagobj.addEntries();

	}
	errcode = eAosXmlInt_Ok;
	contents = "<results objid=\"";
	contents << objid << "\"";
	if (mThumbnail != "")
		contents << " thumbnail=\"" << mThumbnail << "\"/>";
	else
		contents << "/>";
	
	return true;
}


bool
AosObjectWrapper::uploadImage(
		const OmnString &tn_filename,
		TiXmlElement *objdef_elem,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'objdef_elem' should contain the following attributes:
	// 	<obj fname="xxx"
	// 		tn="true|false"
	// 		sm="true|false"
	//		md="true|false"
	//		lg="true|false"
	//		hg="true|false"
	// 	/>
	static OmnStoreMgr *lsStoreMgr = OmnStoreMgr::getSelf();

	u64 seqno;
	bool needToMove = false;
	char fileCat, fileType;
	OmnString path;

	// Determine the seqno
	OmnString orig_fn;
	if (tn_filename == "")
	{
		needToMove = true;
		orig_fn = objdef_elem->Attribute("fname");
		if (orig_fn == "")
		{
			errmsg = "Missing the filename in uploading file!";
			OmnAlarm << errmsg << enderr;
			return false;
		}

		// Retrieve the image seqno
		OmnDataStorePtr store = lsStoreMgr->getStore();
		seqno = store->getImageSeqno();
		if (seqno == 0)
		{
			errmsg = "Failed to retrieve the image seqno!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}
	else
	{
		// The filename is already given. Need to retrieve 
		// the seqno from the filename. Filename is in the
		// form: 
		//	xxx/xxx/CT<seqno>.eee
		// where 'xxx/xxx...' is the path, 'C' is the file
		// category, 'T' is the file type, and <seqno> is
		// the file's seqno. 
		OmnString pp[20];
		bool finished;
		AosStrSplit parts(tn_filename.data(), "/", pp, 20, finished);
		aos_assert_r(parts.entries() > 0, false);

		path = "";
		for (int i=0; i<parts.entries()-1; i++) path << pp[i];	

		OmnString fn = pp[parts.entries()-1];
		char *fnstr = (char *)fn.data();
		int flen = fn.length();
		aos_assert_r(flen >= 7, false);
		fnstr[flen-4] = 0;	
		seqno = ::atoll(&fnstr[2]);
		fileCat = fnstr[0];
		fileType = fnstr[1];

		orig_fn = "";
		orig_fn << fileCat << 'o' << seqno << "." << &fnstr[flen-3];
	}

	// Determine the file extension
	OmnString the_orig_fn = orig_fn;
	orig_fn.escapeFnameWS();
	int len = orig_fn.length();
	if (len < 5)
	{
		errmsg = "File name too short to be a valid image file name: ";
		errmsg << orig_fn;
		return false;
	}

	// Currently we assume the file always ends with an extension
	// The extension is three-char long.
	const char * const data = orig_fn.data();
	char c1 = data[len-4];
	if (c1 != '.')
	{
		errmsg = "File name syntax error: ";
		errmsg << orig_fn;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	OmnString fileext = &data[len-4];

	// Determine the original file resolution
	OmnString identifyFname;
	if (needToMove)
	{
		// Move the file to the right place
		// There should be a file in /tmp/<fname>
		identifyFname = "/tmp/";
		identifyFname << orig_fn;
	}
	else
	{
		identifyFname = mImageDir;
		identifyFname << "/" << path << "/" 
			<< fileCat << "o" << seqno << fileext;
	}

	OmnString idenFn = "/tmp/iden";
	idenFn << seqno << ".txt";

	OmnString idenCmd = "identify ";
	idenCmd << identifyFname << " > " << idenFn;
	system(idenCmd.data());
	OmnFile ffile(idenFn, OmnFile::eReadOnly);
	char word1[100];
	char word2[100];
	char word3[100];
OmnTrace << "idenfn: " << idenFn << ", " << idenCmd << endl;
	if (ffile.getWord(word1, 100) <= 0 ||
	    ffile.getWord(word2, 100) <= 0 ||
		(strcmp(word1, "identify:") == 0 &&
		strcmp(word2, "unable") == 0))
	{
		// The file does not exist. 
		errmsg = "Uploaded file is not an image file: ";
		errmsg << the_orig_fn;
		OmnAlarm << errmsg << enderr;
		
		// Remove the original file
		OmnString cmd = "rm /tmp/";
		cmd << orig_fn;
		system(cmd);

		cmd = "rm /tmp/iden";
		cmd << seqno << ".txt";
		system(cmd);

		return false;
	}	

	aos_assert_r(ffile.getWord(word3, 100) > 0, false);
	int ww, hh;
	sscanf(word3, "%dx%d", &ww, &hh);
	int maxside = (ww > hh)?ww:hh;
cout << __FILE__ << ":" << __LINE__ << " to set dimen: " << word3 << endl;
	objdef_elem->SetAttribute("__dimen", word3);

	ffile.closeFile();
	
	// Remove the file
	idenCmd = "rm ";
	idenCmd << idenFn;
OmnTrace << "To remove the file: " << idenCmd << endl;
	system(idenCmd);

	char filetype;
	int maxsize;
	getFileLength(maxside, filetype, maxsize);

	fileext.toLower();
	if (needToMove)
	{
		OmnString origfn;
		origfn << filetype << "o" << seqno << fileext;
		OmnString mvcmd = "mv ";
		mvcmd << identifyFname << " " << mImageDir 
			<< mDirname << origfn;
cout << "---------------- To move file: " << mvcmd << endl;
		system(mvcmd.data());
	}
	else
	{
		// Check whether the file type (first char) was changed. 
		// If yes, we need to move.
		if (fileCat != filetype)
		{
			// Need to move
			OmnString mvcmd = "mv ";
			mvcmd << mImageDir << mDirname << fileCat << "o" 
				<< seqno << fileext
				<< " " << mImageDir << mDirname << filetype << "o"
				<< seqno << fileext;
			system(mvcmd.data());

			// Need to remove/rename
			switch (fileCat)
			{
			case 'a':
				 // Need to move the thumbnail file
				 switch (filetype)
				 {
				 case 'a':
					  break;

				 case 'b':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  removeFile(fileCat, 's', seqno, fileext);
					  break;

				 case 'c':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  removeFile(fileCat, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  break;

				 case 'd':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  removeFile(fileCat, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  break;

				 case 'e':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  removeFile(fileCat, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  removeFile(fileCat, 'h', seqno, fileext);
					  break;
				 }
				 break;

			case 'b':
				 switch (filetype)
				 {
				 case 'a':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  break;

				 case 'b':
					  break;

				 case 'c':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  break;

				 case 'd':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  break;

				 case 'e':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  removeFile(fileCat, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  removeFile(fileCat, 'h', seqno, fileext);
					  break;
				 }
				 break;

			case 'c':
				 switch (filetype)
				 {
				 case 'a':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  break;

				 case 'b':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  break;

				 case 'c':
					  break;

				 case 'd':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  break;

				 case 'e':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  removeFile(fileCat, 'l', seqno, fileext);
					  removeFile(fileCat, 'h', seqno, fileext);
					  break;

				 }
				 break;

			case 'd':
				 switch (filetype)
				 {
				 case 'a':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  break;

				 case 'b':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  break;

				 case 'c':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  break;

				 case 'd':
					  break;

				 case 'e':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  renameFile(fileCat, filetype, 'l', seqno, fileext);
					  removeFile(fileCat, 'h', seqno, fileext);
					  break;
				 }
				 break;

			case 'e':
				 switch (filetype)
				 {
				 case 'a':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  break;

				 case 'b':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  break;

				 case 'c':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  break;

				 case 'd':
					  renameFile(fileCat, filetype, 't', seqno, fileext);
					  renameFile(fileCat, filetype, 's', seqno, fileext);
					  renameFile(fileCat, filetype, 'm', seqno, fileext);
					  renameFile(fileCat, filetype, 'l', seqno, fileext);
					  break;

				 case 'e':
					  break;
				 }
			}
		}
	}

	orig_fn = mImageDir;
	orig_fn << mDirname << filetype << "o" << seqno << fileext;
	mImgConverter->addRequest(maxsize, filetype, orig_fn, seqno, 
		fileext, ww, hh, mDirname);
	mThumbnail = mDirname;
	mThumbnail << filetype << "t" << seqno << fileext;
	mThumbnail.escapeSymbal('\'', '\'');
	objdef_elem->SetAttribute("__tnail", mThumbnail.data());
	return true;
}


void
AosObjectWrapper::getFileLength(
		const int maxside,
		char &filetype,
		int &maxsize)
{
	// Determine the original image filename. File names are encoded
	// as: 
	// 	a: t				t: 64
	// 	b: t, s				s: 128
	// 	c: t, s, m			c: 256
	// 	d: t, s, m, l		d: 512
	// 	e: t, s, m, l, h	e: 1024
	// 	o: only original
	//
	// 	Filename encoding:
	// 	Char 1: type (a, b, c, d, or e)
	//	Char 2: type 1 (t, s, m, l, h, o)
	//	Other Chars: a sequence number used as unique ID
	//
	if (maxside >= AosImgConverter::eLargeSize) 
	{
		filetype = 'e';
		maxsize = eHugeIdx;
	}
	else if (maxside >= AosImgConverter::eMediumSize) 
	{
		filetype = 'd';
		maxsize = eLargeIdx;
	}
	else if (maxside >= AosImgConverter::eSmallSize) 
	{
		filetype = 'c';
		maxsize = eMediumIdx;
	}
	else if (maxside >= AosImgConverter::eThumbnailSize) 
	{
		filetype = 'b';
		maxsize = eSmallIdx;
	}
	else 
	{
		filetype = 'a';
		maxsize = eThumbIdx;
	}
}


bool 
AosObjectWrapper::delObj(
		TiXmlNode *cmd, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	static OmnDataStorePtr lsStore = OmnStoreMgrSelf->getStore();

	errcode = eAosXmlInt_General;
	mName = cmd->getChildTextByAttr("name", "objid", "");
	mName.escapeSymbal('\'', '\'');
	if (mName == "")
	{
		errmsg = "Missing the objid!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	// Object type serves as the table name
	char *substrs[3];
	int num = aos_str_split((char *)mName.data(), '_', substrs, 3);
	if (num <= 0)
	{
		errmsg = "Internal error!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mTablename = substrs[0];

	// Retrieve the object from the database. 
	OmnString stmt = "select xml from ";
	if (mTablename == "objtable" || mTablename == "images")
	{
		stmt << mTablename << " where dataid='" << mName << "'";
	}
	else
	{
		stmt << mTablename << " where name='" << mName << "'";
	}
	OmnDbRecordPtr record;
	OmnRslt rslt = lsStore->query(stmt, record);
	if (!rslt || !record)
	{
		errmsg = "Object not found: ";
		errmsg << mName;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mXml = record->getStr(0, "", rslt);
	mXml.escapeSymbal('\'', '\'');

	if (mTablename == "objtable" || mTablename == "images")
	{
		OmnString stmt = "delete from ";
		stmt << mTablename << " where dataid='";
		stmt << mName << "'";
OmnTrace << "To run stmt: " << stmt << endl;
		rslt = lsStore->runSQL(stmt);
	}
	else
	{
		rslt = removeFromDb();
	}

	if (!rslt)
	{
		errmsg = "Failed to delete object: ";
		errmsg << mName;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	errcode = eAosXmlInt_Ok;

	// Need to check the object type. 
	TiXmlDocument thedoc;
	thedoc.Parse(mXml);
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlElement *elem;
	if (!root || !(elem = root->ToElement()))
	{
		OmnAlarm << "Retrieved object is not a valid XML object: "
			<< mXml << enderr;
		return true;
	}

	OmnString dirname = elem->Attribute("dirname");
	if (dirname != "") 
	{
		mDirname = dirname;
		mDirname << "/";
	}

	mObjType = elem->Attribute("__otype");
cout << __FILE__ << ":" << __LINE__ << ": objtype: " << mObjType << endl;
	if (mObjType == "image")
	{
		// It is an image object. Need to remove all the image files. 
		OmnString tn = elem->Attribute("__tnail");
cout << __FILE__ << ":" << __LINE__ << ": tnail: " << tn << endl;
		if (tn != "")
		{
			removeAllImageFiles(tn);
		}
	}
	AosTag tag;
	tag.deleteEntries(mName);
	return true;
}


void
AosObjectWrapper::renameFile(
		const char fileCat, 
		const char filetype, 
		const char tt, 
		const u64 seqno, 
		const char *fileext)
{
	OmnString cmd = "mv ";
	cmd << mImageDir << mDirname << fileCat << tt << seqno << fileext
		<< " " << mImageDir << mDirname << filetype 
		<< tt << seqno << fileext;
	system(cmd);
}


void
AosObjectWrapper::removeFile(
		const char fileCat, 
		const char tt, 
		const u64 seqno, 
		const char *fileext)
{
	OmnString cmd = "rm ";
	cmd << mImageDir << mDirname << fileCat << tt << seqno << fileext;
	system(cmd);
}


static char sgDelFileTags[] = {'t', 's', 'm', 'l', 'h'};

bool
AosObjectWrapper::removeAllImageFiles(const OmnString &tn)
{
	// This function removes all the image files. 'tn' is the
	// thumbnail filename, which is in the form:
	//	"path/<A><B><seqno><ext>
	// where:
	// 	<A> identifies the file type
	//	<B> identifies the file subtype
	//	<seqno> is a sequence number
	//  <ext> is the file's extension. 
	// For more information about the meaning of these variables, 
	// please refer to function 'uploadImage(...)'.
	OmnString pp[20];
	bool finished;
	AosStrSplit parts(tn.data(), "/", pp, 20, finished);
	aos_assert_r(parts.entries() > 0, false);
	OmnString fn = pp[parts.entries()-1];
	int len = fn.length();
	aos_assert_r(len >= 7, false);
	char *data = (char *)fn.data();
	OmnString fileext(&data[len-4]);
	data[len-4] = 0;
	u64	seqno = ::atoll(&data[2]);
	char c = data[0];
	removeFile(c, 'o', seqno, fileext);
	removeFile(c, 'i', seqno, fileext);
	removeFile(c, 'y', seqno, fileext);
	int ss = c - 'a';
	for (int i=0; i<=ss; i++)
	{
		removeFile(c, sgDelFileTags[i], seqno, fileext);
	}
	return true;
}


void
AosObjectWrapper::setImgConverter(const AosImgConverterPtr &c)
{
	mImgConverter = c;
}


OmnString
AosObjectWrapper::getTablename(
		const OmnString &dataid, 
		OmnString &idfname) 
{
	OmnString pp[20];
	bool finished;
	AosStrSplit split(dataid.data(), "_", pp, 20, finished);
	aos_assert_r(split.entries() > 1, "");
	if (pp[0] == "images" || pp[0] == "objtable")
	{
		idfname = "dataid";
		return pp[0];
	}

	idfname = "name";
	return pp[0];
}


bool
AosObjectWrapper::updateCounter(
	const OmnString &counter_name,
	const OmnString &dataid,
	const int delta,
	OmnString &errmsg)
{
	OmnString idfname;
	OmnString tname = getTablename(dataid, idfname);
	if (tname == "")
	{
		errmsg = "Failed to retrieve the table name: ";
		errmsg << dataid;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosXmlTagPtr root = retrieveXml(tname, idfname, dataid, errmsg);
	aos_assert_r(root, false);
	AosXmlTagPtr xml = root->getFirstChild();
	aos_assert_r(xml, false);

	OmnString vv = xml->getAttrStr(counter_name, "");
	int count = 0;
	if (vv != "") count = atoi(vv.data());
	if (count < 0) count = 0;
	count += delta;
	xml->setAttr(counter_name, count);

	aos_assert_r(updateXml(xml, tname, idfname, dataid, errmsg), false);
	return true;
}


AosXmlTagPtr
AosObjectWrapper::retrieveXml(
	const OmnString &tname, 
	const OmnString &idfname,
	const OmnString &dataid,
	OmnString &errmsg)
{
	// It retrieves the xml identified by 'dataid'. 
	
	OmnString stmt = "select xml from ";
	stmt << tname << " where " << idfname << "='" << dataid << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	if (!rslt || !record)
	{
		errmsg = "Failed to retrieve the record: ";
		errmsg << dataid;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	OmnString value = record->getStr(0, "", rslt);
	if (value == "")
	{
		errmsg = "Object does not have an XML: ";
		errmsg << dataid;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(value, "");
	if (!xml)
	{
		errmsg = "Failed to retrieve the XML: ";
		errmsg << dataid << "\n" << value;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return xml;
}


bool
AosObjectWrapper::updateXml(
	const AosXmlTagPtr &xml,
	const OmnString &tname,
	const OmnString &idfname, 
	const OmnString &dataid,
	OmnString &errmsg)
{
	OmnString value((char *)xml->getData(), xml->getDataLength());
	value.escapeSymbal('\'', '\'');

	OmnString stmt = "update ";
	stmt << tname << " set xml='" << value << "' where " << idfname << "='"
		<< dataid << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt = store->runSQL(stmt);
	if (!rslt)
	{
		errmsg = "Failed to update database!";
		OmnAlarm << enderr;
		return false;
	}
	return true;
}


bool
AosObjectWrapper::saveTable(const OmnString &tname, OmnString &errmsg)
{
	// Table:
	//	dataid
	//	name
	//	tnail
	//	container
	//	tags
	//	vvpdname
	//	evpdname
	//	type
	//	subtype
	//	creator
	//	createtime
	//	updatetime
	//	xml
	OmnNotImplementedYet;

	/* Not used anymre, Chen Ding, 05/20/2010
	mXmlObj->SetAttribute(AOS_COUNT_READ, "1");
	mXmlObj->SetAttribute(AOS_COUNT_MOD, "1");
	mXmlObj->SetAttribute(AOS_COUNT_UP, "0");
	mXmlObj->SetAttribute(AOS_COUNT_DOWN, "0");
	mXmlObj->SetAttribute(AOS_COUNT_CMT, "0");
	mXmlObj->SetAttribute(AOS_COUNT_RECOM, "0");
	mXmlObj->SetAttribute(AOS_COUNT_COLECT, "0");
	mXmlObj->SetAttribute("zky_cdate", OmnGetTime());
	mXmlObj->SetAttribute("zky_mdate", OmnGetTime());
	mXmlObj->SetAttribute("zky_csdate", OmnGetMDY());
	mXmlObj->SetAttribute("zky_msdate", OmnGetMDY());
	mXmlObj->SetAttribute("zky_realname", 
			AosUser::getUserRealname(mCreator, mSiteId)); 

	ostringstream oss(ostringstream::out);
	oss << *mXmlObj;
	mXml = oss.str().c_str();
	mXml.escapeSymbal('\'', '\'');

	OmnString stmt = "insert into ";
	stmt << tname << " (dataid, name, tnail, container, "
		"type, subtype, creator, createtime, updatetime, "
		"tags, vvpdname, evpdname, xml) values ('";
	stmt << mName << "', '"
		<< mShortname << "', '"
		<< mThumbnail << "', '"
		<< mContainer << "', '"
		<< mObjType << "', '"
		<< mSubtype << "', '"
		<< mCreator << "', "
		<< "from_unixtime(unix_timestamp()), "
		<< "from_unixtime(unix_timestamp()), '"
		<< mTags << "', '"
		<< mVVPD << "', '"
		<< mEVPD << "', '"
		<< mXml << "')";
	
	OmnTrace << "To run stmt: " << stmt << endl;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	if (!store->runSQL(stmt))
	{
		errmsg = "Failed the database operation!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
	*/
	return false;
}


bool
AosObjectWrapper::modifyTable(const OmnString &oldversion)
{
	// images table:
	//	dataid
	//	name
	//	tnail
	//	container
	//	tags
	//	vvpdname
	//	evpdname
	//	type
	//	subtype
	//	creator
	//	createtime
	//	updatetime
	//	xml
	
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();

	OmnString stmt = "update ";
	stmt << mTablename << " set ";
	stmt << "name='" << mShortname 
		<< "', tnail='" << mThumbnail 
		<< "', tags='" << mTags 
		<< "', container='" << mContainer
		<< "', vvpdname='" << mVVPD
		<< "', evpdname='" << mEVPD
		<< "', type='" << mObjType
		<< "', subtype='" << mSubtype
		<< "', updatetime= from_unixtime(unix_timestamp())"
		<< ", xml='" << mXml
		<< "' where dataid='" << mName << "'";
	
	OmnTrace << "To run stmt: " << stmt << endl;
	OmnRslt rslt;
	aos_assert_r(store->runSQL(stmt), false);
	return true;
}


bool
AosObjectWrapper::uploadOneImage(const OmnString &orig_fn)
{
	static OmnStoreMgr *lsStoreMgr = OmnStoreMgr::getSelf();
	OmnDataStorePtr store = lsStoreMgr->getStore();

	// Retrieve the image seqno
	u64 seqno = store->getImageSeqno();
	aos_assert_r(seqno > 0, false);

	// Determine the file extension.
	// Currently we assume the file always ends with an extension
	// The extension is three-char long.
	OmnString fname = orig_fn;
	fname.escapeFnameWS();
	int len = fname.length();
	aos_assert_r(len >= 5, false);

	const char * const data = fname.data();
	char c1 = data[len-4];
	aos_assert_r(c1 == '.', false);
	OmnString fileext = &data[len-4];

	// Determine the original file resolution
	// This is done by running the command 'identify', 
	// getting the results into a temporary file, 
	// reading the results and getting the results:
	// 	'identify /tmp/<tmpDir>/<fname> > /tmp/iden<seqno>.txt'
	OmnString idenFn = "/tmp/iden";
	idenFn << seqno << ".txt";

	OmnString idenCmd = "identify ";
	idenCmd << fname << " > " << idenFn;
cout << __FILE__ << ":" << __LINE__ << ": to identify: " << idenCmd << endl;
	system(idenCmd.data());
	OmnFile ffile(idenFn, OmnFile::eReadOnly);
	char word1[100];
	char word2[100];
	char word3[100];
	if (ffile.getWord(word1, 100) <= 0 ||
	    ffile.getWord(word2, 100) <= 0 ||
		(strcmp(word1, "identify:") == 0 &&
		strcmp(word2, "unable") == 0))
	{
		// The first two words are:
		// 	"identify: unable ...". This means that
		// 	The file does not exist. 
		OmnAlarm << "File not found: " << fname << enderr;
		
		// Remove the original file
		OmnString cmd = "rm /tmp/";
		cmd << orig_fn;
		system(cmd);

		cmd = "rm /tmp/iden";
		cmd << seqno << ".txt";
		system(cmd);

		return false;
	}	

	aos_assert_r(ffile.getWord(word3, 100) > 0, false);
	int ww, hh;
	sscanf(word3, "%dx%d", &ww, &hh);
cout << __FILE__ << ":" << __LINE__ << " to set dimen: " << word3 << endl;
	mXmlObj->SetAttribute("__dimen", word3);
	ffile.closeFile();
	
	int maxside = (ww > hh)?ww:hh;
	char filetype;
	int maxsize;
	getFileLength(maxside, filetype, maxsize);

	// Remove the file
	idenCmd = "rm ";
	idenCmd << idenFn;
	system(idenCmd);

	fileext.toLower();
	OmnString origfn;
	origfn << filetype << "o" << seqno << fileext;
	OmnString mvcmd = "mv ";
	mvcmd << fname << " " << mImageDir 
		<< mDirname << origfn;
cout << "---------------- To move file: " << mvcmd << endl;
	system(mvcmd.data());

	OmnString origFname = mImageDir;
	origFname << mDirname << filetype << "o" << seqno << fileext;
	mImgConverter->addRequest(maxsize, filetype, origFname, seqno, 
		fileext, ww, hh, mDirname);
	mThumbnail = mDirname;
	mThumbnail << filetype << "t" << seqno << fileext;
	mThumbnail.escapeSymbal('\'', '\'');
	mXmlObj->SetAttribute("__tnail", mThumbnail.data());
	return true;
}


// This function creates an object for each file uploaded
// by an '.rar' file. 
bool
AosObjectWrapper::createImages(
		const OmnString &archiveType,
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 	<cmd __tags="xxx"
	// 		desc="xxx"
	// 		tname="xxx"
	// 		__name="xxx"
	//		__tnail="xxx"
	//		fname="xxx"
	//		sm="true|false"
	//		md="true|false"
	//		lg="true|false"
	//		hg="true|false"/>
	//
	errcode = eAosXmlInt_General;

	OmnString fname = mXmlObj->Attribute("fname");
	aos_assert_re(fname != "", false, errmsg, 
			"Missing the filename!");
	mXmlObj->RemoveAttribute("fname");

	aos_assert_re(mTablename != "", false, errmsg, 
			"Missing the container name!");

	mTags = mXmlObj->Attribute("__tags");
	OmnString tags1 = mXmlObj->Attribute("__tags1");
	if (mTags == "") mTags = tags1;
	else if (tags1 != "") mTags << "," << tags1;
	checkTags(mTags); 
	mTags.escapeSymbal('\'', '\'');
	mDesc = mXmlObj->Attribute("desc");
	mDesc.escapeSymbal('\'', '\'');
	mShortname = mXmlObj->Attribute("__name");
	mShortname.escapeSymbal('\'', '\'');
	mObjType = mXmlObj->Attribute("__otype");
	mSubtype = mXmlObj->Attribute("__stype");
	mOrderName = mXmlObj->Attribute("__ofname");
	mContainer = mXmlObj->Attribute("container");
	mContainer.escapeSymbal('\'', '\'');
	mVVPD = mXmlObj->Attribute("vvpd");
	mVVPD.escapeSymbal('\'', '\'');
	mEVPD = mXmlObj->Attribute("evpd");
	mEVPD.escapeSymbal('\'', '\'');

	// Determine the directory name
	OmnString dname = mXmlObj->Attribute("dirname");
	if (dname != "") 
	{
		mDirname = dname;
		mDirname << "/";
	}

	// Untar the file and get all the filenames.
	// The files will be untared into a temporary
	// directory. 
	OmnString tmpdir = "upimgs";
	tmpdir << sgTmpDirSeqno++;
	OmnString cmd = "mkdir /tmp/";
	cmd << tmpdir;
	system(cmd);

	if (archiveType == "rar")
	{
		cmd = "rar -y e /tmp/";
		cmd << fname << " -C /tmp/" << tmpdir;
		system(cmd);
	}
	else
	{
		errmsg = "Unrecognized archive type: ";
		errmsg << archiveType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString tt = "/tmp/";
	tt << tmpdir;
	OmnFile ff;
	std::list<OmnString> fnames;
	ff.getFiles(tt, fnames, false);
	int size = fnames.size();
	aos_assert_r(size > 0, false);

	aos_assert_r(processAiFile(errmsg), false);
	
	bool finished;
	for (int i=0; i<size; i++)
	{
		OmnString pp[20];
		OmnString ff = fnames.front();
		fnames.pop_front();
		AosStrSplit split(ff.data(), "/", pp, 20, finished);
		mXmlObj->SetAttribute("__ofname", 
			pp[split.entries()-1]);
		procOneFile(ff, errmsg, i); 
	}

	// Remove the tar file
	cmd = "rm /tmp/";
	cmd << fname;
	system(cmd);
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosObjectWrapper::processAiFile(OmnString &errmsg)
{
	// If mXmlObj contains the attribute:
	// 	zky_aifname
	// we need to create an entry for the AI file and set the dataid
	// to 'mXmlObj'. 
	OmnString aifname = mXmlObj->Attribute("zky_aifname");
	OmnString aitfname = mXmlObj->Attribute("zky_aitfname");
	mXmlObj->RemoveAttribute("zky_aifname");
	mXmlObj->RemoveAttribute("zky_aitfname");
	if (aifname == "") return true;

	// Save the tags since this function may modify it.
	OmnString tags = mTags;
	OmnString origTnail = mThumbnail;
	OmnString origDataid = mName;
	OmnString origObjtype = mObjType;
	OmnString origDimen = mXmlObj->Attribute("__dimen");
	OmnString origSubtype = mSubtype;

	// Check whether the AI file exists.
	OmnString aname = "/tmp/";
	aname << aifname;
	if (!OmnFile::fileExist(aname))
	{
		errmsg = "AI file does not exist: ";
		errmsg << aifname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Check whether the thumbnail file exists
	aname = "/tmp/";
	aname << aitfname;
	if (!OmnFile::fileExist(aname))
	{
		errmsg = "AI Thumbnail file does not exist: ";
		errmsg << aitfname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	const char *ddd = aitfname.data();
	int len = aitfname.length();
	if (aitfname.length() < 5 || ddd[len-4] != '.')
	{
		errmsg = "Invalid thumbnail filename: ";
		errmsg << aitfname;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Need to RAR the file
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	u64 seqno = store->getImageSeqno();
	OmnString rarcmd = "rar a ";
	rarcmd << mImageDir << mDirname << "ai_" << seqno << ".rar /tmp/"
		<< aifname;
cout << "---------------- To RAR file: " << rarcmd << endl;
	system(rarcmd.data());

	OmnString rmcmd = "rm /tmp/";
	rmcmd << aifname;
	system(rmcmd.data());

	// Obtain the Vector Image ID 
	if (!sgImagesIdGen) sgImagesIdGen = OmnNew AosIdGen("imageid");
	OmnString vectorid = "zky_vec_";
	vectorid << sgImagesIdGen->getNextId();

	// Process the thumbnail. If the thumbnail name is in the
	// form: images_xxxx, it means an object has already been 
	// created for it. Otherwise, we need to create an object
	// for it. 
	OmnString ppp[20];
	bool finished;
	AosStrSplit split(aitfname.data(), "_", ppp, 20, finished);
	OmnString tnailDataid = aitfname;
	bool tnailStandalone = true;
	if (split.entries() <= 1 || ppp[0] != "images")
	{
		// Add the tag: zky_vectnail and the vector image ID
		mTags = tags;
		if (mTags != "") mTags << ",";
		mTags << "zky_vectnail," << vectorid;
		mXmlObj->SetAttribute("__tags", mTags);
		OmnString fnnn = mXmlObj->Attribute("fname");
		mXmlObj->RemoveAttribute("fname");
		OmnString fff = "/tmp/";
		fff << aitfname;
		procOneFile(fff, errmsg, -1);
		tnailDataid = mName;
		tnailStandalone = false;
		if (fnnn != "") mXmlObj->SetAttribute("fname", fnnn);
	}
	else
	{
		// Need to retrieve the thumbnail filename. 
		OmnString stmt = "select tnail from images where dataid='";
		stmt << aitfname << "'";
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnDbRecordPtr record;
		OmnRslt rslt = store->query(stmt, record);
		if (!rslt || !record)
		{
			errmsg = "Failed to retrieve the thumbnail record: ";
			errmsg << aitfname;
			OmnAlarm << errmsg << enderr;
			return false;
		}
		mThumbnail = record->getStr(0, "", rslt);
	}

	if (!sgImagesTableIdGen) sgImagesTableIdGen = 
		OmnNew AosIdGen("imagestableid");
	OmnString id = sgImagesTableIdGen->getNextId();
	mName = mTablename;
	mName << "_" << id;

	aname = "ai_";
	aname << seqno << ".rar";
	OmnString otype = mXmlObj->Attribute("__otype");
	mTags = tags;
	if (mTags != "") mTags << ",";
	mTags << "zky_vecimg," << vectorid;
	mXmlObj->SetAttribute("__tags", mTags);
	mXmlObj->SetAttribute("__tnail", mThumbnail);
	mXmlObj->SetAttribute("zky_vecfname", aname);
	mXmlObj->SetAttribute("objid", mName);
	mXmlObj->SetAttribute("__otype", "VecImage");
	mXmlObj->SetAttribute("__stype", "");
	mXmlObj->SetAttribute("zky_vectndataid", tnailDataid);
	mXmlObj->RemoveAttribute("__dimen");
	if (!tnailStandalone)
		mXmlObj->SetAttribute("zky_vectnail_bd", "true");
	mObjType = "VecImage";
	mSubtype = "";

	// ostringstream oss(ostringstream::out);
	// oss << *mXmlObj;
	// mXml = oss.str().c_str();
	// mXml.escapeSymbal('\'', '\'');

	aos_assert_r(saveTable("images", errmsg), false);

	if (mTags != "")
	{
		AosTag tagobj(mObjType, mName, mTags,
			mShortname, mThumbnail, mOrderName, mDesc, mUser, mXml);
		tagobj.addEntries();
	}

	mTags = tags;
	if (mTags != "") mTags << ",";
	mTags << vectorid << ",zky_from_vec";
	mXmlObj->SetAttribute("__tags", mTags);
	mXmlObj->SetAttribute("__otype", otype);
	mXmlObj->SetAttribute("__stype", origSubtype);
	mXmlObj->RemoveAttribute("zky_aifname");
	mXmlObj->RemoveAttribute("zky_aitfname");
	mXmlObj->RemoveAttribute("zky_vectndataid");
	mXmlObj->RemoveAttribute("zky_vectnail_bd");
	mObjType = origObjtype;
	mSubtype = origSubtype;
	mXmlObj->SetAttribute("zky_vecdataid", mName);
	mXmlObj->SetAttribute("zky_vecid", vectorid);
	mThumbnail = origTnail;
	mXmlObj->SetAttribute("__tnail", mThumbnail);
	mXmlObj->SetAttribute("__otype", mObjType);
cout << __FILE__ << ":" << __LINE__ << " to set dimen: " << origDimen << endl;
	mXmlObj->SetAttribute("__dimen", origDimen);

	mName = origDataid;
	mXmlObj->SetAttribute("objid", mName);

	ostringstream oss1(ostringstream::out);
	oss1 << *mXmlObj;
	mXml = oss1.str().c_str();
	mXml.escapeSymbal('\'', '\'');

	return true;
}


bool
AosObjectWrapper::procOneFile(
		const OmnString &fname, 
		OmnString &errmsg, 
		const int idx)
{
	// This function processes one file. The filename is identified
	// by 'fname'. The function will assign the file name, move
	// the file, convert the file, and create the db record. In short,
	// it does everything that is needed to import an image into 
	// the system. It assumes that all the data are extracted from
	// the XML object into the member data. It will change the 
	// member data 'mName'. 
	
	// 1. Create the objid
	if (!sgImagesTableIdGen) sgImagesTableIdGen = 
		OmnNew AosIdGen("imagestableid");
	OmnString id = sgImagesTableIdGen->getNextId();
	mName = mTablename;
	mName << "_" << id;

cout << __FILE__ << ":" << __LINE__ << ": got the id: "  << mName << endl;
	aos_assert_r(uploadOneImage(fname), false);

	mXmlObj->SetAttribute("objid", mName);

	ostringstream oss(ostringstream::out);
	oss << *mXmlObj;
	mXml = oss.str().c_str();
	mXml.escapeSymbal('\'', '\'');

	// Save the object
	OmnString sname = mShortname;
	if (idx >= 0) sname << idx;

	OmnString stmt = "insert into ";
	stmt << mTablename << " (dataid, name, tnail, container, "
		"tags, vvpdname, evpdname, type, subtype, creator,"
		"createtime, xml) values ('";
	stmt << mName << "', '"
		<< sname << "', '"
		<< mThumbnail << "', '"
		<< mContainer << "', '"
		<< mTags << "', '"
		<< mVVPD << "', '"
		<< mEVPD << "', '"
		<< mObjType << "', '"
		<< mSubtype << "', '"
		<< mCreator << "', "
		<< "from_unixtime(unix_timestamp()), '"
		<< mXml << "')";
	
OmnTrace << "To run stmt: " << stmt << endl;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	if (!store->runSQL(stmt))
	{
		errmsg = "Failed the database operation!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (mTags != "")
	{
		AosTag tagobj(mObjType, mName, mTags,
			mShortname, mThumbnail, mOrderName, mDesc, mUser, mXml);
		tagobj.addEntries();
	}
	return true;
}


bool
AosObjectWrapper::fromImgToImages()
{
	OmnString stmt = "select name, xml from img";
	OmnDbTablePtr table;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt = store->query(stmt, table);
	aos_assert_r(rslt, false);
	aos_assert_r(table && table->entries() > 0, false);
	table->reset();
	OmnDbRecordPtr record;

	mTablename = "images";
	int len;
	u8 *vv;
	while (table->hasMore())
	{
		record = table->next();
		OmnString value = record->getStr(1, "", rslt);
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(value, "");
		aos_assert_r(xml, false);
		xml= xml->getFirstChild();
		aos_assert_r(xml, false);

		mTags = "";
		vv = xml->getAttr("__tags", len);
		if (vv)
		{
			mTags.assign((char *)vv, len);
			mTags.escapeSymbal('\'', '\'');

			u8 *tags1 = xml->getAttr("__tags1", len);
			if (tags1)
			{
				if (mTags == "") mTags.assign((char *)tags1, len);
				else mTags.append((char *)tags1, len);
			}

			if (checkTags(mTags) > 0) 
				xml->setAttr("__tags", mTags.data(), mTags.length());
		}

		mDesc = "";
		vv = xml->getAttr("desc", len);
		if (vv)
		{
			mDesc.assign((char *)vv, len); 
			mDesc.escapeSymbal('\'', '\'');
		}

		mShortname = "";
		vv = xml->getAttr("__name", len);
		if (vv)
		{
			mShortname.assign((char *)vv, len);
			mShortname.escapeSymbal('\'', '\'');
		}

		mObjType = "";
		vv = xml->getAttr("__otype", len);
		if (vv)
		{
			mObjType.assign((char *)vv, len);
		}

		mSubtype = "";
		vv = xml->getAttr("__stype", len);
		if (vv)
		{
			mSubtype.assign((char *)vv, len);
		}

		mOrderName = "";
		vv = xml->getAttr("__ofname", len);
		if (vv)
		{
			mOrderName.assign((char *)vv, len);
		}

		mDirname = "";
		vv = xml->getAttr("dirname", len);
		if (vv)
		{
			mDirname.assign((char *)vv, len);
		}

		if (!sgImagesTableIdGen) sgImagesTableIdGen = 
			OmnNew AosIdGen("imagestableid");
		OmnString id = sgImagesTableIdGen->getNextId();
		mName = mTablename;
		mName << "_" << id;

		xml->setAttr("objid", mName.data(), mName.length());

		ostringstream oss(ostringstream::out);
		oss << (char *)xml->getData();
		mXml = oss.str().c_str();
		mXml.escapeSymbal('\'', '\'');

		OmnString stmt = "insert into ";
		stmt << mTablename << " (dataid, name, tnail, container, "
			"tags, vvpdname, evpdname, type, subtype, creator,"
			"createtime, xml) values ('";
		stmt << mName << "', '"
			<< mShortname << "', '"
			<< mThumbnail << "', '"
			<< mContainer << "', '"
			<< mTags << "', '"
			<< "from_img" << "', '"
			<< mEVPD << "', '"
			<< mObjType << "', '"
			<< mSubtype << "', '"
			<< mCreator << "', "
			<< "from_unixtime(unix_timestamp()), '"
			<< mXml << "')";
	
OmnTrace << "To run stmt: " << stmt << endl;
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnRslt rslt;
		if (!store->runSQL(stmt))
		{
			OmnAlarm << "Failed the database operation!" << enderr;
			return false;
		}

		if (mTags != "")
		{
			AosTag tagobj(mObjType, mName, mTags,
				mShortname, mThumbnail, mOrderName, mDesc, mUser, mXml);
			tagobj.addEntries();
		}

		stmt = "delete from tag_table where dataid='";
		stmt << record->getStr(0, "", rslt) << "'";
		store->runSQL(stmt);
	}

	return true;
}


int
AosObjectWrapper::checkTags(OmnString &)
{
	return true;
}


bool
AosObjectWrapper::toImages(
		const OmnString &tname, 
		const OmnString &query, 
		const OmnString &container,
		const OmnString &creator,
		const OmnString &imgtype)
{
	// This function converts the records from 'tname' to 
	// the images table. 
	OmnString origStmt = "select name, keywords, xml from ";
	origStmt << tname;
	
	OmnDbTablePtr table;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();

	int startIdx = 0;
	const int psize = 100;
	OmnDbRecordPtr record;
	mTablename = "images";
	mVVPD = "";
	mEVPD = "";
	mCreator = creator;
	while (1)
	{
		OmnString stmt = origStmt;
		if (query != "") stmt << " where " << query;
		stmt << " limit " << startIdx << ", " << psize;
		startIdx += psize;

		OmnRslt rslt = store->query(stmt, table);
		aos_assert_r(rslt, false);
		aos_assert_r(table, false);
		if (table->entries() <= 0) break;

		table->reset();

		int len;
		u8 *vv;
		while (table->hasMore())
		{
			record = table->next();
			OmnString value = record->getStr(2, "", rslt);
			AosXmlParser parser;
			AosXmlTagPtr xml = parser.parse(value, "");
			aos_assert_r(xml, false);
			xml= xml->getFirstChild();
			aos_assert_r(xml, false);

			mTags = "";
			vv = xml->getAttr("__tags", len);
			if (vv)
			{
				mTags.assign((char *)vv, len);
				mTags.escapeSymbal('\'', '\'');

				u8 *tags1 = xml->getAttr("__tags1", len);
				if (tags1)
				{
					if (mTags == "") mTags.assign((char *)tags1, len);
					else mTags.append((char *)tags1, len);
				}

				if (checkTags(mTags) > 0) 
					xml->setAttr("__tags", mTags.data(), mTags.length());
			}

			mDesc = "";
			vv = xml->getAttr("desc", len);
			if (vv)
			{
				mDesc.assign((char *)vv, len); 
				mDesc.escapeSymbal('\'', '\'');
			}

			mShortname = "";
			vv = xml->getAttr("__name", len);
			if (vv)
			{
				mShortname.assign((char *)vv, len);
				mShortname.escapeSymbal('\'', '\'');
			}

			mObjType = "image";
			vv = xml->getAttr("__stype", len);
			if (vv)
			{
				mSubtype.assign((char *)vv, len);
			}
			else
			{
				mSubtype = imgtype;
			}

			mOrderName = "";
			vv = xml->getAttr("__ofname", len);
			if (vv)
			{
				mOrderName.assign((char *)vv, len);
			}

			mThumbnail = "";
			vv = xml->getAttr("__tnail", len);
			if (vv)
			{
				mThumbnail.assign((char *)vv, len);
			}

			mDirname = "";
			vv = xml->getAttr("dirname", len);
			if (vv)
			{
				mDirname.assign((char *)vv, len);
			}

			if (!sgImagesTableIdGen) sgImagesTableIdGen = 
				OmnNew AosIdGen("imagestableid");
			OmnString id = sgImagesTableIdGen->getNextId();
			mName = mTablename;
			mName << "_" << id;
			mContainer = container;

			xml->setAttr("objid", mName.data(), mName.length());

			ostringstream oss(ostringstream::out);
			oss << (char *)xml->getData();
			mXml = oss.str().c_str();
			mXml.escapeSymbal('\'', '\'');

			OmnString stmt = "insert into ";
			stmt << mTablename << " ("
				"dataid, "
				"name, "
				"tnail, "
				"container, "
				"tags, "
				"vvpdname, "
				"evpdname, "
				"type, "
				"subtype, "
				"creator, "
				"createtime, "
				"xml) values ('";
			stmt << mName << "', '"
				<< mShortname << "', '"
				<< mThumbnail << "', '"
				<< mContainer << "', '"
				<< mTags << "', '"
				<< mVVPD << "', '"
				<< mEVPD << "', '"
				<< mObjType << "', '"
				<< mSubtype << "', '"
				<< mCreator << "', "
				<< "from_unixtime(unix_timestamp()), '"
				<< mXml << "')";
	
			OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
			OmnRslt rslt;
			if (!store->runSQL(stmt))
			{
				OmnAlarm << "Failed the database operation!" << enderr;
				return false;
			}

			if (mTags != "")
			{
				AosTag tagobj(mObjType, mName, mTags,
					mShortname, mThumbnail, mOrderName, mDesc, mUser, mXml);
				tagobj.addEntries();
			}

			stmt = "delete from tag_table where dataid='";
			stmt << record->getStr(0, "", rslt) << "'";
			store->runSQL(stmt);
		}
	}

	return true;
}


bool
AosObjectWrapper::addLoginEntry(
		const OmnString &username, 
		const OmnString &dataid,
		const AosXmlTagPtr &xml,
		OmnString &contents,
		const OmnRslt &rslt, 
		const OmnString &errmsg)
{
	OmnNotImplementedYet;
	/* Not used anymore, Chen Ding, 05/20/2010
	// Add an entry into the table. 
	if (!sgIdGen) sgIdGen = OmnNew AosIdGen("objtableid");
	mName = "objtable_b";
	mName << sgIdGen->getNextId();

	mXml = "<login objid=\"";
	mXml << mName << "\" uname=\"" << username 
		<< "\" __tags=\"zky_login"
	 	<< "\" time=\"" << OmnGetTime() 
	 	<< "\" stime=\"" << OmnGetMDY();

	OmnString realname, personpic;
	if (xml)
	{
		realname = xml->getAttrStr("__name", "");
		personpic = xml->getAttrStr("personpic", "");
		mXml << "\" realname=\"" << realname
			<< "\" personpic=\"" << personpic;
	}

	mXml << "\" status=\"";
	if (rslt) mXml << "success\"/>";
	else 
	{
		mXml << "failed\" errmsg=\"" << errmsg << "\"/>";
	}

	mObjType = "login";
	mTags = "zky_login";
	mShortname = username;
	mUser = username;
	mShortname << " login";
	OmnString stmt = "insert into objtable (dataid, name, container, "
		"tags, xml, type, creator, createtime, updatetime) values (\"";
	stmt << mName << "\", \""
		<< mShortname << "\", \""
		<< AosCtnrId_LoginLogs << "\", \""
		<< mTags << "\", \'"
		<< mXml << "\', \""
		<< mObjType << "\", \""
		<< username << "\", "
		<< "from_unixtime(unix_timestamp()), "
		<< "from_unixtime(unix_timestamp()))";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt ss = store->runSQL(stmt);
	aos_assert_r(ss, false);

	if (rslt) mTags << ",zky_slogin";
	else mTags << ",zky_flogin";

	AosTag tagobj(mObjType, mName, mTags,
			mShortname, "", "", "", mUser, mXml);
	tagobj.addEntries();

	// Create the response
	if (xml)
	{
		contents = "<results><realname><![CDATA[";
		contents << realname << "]]></realname>"
			<< "<personpic><![CDATA[" << personpic << "]]></personpic></results>";
	}
	else
	{
		contents = "";
	}
	return true;
	*/
	return false;
}


bool
AosObjectWrapper::createComment(
		TiXmlElement *cmd,
		OmnString &contents,
		OmnString &errmsg,
		AosXmlRc &errcode) 
{
	// This function creates a comment. The comment is defined
	// as an XML object that should have already been extracted
	// into 'mXmlObj'. 
	// <cmd commented="xxx"
	// 		icommented="xxx"
	// 		cmt_thread="xxx"/>
	errcode = eAosXmlInt_General;

	mTablename = "objtable";

	// 1. Get a new objid
	if (!sgIdGen) sgIdGen = OmnNew AosIdGen("objtableid");
	OmnString id = sgIdGen->getNextId();
	mName = "objtable_b";
	mName << sgIdGen->getNextId();	
	mXmlObj->SetAttribute("objid", mName);

	mTags = mXmlObj->Attribute("__tags");
	OmnString tags1 = mXmlObj->Attribute("__tags1");
	if (tags1 != "")
	{
		if (mTags != "") mTags << ",";
		mTags << tags1;
	}
	checkTags(mTags); 
	mTags.escapeSymbal('\'', '\'');

	mDesc = mXmlObj->Attribute("desc");
	mDesc.escapeSymbal('\'', '\'');
	mShortname = mXmlObj->Attribute("__name");
	mShortname.escapeSymbal('\'', '\'');
	mSubtype = mXmlObj->Attribute("__stype");
	mSubtype.escapeSymbal('\'', '\'');
	mThumbnail = mXmlObj->Attribute("__tnail");
	mThumbnail.escapeSymbal('\'', '\'');

	// Retrieve the parent container, which can be defined
	// either in 'cmd' (as 'container') or in mXmlObj 
	// (as '__pctnrs')
	mContainer = cmd->Attribute("container");
	if (mContainer == "") 
	{
		mContainer = mXmlObj->Attribute("__pctnrs");
		mContainer.escapeSymbal('\'', '\'');
	}
	else
	{
		mContainer.escapeSymbal('\'', '\'');
		mXmlObj->SetAttribute("__pctnrs", mContainer.data());
	}
	// If the container is not empty, need to add a tag:
	// 		ztg_pctnr_<container dataid>
	// This can be used to query all the data that is in 
	// the container. 
	mContainer.escapeSymbal('\'', '\'');
	if (mContainer == "") mContainer = AOSDATAID_COMMENTS;
	addDataidTag(AOSTAG_CONTAINERS, mContainer, errmsg, "Invalid Containe ID");

	// Retrieve the commented object
	OmnString commented = cmd->Attribute("zky_cmtobj");
	if (commented == "") commented = mXmlObj->Attribute(AOSTAG_COMMENTED);
	aos_assert_r(checkNotEmpty(commented, errmsg, 
		"Missing the commented object DataId"), false);
	addDataidTag(AOSTAG_COMMENTED, commented, errmsg, "Invalid Commented Dataid");
	// aos_assert_r(updateCounter(AOSTAG_CNTCM, commented, 1, errmsg), false);
	aos_assert_r(updateCounter("zky_ctnrs", commented, 1, errmsg), false);

	OmnString icommented = cmd->Attribute("zky_icmobj");
	if (icommented == "") icommented = mXmlObj->Attribute(AOSTAG_ICOMMENTED);
	addDataidTag(AOSZTG ICOMMENTED, icommented, errmsg, "Invalid iCommented Dataid");
	if (icommented != "")
	{
		// aos_assert_r(updateCounter(AOSTAG_CNTCM, icommented, 1, errmsg), false);
		aos_assert_r(updateCounter("zky_ctnrs", icommented, 1, errmsg), false);
	}

	OmnString cmtThread = cmd->Attribute("zky_cmthrd");
	if (cmtThread == "") cmtThread = mXmlObj->Attribute(AOSTAG_CMTTHREAD);
	addDataidTag(AOSTAG_CMTTHREAD, cmtThread, errmsg, "Invalid ThreadID");

	mVVPD = cmd->Attribute("vvpd");
	if (mVVPD == "") mVVPD = mXmlObj->Attribute("vvpd");
	mVVPD.escapeSymbal('\'', '\'');
	mEVPD = cmd->Attribute("evpd");
	if (mEVPD == "") mEVPD = mXmlObj->Attribute("evpd");
	mEVPD.escapeSymbal('\'', '\'');
	mObjType = AOSOTYPE_COMMENT;
	mOrderName = mXmlObj->Attribute("__ofname");

	// Save the object
	aos_assert_r(saveTable("objtable", errmsg), false);

	OmnString ordervalue = mXmlObj->Attribute(mOrderName);
	AosTag tagobj(mObjType, mName, mTags,
		mShortname, mThumbnail, ordervalue, mDesc, mUser, mXml);
	tagobj.addEntries();

	errcode = eAosXmlInt_Ok;
	contents = "<results objid=\"";
	contents << mName << "\"/>";
	return true;
}


bool
AosObjectWrapper::addDataidTag(
		const OmnString &tag,
		const OmnString &dataid, 
		OmnString &errmsg, 
		const OmnString &msg)
{
	// It creates a tag:
	// 	'tag' + 'dataid' and adds it to mTags. Before doing so, 
	// it checks whether 'dataid' is a valid dataid. If it is
	// an empty string, it does nothing. 
	if (dataid == "") return true;
	const char *data = dataid.data();
	int len = dataid.length();
	for (int i=0; i<len; i++)
	{
		char c = data[i];
		if (!((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_' || 
			(c >= '0' && c <= '9')))
		{
			errmsg = msg;
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}

	OmnString tt = tag;
	tt << AOS_TAGSEP;
	if (mTags != "") mTags << ",";
	mTags << tt << dataid;
	return true;
}


bool
AosObjectWrapper::checkNotEmpty(
		const OmnString &data, 
		OmnString &errmsg, 
		const OmnString &msg)
{
	if (data != "") return true;
	errmsg = msg;
	return false;
}


OmnString 
AosObjectWrapper::incrementVersion(const OmnString &version)
{
	// 'version' is in the form:
	// 	<xxx>.<xxx>...<xxx>
	// This function increments the last segment by one.
	OmnString ppp[10];
	bool finished;
	AosStrSplit split(version, ".", ppp, 10, finished);
	OmnString lastone = ppp[split.entries()-1];
	int vv = atoi(lastone.data());
	if (vv < 0) vv = 0;
	vv++;

	OmnString ss;
	for (int i=0; i<split.entries()-1; i++)
	{
		if (i != 0) ss << ".";
		ss << ppp[i];
	}
	if (ss != "") ss << ".";
	ss << vv;
	return ss;
}


bool
AosObjectWrapper::addVersionObj(
		const OmnString &keyfname, 
		const OmnString &oldversion)
{
	OmnString errmsg;
	AosXmlTagPtr root = retrieveXml(mTablename, keyfname, mName, errmsg);
	if (root)
	{
		AosXmlTagPtr xml = root->getFirstChild();
		aos_assert_r(xml, false);

		xml->setAttr(AOSTAG_VERSION, oldversion);
		OmnString xmlvalue((char *)xml->getData(), xml->getDataLength());
		if (!sgVersionIdGen) sgVersionIdGen = OmnNew AosIdGen("versionid");
		OmnString id = "vertab_";
		id << sgVersionIdGen->getNextId();
		// xml->setAttr(AOSTAG_OBJID, id);
		xml->setAttr("objid", id);
		OmnString stmt = "insert into vertab (origobj, dataid, xml) values ('";
		stmt << mName <<"', '" << id << "', '" << xmlvalue << "')";
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnRslt rslt = store->runSQL(stmt);
		aos_assert_r(rslt, false);

		// Insert a tag to the tag_table
		OmnString tag = AOSIILTAG_VERSION;
		tag << AOS_TAGSEP << mName;
		AosTag thetag;
		thetag.addOneEntry(tag, id, AOSOTYPE_VERSION, xmlvalue);
	}

	return true;
}

