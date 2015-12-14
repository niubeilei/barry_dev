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
// 	Created: 10/08/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_DocTags_h
#define Omn_SEUtil_DocTags_h

#include "SEUtil/DocZTGs.h"
#include "SEUtil/DocOTypes.h"
#include "SEUtil/DocSTypes.h"
#include "SEUtil/Containers.h"
#include "Util/String.h"

enum
{
	eAosMaxSiteid = 500,
	eAosReservedDocid = 4000
};

#define AOS_INVDTYPE 0
#define AOS_TAGSEP ":"
#define AOS_CONTAINER_SEP ":"
#define AOS_CIDSEP ":"

#define AOS_DFTSITEID			"100"
#define AOSDFT_OBJID_PREFIX		"objid_"
#define AOS_DFTSITEID			"100"
/////////////////////////add by Jozhi//////////////////////////////
#define AOSTAG_MBLOG_SENDER_CID			"zky_mblog_sender_cid"		// Jozhi, 2011/05/28
#define AOSTAG_MBLOG_SENDER_DOCID		"zky_mblog_sender_docid"	// Jozhi, 2011/05/28
#define AOSTAG_MBLOG_SENDER_UNAME		"zky_mblog_sender_uname"	// Jozhi, 2011/05/28
#define AOSTAG_MBLOG_SENDER_RNAME		"zky_mblog_sender_rname"	// Jozhi, 2011/05/28
#define AOSTAG_MBLOG_SENDER_IMG			"zky_mblog_sender_img"		// Jozhi, 2011/05/28

#define AOSTAG_FOLLOWER_DOCID			"zky_follower_docid"		// Jozhi, 2011/05/28
#define AOSTAG_BYFOLLOWER_DOCID       	"zky_byfollower_docid"		// Jozhi, 2011/05/28

#define AOSTAG_APP_NAME					"zky_appname"				// Jozhi, 2011/06/17
#define AOSTAG_APP_MICROBLOG			"microblog"					// Jozhi, 2011/06/17
#define AOSTAG_APP_IM					"im"						// Jozhi, 2011/06/17

#define AOSTAG_IM_SENDER_CID       		"zky_im_sender_cid"			// Jozhi, 2011/05/28
#define AOSTAG_IM_RECVER_CID       		"zky_im_recver_cid"			// Jozhi, 2011/05/28
#define AOSTAG_IM_SENDER_NAME       	"zky_im_sender_nname"		// Jozhi, 2011/05/28
#define AOSTAG_IM_RECVER_NAME			"zky_im_recver_nname"		// Jozhi, 2011/05/28

#define AOSTAG_MICROBLOG_LOG_CTNR		"microblog_log_ctnr"	// Jozhi, 2011/05/28
#define AOSTAG_CTNR_MICROBLOG			"microblog_ctnr"		// Jozhi, 2011/05/28
#define AOSTAG_INSTANTMSG_CTNR  		"instantmsg_ctnr"		// Jozhi, 2011/05/28

#define AOSTAG_CTNR_FRIEND      		"friendctnr"				// Jozhi, 2011/05/28
#define AOSTAG_CTNR_FOLLOWER 			"followerctnr"
#define AOSTAG_CTNR_BYFOLLOWER 			"byfollowerctnr"
#define AOSTAG_CTNR_PENDINGMSG			"pendingmsgctnr"
#define AOSTAG_CTNR_TALKGROUP			"talkgroup"
#define AOSTAG_CTNR_CFOLLOWER			"cfollowerctnr"
#define AOSTAG_CTNR_DATA				"userdata"

/////////////////////////add by Jozhi end//////////////////////////////


// Below are the reserved root-level attribute names. 

