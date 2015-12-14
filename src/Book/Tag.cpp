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
// 08/03/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/Tag.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "SearchEngine/SeXmlParser.h"
#include "SearchEngine/XmlTag.h"
#include "TinyXml/tinyxml.h"
#include "util_c/strutil.h"
#include "Util/File.h"
#include "Util/StrSplit.h"
#include "XmlInterface/Server/DbConvert.h"


const OmnString sgSelectStmt = "select type,name,thumbnail,"
	"description,creator,unix_timestamp(createtime),ordername,xml from tag_table";
const OmnString sgFieldnames = 
	"tag," 
	"dataid,"
   	"type,"
	"name,"
	"thumbnail,"
	"description,"
	"creator,"
	"createtime,"
	"ordername,"
	"xml";

AosTag::AosTag()
{
}


AosTag::AosTag(
		const OmnString &type,
		const OmnString &dataid, 
		const OmnString &tags,
		const OmnString &name, 
		const OmnString &thumbnail, 
		const OmnString &ordername, 
		const OmnString &desc,
		const OmnString &user, 
		const OmnString &xml)
:
mType(type),
mDataid(dataid),
mTags(tags),
mName(name),
mThumbnail(thumbnail),
mOrderName(ordername),
mDesc(desc),
mCreator(user),
mXml(xml)
{
}


// 
// Description:
// This member function retrieves the named Tag definition. 
// If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosTag::getEntry(const OmnString &tag, const OmnString &dataid)
{
    OmnString stmt = sgSelectStmt;
	stmt << " where tag=\""
		<< tag << "\" AND dataid=\""
		<< dataid << "\"";
	
	OmnTrace << "To retrieve tag entry: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Tag entry: "
			<< stmt << " from db!" << enderr;
		return false;
	}

	mTag			= tag;
	mDataid 		= dataid;
	mType			= record->getStr(0, "", rslt);
	mName 			= record->getStr(1, "", rslt);
	mThumbnail		= record->getStr(2, "", rslt);
	mDesc 			= record->getStr(3, "", rslt);
	mCreator 		= record->getStr(4, "", rslt);
	mCreateTime		= record->getU32(5, 0, rslt);
	mOrderName		= record->getStr(6, "", rslt);
	mXml			= record->getStr(7, "", rslt);
	return rslt;
}


bool
AosTag::addEntries()
{
	OmnString parts[100];
	bool finished;
	AosStrSplit ss(mTags.data(), ",", parts, 100, finished);

	for (int i=0; i<ss.entries(); i++)
	{
		mTag = ss.getEntry(i);
		addToDb();
	}
	return true;
}

bool
AosTag::addOneEntry(
		const OmnString &tag,
		const OmnString &dataid,
		const OmnString &objtype,
		const OmnString &xmlvalue)
{
	mDataid = dataid;
	mType = objtype;
	mName = "";
	mDesc = "";
	mCreator = "";
	mXml = xmlvalue;
	mTag = tag;
	addToDb();
	return true;
}


bool
AosTag::addEntries(
		char **tags,
		const int num,
		const AosSystemData &data)
{
	mDataid = data.getDataid();
	mType = data.getType();
	mName = data.getName();
	mDesc = data.getDesc();
	mCreator = data.getCreator();
	mCreateTime = data.getCreateTime();
	mXml = data.getXml();

	for (int i=0; i<num; i++)
	{
		// Check whether the tag is the same as the one already processed
		bool ignore = false;
		for (int j=0; j<i; j++)
		{
			if (strcmp(tags[j], tags[i]) == 0)
			{
				ignore = true;
			}
		}

		if (ignore) continue;
		mTag = tags[i];
		addToDb();
	}
	return true;
}



OmnString
AosTag::insertStmt() const
{
	OmnString stmt = "insert into tag_table (";
	stmt << sgFieldnames << ") values (\""
		<< mTag << "\", \""
		<< mDataid << "\", \""
		<< mType << "\", \""
		<< mName << "\", \""
		<< mThumbnail << "\", \""
		<< mDesc << "\", \""
		<< mCreator << "\", from_unixtime(unix_timestamp()), \""
		<< mOrderName << "\", \'"
		<< mXml << "\')";
	return stmt;
}


