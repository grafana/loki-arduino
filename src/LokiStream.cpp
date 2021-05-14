#include "LokiStream.h"
#include "Util.h"


LokiStream::LokiStream(uint8_t batchSize, uint8_t numberLabels, uint8_t maxEntryLength) : _batchSize(batchSize), _numberLabels(numberLabels) {

    _batch = new LokiStream::EntryClass * [batchSize];

    //Pre-allocate the memory for each entry
    for (int i = 0; i < batchSize; i++) {
        LokiStream::EntryClass* b = new LokiStream::EntryClass(maxEntryLength);
        _batch[i] = b;
    }

    // batch = new EntrySet *[batchSize];
    labels = new LabelSet * [numberLabels];
};


LokiStream::~LokiStream() {
    for (int i = 0; i < _batchSize; i++) {
        delete _batch[i];
    }
    delete[] _batch;
    // delete[] _labels;
}

void LokiStream::addLabel(String key, String val) {

};

bool LokiStream::addEntry(uint64_t tsNanos, char* val, size_t length) {
    if (_batchPointer >= _batchSize) {
        errmsg = F("batch full");
        return false;
    }
    LOKI_DEBUG_PRINT("length: ")
        LOKI_DEBUG_PRINTLN(length);
    LOKI_DEBUG_PRINT("max length: ")
        LOKI_DEBUG_PRINTLN(_batch[_batchPointer]->maxLength);
    // We malloc'd the value to be maxLength+1 to leave room for a null terminator
    // so we only need to make sure we don't exceed the max length and we still
    // have room for the string terminator
    if (length > _batch[_batchPointer]->maxLength) {
        errmsg = F("entry value exceeds max length provided in constructor");
        return false;
    }

    _batch[_batchPointer]->tsNanos = tsNanos;
    strncpy(_batch[_batchPointer]->val, val, length);
    //Add null terminator
    _batch[_batchPointer]->val[length] = '\0';
    _batchPointer++;
};

void LokiStream::resetEntries() {
    // for (int i = 0; i < _batchSize; i++)
    // {
    //     batch[i] = (EntrySet *)nullptr;
    // }
    // batchPointer = 0;
};

LokiStream::EntryClass::EntryClass(uint8_t maxLen) {
    maxLength = maxLen;
    tsNanos = 0;
    val = (char*)malloc(sizeof(char) * (maxLen + 1));
}

LokiStream::EntryClass::~EntryClass() {
    free(val);
}