#define AOSTAG_SINGLEQUOTE '\''
#define AOSTAG_DOUBLEQUOTE '"'
#define AOSTAG_DOCS_CREATED     "zky_docctd"    // The number of docs created
#define AOSTAG_CHANNELID        "zky_chid"      // Channel id
#define AOSTAG_CTNR_ALBUM		"zky_abmctnr"	// Album Container
#define AOSTAG_ACCT_DUR			"zky_acctdur"	// User Account duration 
#define AOSTAG_ACCESSED_DOCID	"zky_acddocid"	// Accessed Object Docid 
#define AOSTAG_ACCESSED_OBJID	"zky_acdobjid"	// Accessed Object Objid
#define AOSTAG_ACCESSED_STYPE	"zky_acdstype"	// Accessed Object sub type 
#define AOSTAG_ACCESSED_TYPE	"zky_acdtype"	// Accessed Object type 
#define AOSTAG_ACCESSED_VER		"zky_acdver"	// Accessed Object Version
#define AOSTAG_ACCT_TYPE		"zky_acctype"	// User Account Type
#define AOSTAG_ACTIVATION_NUM	"zky_actnum"	// Activation Number
#define AOSTAG_ADDOBJID_ACSTYPE	"zky_addobjid"	// Add Objid Access Type
#define AOSTAG_ADDMEM_ACSTYPE	"zky_addmem"	// Add Member Type
#define AOSTAG_AFTLGI_VPD       "zky_aftlgi_vpd" //After login vpd
#define AOSTAG_ALLOW_CRTSPUSR	"zky_alwcsu"	// Allow creating super user
#define AOSTAG_AFTLOGIN_VPD 	"zky_aftloginvpd"	// Login VPD Name
#define AOSTAG_AMT_ANAME		"zky_amtanm"	// 
#define AOSTAG_CTNR_ATCL		"zky_atclctnr"	// Article Container
#define AOSTAG_AUTOCREATED		"zky_autoct"	// Auto Created
#define AOSTAG_DELMEM_ACSTYPE	"zky_delmem"	// Delete Member Type
#define AOSTAG_DETERM_NODE_METHOD	"zky_dtmnodemd"	// Determining Node Method
#define AOSTAG_DFTLOCALE        "zky_dftlocale" // Default Locale
#define AOSTAG_AIFNAME			"zky_aifname"	// AI Filename
#define AOSTAG_AITNAILNAME		"zky_aitname"	// AI Thumbnail file name
#define AOSTAG_APPNAME			"zky_appnm"		// Application ID 
#define AOSTAG_AUTHOR			"zky_author"	// Author
#define AOSTAG_CTNR_BLOG		"zky_blgctnr"	// Blog Container
#define AOSTAG_BLOCKSIZE		"zky_bsize"		// Block size
#define AOSTAG_BYOBJID			"zky_byobjid"
#define AOSTAG_CHANNELID        "zky_chid"              // Channel id
#define AOSTAG_CLOUDID			"zky_cloudid__a"	// Cloud ID
#define AOSTAG_CTNR_CLOUDCARD	"zky_ccard_ctnr"
#define AOSTAG_CTNR_CMT			"zky_cmtctnr"	// Comment Container
#define AOSTAG_COMPID			"zky_compid"	// Component id 
#define AOSTAG_COLLECTMEM_TYPE1 "zky_collect"
#define AOSTAG_CMTTHREAD		"zky_cmtthrd"	// Commented Thread 
#define AOSTAG_CMTED_DOC		"zky_cmteddoc"	// Commented doc
#define AOSTAG_COUNTERPREFIX	"zky_cnt"		// Counter Prefix
#define AOSTAG_COUNTERCM		"zky_cntcm"		// Being Commented Counter
#define AOSTAG_COUNTERDW		"zky_cntdw"		// Down Counter
#define AOSTAG_COUNTERLK		"zky_cntlk"		// Being Linked Counter
#define AOSTAG_COUNTERRC		"zky_cntrc"		// Being Recommented Counter
#define AOSTAG_COUNTERWR		"zky_cntwr"		// Being Counter ??
#define AOSTAG_COUNTERUP		"zky_cntup"		// Being Counter ??
#define AOSTAG_COUNTERRD		"zky_cntrd"		// Read Counter
#define AOSTAG_CRTID			"zky_cntrid"	// Current ID
#define AOSTAG_COUNTETRDAY      "zky_cntrday"   // Day traffic
#define AOSTAG_COUNTETRWE       "zky_cntrweek"  // Traffic Week
#define AOSTAG_COUNTETRMO       "zky_cntrmon"   // Traffic month
#define AOSTAG_COUNTETRYE       "zky_cntryear"  // Year Traffic
#define AOSTAG_COUNERTUP		"zky_cntup"		// Up Counter
#define AOSTAG_COUNERTWR		"zky_cntwt"		// Write Counter
#define AOSTAG_COOKIES			"zky_cookies"	// Cookies
#define AOSTAG_COPY_ACSTYPE		"zky_copy"		// Copy Type
#define AOSTAG_CREATE_PUBLIC	"zky_crtpub"	// Used to indicate creating public docs
#define AOSTAG_CREATOR			"zky_crtor"		// Document creator
#define AOSTAG_CRT_MEMBERS		"zky_crtnmb"	// Current number of members
#define AOSTAG_CRT_NUM_CTNRS	"zky_crtnctrs"	// Current number of containers
#define AOSTAG_CRT_SESSIONID	"zky_crtsid"	// Current session id
#define AOSTAG_CRT_SEQNO		"zky_crtsqn"	// Current sequence number
#define AOSTAG_CTIME			"zky_ctime"		// Document creation time
#define AOSTAG_LOG_CTIME		"zky_log_ctime"	// Document creation time
#define AOSTAG_CT_EPOCH			"zky_ctmepo__d"	// Create Time (Epoch time)
#define AOSTAG_LOG_CT_EPOCH		"zky_log_ctmepo__d"	// Create Time (Epoch time)
#define AOSTAG_CTNR_OBJID		"zky_ctnroid"	// Container Objid
#define AOSTAG_CONTAINER		"zky_ctnr"		// Container
#define AOSTAG_CONTAINER_NOZK	"container"		// Container
#define AOSTAG_CONDITIONS		"zky_conds"		// Conditions
#define AOSTAG_CONDITIONID		"zky_condid"	// Condition ID
#define AOSTAG_CREATE_ACSTYPE	"zky_create"	// Create Type
#define AOSTAG_CTNRID			"zky_ctnrid"	// Container ID
#define AOSTAG_DELETE_BY		"zky_delby"		// Deleted by
#define AOSTAG_DATA				"zky_data"		
#define AOSTAG_DATANAME			"zky_dataname"	
#define AOSTAG_DATATYPE			"zky_datatype"	
#define AOSTAG_DESCRIPTION		"zky_desc"		// Description
#define AOSTAG_DFTHPVPD			"zky_dfthpvpd"	// Default Homepage VPD
#define AOSTAG_DFTVPDS			"zky_dftvpds"	// Default VPD Tagname
#define AOSTAG_DIMENSION		"zky_dimen"		// Picture dimension
#define AOSTAG_DIRNAME			"zky_dirnm"		// Directory name
#define AOSTAG_DELETE_ACSTYPE	"zky_delete"	// Delete Type
#define AOSTAG_DOCID			"zky_docid"		// DocID
#define AOSTAG_DOCID_UNIQUE		"zky_didunique"	
#define AOSTAG_DOCID_ANAME		"zky_docidnm"	//
#define AOSTAG_DOCS_CREATED		"zky_docctd"	// The number of docs created
#define AOSTAG_DOCNAME			"docname"
#define AOSTAG_DOCSELECTOR		"zky_docselector"
#define AOSTAG_DFTVPD_POLIGY	"zky_dvpdpl"	// Default VPD Policy
#define AOSTAG_EDITOR_FLAG		"zky_editor"	// The "Editor" flag
#define AOSTAG_ELEMID			"zky_elmid"		// Element ID
#define AOSTAG_EMAIL			"zky_email"		//
#define AOSTAG_EMAIL1			"zky_email1"	//
#define AOSTAG_EMPTYTREE_POLICY	"zky_emptrplc"	
#define AOSTAG_EVPD				"zky_evpd"		// EVPD
#define AOSTAG_END				"zky_end"		
#define AOSTAG_ENDTIME			"zky_endtm"		// End time
#define AOSTAG_ENUMTYPE			"zky_enumtype"
#define AOSTAG_FAIL				"zky_fail"		// 
#define AOSTAG_FAILED			"zky_failed"
#define AOSTAG_FEATURESETS 		"zky_featuresets"	 
#define AOSTAG_FFNAME			"zky_ffname"	//image file name. 
#define AOSTAG_FRIENDID			"zky_friendid"	//add friend cloud id. 
#define AOSTAG_FULLTIME         "zky_fulltm"
#define AOSTAG_EPOTIME          "zky_epotime"	// Ketty 2012/03/09
#define AOSTAG_GROUPS			"zky_groups"	// User groups
#define AOSTAG_GROUP_NAME       "zky_Gname"     // Group name
#define AOSTAG_GROUP_MEMBER     "zky_Gmember"   // Group member
#define AOSTAG_HASMSG        	"zky_hasMsg"    // if has message.
#define AOSTAG_CTNR_HOME        "zky_hmctnr"    // Home Container
#define AOSTAG_HPVPD			"zky_hpvpd"		// Homepage VPD Name
#define AOSTAG_WORK_VPD			"zky_workvpd"
#define AOSTAG_PUBLIC_VPD		"zky_publicvpd"
#define AOSTAG_FAMILY_VPD		"zky_familyvpd"
#define AOSTAG_HPCONTAINER      "zky_pctrs"     // Public Home Parent Containe
#define AOSTAG_HOOK             "zky_hook"		// Chen Ding, 2011/01/22
#define AOSTAG_INCVALUE         "zky_incvalue"
#define AOSTAG_ID               "id"
#define AOSTAG_ICOMMENTED		"zky_icmobj"	// Immediate Commented Object
#define AOSTAG_IDDEF			"zky_iddef"		// ID Definition
#define AOSTAG_IILSELECTOR		"iilselector"	
#define AOSTAG_IILID			"zky_iilid"		// iilid
#define AOSTAG_INDEX			"zky_idx"		// Index
#define AOSTAG_INDEXED_ATTRS	"zky_idxdattrs"	// Indexed Attributes
#define AOSTAG_IDXONLY_ATTRS	"zky_idxonlyats"// Index Only Attributes
#define AOSTAG_IMGDIR			"zky_imgdir"	// Image Directory
#define AOSTAG_IMAGEPATH        "image_path"    // Image Path
#define AOSTAG_ITEM_OTYPE		"zky_itmotp"	//
#define AOSTAG_ITEM_STYPE		"zky_itmostp"	//
#define AOSTAG_ITEM_CT_ACTIONS	"zky_itmctac"	//
#define AOSTAG_CTNR_IMAGE		"zky_images"	// Image container
#define AOSTAG_ORIG_IMGFNAME	"zky_imgfn"		// Original Image Filename
#define AOSTAG_INVREAD			"zky_invread"	// Invalid Read
#define AOSTAG_LANGUAGECODE		"zky_langcode"	// 
#define AOSTAG_LABEL_ANAME		"zky_lblaname"	// 
#define AOSTAG_LABEL_SELECTOR	"zky_lblsel"	// 
#define AOSTAG_LOCALE			"zky_locale"	// Locale
#define AOSTAG_LOGCONFIG		"zky_logconfig"	
#define AOSTAG_LOGDIR			"zky_logdir"	
#define AOSTAG_LOGID			"zky_docid"		// Log ID
#define AOSTAG_JSPATH           "js_path"       // JavaScript Path
#define AOSTAG_DICTNODENAME		"zky_dictnm" 	// dictionary tagname
#define AOSTAG_LOGIN_LOG		"zky_lgnlg"		// Login Logs
#define AOSTAG_LOGIN_OBJ		"zky_lgnobj"	// Login Object 
#define AOSTAG_LOGIN_VPD		"zky_lgnvpd"	// Login VPD Name
#define AOSTAG_LOGINNUM			"login_num"		// Login number
#define AOSTAG_LOGTIME			"zky_logtm"		// Log Time
#define AOSTAG_LOGTYPE			"zky_logtp"		// Log type
#define AOSTAG_LSTNID           "zky_lstnid"    // listener id
#define AOSTAG_NEED_LOGINOBJ	"loginobj"		// Need login obj
#define AOSTAG_LISTING_ACSTYPE	"zky_ls_actp"	// Listing Type
#define AOSTAG_MAX_CTNRS		"zky_maxctnrs"	// Max number of containers
#define AOSTAG_MAXDOCS			"zky_maxdocs"	// Max number of docs
#define AOSTAG_MAX_DOCSIZE		"zky_maxdsz"	// Max doc size
#define AOSTAG_MAXLEN			"zky_maxlen"	// Max length
#define AOSTAG_MAXDOCS          "zky_maxdocs"   // Max number of docs
#define AOSTAG_MAX_MEMBERS		"zky_maxmb"		// Max number of members 
#define AOSTAG_MSGSVRID			"zky_msgsvrid"	// Message server id.
#define AOSTAG_MSGTYPE			"zky_msgtp"		// Message type
#define AOSTAG_CTNR_USERDFT		"zky_usrdft_ctnr"
#define AOSTAG_MULLOGIN_TTIME	"zky_mlgttm"	// Multi-Login total time
#define AOSTAG_MULLOGIN_UTIME	"zky_mlgutm"	// Multi-Login used time
#define AOSTAG_MULLOGIN_NUSRS	"zky_mlgnusrs"	// Multi-Login number of users
#define AOSTAG_MTIME			"zky_mtime"		// Last modify time
#define AOSTAG_MT_EPOCH			"zky_mtmepo__d"	// Modify Time (Epoch time)
#define AOSTAG_MODUSER			"zky_musr"		// The user who last modified the doc
#define AOSTAG_MEMBEROF			"zky_memberof"	// 
// #define AOSTAG_MEMBEROFS		"memberofs"	
#define AOSTAG_NAME				"zky_name"		// Generic name tag
#define AOSTAG_NAME_BIND		"namebd"		// Action's attr namebd
#define AOSTAG_NEW_DFTHPVPD		"zky_newhpvpd"	// Create new Homepage VPD
#define AOSTAG_NUM_LOGIN		"zky_numlg"		// Number of logins
#define AOSTAG_NEXTBLOCKSIZE	"zky_nxbsize"	// Next block size
#define AOSTAG_NUMUSAGE			"zky_numusg"	// Number of Usage
#define AOSTAG_OBJID			"zky_objid"		// Object ID (objid)
#define AOSTAG_OBJID_ANAME		"zky_objidnm"	// 
#define AOSTAG_OBJID_RESERVED	"zky_objidrsd"	// Objid reserved flag
#define AOSTAG_OBJIDTEMP		"zky_objidtpl"	// Objid Template
#define AOSTAG_OBJID_TYPE		"zky_oidtype"	// 
#define AOSTAG_OBJIMAGE			"zky_objimg"	// Object Image
#define AOSTAG_OBJNAME			"zky_objnm"		// Object Name
#define AOSTAG_OBJNAMETEMP		"zky_objnmtpl"	// Object Name Template
#define AOSTAG_OPR				"zky_opr"		// operation.
#define AOSTAG_OPERATOR			"opr" 
#define AOSTAG_ORIG_CMTED		"zky_orgcmted"	// Original Commented 
#define AOSTAG_ORIG_FNAME		"zky_orgfnm"	// Original File Name
#define AOSTAG_ORDER			"zky_order"		// 
#define AOSTAG_ORDER_DOC_ACTS	"zky_orderacts"	// 
#define AOSTAG_TABLE					"zky_table"
#define AOSTAG_OTYPE			"zky_otype"		// Object type 
#define AOSTAG_OVERRIDE         "zky_override"  // is override url
#define AOSTAG_OWNER_DOCID		"zky_owndocid"	// Owner docid
#define AOSTAG_OWNER_CID		"zky_ownercid"	// Owner cid
#define AOSTAG_PUB_CONTAINER    "zky_pctrs"     // Public Home Parent Container
#define AOSTAG_PREFIX_METHOD	"zky_prefxmd"   //
#define AOSTAG_PUBACCT_LENGTH	"zky_pactlen"	// Public Account Time Length
#define AOSTAG_PUBACCT_START	"zky_pactst"	// Public Account Start Time
#define AOSTAG_PASSWD			"zky_passwd__n"	// User password
#define AOSTAG_OLDPASSWD		"zky_old_passwd__n"	// User old password
#define AOSTAG_PARENTC			"zky_pctrs"		// Public Home Parent Container
#define AOSTAG_PARM				"parm"		// Public Home Parent Container
#define AOSTAG_PARENT_FOLDER	"zky_pfolder"	// Parent Folder
#define AOSTAG_PNAME			"pname"	// Parent Folder
#define AOSTAG_CTNR_PUBLIC		"zky_public_ctnr"	
#define AOSTAG_PUBLIC_DOC		"zky_public_doc"	
#define AOSTAG_QUANTITY_ANAME	"zky_qtaname"
#define AOSTAG_QUERYID			"queryid"
#define AOSTAG_REALNAME			"zky_realnm"	// User real name
#define AOSTAG_REAL_PARENT 		"zky_realprt"	// Real Parent
#define AOSTAG_REAL_OTYPE		"zky_realotp"	// Real Otype
#define AOSTAG_READ_ACSTYPE		"zky_read"		// Reading Access Type
#define AOSTAG_REQUESTER		"zky_reqtr"		// Requester
#define AOSTAG_REQUESTINDEX		"zky_rqidx"		// Request Index
#define AOSTAG_ROLES			"zky_roles"		// User roles
#define AOSTAG_REQUIRED_LOGIN	"zky_rqlgn"		// Requiring Login
#define AOSTAG_RESOURCE_ID		"zky_rscid"		//
#define AOSTAG_RSCDIR			"zky_rscdir"	// Resource DIR
#define AOSTAG_RSC_FNAME		"zky_rscfn"		// Resource filename
#define AOSTAG_CTNR_RSM			"zky_rsmctnr"	// Electronic Resume Container
#define AOSTAG_SAVEAS			"zky_saveas"	// 
#define AOSTAG_SUPERVISOR		"zky_spv"       // superviser 
#define AOSTAG_SDOCACT_MODONLY	"zky_sact_modonly"	// Used by actions, modify only
#define AOSTAG_SDOCOBJID		"zky_sdobjid"	// smartdoc objid, Chen Ding, 2011/01/22
#define AOSTAG_SDOCCTNR			"zky_sdcctnr"	// create ctnr smartdoc objid
#define AOSTAG_SYSADMIN 		"zky_sysadmin"	
#define AOSTAG_SYSACCT			"zky_sysacct"	
#define AOSTAG_PREV_SDOCOBJID	"zky_prev_sdobjid"	// smartdoc previous process, Ketty
#define AOSTAG_POST_SDOCOBJID	"zky_post_sdobjid"	// smartdoc posterior process, Ketty
#define AOSTAG_SDOC_VOTE		"zky_svobjid"	// smartdoc objid,  Vote
#define AOSTAG_SVRCMD_ACSTYPE	"zky_sc_actp"	// Server Command Type
#define AOSTAG_SEX				"zky_sex"		// User Sex
#define AOSTAG_SENDMSG_ACSTYPE	"zky_sendmsg"	// Send Message Type
#define AOSTAG_SEQNO_PREFIX		"zky_seqnopx"	// 
#define AOSTAG_SEQNOONLY_ANAME	"zky_seqnoan"	// 
#define AOSTAG_SEQNOONLY_ID		"zky_seqnoid"	// 
#define AOSTAG_SEQNOONLY_TNAME	"zky_seqnotn"	// 
#define AOSTAG_SESSIONID		"zky_ssid"		// Session ID
#define AOSTAG_SESSIONTYPE		"zky_ssntype"	// Session type
#define AOSTAG_STEP         	"zky_step"
#define AOSTAG_SITEID			"zky_siteid"	// Site ID
#define AOSTAG_SMARTDOC_TYPE    "zky_sdoctp"    //
#define AOSTAG_SORTED_ATTRS	 	"zky_sorted_attr"	// Container sorted attr. Ken 01/15/2011
#define AOSTAG_SORTEDATTRS	 	"sortedattrs"	// Container sorted attr. Ken 01/15/2011
#define AOSTAG_START			"zky_start"	
#define AOSTAG_STARTTIME		"zky_sttime"	// Start time
#define AOSTAG_STATUS			"zky_status"	// 
#define AOSTAG_SUBTYPE			"zky_stype"		// Document subtype
#define AOSTAG_SUCCESS			"zky_success"	// 
#define AOSTAG_SUM_RECORD		"zky_sum_record"	// 
#define AOSTAG_TAG				"zky_tag"		// Standard Tags
#define AOSTAG_TAGC				"zky_tagc"		// Customer Defined Tags
#define AOSTAG_TEMPLATE			"zky_templ"		// Template
#define AOSTAG_TICKET_ANAME		"zky_ticket_aname"
#define AOSTAG_TEMP_QUERY		"zky_tmp_query"	// temporary data for querying 
#define AOSTAG_TIMESTAMP		"zky_tstmp"		// Time Stamp
#define AOSTAG_TIME             "zky_time"
#define AOSTAG_TNAIL			"zky_tnail"		// Document thumbnail
#define AOSTAG_TOTALTIME		"zky_tltime"	// Total Time
#define AOSTAG_TOTALUSAGE		"zky_tlusg"		// Total usage
#define AOSTAG_FROM_IDNAME		"zky_fmidname"
#define AOSTAG_FRIEND_DOMAINS	"zky_frddomains"// Friend domains
#define AOSTAG_TO_IDNAME		"zky_toidname"
#define AOSTAG_URI              "zky_uri"       // Resource Name Resolve.
#define AOSTAG_URL              "zky_url"       // 
#define AOSTAG_USERNAME			"zky_uname"		// User name
#define AOSTAG_UNAMEPREFIX		"zky_umpre"		// Username Prefix
#define AOSTAG_USEDTIME			"zky_usedtm"	// Used time
#define AOSTAG_USER_HOMEDIR		"zky_ushmdir"	// User Home directory 
#define AOSTAG_USER_HOMEFOLDER	"zky_ushmfld"   // User Home Folder
#define AOSTAG_USER_IMGDIR		"zky_usimgdir"	// User Image directory 
#define AOSTAG_USER_RSCDIR		"zky_usrscdir"	// User Resource directory 
#define AOSTAG_USERID			"zky_usrid"		// User ID
#define AOSTAG_USEROBJID		"zky_usroid"	// User Objid
#define AOSTAG_CTNR_USER		"zky_usrctnr"	// User Container
#define AOSTAG_USER_ROLE		"zky_usrole"	// User Role
#define AOSTAG_USERDOMAIN_OBJID	"zky_usrdm_objid"	// Ken, 01/07/2011
#define AOSTAG_USERSTATUS		"zky_usrst"		// User Status
#define AOSTAG_USERTYPE			"zky_usrtp"		// User Type
#define AOSTAG_VERSION			"zky_ver__n"	// Version
#define AOSTAG_VERSIONIILID		"versioniilid"	
#define AOSTAG_VOTE_ACCESSES	"zky_voteacs" 	// Vote accesses
#define AOSTAG_VOTEDOWN			"zky_votedown" 	// Vote Down
#define AOSTAG_VOTEUP			"zky_voteup"   	// Vote Up
#define AOSTAG_VOTETOTAL        "zky_votetotal" // Vote total
#define AOSTAG_CTNR_LOG        	"zky_logctnrname" 		// Vote 
#define AOSTAG_CTNR_VPD         "zky_vpd_ctnr"
#define AOSTAG_VPDNAME          "zky_vpdname"   // vpd name
#define AOSTAG_VALUEDEF			"zky_valuedef"   
#define AOSTAG_VALUE_UNIQUE		"zky_valunique"   
#define AOSTAG_VVPD				"zky_vvpd"		// VVPD
#define AOSTAG_VPDTMPLNAME		"zky_vpdtmpl"	// VPD Template
#define AOSTAG_VPD_NAMEBASE		"zky_vpdnbs"	// VPD Name Base
#define AOSTAG_VPD_TEMPLATE		"zky_vtmlt"		// VPD Template
#define AOSTAG_CTNR_PUBLISH     "zky_publsh"   	// Publish Container
#define AOSTAG_CTNR_DIARY		"zky_diary"		// Diary Container
#define AOSTAG_WRITE_ACSTYPE	"zky_write"		// Write Access Type
#define AOSTAG_USERPREFIX		"zkyuser"		// User Prefix
#define AOSTAG_SVPD				"svpd"			// User Prefix
#define AOSTAG_PANEL_VPD_NAME	"panel_vpd_name"// panel Vpd Name
#define AOSTAG_GIC_VPDNAME		"gic_vpdname"	// gic vpd name
#define AOSTAG_GIC_TYPE			"gic_type"	// gic vpd name
#define AOSTAG_COUNTDW_TARGET	"countdown_target"		
#define AOSTAG_CRT_COUNTDW 		"crt_countdw"		
#define AOSTAG_CUTDOWN_FLAG		"countdown_flag"		
#define AOSTAG_RUNSMARTDOC		"zky_runsmdoc"	// 
#define AOSTAG_COUNTDOWN_END_ANAME		"endtime_aname"    //
#define AOSTAG_ENDING_TIME				"ending_time"	 
#define AOSTAG_COUNTDOWN_REMAIN_ANME	"remaintime_aname"	 
#define AOSTAG_REMAINING_TIME			"remaining_time"	 
#define AOSTAG_COUNTDOWN_TYPE			"countdown_type"	 
#define AOSTAG_DAYS						"zky_days"
#define AOSTAG_CREATEORDER				"createorder"
#define AOSTAG_OBJID_SEP				"separator"
#define AOSTAG_OBJID_SEQNO_ACT			"seqno_act"
#define AOSTAG_SEQNO_TNAME				"seqno_aname"
#define AOSTAG_SELECTOR					"selector"
#define AOSTAG_JXC_TAG					"zky_jxc_tag"
#define AOSTAG_INITVALUE				"zky_initvalue"
#define AOSTAG_INDEX_ANAME				"zky_index_aname"
#define AOSTAG_COMMENT					"zky_comment"
#define AOSTAG_COMMENTED				"zky_cmted"	//  Commented 
#define AOSTAG_ORIG_CMTED				"zky_orgcmted"	// Original Commented 
#define AOSTAG_COUNTERS         	  "zky_cnts"
#define AOSTAG_SMARTDOCS_VOTE         "zky_sdocvote"
#define AOSTAG_COUNTER_OPERATION      "zky_cntopt"
#define AOSTAG_COUNTER_TYPE	   		  "zky_cnttype"
#define AOSTAG_COUNTER_SUBTYPE	      "zky_subcnttype"
#define AOSTAG_CREATEDOC_ACT			"createdoc_act"
#define AOSTAG_TICKS					"zky_ticks_no"
#define AOSTAG_TRANS_ACK				"ack"
#define AOSTAG_TRANS_RESP				"resp"
#define AOSTAG_TRANS_TYPE				"zky_trans_type"
#define AOSTAG_TRANSID					"zky_transid"
#define AOSTAG_TRANS_STATUS				"zky_trans_status"
#define AOSTAG_TRANS_FINISHED			"zky_trans_finished"
#define AOSTAG_TRANS_FAILED				"zky_trans_failed"
#define AOSTAG_TRANSTIMER				"zky_trans_timer"
#define AOSTAG_TRANS_RESP_HEADER		"trans_header"
#define AOSTAG_TRANS_RESP_FILENAME		"trans_resp"
#define AOSTAG_NEED_RESP				"trans_need_resp"
#define AOSTAG_NEED_INDEX				"trans_need_index"
#define AOSTAG_RESP_BLOCK				"trans_resp_block"
#define AOSTAG_THREAD_ID				"thread_id"
#define AOSTAG_HOSTID					"host_id"
#define AOSTAG_CONNID					"conn_id"
#define AOSTAG_DISTID					"dist_id"
#define AOSTAG_ADDR_PORT				"addr_port"
#define AOSTAG_PROC_THREAD_NUM			"proc_thread_num"
#define AOSTAG_NOFULLDOC_CHECKING		"nofulldoc_ck"
#define AOSVALUE_NOFULLDOC_CHECKING		"63w6tsgfs64wrtg"
#define AOSTAG_VPD_TYPE   			    "zky_vpdtype"
#define AOSTAG_SYSSERVER   			    "zky_syssvr"
#define AOSTAG_DELETED   			    "zky_deletedyet"
#define AOSTAG_VERDEL_FLAG   			"delete_yet"

