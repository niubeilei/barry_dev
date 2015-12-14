#ifndef Aos_HtmlModules_HandleImage_h
#define Aos_HtmlModules_HandleImage_h

#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include <boost/regex.hpp>
#include "HtmlModules/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/SeXmlParser.h"


class AosHandleImage : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum{
		eMaxHtmlCodeLength = 100000
	};
	public:
		AosHandleImage() {}
		~AosHandleImage() {}
		void setEndImage(OmnString& htmlcode);
		bool processImage(OmnString& htmlcode);
		OmnString
			changeImgSrc(int height, int width, const OmnString& src);

};

#endif
