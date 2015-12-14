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
// 01/31/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/types.h"

#include "alarm_c/alarm.h"
#include <string.h>

const char * aos_field_type_2str(aos_field_type_e type)
{
	switch (type)
	{
	case eAosFieldType_Fixed:
		 return "Fixed";

	case eAosFieldType_Container:
		 return "Container";

	case eAosFieldType_Enum:
		 return "Enum";

	case eAosFieldType_Var:
		 return "Var";

	case eAosFieldType_Xml:
		 return "Xml";

	case eAosFieldType_XmlMsg:
		 return "XmlMsg";

	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return "unknown";
	}

	aos_should_never_come_here;
	return "incorrect";
}


const char * aos_content_type_2str(const aos_content_type_e type)
{
	switch (type)
	{
	case eAosContentType_Binary:
		 return "Binary";

	case eAosContentType_CharStrCS:
		 return "CharStrCS";

	case eAosContentType_CharStrCI:
		 return "CharStrCI";

	default: 
		 aos_alarm("Unrecognized type: %d", type);
		 return "Incorrect";
	}

	aos_should_never_come_here;
	return "incorrect";
}


aos_content_type_e aos_content_type_2enum(const char * const value)
{
	aos_assert_r(value, eAosContentType_Invalid);

	if (strcmp(value, "Binary") == 0) return eAosContentType_Binary;
	if (strcmp(value, "CharStrCS") == 0) return eAosContentType_CharStrCS;
	if (strcmp(value, "CharStrCI") == 0) return eAosContentType_CharStrCI;

	return eAosContentType_Invalid;
}


aos_len_type_e aos_len_type_2enum(const char * const str)
{
	aos_assert_r(str, eAosLenType_Invalid);
	if (strcmp(str, "Unknown") == 0) return eAosLenType_Unknown;
	if (strcmp(str, "Contents") == 0) return eAosLenType_Contents;
	if (strcmp(str, "Fixed") == 0) return eAosLenType_Fixed;
	if (strcmp(str, "Conditional") == 0) return eAosLenType_Cond;

	return eAosLenType_Unknown;
}


const char * aos_len_type_2str(const aos_len_type_e type)
{
	switch (type)
	{
	case eAosLenType_Unknown:	
		 return "Unknown";

	case eAosLenType_Contents:
		 return "Contents";

	case eAosLenType_Fixed:
		 return "Fixed";

	case eAosLenType_Cond:
		 return "Conditional";

	default: 
		 aos_alarm("Unrecognized type: %d", type);
		 return "Incorrect";
	}

	aos_should_never_come_here;
	return "incorrect";
}
	

aos_presence_e aos_presence_2enum(const char * const str)
{
	aos_assert_r(str, eAosPresence_Invalid);
	if (strcmp(str, "Unknown") == 0) return eAosPresence_Unknown;
	if (strcmp(str, "Mandatory") == 0) return eAosPresence_Mandatory;
	if (strcmp(str, "ForcedMandatory") == 0) return eAosPresence_ForcedMandatory;
	if (strcmp(str, "Optional") == 0) return eAosPresence_Optional;
	if (strcmp(str, "Conditional") == 0) return eAosPresence_Cond;

	return eAosPresence_Invalid;
}


const char * aos_presence_2str(const aos_presence_e type)
{
	switch (type)
	{
	case eAosPresence_Unknown:
		 return "Unknown";

	case eAosPresence_Mandatory:
		 return "Mandatory";

	case eAosPresence_ForcedMandatory:
		 return "ForcedMandatory";

	case eAosPresence_Optional:
		 return "Optional";

	case eAosPresence_Cond:
		 return "Conditional";

	default: 
		 aos_alarm("Unrecognized type: %d", type);
		 return "Incorrect";
	}

	aos_should_never_come_here;
	return "incorrect";
}


const char * aos_scm_field_type_2str(const aos_scm_field_type_e type)
{
	switch (type)
	{
	case eAosScmFieldType_Xml:
		 return "XML";

	default: 
		 aos_alarm("Unrecognized type: %d", type);
		 return "Incorrect";
	}

	aos_should_never_come_here;
	return "incorrect";
}


aos_scm_field_type_e aos_scm_field_type_2enum(const char * const type)
{
	aos_assert_r(type, eAosScmFieldType_Invalid);

	if (strcmp(type, "XML") == 0) return eAosScmFieldType_Xml;

	aos_alarm("Unrecognized scm_field type: %s", type);
	return eAosScmFieldType_Invalid;
}


char * aos_scope_2str(const aos_scope_e scope)
{
	switch (scope)
	{
	case eAosScope_CrtStmch:
		 return "CrtStmch";

	default:
		 aos_alarm("Unrecognized scope: %d", scope);
		 return "invalid";
	}

	aos_should_never_come_here;
	return "Invalid";
}

aos_scope_e aos_scope_2enum(const char * const scope)
{
	aos_assert_r(scope, eAosScope_Invalid);

	if (strcmp(scope, "CrtStmch") == 0) return eAosScope_CrtStmch;

	aos_alarm("Unrecognized scope: %s", scope);
	return eAosScope_Invalid;
}

char *aos_obj_type_2str(const aos_obj_type_e type)
{
	switch (type)
	{
	case eAosObjType_StmchData:	return "StmchData";
	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return "Invalid";
	}

	aos_should_never_come_here;
	return "Invalid";
}


aos_obj_type_e aos_obj_type_2enum(const char * const type)
{
	aos_assert_r(type, eAosObjType_Invalid);

	if (strcmp(type, "StmchData") == 0) return eAosObjType_StmchData;

	aos_alarm("Unrecognized type: %s", type);
	return eAosObjType_Invalid;
}


