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
// 11/29/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MultiLang_LangTermIds_h
#define Aos_MultiLang_LangTermIds_h

#include "Util/String.h"

#define AOSLT_ACCESS_DENIED						"access_denied"
#define AOSLT_ADD_SLOT							"add_slot"

#define AOSLT_CONTAINER_NOT_FOUND				"ctnr_not_found"

#define AOSLT_DATA_IS_NOT_STRING				"data_is_not_string"
#define AOSLT_DATA_IS_NOT_XML					"data_is_not_xml"
#define AOSLT_DATA_IS_NOT_QUERY_RESULT			"data_is_not_query_rslt"
#define AOSLT_DATA_TYPE							"data_type"
#define AOSLT_DOC_NOT_FOUND						"doc_not_found"
#define AOSLT_DOC_SELECTOR_NAME					"doc_selector_name"
#define AOSLT_DUPLICATED_START					"duplicated_start"

#define AOSLT_ENTRY_NOT_FOUND					"entry_not_found"
#define AOSLT_ERROR								"error"

#define AOSLT_EXPECT_QUERY_RSLT_BUT_CHAR\
	"Expecting Query Result data type but received is char"

#define AOSLT_EXPECT_QUERY_RSLT_BUT_INT64\
	"Expecting Query Result data type but received is int64_t"

#define AOSLT_EXPECT_QUERY_RSLT_BUT_STR\
	"Expecting Query Result data type but received is string"

#define AOSLT_EXPECT_QUERY_RSLT_BUT_U64\
	"Expecting Query Result data type but received is u64"

#define AOSLT_EXPECT_QUERY_RSLT_BUT_XML\
	"Expecting Query Result data type but received is an XML doc"

#define AOSLT_FAILED_CREATING_DOC				"failed_creating_doc"
#define AOSLT_FAILED_CREATE_FILE				"failed_create_file"
#define AOSLT_FAILED_CREATE_USERDIR				"failed_create_usrdir"
#define AOSLT_FAILED_DETERMINE_DFTVPD			"failed_det_dftvpd"
#define AOSLT_FAILED_GET_SLOT					"failed_get_slot"
#define AOSLT_FAILED_MODIFY_DOC					"failed_modify_doc"
#define AOSLT_FAILED_PARSING_FNAMES				"failed_parse_filed"
#define AOSLT_FAILED_PARSE_XML					"failed_parse_xml"
#define AOSLT_FAILED_READ_FILE					"failed_read_file"
#define AOSLT_FAILED_RETRIEVE_DOC				"failed_retrieve_doc"
#define AOSLT_FAILED_RETRIEVE_GIC				"failed_retrieve_gic"
#define AOSLT_FAILED_RETREIVE_LOGIN_VPD			"failed_retrieve_loginvpd"
#define AOSLT_FAILED_RETRIEVE_CONTAINER			"failed_retrieve_ctnr"
#define AOSLT_FAILED_RETRIEVE_DFTVPD			"failed_retrieve_dftvpd"
#define AOSLT_FAILED_RETRIEVE_OWNER_CID			"failed_retrieve_ownercid"
#define AOSLT_FAILED_RESOLVE_URL				"failed_resolve_url"
#define AOSLT_FAILED_ADD_LOG					"failed_add_log"
#define AOSLT_FAILED_RETRIEVE_LOG				"failed_retrieve_log"
#define AOSLT_FILE_NOT_FOUND					"file_not_found"

#define AOSLT_GET_SLAB							"get_slab"
#define AOSLT_GET_SLOT							"get_slot"

#define AOSLT_INTERNAL_ERROR					"internal_error"
#define AOSLT_INVALID_DATA_TYPE					"invalid_datatype"
#define AOSLT_INVALID_DOC_SELECTOR				"invalid_docselector"
#define AOSLT_INVALID_FORMAT					"invalid_format"
#define AOSLT_INVALID_REQUEST					"invalid_request"
#define AOSLT_INVALID_RESULT_TYPE				"invalid_rslt_type"
#define AOSLT_INVALID_SAMPLING_SIZE				"invalid_sample_size"
#define AOSLT_INVALID_SEGMENT_SIZE				"invalid_segsize"
#define AOSLT_INVALID_SDOC						"invalid_sdoc"
#define AOSLT_INVALID_SIZE						"invalid_size"
#define AOSLT_INVALID_START_POSITION			"invalid_start_pos"
#define AOSLT_INVALID_URL						"invalid_url"
#define AOSLT_INVALID_URL_TYPE					"invalid_url_type"
#define AOSLT_INVALID_USER						"invalid_user"
#define AOSLT_ITEM_ID							"item_id"
#define AOSLT_ITEM_NOT_UNIQUE					"item_not_unique"