//Tom 06/01/2011	for SeLogClient/SeLogServer
#define AOSQUERYTYPE_SELOGS         	"qry_selogs"
#define AOSTAG_LOG_OPR					"log_opr"
#define AOSTAG_LOG_PCTROBJID			"pctr_objid"
#define AOSTAG_LOG_INFO					"loginfo"

// Brian Zhang 19/08/2011
#define AOSTAG_LOG_IILIDS				"zky_log_iilids"
#define AOSTAG_DFT_LOGNAME				"zky_dft_logname"
#define AOSTAG_REFCOUNT					"zky_ref_count"
#define AOSTAG_DELETABLE				"zky_delable"
#define AOSTAG_LOGCREATE_TIME			"zky_crtime"
#define AOSTAG_CREATE_SYSUSER			"zky_crt_suser"
#define AOSTAG_SDOC_TESTER				"zky_sdoc_tester"
#define AOSTAG_REQ_PASSWD   			"zky_req_passwd"
#define AOSTAG_FILEOBJID	   			"zky_file_objid"
#define AOSTAG_JOBOBJID	   				"zky_job_objid"
#define AOSTAG_EXPORT_DIR               "zky_export_dir"
#define AOSTAG_EXPORT_FILENAME          "zky_export_filename"
#define AOSTAG_HEADER			        "zky_header"

// Brian Zhang 2012/02/24
#define AOSTAG_DEADLINE                 "zky_deadline"
#define AOSTAG_MODATTR	                "zky_modattr"
#define AOSTAG_TIME_UP		            "zky_time_up"
#define AOSTAG_SELECT_DOC	            "zky_selectdoc"


// Brian Zhang 06/21/2012
#define AOSTAG_RMTYPE	          	  "zky_rmtype"

//Ken 01/05/2011 for microblog
#define AOSTAG_CTNR_MBLOG			"zky_mblogctnr"	//mblog container
#define AOSTAG_CTNR_LOSTFOUND			"zky_lostfound"	//lostfound container
#define AOSTAG_DOCSRC					"zky_docsrc"
#define AOSTAG_ACCESSED_DOCOBJID		"zky_byobjid"
#define	AOSTAG_DOCIDX					"zky_byidx"
#define AOSTAG_DOCPATH					"zky_docpath"
#define	AOSTAG_DOCTMPL					"zky_doctmpl"
#define	AOSTAG_OBJIDTMPL				"objid_tmpl"
#define	AOSTAG_PARTIAL_DOC_FNAMES		"zky_partial_fnames"
#define AOSTAG_FULLDOC_HASH             "zky_fullhash"
#define	AOSTAG_ISFULLDOC				"zky_isfull"
//#define AOSTAG_MONITORTID				"zky_monitor_tid"	// Ketty 2011/12/21
//#define AOSTAG_PREMONITORTID			"zky_pre_monitor_tid"	// Ketty 2011/12/21
#define AOSTAG_BROWSERID				"zky_browserid"
#define AOSTAG_MONITOR					"monitor"
#define AOSTAG_PROCTIME					"zky_proc_time"
#define AOSTAG_READ_DEFAULT				"zky_read_default"
#define AOSTAG_ATTR_ACCESS				"attrs_access"
#define AOSTAG_TIME_ATTRNAME 			"zky_time_aname"
#define AOSTAG_TIMEFORMAT				"zky_time_format"
#define AOSTAG_ID_ATTRNAME				"zky_id_attrname"
#define AOSTAG_VALUE_ATTRNAME			"zky_value_attrname"
#define AOSTAG_DEFAULT_SITE				"zky_dftsite"
#define AOSTAG_DEFAULT_TREE				"zky_dfttree"
#define AOSTAG_DEFAULT_VALUE 			"zky_default_value"
#define AOSTAG_SUM_TYPE					"zky_sumtype"
#define AOSTAG_SUMID					"zky_sumid"
#define AOSTAG_SUM                      "zky_sum"
#define AOSTAG_CTNR_SUMID				"sumid"
#define AOSTAG_AVER                     "zky_aver"
#define AOSTAG_MAX                      "zky_max"
#define AOSTAG_MIN                      "zky_min"


// Phnix 04/20/2011 for TermSum
#define AOSTAG_KEYNAME					"zky_key_name"
#define AOSTAG_YEAR						"zky_year"
#define AOSTAG_YEAR_NUMS				"zky_year_nums"
#define AOSTAG_MONTH					"zky_month"
#define AOSTAG_MONTH_NUMS				"zky_month_nums"
#define AOSTAG_DAY						"zky_day"
#define AOSTAG_DAY_NUMS					"zky_day_nums"
#define AOSTAG_HOUR						"zky_hour"
#define AOSTAG_HOUR_NUMS				"zky_hour_nums"
#define AOSTAG_ENTRIES					"zky_entries"
#define AOSTAG_DOC_PUBLIC				"zky_doc_public"
#define AOSTAG_CID_REQUIRED				"zky_cid_required"
#define AOSTAG_CREATEDOC_ACTIONS        "zky_createdoc_actions"
#define	AOSTAG_PROCDOC_ACTIONS			"zky_procdoc_actions" 
#define	AOSTAG_MODDOC_ACTIONS			"zky_modifydoc_actions" 
#define AOSTAG_MAIN_PUBLIC              "zky_main_public"
#define AOSTAG_ENTRY_ACTIONS			"zky_entry_actions"
#define AOSTAG_ENTRY_PUBLIC				"zky_entry_public"
#define AOSTAG_ENTRY_CID_REQUIRED		"zky_entry_cid_required"
#define AOSTAG_SELECT_FLAG_NAME         "zky_select_flag_name"
#define AOSTAG_REMOVE_FLAG				"zky_remove_flag"
#define AOSTAG_CREATE_MAIN				"zky_create_main"
#define AOSTAG_MODIFY_MAIN				"zky_modify_main"
#define AOSTAG_SMARTDOCS_EMAIL          "zky_sdocemail" 
#define AOSTAG_EXIST     	            "zky_exist"
#define AOSTAG_NODE_ATTRNAME			"zky_nodeaname"
#define AOSTAG_NODE_VALUES				"zky_nodevalues"
#define AOSTAG_NON_EXIST                "zky_non_exist"
#define AOSTAG_DATA_TYPE                "zky_data_type"
#define AOSTAG_XPATH                    "zky_xpath"
#define AOSTAG_COPYFLAG					"zky_copy"
#define AOSTAG_CODE_LOADED				"zky_codeld"
#define AOSTAG_DOC_DOCID                "zky_doc_docid"
#define AOSTAG_VARNAME                  "zky_varname"
#define AOSTAG_DOC_OBJID                "zky_doc_objid"
#define AOSTAG_DOC_CLOUDID				"zky_doc_cloudid"
#define AOSTAG_VALUE_TYPE               "zky_value_type"
#define AOSTAG_VALUESET_TYPE            "zky_valueset_type"
#define AOSTAG_VALUESET     	        "zky_valueset"
#define AOSTAG_VALUESELECTOR            "zky_valueselector"
#define AOSTAG_DOCSELECTOR_TYPE         "zky_docselector_type"
#define AOSTAG_VALUESET_TYPE         	"zky_valueset_type"
#define AOSTAG_KEY						"zky_key"
#define AOSTAG_MAPVALUE					"zky_mapvalue"
#define AOSTAG_SET_ALLNONE 		        "zky_set_allnone" 
#define AOSTAG_FIELDIDX 		        "db_fidx" 
#define AOSTAG_FIELDIDX 		        "db_fidx" 
#define AOSTAG_RUN						"doc_run"
#define AOSTAG_ACTION_TYPE				"type"				// action type
#define AOSTAG_ACTIONEXEC				"act_exec"			// action exec or not
#define AOSTAG_ANAME					"aname"				
#define AOSTAG_SMARTDOC_OPR				"zky_operation"
#define AOSTAG_QUERY				    "query"		
#define AOSTAG_JOINTAG					"join"
#define AOSTAG_ATTRNAME					"attrname"		// Action's attrname
#define AOSTAG_TYPE						"type"	
#define AOSTAG_CONDITION                "condition"
#define AOSTAG_ACTIONS					"actions"
#define AOSTAG_ACTION					"action"
#define AOSTAG_RECORD					"record"
#define AOSTAG_ZKY_SLT					"zky_slt"
#define AOSTAG_TASKNUM					"tasknum"		//task number.
#define AOSTAG_IILNAME					"zky_iilname"	//iil name.
#define AOSTAG_IILKEY					"zky_iilkey"	//iil key.
#define AOSTAG_ALLOWANCE				"zky_allowance"//allowance task.
#define AOSTAG_ALLOWANCETIME         	"allowance_time"
#define AOSTAG_ALLOWANCEVALUE         	"allowance_value"
#define AOSTAG_ONLINETIME         		"onlntime"
#define AOSTAG_ONLINESTATUS         	"onlnstatus"
#define AOSTAG_COUNTER_RSLTPREFIX		"zkytm"
#define AOSTAG_VALUE					"zky_value"
#define AOSTAG_TIME						"zky_time"
#define AOSTAG_CNAME					"zky_cname"
#define AOSTAG_STYPE                    "zky_stype" 
#define AOSTAG_PUBLISHER                "zky_publisher"
#define AOSTAG_FORUM                    "zky_forum" 
#define AOSTAG_TOPIC                    "zky_topic"
#define AOSTAG_THREAD                   "zky_thread"
#define AOSTAG_CTNR_FORUMS              "zky_ctnr_forums"
#define AOSTAG_ACCESS                   "zky_access"
#define AOSTAG_WHOREADME_SDOCID         "zky_whoreadme_sdocid"
#define AOSTAG_WHOREADME_LOG            "zky_whoreadme_log"
#define AOSTAG_ALIPAY_OPR         		"alipay_opr"
#define AOSTAG_REGIST_PAY_STATUS		"usr_level"
#define AOSTAG_CTNR_TEMP				"tmpctnr"
#define AOSTAG_TEMPACCESS				"tmpaccess"
#define AOSTAG_RESERVE_OPR				"zky_reserve_opr"
#define AOSTAG_RES_THRESHOLD_NUM		"zky_threshold_num"
#define AOSTAG_RES_AVAILABLE			"zky_available_num"
#define AOSTAG_RESERVED_NUM				"zky_reserved_num"
#define AOSTAG_RES_ACTUAL				"zky_actual_num"
#define AOSTAG_CTNR_RES					"zky_res_ctnr"
#define AOSTAG_RES_STATUS				"zky_res_status"
#define AOSTAG_RES_CANCEL				"zky_res_cancel"
#define AOSTAG_RES_CONFIRM				"zky_res_confirm"
#define AOSTAG_RES_RESERVED				"zky_res_reserved"
#define AOSTAG_RES_TIMER				"zky_res_timer"
#define AOSTAG_RESERVED_NUMBER			"zky_reserved_number"
#define AOSTAG_RESERVED_OBJID			"zky_reserved_objid"
#define AOSTAG_LHS  					"zky_lhs"
#define AOSTAG_RHS  					"zky_rhs"
#define AOSTAG_STATUS_BREAK				"zky_status_break"
#define AOSTAG_STATUS_RETURN			"zky_status_return"
#define AOSTAG_STATUS_CONTINUE			"zky_status_continue"
#define AOSTAG_INTERRUPT				"zky_interrupt"
#define AOSTAG_PARTIAL_DOC_SIGNATURE	"zky_partdoc_sig"
#define AOSTAG_FULLDOC_SIGNATURE		"zky_fulldoc_sig"
#define AOSTAG_TRANS_INDEX_FILENAME		"zky_transidxfname"
#define AOSTAG_MSGCONTAINER_OBJID		"zky_msgctnrobjid"
#define AOSTAG_XMLDOCID                 "zky_xmldocid"       //felica, 2011/09/08
#define AOSTAG_EVENTDEFS                "zky_event_defs"     //Brian Zhang 10/25/2011 
#define AOSTAG_RESPONSE                 "zky_response"     //Brian Zhang 11/23/2011 
#define AOSTAG_FIELDS	                "fields"     //Brian Zhang 11/23/2011 



#define AOSCDOWNTYPE_TIME 				"Time"	 
#define ZKY_STARTNUM					"zky_startnum"
#define ZKY_ATTRSET						"zky_attrset"
#define TBN_TNAILW					"tnailw"
#define TBN_TNAILH					"tnailh"
#define TBN_IMAGES					"images"
#define TBN_IMAGE					"image"
#define TBN_LAYER					"layer"
#define TBN_PATH					"path"
#define TBN_SRC						"src"
#define AOSCODE_WORDIGNORED		"471"
#define AOSQUERYTYPE_MONITORLOG			"qry_monitorlog"
#define TBN_POSITION_PATH			"position_path"

