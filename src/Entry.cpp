#include "Entry.h"

Entry::Entry()
{
}

void Entry::setTimeNanos(uint64_t ts)
{
  if (_out && _out.length() > 0)
  {
    _out = "";
  }
  _ts = ts;
};

void Entry::addLabel(String key, String val)
{
  //FIXME need to error if adding more than 15 labels
  _labels[_labelCount] = Labels{
    key : key,
    val : val
  };
  _labelCount++;
};

void Entry::setVal(String val)
{
  if (_out && _out.length() > 0)
  {
    _out = "";
  }
  _val = val;
};

String Entry::toString()
{
  StaticJsonDocument<384> doc;
  JsonObject streams = doc["streams"].createNestedObject();
  JsonObject stream = streams.createNestedObject("stream");
  for (int i = 0; i < _labelCount; i++)
  {
    stream[_labels[i].key] = _labels[i].val;
  };
  JsonArray vals = streams["values"].createNestedArray();
  vals.add(_uint64ToString(_ts));
  vals.add(_val);
  serializeJson(doc, _out);
  return _out;
};

String Entry::_uint64ToString(uint64_t input)
{
  String result = "";
  uint8_t base = 10;

  do
  {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}