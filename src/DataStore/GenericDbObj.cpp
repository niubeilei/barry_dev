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
// This class serves as the generic object that manages a database table. 
// The input to this class is an XML definition (refer to the comments
// in the constructor).
//   
//
// Modification History:
// 12/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/GenericDbObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/GenericField.h"
#include "DataStore/DataStore.h"
#include "DataStore/ClassDef.h"
#include "DataStore/DbTrackTable.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"



AosGenericDbObj::AosGenericDbObj()
:
mNumFields(0)
{
}


AosGenericDbObj::AosGenericDbObj(const OmnString &tablename)
:
mNumFields(0),
mTablename(tablename)
{
}


AosGenericDbObj::~AosGenericDbObj()
{
}


bool
AosGenericDbObj::parseDef(
		const OmnString &tabledef, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'tabledef' should be in the following format:
	// 	<tabledef tablename="xxx">
	// 		<field name="xxx" datatype="xxx"/>
	// 		<field name="xxx" datatype="xxx"/>
	// 		...
	// 		<field name="xxx" datatype="xxx"/>
	// 	</tabledef>
	//
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Internal error";

	TiXmlDocument thedoc;
	thedoc.Parse(tabledef.data());
	TiXmlNode *root = thedoc.FirstChild();
	TiXmlElement *xmlobj = root->ToElement();

	// Retrieve the tablename
	const char *tname = xmlobj->Attribute("tablename");
	if (!tname)
	{
		errmsg = "Missing database table name!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mTablename = tname;

	OmnTrace << "The root node: " << xmlobj->Value() << endl;
	TiXmlNode *child; 
	mNumFields = 0;
	for (child = xmlobj->FirstChild(); child; child = child->NextSibling())
	{
		TiXmlElement *elem = child->ToElement();
		aos_assert_r(elem, false);
		AosGenericFieldPtr field = OmnNew AosGenericField();
		aos_assert_r(field, false);
		bool tt = field->parse(elem, errcode, errmsg);
		aos_assert_r(tt, false);
		mFields[mNumFields++] = field;
	}
	return true;
}


bool	
AosGenericDbObj::addField(const OmnString &name, 
				const AosGenericField::DataType type,
				const OmnString &value, 
				const OmnString &dft_value)
{
	AosGenericFieldPtr field = getField(name);
	aos_assert_r(!field, false);
	field = OmnNew AosGenericField();
	aos_assert_r(field, false);
	bool tt = field->parse(name, type, value, dft_value);
	aos_assert_r(tt, false);
	mFields[mNumFields++] = field;
	return true;
}


OmnRslt
AosGenericDbObj::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);

	int i;
	for (i=0; i<mNumFields; i++)
	{
		switch (mFields[i]->getDataType())
		{
		case AosGenericField::eType_Str:
			 mFields[i]->setValue(record->getStr(i, "", rslt));
			 break;

		case AosGenericField::eType_Char:
			 mFields[i]->setValue(record->getChar(i, ' ', rslt));
			 break;

		case AosGenericField::eType_Bool:
			 mFields[i]->setValue(record->getBool(i, false, rslt));
			 break;

		case AosGenericField::eType_Int:
			 mFields[i]->setValue(record->getInt(i, 0, rslt));
			 break;

		case AosGenericField::eType_U32:
			 mFields[i]->setValue(record->getU32(i, 0, rslt));
			 break;

		case AosGenericField::eType_Int64:
			 mFields[i]->setValue(record->getInt64(i, 0, rslt));
			 break;

		case AosGenericField::eType_U64:
			 mFields[i]->setValue(record->getU64(i, 0, rslt));
			 break;

		case AosGenericField::eType_Double:
			 mFields[i]->setValue(record->getDouble(i, 0, rslt));
			 break;

		default:
			 OmnAlarm << "Unrecognized field data type: " 
				 << mFields[i]->getDataType() << enderr;
		}
	}

	return rslt;
}


