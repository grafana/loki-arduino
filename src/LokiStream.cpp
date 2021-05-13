#include "LokiStream.h"

void LokiStream::addLabel(String key, String val){

};

void LokiStream::addEntry(uint64_t tsNanos, char *val){
    EntrySet es = EntrySet{
        tsNanos: tsNanos,
        val: val
    };
    batch[batchPointer] = &es;
    batchPointer++;
};

void LokiStream::resetEntries()
{
    for (int i = 0; i < _batchSize; i++)
    {
        batch[i] = (EntrySet *)nullptr;
    }
    batchPointer = 0;
};

