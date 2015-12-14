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
// 05/12/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "XmlInterface/Template.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "parser/xml_node.h"
#include "rvg_c/rvg.h"
#include "TorturerWrappers/TemplateWrapper.h"
#include "Util/a_string.h"
#include "Util/File.h"
#include "util2/value.h"
#include "util_c/memory.h"


// 
// Description:
// It modifies the object template identified by 'objname'. 
// If the object does not exist, 
// it creates one. The new template is 'contents', which is an xml definition
// of the template. If it failed to create a template from 'contents', it is an error.
// Otherwise, it checks whether the new object is correct. If not, it is 
// an error. If yes, it replaces the old definition with the new one.
//
bool AosTemplate_Modify(
		aos_xml_node_t *node,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	char *objname, *contents;
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "objname", &objname) == 1, false);
	aos_assert_r(node->mf->get_str_by_attr((aos_field_t *)node, "name", "contents", &contents) == 1, false);

	AosTemplateWrapper wp;
	wp.setXml(contents);
	wp.setName(objname);

	int is_incomplete;
	int str_len = strlen(contents);
	aos_xml_node_t *cs = aos_xml_node_create_from_str(0, contents, &str_len, &is_incomplete, 0);
	aos_assert_r(cs, false);
	aos_assert_r(!is_incomplete, false);

	char *desc;
	int len;
	aos_assert_r(!cs->mf->first_child_str(cs, "Description", &desc, &len, 0), false);
	if (desc)
	{
		wp.setDesc(desc);
		aos_free(desc);
	}

	wp.setXml(contents);
	if (!AosTemplateWrapper::templateExist(objname))
	{
		// It is to create a new template.
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