#define AOSCONFIG_SELOG					"selog"
//#define AOSCONFIG_DOCTRANSID_FNAME      "doctransid_fname"
//#define AOSCONFIG_TRANSACTIONS			"transactions"
#define AOSCONFIG_TRANS					"transaction"
//#define AOSCONFIG_TRANS_FILE			"transfile"
#define AOSCONFIG_TRANS_CLIENT			"transclient"
#define AOSCONFIG_TRANS_SERVER 			"transserver"
#define AOSCONFIG_TRANS_MAXCACHESIZE	"trans_max_cachesize"
//#define AOSCONFIG_TRANSMAXQSIZE         "trans_max_qsize"
//#define AOSCONFIG_SYNC					"sync"
#define AOSCONFIG_SERVER				"server"
#define AOSCONFIG_SERVERS				"servers"
#define AOSCONFIG_REPLICPOLICY          "replic_policy"
#define AOSCONFIG_CSLEEPTIME			"csleeptime"
#define AOSCONFIG_RESEND_TIME			"resend_time"
#define AOSCONFIG_RECOVER_TIME			"recover_time"
#define AOSCONFIG_LOCAL					"local"
#define AOSCONFIG_LOG					"zky_log"
#define AOSCONFIG_FILEKEY				"file_key"
#define AOSCONFIG_FILEKEY_SIZE			"file_key_size"
#define AOSCONFIG_STORAGEMGR			"storage_mgr"
#define AOSCONFIG_DOCFILEMGRGROUP		"dfm_grp"
#define AOSCONFIG_STORAGE_POLICY		"storage_policy"
#define AOSCONFIG_DEVICE				"storage_device"
#define AOSCONFIG_PARTITION				"storage_partition"
#define AOSCONFIG_SPACE_ALLOC			"storage_space_alloc"
#define AOSCONFIG_MODULEID				"module_id"
#define AOSCONFIG_CLIENTID              "client_id"     //Ketty 2011/12/26
#define AOSCONFIG_CREATE				"create"
#define AOSCONFIG_SYSTEMADMIN			"admin"
#define AOSCONFIG_BACKUPDIR				"backup_dir"
#define AOSCONFIG_BACKUPFREQ			"backup_freq"
#define AOSCONFIG_BATCHSIZE				"batch_size"
#define AOSCONFIG_CTNR					"container"
#define AOSCONFIG_CUSTFNAME				"scws_custfname"
#define AOSCONFIG_DATAPATH				"datapath"
#define AOSCONFIG_DFTCTNR_TNAIL			"dft_ctnr_tnail"
#define AOSCONFIG_DFTCTNR_TAGS			"dft_ctnr_tags"
#define AOSCONFIG_DFTCTNR_EVPD			"dft_ctnr_evpd"
#define AOSCONFIG_DFTCTNR_VVPD			"dft_ctnr_vvpd"
#define AOSCONFIG_DFTPUBDURATION		"dft_publen"
#define AOSCONFIG_DFTACCTDUR			"dft_acct_dur"
#define AOSCONFIG_DFTDOCID_BLOCKSIZE	"dft_docid_bsize"
#define AOSCONFIG_DFTIILID_BLOCKSIZE	"dft_iilid_bsize"
#define AOSCONFIG_DFTIMGID_BLOCKSIZE	"dft_imgid_bsize"
#define AOSCONFIG_DFT_HOMECTNR			"dft_home_container"
#define AOSCONFIG_DFTREAD_POLICY		"dftpolicy_rd"
#define AOSCONFIG_DFTWRITE_POLICY		"dftpolicy_wt"
#define AOSCONFIG_DFTCREATE_POLICY		"dftpolicy_ct"
#define AOSCONFIG_DFTDELETE_POLICY		"dftpolicy_dl"
#define AOSCONFIG_DFTCOPY_POLICY		"dftpolicy_cp"
#define AOSCONFIG_DFTLISTING_POLICY		"dftpolicy_ls"
#define AOSCONFIG_DFTADDMEM_POLICY		"dftpolicy_am"
#define AOSCONFIG_DFTDELMEM_POLICY		"dftpolicy_dm"
#define AOSCONFIG_DFTSVRCMD_POLICY		"dftpolicy_sc"
#define AOSCONFIG_DFTSENDMSG_POLICY		"dftpolicy_sm"
#define AOSCONFIG_DIRNAME				"dirname"
#define AOSCONFIG_WORKINGDIR			"working_dir"
#define AOSCONFIG_TARGETDIRNAME			"target_dirname"
#define AOSCONFIG_DICTFNAME				"scws_dictfname"
#define AOSCONFIG_DICTINIFNAME			"scws_dictinifname"
#define AOSCONFIG_IDX_FILENAME 			"idx_filename"
#define AOSCONFIG_DOC_FILENAME 			"doc_filename"
#define AOSCONFIG_STARTDOCID			"docid_start"
#define AOSCONFIG_DOC_IDGENNAME 		"docidgen_name"
#define AOSCONFIG_SHOWLOG				"showlog"
#define AOSCONFIG_DOMAINADDR			"domain_addr"
#define AOSCONFIG_ELEMIDMGR				"elem_id_mgr"
#define AOSCONFIG_FILENAME				"filename"
#define AOSCONFIG_IGNORED_HASHNAME		"ignored_hashname"
#define AOSCONFIG_IDBLOCK_SIZE			"idblock_size"
#define AOSCONFIG_IIL_FILENAME 			"iil_filename"
#define AOSCONFIG_IIL_INCSIZE			"iil_incsize"
#define AOSCONFIG_IIL_MAX_HDRFILES		"iil_max_headerfiles"
#define AOSCONFIG_IIL_MAX_DOCFILES		"iil_max_docfiles"
#define AOSCONFIG_IIL_IDGENNAME 		"iilidgen_name"
#define AOSCONFIG_IILMGR				"iilmgr"
#define AOSCONFIG_IILMGR_SANITYCHECK	"iilmgr_sanitycheck"
#define AOSCONFIG_IILMGR_CHECKREF		"iilmgr_checkref"
#define AOSCONFIG_IILMGR_CHECKPTR		"iilmgr_checkptr"
#define AOSCONFIG_SCWS					"scws"
#define AOSCONFIG_STARTIILID			"iilid_start"
#define AOSCONFIG_STARTIMGID			"imgid_start"
#define AOSCONFIG_MAX_IILS				"max_iils"
#define AOSCONFIG_MAX_IILSIZE			"max_iilsize"
#define AOSCONFIG_MAX_NORM_IILSIZE		"max_norm_iilsize"
#define AOSCONFIG_MAX_BIG_IILSIZE		"max_big_iilsize"
#define AOSCONFIG_MIN_IILSIZE        	"min_iilsize"
#define AOSCONFIG_MAX_SUBIILSIZE        "max_subiilsize"
#define AOSCONFIG_MIN_SUBIILSIZE        "min_subiilsize"
#define AOSCONFIG_MAX_FILESIZE			"max_filesize"
#define AOSCONFIG_MESSAGEDOC			"message_doc"
#define AOSCONFIG_MSGSVR				"msgsvr"
#define AOSCONFIG_LARGE_FILESIZE		"large_filesize"   //large File
#define AOSCONFIG_LOCAL_ADDR			"local_addr"
#define AOSCONFIG_LOCAL_PORT			"local_port"
#define AOSCONFIG_LENGTH_TYPE			"length_type"
#define AOSCONFIG_MAX_OBJID_TRIES		"max_objid_tries"
#define AOSCONFIG_MAX_STRLEN			"max_strlen"
#define AOSCONFIG_MINLOGIN_NUM			"min_login_num"
#define AOSCONFIG_NoOwnerGrpFlag		"noower_grp_flag"
#define AOSCONFIG_NoOwnerRoleFlag		"noower_role_flag"
#define AOSCONFIG_NUM_PORTS				"num_ports"
#define AOSCONFIG_OBJNAME_ALBUM			"albums"
#define AOSCONFIG_OBJNAME_BLOGS			"blogs"
#define AOSCONFIG_OBJNAME_COMMENTS		"comments"
#define AOSCONFIG_OBJNAME_ATCL			"articles"
#define AOSCONFIG_OBJNAME_PUBLISH		"publish"
#define AOSCONFIG_OBJNAME_DIARY			"diarylog"
#define AOSCONFIG_REMOTE_ADDR			"remote_addr"
#define AOSCONFIG_REMOTE_PORT			"remote_port"
#define AOSCONFIG_SECURITYMGR			"security_mgr"
#define AOSCONFIG_SERVERREPAIRING		"server_repairing"
#define AOSCONFIG_SESSIONMGR			"session_mgr"
#define AOSCONFIG_TIMED_UNIT			"timed_unit"
#define AOSCONFIG_TORTURER				"Torturer"
#define AOSCONFIG_USER_DATADIR			"user_datadir"
#define AOSCONFIG_VERSION_FILENAME		"version_filename"
#define AOSCONFIG_WEIGHT				"tot_weight"
#define AOSCONFIG_WORDMGR 				"wordmgr"
#define AOSCONFIG_WORDID_HASHNAME 		"wordid_hashname"
#define AOSCONFIG_WORDID_TABLESIZE 		"wordid_tablesize"
#define AOSCONFIG_WORDNORM_HASHNAME		"wordnorm_hashname"
#define AOSCONFIG_XMLDOC_INCSIZE		"xmldoc_incsize"
#define AOSCONFIG_XMLDOC_MAX_HDRFILES	"xmldoc_max_headerfiles"
#define AOSCONFIG_XMLDOC_MAX_DOCFILES	"xmldoc_max_docfiles"
#define AOSCONFIG_IMAGE_TRASHDIR		"image_trashdir"
#define AOSCONFIG_IMGCONFIG				"image"
#define AOSCONFIG_IMGDIR				"imgdir"
#define AOSCONFIG_IMGBACKDIR			"imgback_dir"
#define AOSCONFIG_USER_BASEDIR			"user_basedir"
#define AOSCONFIG_SYSPASSWD				"sys_passwd"
#define AOSCONFIG_LOCALE                "locale"
#define AOSCONFIG_MAXLEN				"maxlen"
#define AOSCONFIG_NEXTBLOCKSIZE			"nextbsize"
#define AOSCONFIG_RANGES				"ranges"
#define AOSCONFIG_CRTID					"crtid"
#define AOSCONFIG_BLOCKSIZE				"crtbsize"
#define AOSCONFIG_BCK_DIRNAME			"backdirname"
#define AOSCONFIG_IDGENS				"IdGens"
#define AOSCONFIG_FILENAME				"filename"
#define AOSCONFIG_CIDPREFIX				"cid_prefix"
#define AOSCONFIG_SECURITY				"security"
#define AOSCONFIG_SESSION_FNAME			"session_fname"
#define AOSCONFIG_ACCTYPE_READ			"read"
#define AOSCONFIG_ACCTYPE_DELMEM 		"delmem"
#define AOSCONFIG_ACCTYPE_ADDMEM		"addmem"
#define AOSCONFIG_ACCTYPE_DELETE		"delete"
#define AOSCONFIG_ACCTYPE_COPY			"copy"
#define AOSCONFIG_ACCTYPE_CREATE		"create"
#define AOSCONFIG_ACCTYPE_WRITE			"write"
#define AOSCONFIG_DFT_ARCD				"dft_access_record"
#define AOSCONFIG_INIT_IDGENS			"init_idgens"
#define AOSCONFIG_PASSWD				"password"
#define AOSCONFIG_FNAME					"filename"
#define AOSCONFIG_FILEDIR				"zky_filedir"
#define AOSCONFIG_COPIES				"copies"
#define AOSCONFIG_FILEMODE				"filemode"
#define AOSCONFIG_LOCATIONS				"locations"
#define AOSCONFIG_DEFS					"defs"
#define AOSCONFIG_IMAGEDIR				"imagedir"
#define AOSCONFIG_SITEMGR				"sitemgr"
#define AOSCONFIG_TIMERMGR				"timermgr"
#define AOSCONFIG_TIMER_FNAME			"timer_fname"
#define AOSCONFIG_TIMER_DIRNAME			"timer_dir"
#define AOSCONFIG_NETWORKMGR			"NetworkMgr"
#define AOSCONFIG_SYSMONITOR			"SysMonitor"

//ken 2012-03-23
#define AOSCONFIG_IDMAP_SINGLE_SIZE		"single_size"
#define AOSCONFIG_IDMAP_QUEUE_SIZE		"queue_size"
#define AOSCONFIG_IDMAP_MAX_TRANS		"max_trans"
#define AOSCONFIG_IDMAP_BATCH_SIZE		"batch_size"
#define AOSCONFIG_IDMAP_POSTPROC		"post_proc"
#define AOSCONFIG_IDMAP_PROCQUEUESIZE	"proc_queue_size"
#define AOSCONFIG_IDMAP_PROCTRANSSIZE	"proc_trans_size"


#define AOSTNAIL_DFT_CTNR				"xxx"

#define AOSTAGS_DFT_CTNR				"container,autocreated"

#define AOSEVPD_DFT_CTNR				"vpd_dft_ctnrevpd"

#define AOSVVPD_DFT_CTNR				"vpd_dft_ctnrvvpd"

#define AOSDOCSTYPE_AUTO				"zky_auto"

#define AOSLOGTYPE_CREATE_USER			"lgtp_ctusr"
#define AOSLOGTYPE_CREATE_CTNR			"lgtp_ctctnr"
#define AOSLOGTYPE_LOGIN				"lgtp_login"

#define AOSTMP_MODIFY_TEMPLATE			"zkytmp_modtmpl"
#define AOSDFTCONTAINER_USER			"useracct"
#define AOSDFT_USRIMGDIR				"/home/images"

#define AOSAPPNAME_SYSTEM				"system"
#define AOSAPPNAME_APPLICATION			"app_self"
#define AOSAPPNAME_UNKNOWN_CLIENT		"unkclient"

#define AOSTAGID_ROLES					"roles"
#define AOSTAGID_GROUPS					"grps"

#define AOSUSRSTATUS_ACTIVE				"active"
#define AOSUSRSTATUS_INVALID			"invalid"
#define AOSUSRSTATUS_NOT_ACTIVATED		"notactivated"		// Not activated yet

#define AOSDIRNAME_IMAGES				"imgs"
#define AOSDIRNAME_RESOURCES			"rscs"

#define AOSSESSIONTYPE_TIMED			"timed"
#define AOSSESSIONTYPE_NUM				"num"
#define AOSSESSIONTYPE_NORM				"norm"

#define AOSUSER_SYSTEM					"system"
#define AOSUSER_GUEST					"guest"

#define AOSLOGID_LOGIN					"zky_login"		// Login Log
#define AOSLOGID_CTNRLOG				"zky_ctnrlg"	// Ctnr administration Log

#define AOSUSERTYPE_SYSROOT				"zky_sysroot"	// System Root
#define AOSUSERTYPE_ROOT				"zky_root"		// Super user
#define AOSUSERTYPE_NORM				"zky_norm"		// Normal users
#define AOSUSERTYPE_GUEST				"zky_guest"		// Guest Users
#define AOSUSERTYPE_UNKNOWN				"zky_unknown"	// Unknown Users
#define AOSUSERTYPE_ADMIN				"zky_admin"		// Admin Users
#define AOSUSERTYPE_UNREG				"zky_unreg"		// Not registered Users

#define AOSLOGIN_LOGIN_SUCCESS			"success"
#define AOSLOGIN_UNAME_NOTUNIQ			"(1030)User name not unique"
#define AOSLOGIN_ACCT_ACTV				"(1031)activated"		
#define AOSLOGIN_ACCT_REACTV			"(1032)reactivated"		
#define AOSLOGIN_NOT_ACTIVATED			"(1033)no_actvivated"	
#define AOSLOGIN_SUCCESS				"(1034)success"		
#define AOSLOGIN_CREATE_USER			"(1035)create_usr"
#define AOSLOGIN_NOPASSWD				"(1036)nopasswd"
#define AOSLOGIN_MISSING_PASSWORD		"(1037)missing password"	
#define AOSLOGIN_USER_NOTFOUND			"(1038)User Not Found"
#define AOSLOGIN_MISS_UNAME				"(1039)Missing User Name"
#define AOSLOGIN_PASSWD_NOT_MATCH		"(1040)Password Not Match"
#define AOSLOGIN_FAILED_ACTIVATE		"(1041)Failed to activate the account"
#define AOSLOGIN_MISS_PASSWORD			"(1042)Missing Password"
#define AOSLOGIN_MISS_CID				"(1042)Missing Cloud ID"

#define AOSERROR_INTERNAL				"(1100)Internal Error"

#define AOSPASSWD_SYSTEM				"2=dal;4dcdarwekj"	// System User Password


#define AOSLOGSTATUS_SUCCESS			"logstatus_success"

#define AOSCID_SYSTEM					"system"
#define AOSDOCIDSTR_IDGENMGR			"200"

#define AOS_CTNR_SEP					","
#define AOS_TAG_SEP						" ,"

#define AOSACTP_LOGIN_PUBLIC			"lpublic"
#define AOSACTP_PRIVATE					"private"
#define AOSACTP_PUBLIC					"public"
#define AOSACTP_ROOT					"root"

#define AOSUSERNAME_SYSROOT				"sysroot"
#define AOSUSERNAME_ROOT				"root"

// 28/09/2010 phnix for SmartDoc Define 
#define AOSTAG_SMARTDOC_OPR				"zky_operation"
#define AOSTAG_QUERY				    "query"		
#define AOSTAG_JOINTAG					"join"
#define AOSTAG_ATTRNAME					"attrname"		// Action's attrname
#define AOSTAG_TYPE						"type"	
#define AOSTAG_ACTIONS					"actions"
#define AOSTAG_ACTION					"action"
#define AOSTAG_RECORD					"record"
#define AOSTAG_ZKY_SLT					"zky_slt"
#define AOSTAG_TASKNUM					"tasknum"		//task number.
#define AOSTAG_IILNAME					"zky_iilname"	//iil name.
#define AOSTAG_ALLOWANCE				"zky_allowance"//allowance task.
#define AOSTAG_ALLOWANCETIME         	"allowance_time"
#define AOSTAG_ALLOWANCEVALUE         	"allowance_value"
#define AOSQUERYTYPE_ALLOWANCE         	"qry_allowance"
#define AOSQUERYTYPE_FRIEND         	"qry_friend"
#define AOSTAG_ONLINETIME         		"onlntime"
#define AOSTAG_ONLINESTATUS         	"onlnstatus"
#define AOSTAG_COND						"cond" 

// lynch modify  2011/08/08
#define AOSTAG_COUNTERCLT				"CounterClt" 
#define AOSTAG_COUNTERSVR				"CounterSvr" 
#define AOSTAG_NUM_VSERVERS				"virtual_num" 
#define AOSTAG_VIRTUALCFG				"virtual_cfg" 
#define AOSTAG_COUNTERDIR				"counter_dir" 
#define AOSTAG_COUNTERFN				"counter_filename"
#define AOSTAG_COUNTERDOCFN				"counter_docfilename"
#define AOSTAG_COUNTERID				"counter_id" 
#define AOSTAG_COUNTERTYPE				"counter_type" 
#define AOSTAG_COUNTERSTATTYPE			"counter_stattype" 
#define AOSTAG_COUNTERNAME				"counter_name"
#define AOSTAG_COUNTERNAMES				"counter_names"
#define AOSTAG_COUNTERVALUE				"counter_value"
#define AOSTAG_COUNTEROLDVALUE			"counter_oldvalue"
#define AOSTAG_COUNTERNEWVALUE			"counter_newvalue"
#define AOSTAG_COUNTERTIME				"counter_time"
#define AOSTAG_COUNTERTIMEFORMAT        "counter_timeformat"
#define AOSTAG_COUNTEROPERATION         "counter_operation"
#define AOSTAG_COUNTERSTARTTIME			"counter_starttime"
#define AOSTAG_COUNTERENDTIME			"counter_endtime"
#define AOSTAG_COUNTERUSERID			"counter_userid"
#define AOSTAG_COUNTERDOCID				"counter_docid"
#define AOSTAG_COUNTERCTNRDOCID			"counter_ctnrdocid"
#define AOSTAG_COUNTERSECTIONS			"counter_sections"
#define AOSTAG_VALUES_FLAG              "zky_values_flag"
#define AOSTAG_NAMES_FLAG               "zky_names_flag"
#define AOSTAG_OPERATION				"operation"
#define AOSTAG_QUERYTYPE				"queryType"
#define AOSTAG_ORDERBY					"orderBy"
#define AOSTAG_LAST_SENT				"counter_lastsend"
#define AOSTAG_UPDATE_COUNTER			"update_counter"
#define AOSTAG_COUNTERSEP3				"ctnrdocid"
#define AOSTAG_COUNTER_RSLTPREFIX		"zkytm"
#define AOSTAG_VALUE					"zky_value"
#define AOSTAG_TIME						"zky_time"
#define AOSTAG_CNAME					"zky_cname"
#define AOSTAG_ALLCNAME					"zky_allcname"
#define AOSTAG_QUERY_STAT				"stat"
#define AOSTAG_SYSACCT_SIGNATURE		"zky_sysacct_sig"
// lynch end 08/08/2011

