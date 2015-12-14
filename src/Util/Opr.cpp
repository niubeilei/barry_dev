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
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Opr.h"

#include "Util/String.h"

static OmnString sgOprString_Objid = "01";


OmnString AosOpr_toStr(const AosOpr opr)
{
	switch (opr)
	{
	case eAosOpr_gt: 		return "gt";
	case eAosOpr_ge: 		return "ge";
	case eAosOpr_eq: 		return "eq";
	case eAosOpr_lt: 		return "lt";
	case eAosOpr_le: 		return "le";
	case eAosOpr_ne: 		return "ne";
	case eAosOpr_an: 		return "an";
	case eAosOpr_Objid: 	return sgOprString_Objid;
	case eAosOpr_like: 		return "lk"; 				// ken 2011/5/25
	case eAosOpr_not_like: 	return "nl"; 				// andy 2014/11/18 
	case eAosOpr_null: 		return "nu"; 				// andy 2014/11/18 
	case eAosOpr_not_null: 	return "nn"; 				// andy 2014/11/18 
	case eAosOpr_prefix:	return "pf";				// ken 2012/5/14
	case eAosOpr_date: 		return "te"; 				// Chen Ding, 08/08/2011
	case eAosOpr_epoch: 	return "ep"; 				// Chen Ding, 08/08/2011
	case eAosOpr_range_ge_le: 	return "r1";
	case eAosOpr_range_ge_lt: 	return "r2";
	case eAosOpr_range_gt_le: 	return "r3";
	case eAosOpr_range_gt_lt: 	return "r4";
	case eAosOpr_in: 			return "in";
	case eAosOpr_not_in: 		return "ni";
	case eAosOpr_distinct: 		return "dt";
	default: return "InvalidOpr";
	}
};


OmnString AosOpr_toStr2(const OmnString opr)
{
	if (opr == "gt") return ">";
	if (opr == "ge") return ">=";
	if (opr == "eq") return "=";
	if (opr == "lt") return "<";
	if (opr == "le") return "<=";
	if (opr == "ne") return "!=";
	if (opr == "lk") return "like";
	if (opr == "nl") return "not like";
	if (opr == "nu") return "is null";
	if (opr == "nn") return "is not null";
	if (opr == "in") return "in";
	if (opr == "ni") return "not in";
	if (opr == "dt") return "dt";
	return opr;
}


AosOpr AosOpr_toEnum(const OmnString &str)
{
	const char *data = str.data();
	const int len = str.length();

	if (len == 1)
	{
		switch (data[0])
		{
		case '>': return eAosOpr_gt;
		case '<': return eAosOpr_lt;
		case '=': return eAosOpr_eq;
		default: return eAosOpr_Invalid;
		}
	}

//	if (len != 2) return eAosOpr_Invalid;

	switch (data[0])
	{
	case '0':
		 if (str == sgOprString_Objid) return eAosOpr_Objid;
		 break;

	case '>':
		 if (data[0] == '>' && data[1] == '=') return eAosOpr_ge;
		 break;

	case '<':
		 if (data[0] == '<' && data[1] == '=') return eAosOpr_le;
		 break;

	case '=':
		 if (data[0] == '=' && data[1] == '=') return eAosOpr_eq;
		 if (data[0] == '=') return eAosOpr_eq;
		 break;

	case '!':
		 if (data[0] == '!' && data[1] == '=') return eAosOpr_ne;
		 break;

	case 'a':
		 if (data[0] == 'a' && data[1] == 'n') return eAosOpr_an;
		 break;

	case 'd':
		 if (data[0] == 'd' && data[1] == 't') return eAosOpr_distinct;
		 break;

	case 'e':
		 if (data[0] == 'e' && data[1] == 'q') return eAosOpr_eq;
		 if (data[0] == 'e' && data[1] == 'p') return eAosOpr_epoch;
		 break;

	case 'g':
		 if (data[0] == 'g' && data[1] == 'e') return eAosOpr_ge;
		 if (data[0] == 'g' && data[1] == 't') return eAosOpr_gt;
		 break;

	case 'i':
		 if (data[0] == 'i' && data[1] == 'n') return eAosOpr_in;
		 break;

	case 'l':
		 if (data[0] == 'l' && data[1] == 'e') return eAosOpr_le;
		 if (data[0] == 'l' && data[1] == 't') return eAosOpr_lt;
		 if (data[0] == 'l' && data[1] == 'k') return eAosOpr_like;	// Ken, 05/25/2011
		 if (data[0] == 'l' && data[1] == 'i' && data[2] == 'k' && data[3] == 'e') return eAosOpr_like;	// Ken, 05/25/2011
		 break;

	case 'n':
		 if (data[0] == 'n' && data[1] == 'e') return eAosOpr_ne;
		 if (data[0] == 'n' 
				&& data[1] == 'o'
				&& data[2] == 't'
				&& data[3] == ' '
				&& data[4] == 'l'
				&& data[5] == 'i'
				&& data[6] == 'k'
				&& data[7] == 'e') return eAosOpr_not_like;
		if (data[0] == 'i' 
				&& data[1] == 's'
				&& data[2] == ' '
				&& data[3] == 'n'
				&& data[4] == 'u'
				&& data[5] == 'l'
				&& data[6] == 'l') return eAosOpr_null;
		if (data[0] == 'i' 
				&& data[1] == 's'
				&& data[2] == ' '
				&& data[3] == 'n'
				&& data[4] == 'o'
				&& data[5] == 't'
				&& data[6] == ' '
				&& data[7] == 'n'
				&& data[6] == 'u'
				&& data[9] == 'l'
				&& data[10] == 'l') return eAosOpr_not_null;
		 if (data[0] == 'n' 
				&& data[1] == 'o'
				&& data[2] == 't'
				&& data[3] == ' '
				&& data[4] == 'i'
				&& data[5] == 'n') return eAosOpr_not_in;
		 break;
	
	// ken 2012/5/14
	case 'p':
		 if (data[0] == 'p' && data[1] == 'f') return eAosOpr_prefix;
		 break;

	case 'r':
		 switch (data[1])
		 {
		 // case 'g': return eAosOpr_range;		Chen Ding, CHENDING20130920
		 case '1': return eAosOpr_range_ge_le;
		 case '2': return eAosOpr_range_ge_lt;
		 case '3': return eAosOpr_range_gt_le;
		 case '4': return eAosOpr_range_gt_lt;
		 default: return eAosOpr_Invalid;
		 }
		 break;

	case 't':
		 if (data[0] == 't' && data[1] == 'e') return eAosOpr_date;
		 break;

	default:
		 break;
	}

	return eAosOpr_Invalid;
}


