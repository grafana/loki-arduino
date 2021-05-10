#include "LokiStream.h"

void LokiStream::resetEntries()
{
    for (int i = 0; i < _batchSize; i++)
    {
        batch[i] = (EntrySet *)nullptr;
    }
    batchPointer = 0;
};