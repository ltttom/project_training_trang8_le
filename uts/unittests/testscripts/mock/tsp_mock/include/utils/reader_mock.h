#include <gmock/gmock.h>
#include <string>
#include <vector>
#include <istream>

#ifndef JSON_JSON_H_INCLUDED
#define JSON_JSON_H_INCLUDED

// using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

#include <json/config.h>

namespace Json {
typedef std::string String;
class mockValue {
public:
  MOCK_METHOD1(isMember, bool(const char*));
  MOCK_METHOD0(asString, String(void));
};

// typedef unsigned int UInt;
// typedef Json::UInt UInt;

static mockValue* M_Value;

class Value {
public:
    static constexpr UInt defaultRealPrecision = 17;
    // Other members and methods of Value...
    bool isMember(const char* key) const {
      return M_Value->isMember(key);
    };

    Value& operator[](const char* key) const
    {
      Value* value = new Value();
      return *value;
    };
    String asString() const {
      return M_Value->asString();
    };
};



enum PrecisionType {
  significantDigits = 0, ///< we set max number of significant digits in string
  decimalPlaces          ///< we set max number of digits after "." in string
};

class CharReader {
  public:
      class Factory {
      public:
          virtual CharReader* newCharReader() const = 0;
      };
  };

class CharReaderBuilder : public CharReader::Factory {
  public:
    Json::Value settings_;
    CharReaderBuilder(): CharReader::Factory() {};
    ~CharReaderBuilder() {};
    CharReader* newCharReader() const override {}
    bool validate(Json::Value* invalid) const;
    Value& operator[](const String& key);
    static void setDefaults(Json::Value* settings);
    static void strictMode(Json::Value* settings);
};

// using IStream = std::istream;

class mockCharReader {
public:
  MOCK_METHOD4(parseFromStream, bool(CharReader::Factory const&, std::istream&, Value*, std::string*));
};

static mockCharReader* M_CharReader;

bool parseFromStream(CharReader::Factory const& factory, std::istream& stream, Value* root, std::string* errs) {
    return M_CharReader->parseFromStream(factory, stream, root, errs);
}

}
#endif // JSON_JSON_H_INCLUDED