#define AOSTAG_NORMALIZE                "zky_normalize"  //felicia
// brian 15/06/2011
#define AOSTAG_AGING_TYPE				"zky_aging_type"
#define AOSTAG_SDOC_OBJID				"zky_sdoc_objid"
#define AOSTAG_SDOC_OBJIDS				"zky_sdoc_objids"
#define AOSTAG_SHM_RESPCODE 			"shm_resp_code"
#define AOSTAG_SHM_RECEIVERS			"shm_receivers"
#define AOSTAG_SHM_TIMEOUT				"shm_timeout"
#define AOSTAG_SHM_CFMCODE				"shm_cfm_code"
#define AOSTAG_SHM_SOBJID				"shm_sobjid"
#define AOSTAG_CELLNUMBER 				"zky_cell_phone"
#define AOSREQOPRSHORTMSGCONFIRM  		"shortmsgconfirm" 
#define AOSARGVALUE_MSG_OK              "confirm_ok"
#define AOSARGVALUE_MSG_DENIED          "confirm_denied"
#define AOSTAG_ATTR_NAME                "attr_name"
#define AOSTAG_ATTR_VALUE               "attr_value"
#define AOSTAG_GSMTAG                   "Gsm"
#define AOSTAG_CONFIRMCODE              "confirmcode"
#define AOSTAG_TESTER                   "tester"
#define AOSATTR_TORTUER                 "tortuer"
#define AOSTAG_RECEIVER                 "rmailaddr"
#define AOSTAG_SERVER                   "server"
#define AOSTAG_MAILADDR                 "rmailaddr"
#define AOS_EMAIL_CONTENT               "rcv_email_content"
#define AOSTAG_POP3_SERVER              "pop3svr"
#define AOSTAG_POP3_EMAILADDR           "email_addr"
#define AOSTAG_POP3_PASSWD              "passwd"
#define AOSTAG_DELETE_INDEX             "idx"
#define AOSTAG_EMAIL_INDEX              "zky_revemail_idx"
#define AOSTAG_DELEMAIL_OPR             "zky_delemail_opr"
#define AOSTAG_DELEMAIL_UIDL            "zky_delemail_uidl"
#define AOSTAG_SMTP_TO                  "to"
#define AOSTAG_SMTP_FROM                "from"
#define AOSTAG_SMTP_SENDERPSWD          "senderpasswd"
#define AOSTAG_SMTP_SUBJECT             "subject"
#define AOSTAG_SMTP_COUNT               "count"
#define AOSTAG_SMTP_SERVER              "smtpsvr"
#define AOSTAG_MODULE_PATH              "module_path"
#define AOSTAG_MODULE_FILE              "module_file"
#define AOSTAG_SERVERS                  "servers"
#define AOSTAG_POP3_USER                "user"
#define AOSTAG_POP3_EMAILDIR            "email_dir"
#define AOSTAG_START_INDEX              "index"
#define AOSTAG_PAGESIZE                 "pagesize"
#define AOSTAG_REVERSE                  "reverse"
#define AOSTAG_EMAIL_ID                 "zky_email_id"
#define AOSTAG_EMAIL_SOBJID             "sobjid"
#define AOSTAG_EMAIL_OPERATOR           "email_opr"
#define AOSTAG_EMAIL_UNAME              "zky_email_uname"
#define AOSTAG_EMAIL_PASSWD             "zky_email_passwd"
#define AOSTAG_TRIES                    "tries"
#define AOSTAG_SOBJID                   "zky_sobjid"
#define AOSTAG_USER_EMLDIR              "zky_email_attach_dir"
#define AOSTAG_EMAIL_DIR             	"email_dir"
#define AOSTAG_EMAIL_USER             	"zky_email_user"
#define AOSTAG_EMAIL_SERVER				"zky_email_server"
#define AOSTAG_ROOTCOMMENT_OBJID		"zky_rtcmd_objid"
#define AOSTAG_EMAIL_CTNR				"zky_email_ctnr"
#define AOSTAG_LOG_DEFINITION			"zky_log_def"
#define AOSTAG_VPD_TYPE					"zky_vpdtype"
#define AOSTAG_LOG_INDEX				"zky_log_index"
#define AOSTAG_LOG_MANAGE				"zky_log_manage"

//create for log server 08/16/2011 Brian Zhang
#define AOSTAG_LOG_IIL					"zky_log_iil"
// Create for addLogEntry 26/08/2011 Brian Zhang
#define AOSTAG_LOG_CTNROBJID			"zky_log_ctnr_objid"
#define AOSTAG_LOGNAME					"zky_logname"

// Brian Zhang, 06/28/2012, DataBlob
#define AOSTAG_NUM_THREADS				"zky_num_thread"
#define AOSTAG_MOREACTIONS_MODE			"zky_more_actions_mode"
#define AOSTAG_RUNFINISH_ACTIONS_INTHRDS "zky_run_finish_acts_threads"



// Ketty 2011/08/15 for xml mark
#define	AOSTAG_NODFT_INDEX				"zky_nodft_index"
#define AOSTAG_GLOBALIDX_ATTRS			"zky_globalidx_attrs"
#define AOSTAG_CTNRIDX_ATTRS			"zky_ctnridx_attrs"
#define AOSTAG_IDX_ATTRS_WORD			"zky_idx_attrs_word"
#define AOSTAG_BOTHIDX_ATTRS			"zky_bothidx_attrs"
#define AOSTAG_NOINDEX_ATTRS			"zky_noindex_attrs"
#define AOSTAG_TAG_SCOPE				"zky_tag_scope"

// for ShortMsgSvr 23/08/2011 Brian Zhang
#define AOSTAG_SMSP						"smsp"
#define AOSTAG_MOBLE_NUMBER				"mbln"
#define AOSTAG_RUNNER					"runner"
#define AOSTAG_AREA_CODE				"arcd"

// for smartdoc checkunique  11/18/2011   Brian Zhang
#define AOSTAG_ISUNIQUE					"zky_isunique_resp"
#define AOSTAG_NOTUNIQUE				"zky_notunique_resp"
#define AOSTAG_ATTR						"attr"
#define AOSTAG_ERRMSG					"zky_errmsg"
#define AOSTAG_SUCMSG					"zky_sucmsg"

// john 12/2010/17 for forum
#define AOSTAG_STYPE                    "zky_stype" 
#define AOSTAG_PUBLISHER                "zky_publisher"
#define AOSTAG_FORUM                    "zky_forum" 
#define AOSTAG_TOPIC                    "zky_topic"
#define AOSTAG_THREAD                   "zky_thread"
#define AOSTAG_CTNR_FORUMS              "zky_ctnr_forums"
#define AOSTAG_ACCESS                   "zky_access"

#define AOSQUERYTYPE_ALLOWANCE         	"qry_allowance"
#define AOSQUERYTYPE_FRIEND         	"qry_friend"
#define AOSQUERYTYPE_WHOREADME          "qry_whoreadme"

#define AOSFLAG_WHOVISITED				"62036xdk64"
#define AOSPASSWD_INIT_IDGENS			"32kladta53"
#define AOSREQID_READ_MSG				"read_msg"
#define AOSREQID_VERIFY_USR				"verify_usr"
#define AOSREQID_REGIST_SVR				"registSvr"
#define AOSREQID_USER_VERIFY			"verifyusr"
#define AOSREQID_NOTIFY_USER			"notifyuser"

#define AOSQUERYTYPE_COMMENT            "qry_cmt"

#define AOSQUERYTYPE_USERCUSTOM			"qry_usercustom"
#define AOSQUERYTYPE_SUM				"sum"

#define AOSDATASYNCNAME_CREATE_DOC      "ds_creat_doc"
#define AOSTAG_SCHEDULE_RSC_OBJID       "schedule_rsc_objid"

#define AOSANAME_PREF_USER_CTNR			"zkyctnr"

#define AOSTAG_MAX_VERSIONS				"max_versions"
#define AOSTAG_MAX_LOG_LENGTH			"max_loglen"

#define AOSTAG_VPD_OBJID				"zky_vpdobjid"
#define AOSTAG_ATTRVALUE                "attrvalue"
#define AOSTAG_VALUE_SELECTOR			"zky_valselector"
#define AOSTAG_ZKY_TYPE					"zky_type"
#define AOSTAG_CLIENT_FINISH_LATER      "trans_clt_finish_later"
#define AOSTAG_MAXID					"max_id"
#define AOSTAG_NEXTID					"next_id"
#define AOSTAG_DOC_TRANSID              "zky_doc_transid"
#define AOSTAG_DOC_REQTYPE              "zky_doc_reqtype"
#define AOSTAG_CTNR_INSTANT_MSG			"zky_im_ctnr"
#define AOSTAG_STATE_LOG_TPLT		    "state_logtplt"
#define AOSTAG_LOG_CONTAINER			"log_ctnr"
#define AOSTAG_LOG_NAME					"logname"
#define AOSTAG_EVENT					"event"
#define AOSTAG_FROM_STATE				"from_state"
#define AOSTAG_TO_STATE					"to_state"
#define AOSTAG_STATEMACHINE_ID			"statemachine_id"
#define AOSTAG_EXIT_ACTIONS				"exit_actions"
#define AOSTAG_ENTER_ACTIONS			"enter_actions"
#define AOSTAG_TRANSIT_ACTIONS			"transit_actions"
#define AOSTAG_MANAGED_DOC				"managed_doc"
#define AOSTAG_EVENTNAME				"eventname"
#define AOSTAG_LOGENTRY_TYPE			"entry_type"
#define AOSTAG_STATE					"state"
#define AOSTAG_STATEMACHINE				"statemachine"
#define AOSTAG_CURRENT_STATE			"crt_state"
#define AOSTAG_WORKING_DOC				"working_doc"
#define AOSTAG_MEMBERS					"members"
#define AOSTAG_NUM_MEMBERS				"num_members"
#define AOSTAG_CTNR_IDGENS				"zky_idgen_ctnr"
#define AOSTAG_MEMBER_ANAME				"member_aname"
#define AOSTAG_SEQNO_POSTFIX            "zky_seqno_postfix"
#define AOSTAG_SEQNO_LENGTH             "zky_seqno_length"
#define AOSTAG_SEQNO_EXCEPTIONS         "zky_seqno_exceptions"
#define AOSTAG_SEQNO_REUSABLE           "zky_seqno_reusable"
#define AOSTAG_SEQNO_VALUE              "zky_seqno_value"
#define AOSTAG_SEQNO_PADDING            "zky_seqno_padding"
#define AOSTAG_DATAGEN_TYPE1            "zky_datagen_type"
#define AOSTAG_SEQNO_TYPE               "zky_datagenseqno_type"
#define AOSTAG_TIMESEL_TYPE             "zky_timesel_type"
#define AOSTAG_LOCK_REQUESTER           "zky_lock_requester"
#define AOSTAG_LOCK_TYPE                "zky_lock_type"
#define AOSTAG_LOCK_TIMER               "zky_lock_timer"
#define AOSTAG_LOCK_DID                 "zky_lock_did"
#define AOSTAG_SET_CONTENTS             "zky_set_contents"

#define AOSTAG_CTNR_DEFAULT_APPS        "zky_ctnr_defalut_apps"  //felicia
#define AOSTAG_CTNR_DEFAULT_INSTANCES   "zky_ctnr_defalut_instance"  //felicia
#define AOSTAG_STATE_TRANSIT_TPLT       "state_transtlp"
#define AOSTAG_NEXT_STATE               "next_state"
#define AOSSTATE_START                  "start"
#define AOSSTATE_END                    "end"

#define AOSVALUE_CLOUDID_VAR            ".$cloudid"
#define AOSTAG_DATAGEN_TYPE				"datatype"
#define AOSTAG_DICT_TERMID				"term_id"
#define AOSTAG_VPDNAME_PREFIX			"zkyvpd"

