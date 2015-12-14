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
#ifndef aos_omni_parser_types_h
#define aos_omni_parser_types_h

#ifdef __cplusplus
extern "C" {
#endif

// 
// Type "aos_field_type_e"
//
typedef enum
{
	eAosFieldType_Invalid,
	
	eAosFieldType_Fixed,
	eAosFieldType_Container,
	eAosFieldType_Enum,
	eAosFieldType_Var,
	eAosFieldType_Xml,
	eAosFieldType_XmlMsg,
	
	eAosFieldType_Last
} aos_field_type_e;

static inline int aos_field_type_check(const aos_field_type_e type)
{
	return (type > eAosFieldType_Invalid && 
			type < eAosFieldType_Last)?1:0;
}

extern const char * aos_field_type_2str(aos_field_type_e);

// 
// Type "aos_parser_offset_type_e"
//
typedef enum 
{
	eAosOffsetType_Invalid,

	eAosOffsetType_Relative,
	eAosOffsetType_Absolute,

	eAosOffsetType_Last
} aos_parser_offset_type_e;

static inline int aos_offset_type_check(aos_parser_offset_type_e type)
{
	return (type > eAosOffsetType_Invalid && 
			type < eAosOffsetType_Last)?1:0;
}


// 
// Type "aos_content_type_e"
//
typedef enum
{
	eAosContentType_Invalid,

	eAosContentType_Binary,
	eAosContentType_CharStrCS,
	eAosContentType_CharStrCI,

	eAosContentType_Last
} aos_content_type_e;

static inline int aos_content_type_check(aos_content_type_e type)
{
	return (type > eAosContentType_Binary && 
			type < eAosContentType_Last)?1:0;
}

extern const char * aos_content_type_2str(const aos_content_type_e type);
extern aos_content_type_e aos_content_type_2enum(const char * const);

// 
// Type "aos_msg_selector_type_e"
//
typedef enum
{
	eAosMSType_Invalid = 0,

	eAosMSType_Fixed,
	eAosMSType_Enum,

	eAosMSType_Last
} aos_msg_selector_type_e;


static inline int aos_msg_selector_type_check(
		aos_msg_selector_type_e type)
{
	return (type > eAosMSType_Invalid && 
			type <eAosMSType_Last)?1:0;
}

// 
// Type "aos_cs_type_e"
//
typedef enum
{
	eAosCSType_Invalid = 0,

	eAosCSType_Fixed,
	eAosCSType_Enum,

	eAosCSType_Last
} aos_cs_type_e;


static inline int aos_cs_type_check(aos_cs_type_e type)
{
	return (type > eAosCSType_Invalid && 
			type < eAosCSType_Last)?1:0;
}

// 
// Type "aos_msg_type_e"
//
typedef enum 
{
	eAosMsgType_Invalid, 

	eAosMsgType_Xml, 
	eAosMsgType_XmlNode,

	eAosMsgType_Last
} aos_msg_type_e;


static inline int aos_msg_type_check(aos_msg_type_e type)
{
	return (type > eAosMsgType_Invalid && 
			type < eAosMsgType_Last)?1:0;
}


// 
// Type "aos_scm_field_type_e"
//
typedef enum
{
	eAosScmFieldType_Invalid,
	
	eAosScmFieldType_Xml,
	
	eAosScmFieldType_Last
} aos_scm_field_type_e;

static inline int aos_scm_field_type_check(aos_scm_field_type_e type)
{
	return (type > eAosScmFieldType_Invalid && 
			type < eAosScmFieldType_Last)?1:0;
}

extern const char * aos_scm_field_type_2str(const aos_scm_field_type_e type);
extern aos_scm_field_type_e aos_scm_field_type_2enum(const char * const type);


/////////////////////////aos_len_unit_e/////////////////

typedef enum
{
	eAosLenUnit_Invalid,

	eAosLenUnit_Unknown,
	eAosLenUnit_Bit,
	eAosLenUnit_Byte,

	eAosLenUnit_Last
} aos_len_unit_e;

static inline int aos_len_unit_check(aos_len_unit_e type)
{
	return (type > eAosLenUnit_Invalid && 
			type < eAosLenUnit_Last)?1:0;
}


/////////////////////////aos_len_type_e/////////////////

typedef enum
{
	eAosLenType_Invalid,

	eAosLenType_Unknown,	
	eAosLenType_Contents,	// Determined by contents
	eAosLenType_Fixed,		// Fixed length
	eAosLenType_Cond,		// Conditional

	eAosLenType_Last
} aos_len_type_e;

extern aos_len_type_e aos_len_type_2enum(const char * const str);
extern const char * aos_len_type_2str(const aos_len_type_e code);

static inline int aos_len_type_check(aos_len_type_e type)
{
	return (type > eAosLenType_Invalid && 
			type < eAosLenType_Last)?1:0;
}

/////////////////////////aos_presence_e/////////////////
typedef enum
{
	eAosPresence_Invalid,

	eAosPresence_Unknown,
	eAosPresence_Mandatory,
	eAosPresence_ForcedMandatory,
	eAosPresence_Optional,
	eAosPresence_Cond,

	eAosPresence_Last
} aos_presence_e;

static inline int aos_presence_check(aos_presence_e type)
{
	return (type > eAosPresence_Invalid && 
			type < eAosPresence_Last)?1:0;
}

extern aos_presence_e aos_presence_2enum(const char * const str);
extern const char * aos_presence_2str(const aos_presence_e code);


/////////////////////////aos_xml_node_type_e/////////////////
typedef enum
{
	eAosXmlNodeType_Invalid,

	eAosXmlNodeType_Int,
	eAosXmlNodeType_Str,
	eAosXmlNodeType_IpAddr,

	eAosXmlNodeType_Last,
} aos_xml_node_type_e;

static inline int aos_xml_node_type_check(aos_xml_node_type_e type)
{
	return (type > eAosXmlNodeType_Invalid && 
			type < eAosXmlNodeType_Last)?1:0;
}

extern aos_xml_node_type_e aos_xml_node_type_2enum(const char * const str);
extern const char * aos_xml_node_type_2str(const aos_xml_node_type_e code);




/////////////////////// aos_scope_e ///////////////////////
typedef enum
{
	eAosScope_Invalid = 0,

	eAosScope_CrtStmch,

	eAosScope_Last
} aos_scope_e;

static inline int aos_scope_check(const aos_scope_e type)
{
	return (type > eAosScope_Invalid && 
			type < eAosScope_Last);
}

extern char * aos_scope_2str(const aos_scope_e scope);
extern aos_scope_e aos_scope_2enum(const char * const scope);



/////////////////////// aos_obj_type_e ///////////////////////
typedef enum
{
	eAosObjType_Invalid = 0,

	eAosObjType_StmchData, 
	eAosObjType_ptr,

	eAosObjType_Last
} aos_obj_type_e;

static inline int aos_obj_type_check(const aos_obj_type_e type)
{
	return (type > eAosObjType_Invalid && 
			type < eAosObjType_Last);
}

extern char *aos_obj_type_2str(const aos_obj_type_e type);
extern aos_obj_type_e aos_obj_type_2enum(const char * const type);

#ifdef __cplusplus
}
#endif

#endif

