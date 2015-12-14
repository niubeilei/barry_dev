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
// 07/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/HmUtil.h"

#include "HtmlModules/var_expr.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/String.h"

const int sgMaxTerms = 20;
const int sgMaxConds = 20;

OmnString AosCreateQuery(const AosXmlTagPtr &query)
{
	// 'query' is an XML object in the form:
	//	<conds>
	//		<cond type="AND">
	//			<term type="xxx" ...>
	//				<lhs type="const|var">xxx</lhs>
	//				<rhs type="const|var">xxx</rhs>
	//			</term>
	//			<term type="xxx" .../>
	//		</cond>
	//		...
	//	</conds>
	//
	// This function checks whether the queries contain variables, 
	// and if yes, whether it can resolve them. If yes, it will 
	// resolve them and modify the original query. Otherwise, 
	// it returns an empty string.
	aos_assert_r(query, "");
	AosXmlTagPtr cond = query->getFirstChild("cond");
	if (!cond) return "";

	int num_conds = 0;
	while (cond && num_conds++ < sgMaxConds)
	{
		// Found a condition. Process the condition.
		AosXmlTagPtr term = cond->getFirstChild();
		if (!term)
		{
			OmnAlarm << "Each condition should have at least one term!" 
				<< query->toString() << enderr;
			return "";
		}

		int num_terms = 0;
		while (term && num_terms++ < sgMaxTerms)
		{	
			// Check 'lhs' type
			if (!AosEvalQueryParm(term)) return "";
			term = cond->getNextChild();
		}

		cond = query->getNextChild("cond");
	}

	return query->toString();
}


bool AosEvalQueryParm(const AosXmlTagPtr &querydp)
{
	// This function evaluates the query parameters. 
	// 'querydp' should be in the form:
	//		<term type=xx...>
	//			<lhs type="const|var">
	//              xxxxx or
	//              <parm .../>
	//			</lhs>
	//			<rhs type="const|var">
	//              xxxxx or 
	//				<parm .../>
	//			</rhs>
	//		</term>
	AosXmlTagPtr lhs = querydp->getFirstChild("lhs");
	OmnString value;
	if (lhs && lhs->getAttrStr("type") == "var")
	{
		AosXmlTagPtr parm = lhs->getFirstChild("parm");
		if (parm)
		{
			if (!AosRetrieveVar(parm, value)) return false;
			lhs->setText(value, true);
		}
	}

	AosXmlTagPtr rhs = querydp->getFirstChild("rhs");
	if (rhs && rhs->getAttrStr("type") == "var")
	{
		AosXmlTagPtr parm = rhs->getFirstChild("parm");
		if (parm)
		{
			if (!AosRetrieveVar(parm, value)) return false;
			rhs->setText(value, true);
		}
	}

	return true;
}

