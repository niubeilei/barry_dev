#ifndef JSON_JSON_H_INCLUDED
#define JSON_JSON_H_INCLUDED

# include "JSON/autolink.h"
# include "JSON/value.h"
# include "JSON/reader.h"
# include "JSON/writer.h"
# include "JSON/features.h"

using namespace Json;

typedef Json::Value						JSONValue;
typedef Json::Reader					JSONReader;
typedef Json::Writer					JSONWriter;
typedef Json::FastWriter				JSONFastWriter;


#endif // JSON_JSON_H_INCLUDED
