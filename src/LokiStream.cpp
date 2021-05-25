#include "LokiStream.h"


LokiStream::LokiStream(uint8_t batchSize, uint8_t maxEntryLength, const char* labels) : _batchSize(batchSize), _maxEntryLength(maxEntryLength) {

    _batch = new LokiStream::EntryClass * [batchSize];
    _labels = labels;

    //Pre-allocate the memory for each entry
    for (int i = 0; i < batchSize; i++) {
        LokiStream::EntryClass* b = new LokiStream::EntryClass(maxEntryLength);
        _batch[i] = b;
    }

};

LokiStream::~LokiStream() {
    for (int i = 0; i < _batchSize; i++) {
        delete _batch[i];
    }
    delete[] _batch;
}

bool LokiStream::addEntry(uint64_t tsNanos, const char* val, size_t length) {
    errmsg = nullptr;
    if (_batchPointer >= _batchSize) {
        errmsg = "batch full";
        return false;
    }
    // We malloc'd the value to be maxLength+1 to leave room for a null terminator
    // so we only need to make sure we don't exceed the max length and we still
    // have room for the string terminator
    if (length > _batch[_batchPointer]->maxLength) {
        errmsg = "entry value exceeds max length provided in constructor";
        return false;
    }

    _batch[_batchPointer]->tsNanos = tsNanos;
    strncpy(_batch[_batchPointer]->val, val, length);
    //Add null terminator
    _batch[_batchPointer]->val[length] = '\0';
    _batchPointer++;

    return true;
};

void LokiStream::resetEntries() {
    _batchPointer = 0;
};

LokiStream::EntryClass::EntryClass(uint8_t maxLen) {
    maxLength = maxLen;
    tsNanos = 0;
    val = (char*)malloc(sizeof(char) * (maxLen + 1));
}

LokiStream::EntryClass::~EntryClass() {
    free(val);
}