OmnString
AosGenericDbObj::updateStmt() const
{
    OmnString stmt = "update ";
	OmnString clause;
	stmt << mTablename << " set ";
	int i;
	int valueIdx = 0;
	int keyIdx = 0;
	for (i=0; i<mNumFields; i++)
	{
		if (mFields[i]->isPrimaryKey())
		{
			if (keyIdx != 0) clause << " AND ";
			keyIdx++;
			if (mFields[i]->isStrType())
			{
				clause << mFields[i]->getFieldName() 
					<< "='" << mFields[i]->getValueStr() << "'";
			}
			else
			{
				clause << mFields[i]->getFieldName() << "=" 
					<< mFields[i]->getValueStr();
			}
		}
		else
		{
			if (valueIdx != 0) stmt << ", ";
			valueIdx++;
			switch (mFields[i]->getDataType())
			{
			case AosGenericField::eType_Str:
				 stmt << mFields[i]->getFieldName() 
					 << "='" << mFields[i]->getValueStr() << "'";
				 break;
	
			case AosGenericField::eType_Char:
				 stmt << mFields[i]->getFieldName() 
					 << "='" << mFields[i]->getValueStr() << "'";
				 break;
	
			case AosGenericField::eType_Bool:
				 stmt << mFields[i]->getFieldName() 
					 << "='" << mFields[i]->getValueStr() << "'";
				 break;
	
			case AosGenericField::eType_Int:
				 stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
				 break;
	
			case AosGenericField::eType_U32:
				 stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
				 break;
	
			case AosGenericField::eType_Int64:
				 stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
				 break;
	
			case AosGenericField::eType_U64:
				 stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
				 break;
	
			case AosGenericField::eType_Double:
				 stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
				 break;
	
			default:
				 OmnAlarm << "Unrecognized field data type: " 
					 << mFields[i]->getDataType() << enderr;
			}
		}
	}

	stmt << " where " << clause;
	return stmt;
}


OmnString
AosGenericDbObj::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
	int i;
	OmnString names;
	OmnString values;
	bool first_field = true;
	AosGenericFieldPtr field;
	OmnString value;
	bool ignore;
	for (i=0; i<mNumFields; i++)
	{
		ignore = false;
		field = mFields[i];
		if (field->isValueSet())
		{
			value = field->getValueStr();
		}
		else if (!field->getDftValue(value))
		{
			if (field->isMandatory() || field->isPrimaryKey())
			{
				OmnAlarm << "The field: " << field->getFieldName()
					<< " is mandatory but value is not set!" << enderr;
				return "";
			}
			else
			{
				ignore = true;
			}
		}

		if (!ignore)
		{
			if (!first_field) 
			{
				names << ", ";
				values << ", ";
			}

			first_field = false;
			names << field->getFieldName();
			
			if (field->isStrType())
			{
				values << "'" << value << "'";
			}
			else
			{
				values << value;
			}
		}
	}
	OmnString stmt = "insert into ";
	stmt << mTablename << "(" << names << ") values (" << values << ")";
    return stmt;
}


OmnString
AosGenericDbObj::existStmt() const
{
    OmnString stmt = "select ";
	OmnString clause;
	int i;
	int valueIdx = 0;
	int keyIdx = 0;
	for (i=0; i<mNumFields; i++)
	{
		if (mFields[i]->isPrimaryKey())
		{
			if (keyIdx != 0) clause << " AND ";
			keyIdx++;
			if (mFields[i]->isStrType()) 
			{
				clause << mFields[i]->getFieldName() 
					<< "='" << mFields[i]->getValueStr() << "'";
			}
			else
			{
				clause << mFields[i]->getFieldName() 
					<< "=" << mFields[i]->getValueStr();
			}
		}
		else
		{
			if (valueIdx != 0) stmt << ", ";
			valueIdx++;
			stmt << mFields[i]->getFieldName();
		}
	}

	stmt << " from " << mTablename << " where " << clause;
    return stmt;
}


OmnString 
AosGenericDbObj::retrieveStmt() const
{
	OmnString stmt = "select * from ";
	stmt << mTablename << " where ";
	int keyIdx = 0;
	for (int i=0; i<mNumFields; i++)
	{
		if (mFields[i]->isPrimaryKey())
		{
			if (keyIdx != 0) stmt << " AND ";
			keyIdx++;

			if (mFields[i]->isStrType())
			{
				stmt << mFields[i]->getFieldName() << "='" 
					<< mFields[i]->getValueStr() << "'";
			}
			else
			{
				stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
			}
		}
	}
	return stmt;
}