#define AOSTAG_SIZE						"zky_size"
#define AOSTAG_MIN_START				"zky_min_start"
#define AOSTAG_MIN_START_FLAG			"zky_min_start_flag"
#define AOSTAG_MAX_START				"zky_max_start"
#define AOSTAG_MAX_START_FLAG			"zky_max_start_flag"
#define AOSTAG_MIN_SIZE					"zky_min_size"
#define AOSTAG_MIN_SIZE_FLAG			"zky_min_size_flag"
#define AOSTAG_MAX_SIZE					"zky_max_size"
#define AOSTAG_MAX_SIZE_FLAG			"zky_max_size_flag"
#define AOSTAG_LOG_ERRORS				"zky_log_errors"
#define AOSTAG_INVALID_START			"zky_invalid_start"
#define AOSTAG_INVALID_SIZE				"zky_invalid_size"
#define AOSTAG_LOG_OPERATIONS			"zky_log_operations"
#define AOSTAG_OPERATION_CTNR_OBJID		"zky_opr_ctnr_objid"
#define AOSTAG_OPERATION_LOGNAME		"zky_opr_logname"
#define AOSTAG_ERROR_CTNR_OBJID			"zky_err_ctnr_objid"
#define AOSTAG_ERROR_LOGNAME			"zky_err_logname"
#define AOSTAG_NEXT_TRIES				"zky_next_tries"
#define AOSTAG_SLABS					"zky_slabs"
#define AOSTAG_NORMAL_URL				"zky_norm_url"
#define AOSTAG_PARMED_URL				"zky_parmed_url"
#define AOSTAG_FULL_URL					"zky_full_url"
#define AOSTAG_MULTIHOME_URL			"zky_mulhome_url"
#define AOSTAG_HOMES					"zky_homes"
#define AOSTAG_OPR_DEFS					"zky_opr_defs"
#define AOSTAG_OPRCODE					"zky_oprcode"
#define AOSTAG_OPERATIONS				"zky_operations"
#define AOSTAG_DATA_SEP					"zky_data_sep"
#define AOSTAG_RESULT_SEP				"zky_rslt_sep"
#define AOSTAG_SAMPLING_SIZE			"zky_sampling_size"
#define AOSTAG_IS_INTEGRAL				"zky_is_integral"
#define AOSTAG_SEGMENT_SIZE				"zky_seg_size"
#define AOSTAG_MAX_SEGMENTS				"zky_max_segs"
#define AOSTAG_OPRDOC_CTNR_PREF			"zky_oprdoc_ctnroidpre"
#define AOSTAG_GRANT					"zky_grant"
#define AOSTAG_DENY						"zky_deny"
#define AOSTAG_ARGNAME					"zky_argname"
#define AOSTAG_RETRIEVED_DOC			"zky_retrieved_doc"
#define AOSTAG_TEMPLATE_SDOC			"zky_tmpl_sdoc"
#define AOSTAG_PRE_ACTIONS				"zky_pre_actions"
#define AOSTAG_POST_ACTIONS				"zky_post_actions"
#define AOSTAG_ISPUBLIC					"zky_ispublic"
#define AOSTAG_URLDOC_DOCID				"zky_urldoc_docid"
#define AOSTAG_MAP						"zky_map"
#define AOSTAG_WITH_DEFAULT				"zky_with_dft"
#define AOSTAG_TARGET_DOC				"zky_targetdoc"
#define AOSTAG_DOCS_TO_PROCESS			"zky_docs_proc"
#define AOSTAG_VALUE_ANAMES				"zky_value_anames"
#define AOSTAG_SEPARATOR				"zky_sep"
#define AOSTAG_STORED_ANAME				"zky_stored_aname"
#define AOSTAG_RESULT_TYPE				"zky_rslt_type"
#define AOSTAG_VALUE_MAP				"zky_rslt_map"
#define AOSTAG_PERSONAL_VPD				"zky_personal_vpd"
#define AOSTAG_MODE						"zky_mode"
#define AOSTAG_URL_QUERY_STR			"zky_url_qrystr"
#define AOSTAG_FILENAME					"zky_fname"
#define AOSTAG_LINE						"zky_line"
#define AOSTAG_ENTRY_ID					"zky_entryid"
#define AOSTAG_URL_DOCDID				"zkyurldocdid"	//ken 2011/12/30
#define AOSTAG_ENTRY_SEP				"zky_entrysep"
#define AOSTAG_FIELD_SEP				"zky_fieldsep"
#define AOSTAG_STATEMACHINE_SELECTOR	"zky_stmc_sel"
#define AOSTAG_STATES					"zky_states"
#define AOSTAG_START_STATEMACHINES		"zky_statemachines"
#define AOSTAG_STATEMACHINE_LOOKUPS		"zky_stmc_lookups"
#define AOSTAG_REASON_VAL_SELECTOR		"zky_rsn_valsel"
#define AOSTAG_INSTANCEDOC_SEL			"zky_instdoc_sel"
#define AOSTAG_STMC_FINISHED_FLAG		"zky_finishflag"
#define AOSTAG_ALLOWED_OPERATIONS		"zky_allowd_oprs"
#define AOSTAG_INSTANCEDOC_CREATOR		"zky_instdoc_ctr"
#define AOSTAG_INIT_STATE_SEL			"zky_initst_sel"
#define AOSTAG_STATE_ID					"zky_state_id"
#define AOSTAG_STMT_START_ACTIONS		"zky_start_acts"
#define AOSTAG_VALUE_SEL				"zky_valuesel"
#define AOSTAG_NEGATE					"zky_negate"
#define AOSTAG_USERDOMAINS				"zky_userdomains"
#define AOSTAG_USER_GROUPS				"zky_usergroups"
#define AOSTAG_BLOCK_NORMAL_EVENT_PROC	"zky_blk_nmevproc"
#define AOSTAG_INST_CTNR_OBJID			"zky_instctoid"
#define AOSTAG_CREATE_CONTAINER			"zky_createctnr"
#define AOSTAG_CTNR_IS_PUBLIC			"zky_ctnrpublic"
#define AOSTAG_CTNR_PARENT_OBJID		"zky_ctnrprtoid"
#define AOSTAG_SDOC_CREATOR				"zky_sdoccreator"
#define AOSTAG_INST_DOC_PUBLIC			"zky_instdocpub"
#define AOSTAG_STMC_OBJID				"zky_stmcobjid"
#define AOSOTYPE_STMC_INST_DOC			"zky_stmcinstdoc"
#define AOSTAG_SUBJECT					"zky_subject"
#define AOSTAG_STMC_INST_OBJID			"zky_stmcinstoid"
#define AOSTAG_ENTRY_TYPE				"zky_entrytype"
#define AOSTAG_INSTDOC_SELECTOR			"zky_instdocsel"
#define AOSTAG_LOG_CTNR_OBJID			"zky_logctnroid"
#define AOSTAG_SMARTDOC_OBJID			"zky_sdocobjid"
#define AOSTAG_STOP_REASON				"zky_stopreason"
#define AOSTAG_ACTION_SOURCE			"zky_actionsrc"
#define AOSTAG_ACTION_ID				"zky_actionid"
#define AOSTAG_STMC_STOPPED_FLAG		"zky_stmcstpflag"
#define AOSTAG_USER_OPERATIONS			"zky_usroprts"
#define AOSTAG_STMC_LOGNAME				"zky_stmclogname"
#define AOSTAG_CONTENTS					"zky_contents"
#define AOSTAG_STATENAME				"zky_statemanem"
#define AOSTAG_FILEPOS					"zky_filepos"
#define AOSTAG_STMC_STOP_ACTIONS		"zky_stmcstpacts"
#define AOSTAG_EDGE_ACTIONS				"zky_edgeactions"
#define AOSTAG_FROM_STATE_ID			"zky_fromstateid"
#define AOSTAG_TO_STATE_ID				"zky_tostateid"
#define AOSTAG_TRANSIT_ON_ENTER			"zky_transitonent"
#define AOSTAG_CREATION_ACTIONS			"zky_crtactions"
#define AOSTAG_STATE_NAME				"zky_statename"
#define AOSTAG_STATE_DFT_STATUS			"zky_stdftstatus"
#define AOSTAG_STMCTYPE					"zky_stmctype"
#define AOSTAG_SYSTEM_OPERATIONS		"zky_sysoprs"
#define AOSTAG_BEFORE_VALUE				"zky_beforevalue"
#define AOSTAG_AFTER_VALUE				"zky_aftervalue"
#define AOSTAG_DENY_ACCESS				"zky_denyaccess"
#define AOSTAG_CTNR_DFT_PROC			"zky_ctnrdftproc"
#define AOSTAG_VALUE_DFT_PROC			"zky_valdftproc"
#define AOSTAG_VALUE_DFT_VALUE			"zky_valdftvalue"
#define AOSTAG_CTNR_DFT_VALUE			"zky_ctnrftvalue"
#define AOSTAG_GET_REQUESTER_TYPE		"zky_getreqttype"
#define AOSTAG_VALUESEL_TAGNAME			"zky_valseltagname"
#define AOSTAG_USRLSTCOND_USRTAG		"zky_usrltcdusrtag"
#define AOSTAG_USRLSTCOND_VALTAG		"zky_usrltcdtname"
#define AOSTAG_USERLISTING_TYPE			"zky_usrlsttype"
#define AOSTAG_USERSOURCE				"zky_usersource"
#define AOSTAG_VALUES_PERCENT			"zky_valuespct"
#define AOSTAG_VALUE_PERCENT			"zky_valuepct"
#define AOSTAG_MATCH_SOURCE				"zky_matchsrc"
#define AOSTAG_USER_TYPE				"zky_usertype"
#define AOSTAG_MAPTYPE					"zky_maptype"
#define AOSTAG_DEFAULT					"zky_default"
#define AOSTAG_ACCESSES					"zky_accesses"
#define AOSTAG_CONDITION_THEN			"zky_condthen"
#define AOSTAG_SYS_URL					"_zt22sys.yyy"
#define AOSTAG_CTNRDOCID				"zky_ctnrdocid"
#define AOSTAG_VIRS						"zky_virs"
#define AOSTAG_PHYS						"zky_phys"
#define AOSTAG_LOGSVR					"SeLogSvr"
#define AOSTAG_LOGMGR					"SeLogMgr"
#define AOSTAG_NEEDRESP					"zky_needresp"
#define AOSTAG_ISCHECKING				"ischecking"
#define AOSTAG_CRTDOCSEL				"zky_crtdocsel"
#define AOSTAG_NUM_SERVERS				"num_servers"
#define AOSTAG_NUM_VIRTUALS				"zky_numvirtuals"
#define AOSTAG_ISLOCAL					"isLocal"
#define AOSTAG_SERVERID					"server_id"
#define AOSTAG_SOURCE					"zky_source"
#define AOSTAG_PARENT_CONTAINER			"zky_parentctnr"
#define AOSTAG_IDGEN					"zky_idgen"
#define AOSTAG_DLLRETURN				"dll_return"
#define AOSTAG_DISTRIBUTOR				"distributor"
#define AOSTAG_FILE_PREFIX				"zky_fileprefix"
#define AOSTAG_FILE_SEQNO				"zky_fileseqno"
#define AOSTAG_VIRTUAL_DIR				"zky_virturaldir"
#define AOSTAG_MAP_IILID				"zky_mapiilid"
#define AOSTAG_TRANSID_ANAME			"zky_transidaname"
#define AOSTAG_EVENT_CONDS				"zky_eventconds"
#define AOSTAG_RESP_PLUS_EVENTS			"zky_respandevents"
#define AOSTAG_RESP_ONLY				"zky_resp_event"
#define AOSTAG_MARK_RESP_ONLY			"zky_transmodel"
#define AOSTAG_MAX_FILESIZE				"zky_maxfilesize"
#define AOSTAG_MAX_TRANS				"zky_maxtrans"
#define AOSTAG_TRACK					"zky_track"
#define AOSTAG_MARK						"zky_mark"
#define AOSTAG_EVENTS					"zky_events"
#define AOSTAG_NUM_EVENTS				"zky_numevents"
#define AOSTAG_RESP_EVENT				"zky_respevent"
#define AOSTAG_TRANS_MODEL				"zky_transmodel"
#define AOSTAG_FLUSH_FLAG				"zky_flushflag"
#define AOSTAG_USE_EVENT_CONDS			"zky_useevtconds"
#define AOSTAG_MARK_SAVE_RESP			"zky_marksaversp"
#define AOSTAG_CHECK_ACCESS				"zky_checkaccess"
#define AOSTAG_UDPCOMM_SEQNO			"zky_udpcommsqno"
#define AOSTAG_PROC_FILE				"zky_procfile"
#define AOSTAG_PROC_LINE				"zky_procline"
#define AOSTAG_ERROR					"zky_error"
#define AOSTAG_SENDMAIL_FLAG			"zky_send_flag"
#define AOSTAG_FORMATTER				"zky_formatter"
#define AOSTAG_ORIG_FORMATTER			"zky_origformtr"
#define AOSTAG_TARGET_FORMATTER			"zky_targtformtr"
#define AOSTAG_CONTAINER_OBJID			"zky_ctnrobjid"
#define AOSTAG_INDEXING					"zky_indexing"
#define AOSTAG_POLICY					"zky_policy"
#define AOSTAG_RECORD_LENGTH			"zky_rcdlength"
#define AOSTAG_MAX_RECORDS				"zky_maxrcds"
#define AOSTAG_TASK_NAME				"zky_taskname"
#define AOSTAG_FILE_INFO				"zky_fileinfo"
#define AOSTAG_START_TIME				"zky_starttime"
#define AOSTAG_END_TIME					"zky_endtime"
#define AOSTAG_STATUS_RECORDS			"zky_statusrcds"
#define AOSTAG_TASK_INFO				"zky_taskinfo"
#define AOSTAG_TIME_TAKEN				"zky_timetaken"
#define AOSTAG_START_POS				"zky_startpos"
#define AOSTAG_LENGTH					"zky_length"
#define AOSTAG_TOTAL_LENGTH				"zky_total_length"
#define AOSTAG_BLOCK_IDX				"zky_block_idx"
#define AOSTAG_DURATION					"zky_duration"
#define AOSTAG_ERROR_LOG				"zky_errorlog"
#define AOSTAG_FULL_NAME				"zky_fullname"
#define AOSTAG_BUFF_SIZE				"zky_buffsize"
#define AOSTAG_BUFF_INCSIZE				"zky_buffincsize"
#define AOSTAG_READ_SIZE				"zky_readize"
#define AOSTAG_OFFSET					"zky_offset"
#define AOSTAG_NEED_CONVERT				"zky_needconvert"
#define AOSTAG_CONVERT_IDX				"zky_convert_idx"
#define AOSTAG_IILASMIDX				"zky_iilasm_idx"
#define AOSTAG_IILASMNAME				"zky_iilasm_name"
#define AOSTAG_ELEMIDX					"zky_elem_idx"
#define AOSTAG_KEYFIELDIDX				"zky_key_field_idx"
#define AOSTAG_NAMEFIELDIDX				"zky_name_field_idx"
#define AOSTAG_VALUEFIELDIDX			"zky_value_field_idx"
#define AOSTAG_ISCONST					"zky_isconst"
#define AOSTAG_MINLEN					"zky_minlen"
#define AOSTAG_MINYEAR					"zky_minyear"
#define AOSTAG_MAXYEAR					"zky_maxyear"
#define AOSTAG_STRTYPE					"zky_strtype"
#define AOSTAG_PADDING					"zky_padding"
#define AOSTAG_LEFTALGIN				"zky_leftalgin"
#define AOSTAG_FIXED_POSITION			"zky_fixedpos"
#define AOSTAG_IIL_ASSEMBLERS			"zky_iilassemblers"
#define AOSTAG_IIL_ASSEMBLER			"zky_iilassembler"
#define AOSTAG_GLOBAL_VALUE_UNIQUE		"zky_gbvalueunq"
#define AOSTAG_GLOBAL_DOCID_UNIQUE		"zky_gbdocidunq"
#define AOSTAG_GLOBAL_PERSIS			"zky_gbpersis"
#define AOSTAG_GLOBAL_ORDER				"zky_gborder"
#define AOSTAG_CTNR_VALUE_UNIQUE		"zky_ctnrvalueunq"
#define AOSTAG_CTNR_DOCID_UNIQUE		"zky_ctnrdocidunq"
#define AOSTAG_CTNR_PERSIS				"zky_ctnrpersis"
#define AOSTAG_CTNR_ORDER				"zky_ctnrorder"
#define AOSTAG_INITDOCID				"zky_init_docid"
#define AOSTAG_ADD_FLAG					"zky_addflag"
#define AOSTAG_UPDATE_FLAG				"zky_updateflag"
#define AOSTAG_SET_FLAG					"zky_setflag"

#define AOSTAG_GLOBAL_INDEX				"zky_gb_index"
#define AOSTAG_CTNR_INDEX				"zky_ctnr_index"

