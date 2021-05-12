#ifndef Entry_H
#define Entry_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Labels {
public: 
    String key;
    String val;
};

class Entry
{
public:
    Entry();
    void setTimeNanos(uint64_t ts);
    void addLabel(String key, String val);
    void setVal(String val);
    String toString();

private:
    int _labelCount = 0;
    Labels _labels[15];
    uint64_t _ts;
    String _val;
    String _out;

    String _uint64ToString(uint64_t input);
};

#endif