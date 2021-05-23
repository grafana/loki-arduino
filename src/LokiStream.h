#ifndef LokiStream_H
#define LokiStream_H

#include <Arduino.h>
#include "LokiDebug.h"



class LokiStream {
public:
    LokiStream(uint8_t batchSize, uint8_t maxEntryLength, const char* labels);
    ~LokiStream();

    bool addEntry(uint64_t tsNanos, char* val, size_t length);
    void resetEntries();

    const char* errmsg;

private:
    friend class LokiStreams;
    class EntryClass;

    int _batchSize = 0;
    int _maxEntryLength = 0;
    const char* _labels;

    LokiStream::EntryClass** _batch = nullptr;
    uint8_t _batchPointer = 0;

};

class LokiStream::EntryClass {
public:
    EntryClass(uint8_t maxLength);
    ~EntryClass();
    uint8_t maxLength;
    uint64_t tsNanos;
    char* val;
};

#endif