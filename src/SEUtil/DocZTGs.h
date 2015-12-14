////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/05/2011	Copied from DocZTGs.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_DocZTGs_h
#define Omn_SEUtil_DocZTGs_h

#include "Util/String.h"
// The following are used to identify IIL tags. As an example, 
// AOSZTG_ATTRNAME is the prefix of the ID of IILs used for attributes. 
// If a doc has an attribute, say 'firstname', we will create a value IIL
// for it. The IIL ID will be: AOSZTG_ATTRNAME + 'firstname', or
// "_zt0afirstname". When we want to search for docs based on values
// of the attribute 'firstname', we can use the IIL identified 
// as "_zt0afirstname". Since "_zt" is reserved by Zykie, this ensures
// that the IIL contains only the docs that do have the attribute
// 'firstname'. Otherwise, if we simply use the attribute name 
// 'firstname' to identify the IIL, there is no guarrantee that 
// docs that contain the word 'firstname' will not mistakenly 
// put into the IIL. 
// If a document contains a word that starts with "_zk", we will
// prepend the word with "_zk00", which is done by the word normalizer.
// When searching a word, if the word starts with "_zk", it is prepended
// with "_zk00". 
// For attributes, we do not care whether they start with "_zk" since
// if it is to create an IIL for an attribute, we always prepend it
// with AOSZTG_ATTRNAME. When searching docs based on attribute names,
// we will automatically prepend AOSZTG_ATTRNAME. This ensures that
// we will always use the right IILs. 
#define AOSZTG_ANAME					"_zt19"	// Attribute Name-Value IIL List