bool
AosGenericDbObj::addRecordToDb(
		TiXmlElement *def, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// This function creates a new record to the database
	// based on the contents 'def', which is an XML node. 
	// 'def' shall be in the following format:
	// 	<tag objid="xxx" ...>
	// 		...
	// 	</tag>
	// where the objid shall be in the form:
	// 	"tablename_xxx"
	// Each type of object is mapped to a different table. There is
	// a table that maps tablename to the XML definition of the
	// corresponding class. We need to look up that XML definition
	// and construct this class based on that. We then use this
	// instantiated instance to save the record into the database. 
	errcode = eAosXmlInt_SyntaxError;
	aos_assert_r(def, false);	

	// Retrieve the 'objid'
	const char *objid = def->Attribute("objid");
	if (!objid)
	{
		errmsg = "Missing 'objid' in the XML object";
		return false;
	}

	// Get the object table name from 'objid'
	char *parts[2];
	int num = aos_str_split(objid, '_', parts, 2);
	aos_assert_r(num == 2, false);

	// Retrieve the class def
	AosClassDef cd;
	cd.setObjTablename(parts[0]);
	aos_str_split_releasemem(parts, 2);
	bool ttt = cd.retrieveFromDb();
	if (!ttt)
	{
		errmsg = "Object definition not found in database: ";
		errmsg << objid;
		return false;
	}

	// Parse the class def
	ttt = parseDef(cd.getClassDef(), errcode, errmsg);
	if (!ttt) 
	{
		return false;
	}

	// Set the object based on 'def'
	ttt = setObjAttrs(def, errcode, errmsg);
	if (!ttt) return false;

	// Time to save the object into the database.
	ttt = addToDb();
	if (!ttt)
	{
		errmsg = "Failed to store the object into the database";
		return false;
	}

	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosGenericDbObj::setObjAttrs(
		TiXmlElement *def, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'def' is an XML object in the following format:
	// 	<tag ...>
	// 		<field name="xxx">xxx</field>
	// 		<field name="xxx">xxx</field>
	// 		...
	// 		<field name="xxx">xxx</field>
	// 	</tag>
	//
	// This function assumes this instance has been properly created by 
	// its XML definition. It sets all the fields that are contained 
	// in 'def', leaving all other fields untouched.
	//
	errcode = eAosXmlInt_SyntaxError;

	TiXmlNode *child;
	for (child = def->FirstChild(); child; child = child->NextSibling())
	{
		TiXmlElement *ee = child->ToElement();
		if (!ee)
		{
			errmsg = "In setting object, encounted a node that is not an element";
			OmnAlarm << errmsg << enderr;
			return false;
		}

		const char *fname = ee->Attribute("name");
		if (!fname)
		{
			errmsg = "Retrieved a field node but it does not contain the 'name'"
				" attribute";
			OmnAlarm << errmsg << enderr;
			return false;
		}

		AosGenericFieldPtr field = getField(fname);
		if (!field)
		{
			errmsg = "Failed to retrieve the field: ";
			errmsg << fname;
			OmnAlarm << errmsg << enderr;
			return false;
		}

		bool ttt = field->setValue(ee, errcode, errmsg);
		if (!ttt) return false;
	}

	return true;
}


bool
AosGenericDbObj::setFieldValue(const OmnString &name, const OmnString &value)
{
	AosGenericFieldPtr field = getField(name);
	aos_assert_r(field, false);
	return field->setValue(value);
}


OmnString
AosGenericDbObj::removeStmt() const
{
    OmnString stmt = "delete from ";
	stmt << mTablename << " where ";
	int idx = 0;
	for (int i=0; i<mNumFields; i++)
	{
		if (mFields[i]->isPrimaryKey())
		{
			if (idx != 0) stmt << " AND ";
			idx++;

			if (mFields[i]->isStrType())
			{
				stmt << mFields[i]->getFieldName() << "='"
					<< mFields[i]->getValueStr() << "'";
			}
			else
			{
				stmt << mFields[i]->getFieldName() << "=" << mFields[i]->getValueStr();
			}
		}
	}
    return stmt;
}


OmnString
AosGenericDbObj::removeAllStmt() const
{
	OmnString stmt = "delete from ";
	stmt << mTablename;
	return stmt;
}


AosGenericFieldPtr
AosGenericDbObj::getField(const OmnString &fname)
{
	int i;
	for (i=0; i<mNumFields; i++)
	{
		if (mFields[i]->getFieldName() == fname) return mFields[i];
	}
	return 0;
}


void
AosGenericDbObj::resetValues()
{
	int i;
	for (i=0; i<mNumFields; i++)
	{
		mFields[i]->resetValue();
	}
}


bool
AosGenericDbObj::retrieveFromDb(
		const TiXmlElement *query, 
		bool &retrieved,
		int &num_matched,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// This function sets this object based on 'query'. It then 
	// retrieves the object from the databaes table. The query will
	// be constructed by concatenating the value of all the fields
	// whose value is set by 'query'. The selected fields can be
	// controlled by the <retrieved> subtag (see below). The value
	// attribute can be a list of field names or 'all'. If the
	// tag is not present, it will retrieve the remaining fields. 
	//
	// 'query' should be in the form:
	// 	<tag ...>
	// 		<queried_fields>
	// 			<field name="xxx">value</field>
	// 			<field name="xxx">value</field>
	// 			...
	// 		</queried_fields>
	// 		<retrieved value="xxx">
	// 	</tag>
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Internal error";
	retrieved = false;
	num_matched = -1;

	// First, use 'query' to set the corresponding field values.
	aos_assert_r(query, false);
	const TiXmlNode *nn = query->FirstChild("queried_fields");
	if (!nn)
	{
		errmsg = "Missing the <queried_fields> tag";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	const TiXmlElement *queried_fields = nn->ToElement();
	if (!queried_fields)
	{
		errmsg = "The <queried_fields> is not an element";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	resetValues();
	bool tt = setObjAttrs((TiXmlElement *)queried_fields, errcode, errmsg);
	aos_assert_r(tt, false);

	// Retrieve the '<retrieved>' tag
	OmnString retrieved_fields;
	nn = query->FirstChild("retrieved");
	if (nn)
	{
		const TiXmlElement *elem = nn->ToElement();
		if (!elem)
		{
			errmsg = "The <retrieved> is not an element!";
			OmnAlarm << errmsg << enderr;
			return false;
		}
		retrieved_fields = elem->Attribute("value");
	}

	// Next, construct the names and query clause
	int i;
	OmnString names;
	OmnString query_clause;
	AosGenericFieldPtr field;
	bool is_all_fields = (retrieved_fields == "all" || retrieved_fields == "");
	bool first_queried_field = true;
	bool first_retrieved_field = true;
	for (i=0; i<mNumFields; i++)
	{
		field = mFields[i];
		if (field->isValueSet())
		{
			if (!first_queried_field) query_clause << " and ";
			first_queried_field = false;
			query_clause << field->getFieldName();
			if (field->isStrType())
			{
				query_clause << "='" << field->getValueStr() << "'";
			}
			else
			{
				query_clause << "=" << field->getValueStr();
			}
		}
		else
		{
			if (is_all_fields)
			{
				if (!first_retrieved_field) names << ",";
				first_retrieved_field = false;
				names << field->getFieldName();
			}
		}
	}
	if (!is_all_fields) names = retrieved_fields;
	if (names == "")
	{
		errmsg = "Nothing to be retrieved!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (query_clause == "")
	{
		errmsg = "The query caluse is empty!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Construct the query statement
	OmnString stmt = "select ";
	stmt << names << " from " << mTablename << " where " << query_clause;

	// Retrieve the record
	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		errmsg = "Failed retrieving the object from database";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!table)
	{
		// The query did not get any record. This may or may not be an error.
		return true;
	}

	int num_retrieved = table->entries();	
	if (num_retrieved > 1)
	{
		// It retrieved multiple instances. This may or may not be an error. 
		return true;
	}

	table->reset();
	OmnDbRecordPtr record = table->next();

	// Ready to serialize from the record
	// The retrieved fields are listed in 'names'. 
	char *parts[eMaxNumFields];
	int num_names = aos_str_split(names.data(), ',', parts, eMaxNumFields);
	for (i=0; i<num_names; i++)
	{
		field = getField(parts[i]);
		if (!field)
		{
			// It is an internal error
			errmsg = "Failed to retrieve the name (an internal error): ";
			errmsg << parts[i];
			OmnAlarm << errmsg << enderr;
			aos_str_split_releasemem(parts, eMaxNumFields);
			return false;
		}

		switch (field->getDataType())
		{
		case AosGenericField::eType_Str:
			 field->setValue(record->getStr(i, "", rslt));
			 break;

		case AosGenericField::eType_Char:
			 field->setValue(record->getChar(i, ' ', rslt));
			 break;

		case AosGenericField::eType_Bool:
			 field->setValue(record->getBool(i, false, rslt));
			 break;

		case AosGenericField::eType_Int:
			 field->setValue(record->getInt(i, 0, rslt));
			 break;

		case AosGenericField::eType_U32:
			 field->setValue(record->getU32(i, 0, rslt));
			 break;

		case AosGenericField::eType_Int64:
			 field->setValue(record->getInt64(i, 0, rslt));
			 break;

		case AosGenericField::eType_U64:
			 field->setValue(record->getU64(i, 0, rslt));
			 break;

		case AosGenericField::eType_Double:
			 field->setValue(record->getDouble(i, 0, rslt));
			 break;

		default:
			 OmnAlarm << "Unrecognized field data type: " 
				 << field->getDataType() << enderr;
		}
	}

	retrieved = true;
	aos_str_split_releasemem(parts, eMaxNumFields);
	return true;
}