#define AOSTAG_BUFF_LENGTH				"zky_bufflength"
#define AOSTAG_SIZE_TO_SEND				"zky_size2send"
#define AOSTAG_MAX_DOCS					"zky_maxdocs"
#define AOSTAG_DOC_ASSEMBLER			"zky_docassembler"
#define AOSTAG_NUM_DOCS					"zky_numdocs"
#define AOSTAG_LEVEL					"zky_level"
#define AOSTAG_LAST_MERGE				"zky_last_merge"
#define AOSTAG_NODE_LEVEL				"zky_node_level"
#define AOSTAG_IIL_LEVEL				"zky_iil_level"
#define AOSTAG_SIZEID					"zky_sizeid"
#define AOSTAG_RECORDSIZE				"zky_rcdsize"
#define AOSTAG_DOCID_BATCHSIZE			"zky_docidbsz"
#define AOSTAG_DOCSIZE					"zky_docsize"
#define AOSTAG_THREAD_SAFE				"zky_threadsafe"
#define AOSTAG_VIRTUAL_ID				"zky_virtualid"
#define AOSTAG_RECORD_SIZE				"zky_rcdsize"
#define AOSTAG_IMPORTDATA				"zky_importdata"
#define AOSTAG_IMPORT_TASK				"zky_import_task"
#define AOSTAG_FIXED_LENGTH				"zky_fixed_length"	
#define AOSTAG_CENTURIES				"zky_centuries"
#define AOSTAG_TIMETYPE					"zky_timetype"
#define AOSTAG_COUNTNAME				"zky_counternm"
#define AOSTAG_NAME_ANAME				"zky_nameaname"
#define AOSTAG_VALUE_ANAME				"zky_valueaname"
#define AOSTAG_COUNTER_NAMES			"zky_counternms"
#define AOSTAG_IS_TIMED_QUERY			"zky_istimedqry"
#define AOSTAG_ORDER_BY_NAME			"zky_orderbyname"
#define AOSTAG_START_VALUE				"zky_startvalue"
#define AOSTAG_END_VALUE				"zky_endvalue"
#define AOSTAG_TIME_GRAN				"zky_timegran"
#define AOSTAG_TIME_FORMAT				"zky_timeformat"
#define AOSTAG_COUNTERKEY				"zky_counterkey"
#define AOSTAG_TIMEGRAN					"zky_timegran"
#define AOSTAG_COUNTER_ID				"zky_counter_id"
#define AOSTAG_TIME_CONDITION			"zky_time_condition"
#define AOSTAG_ACCUMULATE				"zky_accumulate"
#define AOSTAG_MEMBER					"zky_member"
#define AOSTAG_SET_RUNDATA				"zky_setrdata"
#define AOSTAG_RUNDATA_VAR_NAME			"zky_rdatavarname"
#define AOSTAG_USING_XML_AS_RESULT      "zky_usexmlasrslt"
#define AOSTAG_ORDER_BY_VALUE           "zky_order_by_value"
#define AOSTAG_JOB						"job"
#define AOSTAG_JOBID					"zky_jobid"
#define AOSTAG_JOB_NAME					"zky_job_name"
#define AOSTAG_JOB_OBJID				"zky_jobobjid"
#define AOSTAG_JOB_VERSION				"zky_job_version"
#define AOSTAG_TASK						"zky_task"
#define AOSTAG_TASKDEF					"zky_taskdef"
#define AOSTAG_IS_BACKEND_SERVER		"zky_isbackendsvr"
#define AOSTAG_PHYSICAL_SERVER_ID		"zky_physerverid"
#define AOSTAG_PHYSICAL_CLIENT_ID		"zky_phyclientid"
#define AOSTAG_TASKID					"zky_taskid"
#define AOSTAG_TASK_WORKDATA_ID			"zky_taskworddataid"
#define AOSTAG_RAW_FORMAT				"zky_rawformat"
#define AOSTAG_LOG_ENTRY_KEY			"zky_logentrykey"
#define AOSTAG_JOB_DOCID				"zky_jobdocid"
#define AOSTAG_TASK_SEQNO				"zky_taskseqno"
#define AOSTAG_JOBSPLITTER_ID			"zky_jobsplitterid"
#define AOSTAG_JOBSCHEDULER_ID			"zky_jobschedulerid"
#define AOSTAG_PARTITION_SIZE			"zky_partitionsize"
#define AOSTAG_TASK_STYPE				"zky_taskstype"
#define AOSTAG_TASK_TAGS				"zky_tasktags"
#define AOSTAG_TASK_NAME				"zky_taskname"
#define AOSTAG_PROGRESS					"zky_progress"
#define AOSTAG_JOBDOC_SELECTOR			"zky_jobdocseltr"
#define AOSTAG_JOB_STATUS				"zky_jobstatus"
#define AOSTAG_FINISHED					"zky_finished"
#define AOSTAG_JOBSERVERID				"zky_job_serverid"
#define AOSTAG_JOB_STARTED_TIME			"zky_job_started_time"
#define AOSTAG_JOB_CREATED_TIME			"zky_job_created_time"
#define AOSTAG_JOBSTATUS				"zky_job_status"
#define AOSTAG_JOBLOG_TYPE				"zky_joblog_type"
#define AOSTAG_JOB_CTIME				"zky_job_ctime"
#define AOSTAG_JOB_CTMEPO				"zky_job_ctmepo__d"
#define AOSTAG_NEED_LOG_ATTRS			"zky_need_log_attrs"
#define AOSTAG_JOB_PUBLIC				"zky_job_public"
#define AOSTAG_TASK_CTNR				"zky_task_ctnr"
#define AOSTAG_ENV						"env"
#define AOSTAG_TASK_ENV					"task_env"
#define AOSTAG_SINGLERUN				"zky_singlerun"
#define AOSTAG_JOBNAME					"zky_jobname"
#define AOSTAG_JOB_PROGRESS				"zky_job_progress"
#define AOSTAG_JOB_FINISHED_TIME		"zky_job_finished_time"
#define AOSTAG_JOB_TAKE_TIME			"zky_job_take_time"
#define AOSTAG_SCHEDULER				"scheduler"
#define AOSTAG_SPLITTER					"splitter"
#define AOSTAG_SPLITTERS				"splitters"
#define AOSTAG_DATA_COLLECTORS			"data_collectors__n"
#define AOSTAG_SERVICE_NAME				"zky_service_name"

// Chen Ding, 04/29/2012
#define AOSTAG_COUNTER_SEP				"zky_nnnn"
#define AOSTAG_COUNTERSEP1				"zky_nnnn"
#define AOSTAG_COUNTERSEP2				"zky_nnnn"
#define AOSTAG_RETRIEVE_COUNTER			"zky_nnnn"
#define AOSTAG_RETRIEVE_COUNTERSET		"zky_nnnn"
#define AOSTAG_RETRIEVE_COUNTERSBYQRY	"zky_nnnn"

//ken 2012/05/09
#define AOSTAG_NEEDEXIST     	        "zky_needexist"
#define AOSTAG_MAP_VALUES				"zky_mapvalues"

#define AOSTAG_USE_GUEST                "zky_use_guest_flag"
#define AOSTAG_NUM_TASKS				"zky_numtasks"
#define AOSTAG_SEQNO					"zky_seqno"
#define AOSTAG_SERVER_NAME				"zky_servername"
#define AOSTAG_MATCH_TYPE				"zky_matchtype"
#define AOSTAG_VALUE_COND				"zky_valuecond"
#define AOSTAG_VALUE1					"zky_value1"
#define AOSTAG_VALUE2					"zky_value2"
#define AOSTAG_OPR1						"zky_opr1"
#define AOSTAG_OPR2						"zky_opr2"
#define AOSTAG_SELECTORS				"zky_selectors"
#define AOSTAG_ENTRYSELECTOR			"zky_entryselector"
#define AOSTAG_FIELD_IDX				"zky_fieldidx"
#define AOSTAG_CREATE_FLAG				"zky_createflag"
#define AOSTAG_DOCID_COND				"zky_docidcond"
#define AOSTAG_IIL_SCANNERS				"zky_iilscanners"
#define AOSTAG_IIL_SCANNER				"zky_iilscanner"
#define AOSTAG_IGNORE_MATCHERROR		"zky_ignorematcherr"
#define AOSTAG_IGNORE_MATCHERROR_MAXNUM "zky_ignorematcherrmaxnum"
#define AOSTAG_REJECT					"zky_reject"
#define AOSTAG_TASK_FAIL_POLICY			"zky_taskfailplc"
#define AOSTAG_COMPOSOR_SIZE			"zky_composorsize"
#define AOSTAG_COMPOSOR_MAXLEN			"zky_composormaxlen"
#define AOSTAG_END_POS					"zky_endposition"
#define AOSTAG_STR_COMPOSOR				"zky_strcomposor"
#define AOSTAG_ORIG_SEP					"zky_origsep"
#define AOSTAG_TARGET_SEP				"zky_targetsep"
#define AOSTAG_ELEM_INDEX				"zky_elemindex"
#define AOSTAG_ELEM_TYPE				"zky_elemtype"
#define AOSTAG_GROUPBY_OPR				"zky_groupbyopr"
#define AOSTAG_GROUP_STARTPOS			"zky_groupstartpos"
#define AOSTAG_GROUPID_LEN				"zky_groupidlen"
#define AOSTAG_FILTER					"zky_filter"
#define AOSTAG_FILTERS					"zky_filters"
#define AOSTAG_ELEMENTS					"zky_elements"
#define AOSTAG_RUNNING_MODE				"zky_runningmode"

//ken 2012/05/28
#define AOSTAG_EPOCH_DAY_IDX			"zky_epoch_day_idx"
#define AOSTAG_SET_DOCID				"zky_setdocid"
#define AOSTAG_GROUPBY_PROC				"zky_groupbyproc"
#define AOSTAG_COND_RECORD_PICKER		"zky_condrcdpicker"
#define AOSTAG_UNCOND_RECORD_PICKER		"zky_uncondrcdpicker"
#define AOSTAG_IS_BLACKLIST				"zky_isblacklist"
#define AOSTAG_MATCH_FIELD_IDX			"zky_match_field_idx"
#define AOSTAG_SOURCE_FIELD_IDX			"zky_source_field_idx"
#define AOSTAG_COPY_MEMORY				"zky_copymemory"
#define AOSTAG_SOURCESTR				"zky_sourcestr"
#define AOSTAG_FIELD_OPR				"zky_fieldopr"
#define AOSTAG_FROM_DOCID_IDX			"zky_fromdocididx"
#define AOSTAG_STRVALUE_LEN				"strvaluelen"
#define AOSTAG_FROM_FIELD_IDX			"zky_fromfldidx"
#define AOSTAG_TO_FIELD_IDX				"zky_tofldidx"
#define AOSTAG_SET_RECORD_DOCID			"zky_setrcddocid"
#define AOSTAG_IILTYPE					"zky_iiltype"
#define AOSTAG_GROUP					"zky_group"
#define AOSTAG_ISGROUP					"zky_isgroup"
#define AOSTAG_QUERYKEYGROUPINGTYPE		"zky_querykeygroupingtype"
#define AOSTAG_QUERYVALUEGROUPINGTYPE	"zky_queryvaluegroupingtype"
#define AOSTAG_NEEDFILTER				"zky_needfilter"
#define AOSTAG_FILTERTYPE				"zky_filtertype"
#define AOSTAG_MAX_LENGTH				"zky_maxlength"
#define AOSTAG_CONSTANT					"zky_const"
#define AOSTAG_TOKENID					"zky_tokenid"
#define AOSTAG_RESOURCEID_STR			"zky_rscidstr"
#define AOSTAG_RESOURCEID_U64			"zky_rscu64"
#define AOSTAG_RESOURCEID_INT64			"zky_rscint64"
#define AOSTAG_TOKENS					"zky_tokens"
#define AOSTAG_START_TYPE				"zky_starttype"
#define AOSTAG_NUM_SLOTS				"zky_numslots"
#define AOSTAG_MOVE_FILE_FLAG			"zky_movefileflag"
#define AOSTAG_TASK_SDOC_OBJID			"zky_tasksobjid"
#define AOSTAG_LEFT_ALIGN				"zky_leftalign"
#define AOSTAG_COPY_FLAG				"zky_copyflag"
#define AOSTAG_SUBSTR_TYPE				"zky_substrtype"
#define AOSTAG_TASK_CHAIN				"zky_taskchain"
#define AOSTAG_FROM_TASKID				"zky_fromtaskid"
#define AOSTAG_TO_TASKID				"zky_totaskid"
#define AOSTAG_FROM_SDOCOBJID			"zky_fromsdobjid"
#define AOSTAG_TO_SDOCOBJID				"zky_tosdobjid"
#define AOSTAG_NEXT_TASKID				"zky_nextaskid"
#define AOSTAG_NEXT_TASK_SDOC_DOCID		"zky_nexttasksdocid"
#define AOSTAG_NUM_PHYSICALS			"zky_numphysicals"
#define AOSTAG_NETWORKMGR				"zky_networkmgr"
#define AOSTAG_TASKDATA					"zky_taskdata"
#define AOSTAG_PHYSICALID				"zky_physicalid"
#define AOSTAG_EVEN_DISTRIBUTION		"evenly_distribute"
#define AOSTAG_IS_READY					"zky_isready"
#define AOSTAG_SPLIT_TYPE				"splittype"
#define AOSTAG_NUM_RECORDS				"num_records"
#define AOSTAG_PHONENUMLEN				"phone_num_len"
#define AOSTAG_WITHCOUNTRYCODE			"with_country_code"
#define AOSTAG_NUM_SYNCS				"zky_numsyncs"
#define AOSTAG_MERGER_FINISHED			"zky_mergerfinished"
#define AOSTAG_UNIQUE					"zky_unique"
#define AOSTAG_TASK_TYPE				"zky_tasktype"
#define AOSTAG_PREFIX_LENGTH			"zky_prefixlen"
#define AOSTAG_MAX_TASKS				"zky_maxtasks"
#define AOSTAG_ASSEMBLER_ID				"zky_assemblerid"
#define AOSTAG_NUM_RCDS_PER_TASK		"zky_numrcdsptk"
#define AOSTAG_COMPARE_FUNC				"zky_cmparefunc"
#define AOSTAG_STABLE					"zky_stable"
#define AOSTAG_INFO_NAME				"zky_infoname"
#define AOSTAG_REMOTE_ADDR				"zky_remoteaddr"
#define AOSTAG_REMOTE_PORT				"zky_remoteport"
#define AOSTAG_IS_RESPONSE				"zky_isresponse"
#define AOSTAG_MODULE_ID				"zky_moduleid"
#define AOSTAG_COMMAND_ID				"zky_commandid"
#define AOSTAG_WAIT_THREAD_ID			"zky_waitthrdid"
#define AOSTAG_IS_STABLE				"zky_isstable"
#define AOSTAG_KEY_MAX_LENGTH			"zky_keymaxlen"
#define AOSTAG_VALUE_START_POS			"zky_valstartpos"
#define AOSTAG_VALUE_LENGTH				"zky_vlen"
#define AOSTAG_BUFFARRAY				"zky_buffarray"
#define AOSTAG_VALUE_DATA_TYPE			"zky_valdatatype"
#define AOSTAG_RECORD_PICKER			"zky_recordpicker"
#define AOSTAG_VALUE_TOO_LONG_POLICY	"zky_valtoolongpc"
#define AOSTAG_TRIM_FRONT				"zky_trimfront"
#define AOSTAG_LEFT_ALIGNMENT			"zky_leftalign"
#define AOSTAG_BOOL_TYPE				"zky_booltype"
#define AOSTAG_MAX_BUFFSIZE				"zky_maxbuffsize"
#define AOSTAG_CASE_SENSITIVE			"zky_casesensitive"
#define AOSTAG_DATACOLLECTOR_ID			"zky_datacolid"
#define AOSTAG_IS_CONTROLLING_SCANNER	"zky_isctlthrds"
#define AOSTAG_MERGER_FIRST				"merger_first"
#define AOSTAG_RUN_IN_MULTITHREADS		"zky_runinmultithrds"
#define AOSTAG_SET_FIELD_NULL			"zky_setfieldnull"
#define AOSTAG_TOO_LONG_POLICY			"zky_datatoolongplc"
#define AOSTAG_TOO_SHORT_POLICY			"zky_datatooshortplc"
#define AOSTAG_FRONT_PADDING			"zky_frontpadding"
#define AOSTAG_BACK_PADDING				"zky_backpadding"
#define AOSTAG_TOO_SHORT_PADDING		"zky_tooshortpad"
#define AOSTAG_TOO_LONG_PADDING			"zky_toolongpad"
#define AOSTAG_SET_TRAILING_NULL		"zky_settrailnull"
#define AOSTAG_WITH_DOCID				"zky_withdocid"
#define AOSTAG_NULL_POLICY				"zky_nullpolicy"
#define AOSTAG_NULL_PADDING				"zky_nullpadding"
#define AOSTAG_NEED_SORT_VALUE			"zky_needsortvalue"
#define AOSTAG_RFACTS_IN_BGD 			"zky_racts_inbgd"
#define AOSTAG_SORT_FULL	 			"zky_sort_full"
#define AOSTAG_RUN_INSHELL	 			"zky_run_inshell"
#define AOSTAG_FIELD_NAME				"zky_field_name"
#define AOSTAG_RECORD_NAME				"zky_record_name"
#define AOSTAG_OVERRIDE_DOC_FLAG		"zky_overridedocflag"
#define AOSTAG_DATA_SCANNER				"zky_datascanner"
#define AOSTAG_DATA_BLOB				"zky_datablob"
#define AOSTAG_DATA_PROCS				"zky_dataprocs"
#define AOSTAG_DATA_CACHER				"zky_datacacher"
#define AOSTAG_NULL_DELIMITER			"zky_delimiter"
#define AOSTAG_ORIG_TIME_FORMAT			"zky_origtmfmt"
#define AOSTAG_TARGET_TIME_FORMAT		"zky_targettmfmt"
#define AOSTAG_INCTYPE					"zky_inctype"
#define AOSTAG_COULDSETQUERYCONTEXT		"zky_couldsetquerycontext"
#define AOSTAG_OUTPUTRECORD             "zky_output_record"
#define AOSTAG_FILE_READ_SIZE			"zky_file_readsize"
#define AOSTAG_SEGMENTS					"zky_file_segment"
#define AOSTAG_FILESIZE					"zky_filesize"
#define AOSTAG_REPLICATION_POLICY       "replication_policy"
#define AOSTAG_SEGMENTTYPE 				"segment_type"
#define AOSTAG_TASK_TRANSID				"zky_tasktransid"
#define AOSTAG_NEXT_TASK_TRANSID		"zky_nxttasktransid"
#define AOSTAG_TRANS_PROGRESS			"zky_transprogrs"
#define AOSTAG_FIRST_DOCID				"zky_firstdocid"
#define AOSTAG_IILSCANVALUE				"zky_iilscanvalue"


