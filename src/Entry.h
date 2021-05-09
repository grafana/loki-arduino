#ifndef Entry_H
#define Entry_H

#include <ArduinoJson.h>

class Entry
{
public:
    Entry();
    void setTime(uint64_t ts);
    void addLabel(String key, String val);
    void setVal(String val);
    String toString();

private:
    StaticJsonDocument<384> _doc;
    JsonObject _stream;
    uint64_t _ts;
    String _val;
    String _out;
};

#endif