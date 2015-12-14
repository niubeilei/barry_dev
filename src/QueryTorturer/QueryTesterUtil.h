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
// 2013/09/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_QueryTesterUtil_h
#define Aos_QueryTorturer_QueryTesterUtil_h

#include "Util/Buff.h"
#include "Util/Opr.h"


class AosQueryTesterUtil
{
public:
	struct CondDef 
	{
		bool order;
		bool reverse;
		bool is_str_field;
		int field_idx;
		AosOpr opr;
		u64 value;
		OmnString iilname;
		u64 maxValue;
		u64 minValue;
		u64 value1;
		u64 value2;
		u64 max;
		u64 min;
		OmnString field_name;

		bool serializeTo(const AosBuffPtr &buff)
		{
			buff->setU8(order);
			buff->setU8(reverse);
			buff->setU8(is_str_field);
			buff->setInt(field_idx);
			buff->setU32(opr);
			buff->setU64(value);
			buff->setOmnStr(iilname);
			buff->setU64(value1);
			buff->setU64(value2);
			return true;
		}

		bool serializeFrom(const AosBuffPtr &buff)
		{
			order = buff->getU8(false);
			reverse = buff->getU8(false);
			is_str_field = buff->getU8(true);
			field_idx = buff->getInt(-1);
			opr = (AosOpr)buff->getU32(0);
			value = buff->getU64(0);
			iilname = buff->getOmnStr("");
			value1 = buff->getU64(0);
			value2 = buff->getU64(0);
			return true;
		}

		void setMax(u64 max)
		{
			maxValue = max;
		}

		void setMin(u64 min)
		{
			minValue = min;
		}

		OmnString toString() const
		{
			OmnString ss = "Order:";
			ss << order << ", Reverse:" 
				<< reverse << ", StrField:" 
				<< is_str_field << ", Opr:" 
				<< AosOpr_toStr(opr) << ", FIdx:" 
				<< field_idx << ", iil:" 
				<< iilname << ", min:" 
				<< value1 << ", max:" 
				<< value2 << ", value:" 
				<< value << "\n";
			return ss;
		}

		bool isInRange(u64 &val)
		{
			if (val < min || val > max)
				return false;

			if (opr == eAosOpr_ne)
				return (val != value1);

			if (opr == eAosOpr_eq)
				return (val == value1);

			return true;

/*
			switch (opr)
			{
				case eAosOpr_gt:
					ret = (val > value1);
					break;

				case eAosOpr_ge:
					ret = (val >= value1); 
					break;

				case eAosOpr_eq:
					ret = (val == value1);
					break;

				case eAosOpr_lt:
					ret = (val < value1);
					break;

				case eAosOpr_le:
					ret = (val <= value1);
					break;

				case eAosOpr_ne:
					ret = (val != value1);
					break;

				case eAosOpr_range_ge_le:
					ret = (val <= value2 && val >= value1);
					break;

				case eAosOpr_range_ge_lt:
					ret = (val < value2 && val >= value1);
					break;

				case eAosOpr_range_gt_le:
					ret = (val <= value2 && val > value1);
					break;

				case eAosOpr_range_gt_lt:
					ret = (val < value2 && val > value1);
					break;

				default: 
					ret = false;
					break;
			}
*/
		}

		u64 getTotal()
		{
			int total = max - min + 1;

			if (opr == eAosOpr_ne)
			{
				if (value1 >= min && value1 <= max)
					total--;
			}

			if (opr == eAosOpr_eq)
			{
				if (value1 >= min && value1 <= max)
					total = 1;
				else
					total = 0;
			}

			if (total < 0)
				total = 0;

			return total;

/*
			u64 total = 0;
			switch (opr)
			{
				case eAosOpr_gt:
					total = maxValue - value1;
					break;

				case eAosOpr_ge:
					total = maxValue - value1 + 1;
					break;

				case eAosOpr_eq:
					if (value1 <= maxValue && value1 >= minValue)
						total = 1;
					else
						total = 0;

					break;

				case eAosOpr_lt:
					total = value1 - minValue;
					break;

				case eAosOpr_le:
					total = value1 - minValue + 1;
					break;

				case eAosOpr_ne:
					if (value1 <= maxValue && value1 >= minValue)
						total = maxValue - minValue;
					else
						total = maxValue - minValue + 1;

					break;

				case eAosOpr_range_ge_le:
					total = max - min + 1;
					break;

				case eAosOpr_range_ge_lt:
					total = max - min;
					break;

				case eAosOpr_range_gt_le:
					total = max - min;
					break;

				case eAosOpr_range_gt_lt:
					total = max - min - 1;
					break;

				default: 
					break;
			}

			if (total < 0)
				total = 0;
*/
		}

		OmnString getCondClause()
		{
			OmnString condStr = "";

			OmnScreen << toString() << endl;
			switch (opr)
			{
				case eAosOpr_gt:
					condStr << "(" << field_name  
						<< " > " << value1 << ")";
					break;

				case eAosOpr_ge:
					condStr << "(" << field_name  
						<< " >= " << value1 << ")";
					break;

				case eAosOpr_eq:
					condStr << "(" << field_name  
						<< " = " << value1 << ")";
					break;

				case eAosOpr_lt:
					condStr << "(" << field_name  
						<< " < " << value1 << ")";
					break;

				case eAosOpr_le:
					condStr << "(" << field_name  
						<< " <= " << value1 << ")";
					break;

				case eAosOpr_ne:
					condStr << "(" << field_name  
						<< " != " << value1 << ")";
					break;

				case eAosOpr_an:
					condStr << "(" << field_name  
						<< " =~ " << value1 << ")";
					break;

				case eAosOpr_Objid:
					condStr << "objid(" << field_name << ")";
					break;

				case eAosOpr_like:
					condStr << "(" << field_name  
						<< " like " << value1 << ")";
					break;

				case eAosOpr_prefix:
					condStr << "prefix(" << field_name << ")";
					break;

				case eAosOpr_date:
					condStr << "date(" << field_name << ")";
					break;

				case eAosOpr_epoch:
					condStr << "epoch(" << field_name << ")";
					break;

				case eAosOpr_range_ge_le:
					condStr << "(" 
						<< field_name  << " >= " << value1 << " && "
						<< field_name << " <= " << value2 
						<< ")";
					break;

				case eAosOpr_range_ge_lt:
					condStr << "(" 
						<< field_name  << " >= " << value1 << " && "
						<< field_name << " < " << value2 
						<< ")";
					break;

				case eAosOpr_range_gt_le:
					condStr << "(" 
						<< field_name  << " > " << value1 << " && "
						<< field_name << " <= " << value2 
						<< ")";
					break;

				case eAosOpr_range_gt_lt:
					condStr << "(" 
						<< field_name  << " > " << value1 << " && "
						<< field_name << " < " << value2 
						<< ")";
					break;

				default: 
					break;
			}

			OmnScreen << condStr << endl;
			return condStr;
		}

		OmnString getFieldname()
		{
			return field_name;
		}
	};
};

#endif