// Brian Zhang 08/22/2012
#define AOSTAG_ROWSIZE					"zky_rowsize"
#define AOSTAG_SEGMENT_ORDER_MODE		"zky_blkodrmode"
#define AOSTAG_IS_PUBLIC 				"zky_is_public"
#define AOSTAG_FILE_SIZE 				"zky_file_size"
#define AOSTAG_SEGMENT_ID 				"zky_seqment_id"
#define AOSTAG_VIRTUALFILE_OBJID		"zky_vfobjid"
#define AOSTAG_RECORD_LEN				"zky_rcdlen"
#define AOSTAG_VIRTUAL_FILE_MEM_CTNR	"zky_vfmctnr" 
#define AOSTAG_NEED_COMPRESS			"zky_need_compress" 
#define AOSTAG_COMPRESS_TYPE			"zky_compress_type" 
#define AOSTAG_VFBNAME					"vfbdata"
//#define AOSTAG_BINARY_DOC_SIGNATURE		"zky_binarydoc_sig"
#define AOSTAG_BINARY_NODENAME			"zky_binary_nodename"
//#define AOSTAG_BINARY_SOURCELEN			"zky_binary_sourcelen"
#define AOSTAG_COMPRESS_ALG				"zky_compress_alg"
#define AOSTAG_PROCALL					"zky_procall"
#define AOSTAG_EPOCHDAY					"zky_epochday"
#define AOSTAG_ADDTOKEY					"zky_addtokey"
#define AOSTAG_NEEDSPLIT				"zky_needsplit"
#define AOSTAG_SPLIT_STR				"splitstr"
#define AOSTAG_COLUMNCOMPRESSION		"zky_colcomp"
#define AOSTAG_KEYFIELDNAME				"zky_key_field_name"
#define AOSTAG_VALUEFIELDNAME			"zky_value_field_name"
#define AOSTAG_EPOCHDAYNAME				"zky_epoch_day_name"
#define AOSTAG_PERSIS					"zky_persis"
#define AOSTAG_CONVERT_NAME				"zky_convert_name"
#define AOSTAG_GET_PREFIX				"zky_get_prefix"
#define AOSTAG_RECURSION				"zky_recursion"
#define AOSTAG_IGNORE_SERIALIZE			"zky_ignore_serialize"
#define AOSTAG_MATCH_REJECT				"zky_match_reject"
#define AOSTAG_NOMATCH_REJECT			"zky_nomatch_reject"
#define AOSTAG_FIELDINFO_AS				"zky_fieldinfo_as"
#define AOSTAG_LOW_PRIORITY				"low_priority"
#define AOSTAG_HIGH_PRIORITY			"high_priority"
#define AOSTAG_DELAYED					"delayed"
#define AOSTAG_IGNORE					"ignore"
#define AOSTAG_TABLENAME				"tablename"
#define AOSTAG_TABLEOBJID				"tableobjid"
#define AOSTAG_INNET_IILASM				"zky_innet_iilasm"
#define AOSTAG_ONNET_IILASM				"zky_onnet_iilasm"
#define AOSTAG_OUTNET_IILASM			"zky_outnet_iilasm"
#define AOSTAG_EXTENSION_FIELDS_OBJID	"zky_extension_fields_objid"
#define AOSTAG_EXTENSION_CONFIG_OBJID	"zky_extension_config_objid"
#define AOSTAG_CONFIG_ID				"zky_config_id"
#define AOSTAG_PROC_DATETIME			"proc_datetime"
#define AOSTAG_PROC_START_DAY			"proc_start_day"
#define AOSTAG_PROC_END_DAY				"proc_end_day"
#define AOSTAG_GET_CARRIER				"zky_get_carrier"
#define AOSTAG_GET_HEAD					"zky_get_head"
#define AOSTAG_AREACODEFIELDNAME		"zky_area_code_field_name"
#define AOSTAG_NOUPDATETASKPROCNUM		"zky_no_update_task_procnum"
#define AOSTAG_QUERYVERSION				"zky_queryver"
#define AOSTAG_VALUEFROMFIELD			"zky_value_from_field"
#define AOSTAG_MAPNAME					"zky_map_name"
#define AOSTAG_BITMAP_ID				"zky_bitmap_id"
#define AOSTAG_MESSAGE_TYPE				"zky_msgtype"
#define AOSTAG_REQUEST_FLAG				"zky_requestflag"
#define AOSTAG_EXECUTOR_ID				"zky_executor_id"
#define AOSTAG_SECTION_ID				"zky_section_id"
#define AOSTAG_NEED_RESPONSE			"zky_need_resp"
#define AOSTAG_BITMAP_IDS				"zky_bitmap_ids"
#define AOSTAG_MAX_ID					"zky_max_id"
#define AOSTAG_PREFIX					"zky_prefix"
#define AOSTAG_APPLICATION_NAME			"zky_applicname"
#define AOSTAG_SESSION_ID				"zky_sessionid"
#define AOSTAG_USER_ID					"zky_userid"
#define AOSTAG_QUERY_ID					"zky_queryid"
#define AOSTAG_DOC_SECTION				"doc_section"
#define AOSTAG_INTERNAL_ERROR			"zky_internal_error"
#define AOSTAG_TIMER					"zky_timer"
#define AOSTAG_LINENO					"zky_lineno"
#define AOSTAG_ERRID					"zky_errid"
#define AOSTAG_LOGICID					"zky_logic_id"
#define AOSTAG_SPLIT_SIZE				"zky_split_size"
#define AOSTAG_SPLIT_NUM				"zky_split_num"
#define AOSTAG_SPLIT_FILE_NUM			"zky_split_file_num"
#define AOSTAG_ISFILES					"zky_isfiles"
#define AOSTAG_DEVID					"zky_devid"
#define AOSTAG_STORAGE_FILEID			"zky_storage_fileid"
#define AOSTAG_TASKDATA_TAGNAME			"task_data__n"
#define AOSTAG_STREAMDATA_TAGNAME		"stream_data_tag"
#define AOSTAG_DYNAMIC_SDOC				"dynamic_sdoc"
#define AOSTAG_CREATEDLLOBJ_METHOD		"AosCreateDLLObjFunc"
#define AOSTAG_DLL_LIBNAME				"dll_libname"
#define AOSTAG_DLL_METHOD				"dll_method"
#define AOSTAG_DLL_VERSION				"dll_version"
#define AOSTAG_QUERY_CONTAINER_OBJID	"query_ctnr_objid"
#define AOSTAG_QUERY_ATTRNAME			"query_attrname"
#define AOSTAG_QUERY_OPR				"query_opr"
#define AOSTAG_QUERY_VALUE				"query_value"
#define AOSTAG_QUERY_REVERSE			"query_reverse"
#define AOSTAG_QUERY_ORDER				"query_order"
#define AOSTAG_DOCFILEMGRID				"zky_docfilemgr_id"
#define AOSTAG_SNAPSHOTID				"zky_snapshot_id"
#define AOSTAG_MODULEID					"zky_moduleid"
#define AOSTAG_INTERFACE_ID				"zky_interface"
#define AOSTAG_LIBNAME					"zky_libname"
#define AOSTAG_CLASSNAME				"zky_classname"
#define AOSTAG_CREATEJIMO_FUNC_PREFIX	"AosCreateJimoFunc"
#define AOSTAG_INTERFACE				"zky_interface"
#define AOSTAG_JIMO_VERSION				"zky_jimoversion"
#define AOSTAG_CALLED_JIMO_OBJID		"called_jimo_objid"
#define AOSTAG_CALLED_WORKER_OBJID 		"called_worker_objid"
#define AOSTAG_USE_JIMO					"zky_use_jimo"
#define AOSTAG_METHOD					"zky_method"
#define AOSTAG_WORKER_TYPE				"zky_worker_type"
#define AOSTAG_INPUT_DATASETS			"zky_inputdatasets"
#define AOSTAG_JOB_SPLITTER				"zky_jobsplitter"
#define AOSTAG_JIMOS					"zky_jimos"
#define AOSTAG_INSTAANCE_NAMECREATOR	"zky_inst_namecreator"
#define AOSTAG_TASK_VERSION				"zky_task_version"
#define AOSTAG_FILE_FORMAT				"zky_file_format"
#define AOSTAG_FILE_NAME				"zky_file_name"
#define AOSTAG_BINARYDOC				"binarydoc"
#define AOSTAG_TREUDELETE				"true_delete"
#define AOSTAG_NEEDDELETE				"need_delete"
#define AOSTAG_MAXNUMTHREAD				"zky_max_num_thread"
#define AOSTAG_BLOCKIDX					"zky_block_idx"
#define AOSTAG_ISUNICOMFILE				"zky_is_unicomfile"
#define AOSTAG_EXTENSION				"zky_extension"
#define AOSTAG_JIMO_OBJID				"jimo_objid"
#define AOSTAG_CURRENT_VERSION			"current_version"
#define AOSTAG_VERSIONS					"versions"
#define AOSTAG_VERSIONS_PREFIX			"ver"

#define AOSTAG_INPUT					"input"
#define AOSTAG_INPUT_FIELD_NAME			"zky_input_field_name"
#define AOSTAG_OUTPUT					"output"
#define AOSTAG_OUTPUT_FIELD_NAME		"zky_output_field_name"


// Ken Lee, 2013/07/10
#define AOSTAG_CITY_FIELD_IDX			"zky_city_field_idx"
#define AOSTAG_CARRIER_FIELD_IDX		"zky_carrier_field_idx"
#define AOSTAG_INNET_ST_IILASM			"zky_innet_st_iilasm"
#define AOSTAG_ONNET_ST_IILASM			"zky_onnet_st_iilasm"
#define AOSTAG_OUTNET_ST_IILASM			"zky_outnet_st_iilasm"
#define AOSTAG_SOURCE_FNAME				"source_fname"
#define AOSTAG_SOURCE_LENGTH			"source_length"
#define AOSTAG_SOURCE_NAME				"source_name"
#define AOSTAG_SOURCE_PATH				"source_path"
#define AOSTAG_IGNORE_PARA				"ignore_para"

// Ken Lee, 2013/07/15
#define AOSTAG_TEXT_QUALIFIER			"zky_text_qualifier"
#define AOSTAG_CHARACTER				"zky_character"
#define AOSTAG_IGNORE_HEAD				"zky_ignore_head"
#define AOSTAG_IGNORE_EMPTY             "zky_ignore_empty"
#define AOSTAG_ROW_DELIMITER			"zky_row_delimiter"
#define AOSTAG_FIELD_DELIMITER			"zky_field_delimiter"
#define AOSTAG_METADATA_TYPE			"zky_metadata_type"
#define AOSTAG_METADATA_ATTRNAME		"zky_metadata_attrname"

// Ken Lee, 2013/08/05
#define AOSTAG_IS_RANGE_FIELD			"zky_is_range_field"
#define AOSTAG_START_FIELD_NAME			"zky_start_field_name"
#define AOSTAG_END_FIELD_NAME			"zky_end_field_name"

// Andy Zhang, 2013/08/15
#define AOSTAG_FLREVIEWEDJOBOS			"FLReviewedJobOS"
#define AOSTAG_LOOKLEADER				"lookLeader"
#define AOSTAG_OTHERREADERS				"otherReaders"
#define AOSTAG_UN_READ_PERMISSION		"un_read_permission"
#define AOSTAG_YUNYUYAN_ACCOUNT 		"yunyuyan_account"
#define AOSTAG_FROM_TASK_DOCID			"zky_from_task_docid"
#define AOSTAG_TO_TASK_DOCID			"zky_to_task_docid"

#define AOSTAG_JIMO_NAME				"jimo_name"
#define AOSTAG_NUM_ENTRIES				"num_entries"
#define AOSTAG_JIMO_TYPE				"jimo_type"

#define AOSTAG_TASKSERVERID				"zky_task_serverid"
#define AOSTAG_TASK_FAILNUM				"zky_task_failnum"
#define AOSTAG_START_STAMP              "zky_start_stamptime"
#define AOSTAG_REFERENCE_TYPE			"zky_reftype"
#define AOSTAG_REFERENCED_ID			"zky_refedid"
#define AOSTAG_EXTENSION_DOC			"zky_extdoc"
#define AOSTAG_EMBEDDD_ID				"zky_embeddedid"
#define AOSTAG_REFERENCED_OBJID			"zky_ref_objid"
#define AOSTAG_REFERENCED_DOCID			"zky_ref_docid"
#define AOSTAG_RUNDATA_DOC				"zky_rdata_doc"
#define AOSTAG_RUNDATA_DOC_NAME			"zky_rdata_docname"

//felicia, 2013/12/18
#define AOSTAG_XMLEDITOR_NODE_INDEX		"zky_xmleditor_node_index"

#define AOSTAG_NEEDDELETE				"need_delete"
#define AOSTAG_IS_FIXED					"is_fixed"
#define AOSTAG_TIME_FIELDNAME			"time_fieldname"
#define AOSTAG_START_DAY				"start_day"
#define AOSTAG_IILNAMES					"iilnames"
#define AOSTAG_TIME_PLAN				"time_plan"
#define AOSTAG_INDEX_TYPE				"index_type"
#define AOSTAG_PERIODS					"periods"
#define AOSTAG_NUM_DAYS					"num_days"
#define AOSTAG_MAX_PERIODS				"max_periods"
#define AOSTAG_JQL_SCHEDULES_DEF_CTNR	"zky_schedules_def_ctnr"

#define AOSTAG_SNAPSHOT_ID				"zky_snapshot_id"
#define AOSTAG_DOCTYPE					"zky_doctype"
#define AOSTAG_TASK_DOCID				"zky_task_docid"
#define AOSTAG_IS_COMMIT				"zky_is_commit"
#define AOSTAG_IS_DELETE_FILE			"zky_delete_file"
#define AOSTAG_NEEDSWAP					"zky_needswap"
#define AOSTAG_USE_KEY_AS_VALUE			"zky_use_key_as_value"
#define AOSTAG_AGGREGATIONS 			"aggregations"
#define AOSTAG_AGGREGATION 				"aggregation"

//andy,2014/06/20  
#define AOSTAG_DATASOURCE				"data_source"
#define AOSTAG_HBASE					"zky_hbase"
#define AOSTAG_HBASE_DBNAME				"zky_hbase_dbname"
#define AOSTAG_HBASE_ADDR				"zky_hbase_addr"
#define AOSTAG_HBASE_PORT				"zky_hbase_port"
#define AOSTAG_HBASE_TABLE_NAME			"zky_hbase_table_name"
#define AOSTAG_CUBE_ID					"zky_cubeid"
#define AOSTAG_SHUFFLE_ID				"zky_shuffle_id"
#define AOSTAG_SHUFFLE_TYPE 			"zky_shuffle_type"
#define AOSTAG_SHUFFLE_VALUE			"zky_shuffle_value"

//Phil, 2014/08/21
#define AOSTAG_DATA_PKTS				"zky_total_packets"
#define AOSTAG_DATA_BYTES				"zky_total_bytes"

// Chen Ding, 2014/10/26
#define AOSTAG_OBJECT_TYPE				"zky_objtype"
#define AOSTAG_VERBS					"zky_verbs"
#define AOSTAG_OBJECTS					"zky_objects"

#define AOSTAG_NOT_A_NUMBER				"NaN"

// Andy Zhang, 2014/12/26
#define AOSTAG_DATA_ENGINE				"data_engine"
#define AOSTAG_INPUT_DATASETS_TAG		"input_datasets"
#define AOSTAG_OUTPUT_DATASETS			"output_datasets"
#define AOSTAG_TASKS_TAG				"tasks"
#define AOSTAG_TASK_TAG					"task"
#define AOSTAG_DATASET_SPLITTER_TAG		"dataset_splitter"
#define AOSTAG_DATAFIELDS				"datafields"
#define AOSTAG_DATARECORDS				"datarecords"
#define AOSTAG_PICKER					"picker"
#define AOSTAG_SCHEMA_DOCID				"zky_schema_docid"

#define AOSTAG_DATE_TIME				"date_time"
#define AOSTAG_DATE_FIELD_FORMAT		"zky_dataField_format"

#endif

