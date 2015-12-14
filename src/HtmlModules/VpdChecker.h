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
// 09/14/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HTML_MODULES_VPDCHECKER_H
#define AOS_HTML_MODULES_VPDCHECKER_H

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#define AOSVPDTYPE_ATOMIC_PANE			"apane"
#define AOSVPDTYPE_COMPOUND_PANE        "cpane"
#define AOSVPDTYPE_NORMAL_PANE          "npane"
#define AOSVPDTYPE_PANEL				"panel"
#define AOSVPDTYPE_GIC					"gic"

class AosVpdChecker
{
public:
	enum VpdType
	{
		eInvalid,

		eAPane,
		eCPane,
		eNPane,
		ePanel,
		eGic,

		eMax
	};

	static bool checkVpd(
				    const AosXmlTagPtr &vpd,
				    const AosRundataPtr &rdata);
	static bool verifyVpd(
					const AosXmlTagPtr &vpd, 
					bool &changed, 
					const AosRundataPtr &rdata);
	static OmnString determineVpdType(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static bool verifyAtomicPane(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static bool verifyCompPane(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static bool verifyNormalPane(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static bool verifyPanel(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static bool verifyGic(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	static VpdType toEnum(const OmnString &type)
	{
		if (type == AOSVPDTYPE_ATOMIC_PANE) return eAPane;
		if (type == AOSVPDTYPE_COMPOUND_PANE) return eCPane;
		if (type == AOSVPDTYPE_NORMAL_PANE) return eNPane;
		if (type == AOSVPDTYPE_PANEL) return ePanel;
		if (type == AOSVPDTYPE_GIC) return eGic;
		return eInvalid;
	}
};
#endif
