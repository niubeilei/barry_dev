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
// 08/01/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/TaggedData/TaggedData.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"


AosTaggedData::AosTaggedData()
{
}


AosTaggedData::~AosTaggedData()
{
}


// 
// Description:
// This member function retrieves the data starting with 'start_idx'
// and the maximum number of objects to be retrieved is 'num'. 
// Retrieved data are stored in 'data'.
int
AosTaggedData::getData(
		const OmnString &tags, 
		const OmnString &names,
		const int start_idx, 
		const int num,
		const OmnString &query,
		const OmnString &order,
		const bool getTotal,
		OmnString &data, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 
	// It retrieves the RVG identified by "name" from the database.
	// Tags are defined in 'tags', in the form:
	// 	"tag1|$|tag2|$|..."
	// Tags are optional.
	// 
	errcode = eAosXmlInt_General;
	data = "";

	const int max_parts = 100;
	char *parts[max_parts];
	int num_tags = 0;
	if (tags != "")
	{
		num_tags = aos_sstr_split(tags.data(), "|$|", parts, max_parts);
		aos_assert_r(num_tags > 0, -1);
	}


	if (names.length() < 1)
	{
		errmsg = "Missing field names!";
		OmnAlarm << errmsg << enderr;
		return -1;
	}

	const int max_names = 10;
	char *names_parts[max_names];
	int num_names = aos_str_split(names.data(), ',', names_parts, max_names);
	if (num_names < 1)
	{
		errmsg = "No names specified!";
		OmnAlarm << errmsg << enderr;
		aos_str_split_releasemem(parts, num_tags);
		return -1;
	}

	OmnDbTablePtr table;
   	OmnString where_stmt = " from tag_table where (tag='";
	where_stmt << parts[0] << "'";
	for (int i=1; i<num_names; i++)
	{
		where_stmt << " OR tag='" << parts[i] << "'";
	}
	where_stmt << ")";
	if (query != "") where_stmt << " AND " << query << " ";

	OmnDbRecordPtr record;
	OmnRslt rslt;
	u32 total_num = 0;
	if (getTotal)
	{
		// Need to get the total
		OmnString stmt = "select count(distinct dataid) ";
		stmt << where_stmt;
		rslt = retrieveRecord(stmt, record);
		if (!rslt)	
		{
			errmsg = "Failed to retrieved the total: ";
			errmsg << stmt;
			OmnAlarm << errmsg << enderr;
			return -1;
		}
		total_num = record->getU32(0, 0, rslt);
	}

	OmnString stmt = "select ";
	for (int i=0; i<num_names; i++)
	{
		if (i != 0) stmt << ", ";
		if (strcmp(names_parts[i], "dataid") == 0)
			stmt << " distinct dataid";
		else
			stmt << names_parts[i];
	}

	stmt << where_stmt;
	stmt << order << " limit " << start_idx << ", " << num;
	cout << "To retrieve Object: " << stmt << endl;

	rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Object: " 
			<< stmt << " from db!" << enderr;
		aos_str_split_releasemem(parts, num_tags);
		aos_str_split_releasemem(names_parts, num_names);
		return false;
	}

	aos_assert_r(table, false);
	table->reset();
	OmnString ss;
	data = "<Records ";
	if (getTotal) data << "total=\"" << total_num << "\" num=\""
		<< table->entries() << "\">";

	while (table->hasMore())
	{
		record = table->next();

		aos_assert_r(num_names == record->getNumFields(), -1);
		data << "<Record ";
		for (int i=0; i<num_names; i++)
		{
			data << names_parts[i] << "=\"" << record->getStr(i, "", rslt)
				<< "\" ";
		}
		data << "/>";
	}

	data << "</Records>";
	aos_str_split_releasemem(parts, num_tags);
	aos_str_split_releasemem(names_parts, num_names);
	errcode = eAosXmlInt_Ok;
	return table->entries();
}


OmnRslt
AosTaggedData::serializeFromDb()
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::updateStmt() const
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::removeStmt() const
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::removeAllStmt() const
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::existStmt() const
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::insertStmt() const
{
	OmnAlarm << "Should not access this function!" << enderr;
	return false;
}


OmnString
AosTaggedData::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnRslt     
AosTaggedData::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnNotImplementedYet;
	return false;
}

