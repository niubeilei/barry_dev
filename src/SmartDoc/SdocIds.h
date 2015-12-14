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
// 07/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocIds_h
#define Aos_SmartDoc_SdocIds_h

#include "Util/String.h" 

struct AosSdocId
{
	enum E
	{
		eInvalid, 

		eActionsOnDocs,
		eAddSchedule,
		eAlipay,
		eAllowance,
		eBatchDelete,
		eCheckUnique,
		eComposerTN,
		eCondActions,
		eCopyDocs,
		eCopyDocsFromCtnr,
		eCounter,
		eCountDown,
		eCreateContainer,
		eCreateDocs,
		eCreateOrder,
		eCreateDoc,
		eDataCollect,
		eConvertData,
		eDataSync,
		eDelFloatingVpd,
		eEvent,
		eExport,
		eExportData,
		eGetTotal,
		eImport,
		eIsDeleted,
		eJoin,
		eProcServer,
		eReserveRes,
		eRunAction,
		eSetVote,
		eShell,
		eStatemachine,
		eSum,
		eThumbNail,
		eVerCode,
		eVote,
		eWriteDoc,
		eActionsondocs,
		eSendMail,
		e00002,
		eSchedule,

		eRelation1101,
		eRelation2106,
		eRelation2107,
		eRunLoopAction,

		eSdoc00002,
		eWarehouse,
		eInlineSearch, //ken 2011/12/01
		eModifyCompDoc,	// Chen Ding, 12/17/2011
		eDeleteCompDocMembers,	// Chen Ding, 12/17/2011
		eGetMediaData,			// Jackie, 12/22/2011
		eCountByDay,			// Brian Zhang 2012/02/24
		eFenHong,			// felicia, 2012/03/08

		eTXTImport,
		eStoreQuery,
		eDocument,     //felicia, 2013/03/19
		
		eOutputDataFileFormat,			//Andy zhang 2013/06/27
		eOutputDataXmlFormat,			//Andy zhang 2013/06/27
		eOutputDataRecordFormat,			//Andy zhang 2013/06/27
		eBatchGetIILDocids,			//Jackie 2013/10/09
		eCompareTwoDocs,			//Jackie 2014/01/05
		eImportDocument,			//felicia, 2013/09/09
		
		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &id);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};
#endif

