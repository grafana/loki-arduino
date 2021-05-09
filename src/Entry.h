#ifndef Entry_H
#define Entry_H

class Entry
{
public:
    Entry();
    void setTime(uint64_t ts);
    void addLabel(String key, String val);
    void setVal(String val);
    String toString();
};

#endif