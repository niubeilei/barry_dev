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
// 05/06/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "XmlInterface/Charset.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "Util/a_string.h"


// 
// Description:
// It modifies the Charset identified by 'objid'. If the object does not exist, 
// it creates one. The new Charset is 'contents', which is an xml definition
// of the charset. If it failed to create charset from 'contents', it is an error.
// Otherwise, it checks whether the new object is correct. If not, it is 
// an error. If yes, it replaces the old definition with the new one.
//
bool AosCharset_Modify(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	aos_assert_r(node, false);

	char *objid, *contents; 

   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objid", &objid) == 1, false);
   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "contents", &contents) == 1, false);

	// 
	// Create the charset. 
	//
	aos_charset_t *cs = aos_charset_create_str(contents);
	if (!cs)
	{
		OmnAlarm << "Incorrect contents: " << contents << enderr;
		errcode = eAosXmlInt_SyntaxError;
		return false;
	}

	AosCharsetWrapper wp;
	wp.setName(objid);
	char *kw = cs->mf->get_keywords(cs);
	if (kw) wp.setKeywords(kw);

	char *desc = cs->mf->get_desc(cs);
	if (desc) wp.setDescription(desc);

	wp.setXml(contents);
	if (!AosCharsetWrapper::charsetExist(objid))
	{
		// It is to create a new Charset.
		OmnRslt rslt = wp.addToDb();
		if (!rslt)
		{
			OmnAlarm << "Failed to save to database: " << rslt.getErrmsg() << enderr;
			errcode = eAosXmlInt_DbError;
			errmsg = "Failed to save to database.";
			return false;
		}
	}
	else
	{
		// It is to modify an existing one. We simply replace the existing
		// one with the new one.
		OmnRslt rslt = wp.updateToDb();
		if (!rslt)
		{
			OmnAlarm << "Failed to modify: " << rslt.getErrmsg() << enderr;
			errcode = eAosXmlInt_DbError;
			errmsg = "Failed to update the database.";
			return false;
		}
	}

	errcode = eAosXmlInt_Ok;
	return true;
}


/*
bool AosCharset_Read(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg) 
{
	aos_assert_r(node, false);
	char *objid; 

   	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objid", &objid) == 1, false);

	AosCharsetWrapper wp;
	if (wp.getCharset(objid))
	{
		// Found the Charset. 
		contents = wp.getXml();
		errcode = eAosXmlInt_Ok;
		return true;
	}

	// Did not find the object
	errcode = eAosXmlInt_ObjectNotFound;
	errmsg = "Name: ";
	errmsg << objid;
	return false;
}
*/

#endif