#define AOSZTG_DELTIME					"_zt1d" // Delete time
#define AOSZTG_DELUNAME					"_zt1e" // Delete Username
#define AOSZTG_DELDOCTYPE				"_zt1f" // Delete Doctype 
#define AOSZTG_DELAPPNAME				"_zt1g" // Delete app name 
#define AOSZTG_SITEID					"_zt1h" // SiteId
#define AOSZTG_COUNTER					"_zt1i" // Counters
#define AOSZTG_DELETELOG				"_zt1j" // Delete Doc Log
#define AOSZTG_LOGINEID					"_zt1k" // Login Entry ID
#define AOSZTG_LOGINUNAME				"_zt1l" // Login Username
#define AOSZTG_LOGINCTNR				"_zt1m" // Login Container
#define AOSZTG_LOGINTIME				"_zt1n" // Login Time
#define AOSZTG_CONTAINED_STR			"_zt1o" // Contained in a container by a key
#define AOSZTG_SITEOBJID				"_zt1p" // Object ID on a given site
#define AOSZTG_LOG						"_zt1q" // Logs
#define AOSZTG_CRT_SSID					"_zt1r" // Stores the current session ID
#define AOSZTG_LOGIN_SESSION			"_zt1s" // The login session IIL
#define AOSZTG_FSEQNO_USER				"_zt1t" // User File Seqno
#define AOSZTG_FSEQNO_PFOLDER			"_zt1u" // Public Folder File Seqno
#define AOSZTG_SESSION_STATUS			"_zt1v" // Reserved attr name for Session ID 
#define AOSZTG_ESCAPE					"_zt1w" // Used to escape
#define AOSZTG_URL						"_zt1x" // Used to index URLs
#define AOSZTG_PRICONTAINER				"_zt21" // Private Container prefix
#define AOSZTG_URLOBJID         		"_zt22" // URLs obj id
#define AOSZTG_LOGCTNR_LOGIN			"_zt23" // Login Log Container
#define AOSZTG_LOGCTNR_INVREAD			"_zt24" // Invalid Read Log Container
#define AOSZTG_VOTE						"_zt25" // Vote attribute name prefix
#define AOSZTG_SESSIONID				"_zt26" // Invalid Read Log Container
#define AOSZTG_CLOUDID					"_zt27" // For cloud id
#define AOSZTG_TRAFFICLIST      		"_zt28" // Traffic IIL List
#define AOSZTG_VOTELIST         		"_zt29" // Vote List
#define AOSZTG_SDOC_SEQNO				"_zt2a" // Smartdoc created doc
#define AOSZTG_CNTPREFIX        		"_zt2b" // Counter IIL Name Prefix
#define AOSZTG_GROUPORDER       		"_zt2c" // Conter order
#define AOSZTG_WHOVISITED       		"_zt30" // Who Visited
#define AOSZTG_MICROBLOG				"_zt31"
#define AOSZTG_TEMPOBJ					"_zt32" //temp obj
#define AOSZTG_ALLOWANCE				"_zt33" //allowance sdoc
#define AOSZTG_SYSCOUNTERS				"_zt34" // For counters
#define AOSZTG_TEMP_MSGS				"_zt35" // temporary message
#define AOSZTG_PMNT_MSGS				"_zt36" // permantent message
#define AOSZTG_HOOK						"_zt38" // For all hooks. Chen Ding 01/22/2011
#define AOSZTG_CMT_IIL					"_zt39" //comment doc iil.
#define AOSZTG_DELETEDDOC				"_zt3a" // Deleted doc
#define AOSZTG_SUMMARY					"_zt3b" 
#define AOSZTG_SESSIONIIL				"_zt3c" // Used by SessionMgr
#define AOSZTG_LOSTFOUND_LOGIN			"_zt3e"
#define AOSZTG_USER_OBJID_PREFIX		"_zt3f"
#define AOSZTG_DICTIONARYTERM			"_zt3g"
#define AOSZTG_GICID                    "_zt3h" //felicia,used for appinst
#define AOSTAG_APPINST                  "_zt3i" //felicia,used for appinst
#define AOSZTG_GICTYPE                  "_zt3j" //felicia,used for appinst
#define AOSZTG_SYSCTNR_OBJID_PREFIX		"_zt3k"
#define AOSZTG_CONTAINER				"_zt3l"
#define AOSZTG_KEYWORD 					"_zt3m"
#define AOSZTG_ATTRWORD_INDEX			"_zt3n"
#define AOSZTG_CTNR_ATTRWORD_IIL		"_zt3o"
#define AOSZTG_CTNRCTIME				"_zt3p"
#define AOSZTG_CTNROBJID				"_zt3q"
#define AOSZTG_SYSTEM_DOC				"_zt3r"
#define AOSZTG_DESCENDANT				"_zt3s"
#define AOSZTG_ANCESTOR					"_zt3t"
#define AOSZTG_VERSIONCOUNT_IILNAME		"_zt3u"
#define AOSZTG_LOGLISTING_IILNAME		"_zt3v"
#define AOSZTG_LOGGLOBALIILNAME         "_zt3w"
#define AOSZTG_IDGEN_IIL_NAME			"_zt3x"
#define AOSZTG_RELATION					"_zt3y"
#define AOSZTG_INLINESEARCH				"_zt3z" //ken 2011/12/05 ,used for inlinesearch
#define AOSZTG_RESERVED_OBJID			"_zt40" // Chen, 12/24/2011
#define AOSZTG_ALARM_CTNR				"_zt41" // Chen, 12/31/2011
#define AOSZTG_STMC_INST_DOC			"_zt42" // Chen, 01/04/2012
#define AOSZTG_SOBJ_ID_MGR				"_zt43" // Chen, 02/16/2012
#define AOSZTG_SUPERIIL					"_zt44" // Chen, 05/08/2012, for all super IILs
#define AOSZTG_SEGMENT_OBJID			"_zt45" // Chen, 07/24/2012, for all doc segments
#define AOSZTG_VIRTUALFILE_SEGMENT_CTNR	"_zt46" // Chen, 07/31/2012, for virtual file segment ctnrs
#define AOSZTG_VIRTUALFILE_CTNR         "_zt47" // xulei 07/31/2012, for virtual file.
#define AOSZTG_VIRTUALFILESIZE          "_zt48" // Chen, 07/30/2012, for virtual file sizes
#define AOSZTG_SYS_IDGENS				"_zt49IDGens"	// Chen Ding, 01/06/2013
#define AOSZTG_APP_IDGENS				"_zt4aIDGens"	// Chen Ding, 01/06/2013
#define AOSZTG_DOCUMENT					"_zt4bDocuments"	// felicia, 03/20/2013
#define AOSZTG_LOG_IILNAME_ATTR			"_zt4c"	// Chen Ding, 2013/06/14
#define AOSZTG_LOG_IILNAME_HIT			"_zt4d"	// Chen Ding, 2013/06/14
#define AOSZTG_JQL_STATEMENT            "_zt4e" // Chen Ding, 2013/06/14
#define AOSZTG_DISK_RECYCLE      		"_zt4f" // used for job disk Garbage Recycle 
#define AOSZTG_JIMO_CONTAINER      		"_zt4g" // used for jimo container prefix (Chen Ding, 2014/01/01)
#define AOSZTG_TIME_PARALLEL_IIL   		"_zt4h" // Identify time based parallel IIL (Chen Ding, 2014/01/10)
#define AOSZTG_SIZE_PARALLEL_IIL   		"_zt4i" // Identify size based parallel IIL (Chen Ding, 2014/01/10)
#define AOSZTG_JQL_TABLENAME_IIL		"_zt4g_tablename" // for JQL table names, Jackie, 2013/12/25
#define AOSZTG_INDEXMGR_IIL				"_zt4g_idxmgr" // for IndexMgr names, Felicia, 2014/01/18
#define AOSZTG_DATASET_IIL				"_zt4g_dataset" // for dataset names, Phil, 2014/01/20
#define AOSZTG_JIMOLOGIC_DOC 			"_zt4g_jimologic" // for jimologic names, Barry, 2015/06/23
#define AOSZTG_DATABASE_IIL				"_zt4g_database" // for database names, Phil, 2014/04/04
#define AOSZTG_SCHEMA_IIL				"_zt4g_schema" // for schema names, Phil, 2014/01/20
#define AOSZTG_DATA_SCANNER_IIL			"_zt4g_data_scanner" // for data scannernames
#define AOSZTG_DATA_RECORD_IIL			"_zt4g_data_record" // for data record names 
#define AOSZTG_DATA_PROC_IIL			"_zt4g_data_proc" // for data record names 
#define AOSZTG_DATA_FIELD_IIL			"_zt4g_data_field" // for data field names 
#define AOSZTG_VIRTUAL_FIELD_IIL		"_zt4g_virtual_field" // for data field names 
#define AOSZTG_DATA_CONNECTOR_IIL		"_zt4g_data_connector" // for data field names 
#define AOSZTG_JOB_IIL					"_zt4g_job" 
#define AOSZTG_SERVICE_IIL				"_zt4g_service" 
#define AOSZTG_USER_CTNR 				"_zt4g_user_ctnr" 
#define AOSZTG_TASK_IIL					"_zt4g_task"
#define AOSZTG_LOAD_DATA_IIL			"_zt4g_load_data" 
#define AOSZTG_STATIIL                  "_zt4k" // for stat iil, Linda, 2014/01/18
#define AOSZTG_IILNAME_INDEX			"_zt4l"	// For IndexMgr use
#define AOSZTG_MAP_IIL					"_zt4g_map" 
#define AOSZTG_DICT_DOC					"_zt4g_dict" 
#define AOSZTG_JIMO_LOGIC_IIL			"_zt4g_jimo_logic" // for jimo logic, Young, 2014/08/20
#define AOSZTG_STATISTICS				"_zt4g_statistics" // for statistics, felicia, 2014/09/16
#define AOSZTG_LOG_GROUP_TIME			"_zt4h_time" // for log aging
#define AOSZTG_LOG_GROUP_SIZE			"_zt4h_size" // for log aging
#define AOSZTG_ALARM					"zky_system_alarm_zt4g" // for alarms, felicia, 2014/10/30
#define AOSZTG_SNAPSHOTID				"_zt4h" // for Snapshotid, ice, 2014/10/29
#define AOSZTG_GENERICOBJ				"_zt4r" // for genericobj , andy, 2014/10/29
#define AOSZTG_AUTO_INCR_IIL			"_zt4s" // auto increment number, Chen Ding, 2015/01/15
#define AOSZTG_SEQUENCE_DOC             "_zt4g_sequence" // sequence, Young, 2015/02/12
#define AOSZTG_DATAPROC_CACHE_IIL		"_zt4i" // Chen Ding, 2015/05/26
#define AOSZTG_VALUE_MAP_IILNAME		"_zt4j" // Chen Ding, 2015/09/13
#define AOSZTG_COMPOUND_IILNAME			"_zt4l" // Chen Ding, 2015/09/13

#define AOSZTG_ONLINESTATUS    	"online_status"
#define AOSZTG_BUDDYLIST      	"buddylist"
#define AOSZTG_APPLY_BUDDY 		"buddyapply"
#define AOSZTG_COUNTDOWN 		"zky_countdown"
#define AOSZTG_SCHEDULER		"zky_scheduler"
#endif
