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
// 05/27/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "XmlInterface/Dialog.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "TorturerWrappers/DialogWrapper.h"
#include "Util/a_string.h"
#include "Util/File.h"
#include "util2/value.h"
#include "util_c/memory.h"


// 
// Description:
// It modifies the object identified by 'objname'. All the information is defined in 'node':
// 	<formcomponent name='contents'>
// 	<formcomponent name='description'>
// 	<formcomponent name='objid'>
//
bool AosDialog_Modify(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	char *objid, *contents, *description;
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objid", &objid) == 1, false);
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "contents", &contents) == 1, false);
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "description", &description) == 1, false);

	AosDialogWrapper wp;
	wp.setXml(contents);
	wp.setObjid(objid);
	wp.setDesc(description);

	int is_incomplete;
	int str_len = strlen(contents);
	aos_xml_node_t *cs = aos_xml_node_create_from_str(0, contents, &str_len, &is_incomplete, 0);
	aos_assert_r(cs, false);
	aos_assert_r(!is_incomplete, false);

	if (!AosDialogWrapper::objectExist(objid))
	{
		// It is to create a new dialog.
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

#endif
