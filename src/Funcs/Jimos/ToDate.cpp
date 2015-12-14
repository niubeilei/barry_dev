#include "Funcs/Jimos/ToDate.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <boost/xpressive/xpressive_dynamic.hpp>


extern "C"
{
    AosJimoPtr AosCreateJimoFunc_AosToDate_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosToDate(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch(...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}

}


AosToDate::AosToDate(const int version)
:
AosGenFunc("ToDate", version)
{
}


AosToDate::AosToDate()
:
AosGenFunc("ToDate", 1)
{
}

AosToDate::AosToDate(const AosToDate &rhs)
:
AosGenFunc(rhs)
{
}


AosToDate::~AosToDate()
{
}

bool 
AosToDate::config(
		 const AosRundataPtr &rdata,
		 const AosXmlTagPtr &worker_doc,
		 const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosDataType::E 
AosToDate::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDateTime;
}


bool AosToDate::getValue(
		AosRundata *rdata,
		AosValueRslt &value,
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	OmnString data_str = mValue.getStr();
	
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
    
	OmnString data_format = mValue.getStr();

	//2015-11-30  wumeng  JIMODB-1291
	//if data_format == "%Y"      automatic completion:"%Y-%m-%d"
	//if data_format == "%Y-%m"   automatic completion:"%Y-%m-%d"
	if(data_format == "%Y")
	{
		data_format = "%Y-%m-%d";
		data_str << "-01-01";
	}
	else if(data_format == "%Y-%m")
	{
		data_format = "%Y-%m-%d";
		data_str << "-01";
	}
	AosDateTime dt(data_str, data_format);
	value.setDateTime(dt);
	return true;
}


bool
AosToDate::syntaxCheck(
		const AosRundataPtr &rdata,
		OmnString &errmsg)
{
	if (mSize != 2)
	{
		errmsg << "ToDate needs at 2 parameters.";
		return false;
	}
	
	return true;

}


AosJimoPtr 
AosToDate::cloneJimo() const
{
	try
	{
		return OmnNew AosToDate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;

}



