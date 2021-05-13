#include "LokiStream.h"
#include "Util.h"

void LokiStream::addLabel(String key, String val){

};

void LokiStream::addEntry(uint64_t tsNanos, char *val){
    batch[batchPointer]->tsNanos = tsNanos;
    strcpy(batch[batchPointer]->val, val);
    LOKI_DEBUG_PRINTLN(val);
    // batch[batchPointer] = &es;
    batchPointer++;
};

void LokiStream::resetEntries()
{
    // for (int i = 0; i < _batchSize; i++)
    // {
    //     batch[i] = (EntrySet *)nullptr;
    // }
    // batchPointer = 0;
};