OmnString AosOpr_toStrCode(const OmnString &str)
{
	//	'='			'eq'
	//	'=='		'eq'
	//	'!='		'ne'
	//	'lt'		'lt'
	//	'le'		'le'
	//	'gt'		'gt'
	//	'ge'		'ge'
	//	'an'		'an'
	//	'lk'		'lk'	ken 2011/5/25
	//	'pf'		'pf'	ken 2012/5/14
	
	const char *data = str.data();
	const int len = str.length();

	if (len <= 0) return "";
	if (len == 1)
	{
		switch (data[0])
		{
		case '=': return "eq";
		case '<': return "lt";
		case '>': return "gt";
		default: return str;
		}
	}

	if (len == 2)
	{
		if (data[0] == '>' && data[1] == '=') return "ge";
		if (data[0] == '<' && data[1] == '=') return "le";
		if (data[0] == '=' && data[1] == '=') return "eq";
		if (data[0] == '!' && data[1] == '=') return "ne";
		return str;
	}
	return str; 
}


/*
// Chen Ding, 2014/10/25
AosOpr JmoOpr2AosOpr(const JmoQueryOperator::type t)
{
	switch (t)
	{
	case JmoQueryOperator::GT:			return eAosOpr_gt;
	case JmoQueryOperator::GE:			return eAosOpr_ge;
	case JmoQueryOperator::LT:			return eAosOpr_lt;
	case JmoQueryOperator::LE:			return eAosOpr_le;
	case JmoQueryOperator::EQ:			return eAosOpr_eq;
	case JmoQueryOperator::NE:			return eAosOpr_ne;
	case JmoQueryOperator::PX:			return eAosOpr_prefix;
	case JmoQueryOperator::AN:			return eAosOpr_an;
	case JmoQueryOperator::LIKE:		return eAosOpr_like;
	case JmoQueryOperator::DATE:		return eAosOpr_date;
	case JmoQueryOperator::EPOCH:		return eAosOpr_epoch;
	case JmoQueryOperator::IN:			return eAosOpr_in;
	case JmoQueryOperator::DISTINCT:	return eAosOpr_distinct;
	case JmoQueryOperator::GT_LT:		return eAosOpr_range_gt_lt;
	case JmoQueryOperator::GT_LE:		return eAosOpr_range_gt_le;
	case JmoQueryOperator::GE_LT:		return eAosOpr_range_ge_lt;
	case JmoQueryOperator::GE_LE:		return eAosOpr_range_ge_le;
	default: 							return eAosOpr_Invalid;
	}
}
*/