bool
AosTag::deleteEntries(const OmnString &dataid)
{
	OmnString stmt = "delete from tag_table where dataid='";
	stmt << dataid << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnRslt rslt = store->runSQL(stmt);
	if (!rslt)
	{
		OmnAlarm << "Failed to delete tag entries: "
			<< stmt << enderr;
		return false;
	}
	return true;
}


bool
AosTag::cleanTagTable()
{
	// This function loops over all the entries in tag_table
	// and deletes those entries where the referenced record
	// identified by the entry does not exist. The tag_table
	// primary key is:
	// 		[dataid, tag]
	// For each such record in tag_table, check whether 'dataid'
	// exists. If not, delete the record.
	//
	// In addition, if 'thumbnail' does not exist, the tag_table
	// has an XML object, and the xml object has the attribute
	// '__tnail', update the 'thumbnail' field.
	OmnString origStmt = "select dataid, tag, thumbnail, xml from tag_table";
	int startIdx = 0;
	while (1)
	{
		OmnString stmt = origStmt;
		stmt << " limit " << startIdx << ", 100";
		startIdx += 100;
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnRslt rslt;
		OmnDbTablePtr table;
		rslt = store->query(stmt, table);
		aos_assert_r(rslt, false);
		if (table->entries() <= 0) break;

		table->reset();
		bool finished;
		while (table->hasMore())
		{
			OmnDbRecordPtr record = table->next();
			OmnString dataid = record->getStr(0, "", rslt);
			OmnString tag = record->getStr(1, "", rslt);
			OmnString thumbnail = record->getStr(2, "", rslt);
			OmnString xmlstr = record->getStr(3, "", rslt);

			OmnString parts[50];
			AosStrSplit split(dataid.data(), "_", parts, 50, finished);
			if (split.entries() <= 1)
			{
				OmnAlarm << "Found an invalid entry: "
					<< dataid << ":" << tag
					<< enderr;
				continue;
			}
			
			OmnString tname = split[0];
			OmnString idfname = AosConvertDb::getIdfname(tname);
			if (idfname == "")
			{
				OmnAlarm << "Invalid table: " << dataid << ":" << tag << enderr;
				continue;
			}

			stmt = "select count(*) from ";
			stmt << tname << " where "
				<< AosConvertDb::getIdfname(tname)
				<< "='" << dataid << "'";
			OmnDbRecordPtr rcd;
			rslt = store->query(stmt, rcd);
			if (!rslt || !rcd)
			{
				OmnAlarm << "Failed to query the database: " 
					<< stmt << enderr;
				continue;
			}

			if (rcd->getInt(0, 0, rslt) == 0)
			{
				// The record does not exist. Need to delete
				// the record
				stmt = "delete from tag_table where dataid='";
				stmt << dataid << "' AND tag='" << tag << "'";
				rslt = store->runSQL(stmt);
				if (!rslt)
				{
					OmnAlarm << "Failed to delete the record: "
						<< dataid << ":" << tag << enderr;
				}
			}
			else
			{
				// The record exist, check whether 'thumbnail'
				// is empty. 
				if (thumbnail == "" && xmlstr != "")
				{
					AosXmlParser parser;
					AosXmlTagPtr xml = parser.parse(xmlstr, "");
					if (xml && (xml= xml->getFirstChild()))
					{
						int tlen;
						u8 *vv = xml->getAttr("__tnail", tlen);
						if (vv && tlen > 0)
						{
							OmnString ss = "update tag_table set thumbnail='";
							ss.append((char *)vv, tlen);
							ss << "' where dataid='" << dataid
								<< "' AND tag='" << tag << "'";
							rslt = store->runSQL(ss);
							if (!rslt)
							{
								OmnAlarm << "Failed to update the record: "
									<< dataid << ":" << tag << enderr;
							}
						}
					}
				}
			}
		}
	}
	return true;
}