#define AOSLT_MISSING_ALGORITHM					"missing_algo"
#define AOSLT_MISSING_ARG_NAME					"missing_argname"
#define AOSLT_MISSING_ARGS						"missing_args"
#define AOSLT_MISSING_CREATEDOC_SDOC			"missing_createdoc_sdoc"
#define AOSLT_MISSING_CONTAINER					"missing_ctnr"
#define AOSLT_MISSING_CONTAINER_OBJID			"missing_ctnr_objid"
#define AOSLT_MISSING_COUNTERS					"missing_counters"
#define AOSLT_MISSING_DOC						"missing_doc"
#define AOSLT_MISSING_DOC_SELECTOR				"missing_docsel"
#define AOSLT_MISSING_DOC_SEPARATOR				"missing_doc_separator"
#define AOSLT_MISSING_DOCS_TO_PROCESS			"missing_docs_proc"
#define AOSLT_MISSING_DEADLINE					"missing_deadline"
#define AOSLT_MISSING_ENTRY_SEPARATOR			"missing_entry_sep"
#define AOSLT_MISSING_FIELD_SEPARATOR			"missing_field_separator"
#define AOSLT_MISSING_DATA_SEPARATOR			"missing_data_sep"
#define AOSLT_MISSING_FIELD						"missing_field"
#define AOSLT_MISSING_FIELDS					"missing_fields"
#define AOSLT_MISSING_FILENAME					"missing_filename"
#define AOSLT_MISSING_FILEOBJID					"missing_file_objid"
#define AOSLT_MISSING_ITEM_ID					"missing_item_id"
#define AOSLT_MISSING_MAP_TAG					"missing_map_tag"
#define AOSLT_MISSING_MODATTR					"missing_mod_attr"
#define AOSLT_MISSING_LOGIN_VPD					"missing_loginvpd"
#define AOSLT_MISSING_OBJIDS					"missing_objids"
#define AOSLT_MISSING_PARENT_CONTAINER			"missing_pctnr"
#define AOSLT_MISSING_RECEIVED_DOC				"missing_rec_doc"
#define AOSLT_MISSING_REQUEST_DOC				"missing_req_doc"
#define AOSLT_MISSING_SDOC						"missing_sdoc"
#define AOSLT_MISSING_SDOC_OBJID				"missing_sdoc_objid"
#define AOSLT_MISSING_STORED_ANAME				"missing_stored_aname"
#define AOSLT_MISSING_TAG						"missing_tag"
#define AOSLT_MISSING_TARGET_DOC				"missing_target_doc"
#define AOSLT_MISSING_TEMPLATE_OBJID			"missing_template_objid"
#define AOSLT_MISSING_TEMPLATE_DOC				"missing_template_doc"
#define AOSLT_MISSING_TEMPLATE_SDOC				"missing_template_sdoc"
#define AOSLT_MISSING_USER_DOMAIN				"missing_user_domain"
#define AOSLT_MISSING_VALUE_ANAME				"missing_value_aname"
#define AOSLT_MISSING_URL						"missing_url"
#define AOSLT_MISSING_WAREHOUSE_ID				"missing_warehouse_id"
#define AOSLT_MISSING_WAREHOUSE_ITEMS			"missing_warehouse_items"
#define AOSLT_MISSING_WORKING_DOC				"missing_working_doc"
#define AOSLT_MISSING_COUNTER					"missing_counter"
#define AOSLT_MISSING_OPERATION					"missing_operation"

#define AOSLT_URL_TYPE_MISMATCH					"mismatch_url_type"

#define AOSLT_NO_DEFAULT_USEROPRARD				"no_dft_user_oprard"
#define AOSLT_NO_VALUE_FOUND					"no_value_found"

#define AOSLT_PICKER_NOT_FOUND					"picker_not_found"

#define AOSLT_QUANTITY_IS_ZERO					"quantity_is_zero"

#define AOSLT_SDOC_IS_NULL						"sdoc_is_null"
#define AOSLT_SEGMENT_SIZE_TOO_BIG				"segsize_too_big"
#define AOSLT_SERVER_STOPPING					"server_stopping"
#define AOSLT_SIZE								"size"
#define AOSLT_SIZE_TOO_BIG						"size_too_big"
#define AOSLT_STATUS_OK							"status_ok"
#define AOSLT_START_POSITION					"start_pos"

#define AOSLT_UNRECOGNIZED_DATA_TYPE			"unrecog_data_type"
#define AOSLT_UNRECOGNIZED_DATA_PICKER_ID		"unrecog_data_picker"
#define AOSLT_UNRECOGNIZED_URL_TYPE				"unrecog_url_type"
#define AOSLT_URL_UNDEFINED						"url_undefined"
#define AOSLT_USER_NOT_FOUND					"user_not_found"
#define AOSLT_USER_NOT_VALID					"user_not_valid"


#define AOSLT_MODULE_LOG_SERVER_NOT_FOUND		"module_log_server_not_found"

#endif
