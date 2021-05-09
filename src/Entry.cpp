#include "Entry.h"

Entry::Entry()
{
    _stream = _doc["streams"].createNestedObject();
}

void Entry::setTime(uint64_t ts)
{
    if (_out && _out.length() > 0) {
        _out.clear();
    }
    _ts = ts;
};

void Entry::addLabel(String key, String val)
{
    _stream[key] = val;
};

void Entry::setVal(String val)
{
    if (_out && _out.length() > 0) {
        _out.clear();
    }
    _val = val;
};

String Entry::toString()
{
    JsonArray vals = _stream["values"].createNestedArray();
    //FIXME get time from int
    vals.add("ts");
    vals.add(_val);
    serializeJson(_doc,_out);
    return _out;
};
