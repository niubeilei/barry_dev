#ifndef Aos_Funcs_Jimos_ToDate_h
#define Aos_Funcs_Jimos_ToDate_h
#include "Funcs/GenFunc.h"
class AosToDate : public AosGenFunc
{
	OmnDefineRCObject;
private:
public:
	AosToDate(const int version);
	AosToDate();
	AosToDate(const AosToDate &rhs);
	~AosToDate();

	virtual AosDataType::E getDataType(
			AosRundata *rdata,
			AosDataRecordObj *record);

	virtual bool getValue(
			AosRundata *rdata, 
			AosValueRslt &value, 
			AosDataRecordObj *record);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc);

	virtual bool syntaxCheck(
			const AosRundataPtr &rdata, 
			OmnString &errmsg);
};
#endif
