#ifndef LokiStream_H
#define LokiStream_H

#include <Arduino.h>

struct LabelSet {
    String key;
    String val;
};

struct EntrySet {
    uint64_t tsNanos;
    char* val;
};



class LokiStream {
public:
    LokiStream(uint8_t batchSize, uint8_t numberLabels, uint8_t maxEntryLength, const char* labels);
    ~LokiStream();

    void addLabel(String key, String val);
    bool addEntry(uint64_t tsNanos, char* val, size_t length);
    void resetEntries();

    const __FlashStringHelper* errmsg;

private:
    friend class LokiStreams;
    class EntryClass;

    int _batchSize = 0;
    int _numberLabels = 0;
    const char* _labels;

    LokiStream::EntryClass** _batch = nullptr;
    uint8_t _batchPointer = 0;

    LabelSet** labels = nullptr;
    uint8_t labelPointer = 0;

    String _uint64ToString(uint64_t input);
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