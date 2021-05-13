#ifndef LokiStream_H
#define LokiStream_H

#include <Arduino.h>

struct LabelSet
{
    String key;
    String val;
};

struct EntrySet
{
    uint64_t tsNanos;
    char *val;
};

class EntryClass
{
public:
    EntryClass(int maxLength)
    {
        tsNanos = 0;
        val = (char *)malloc(sizeof(char) * (maxLength + 1));
    };

    uint64_t tsNanos;
    char *val;
};

class LokiStream
{
public:
    LokiStream(int batchSize, int numberLabels) : _batchSize(batchSize), _numberLabels(numberLabels)
    {

        batch = new EntryClass *[batchSize];

        //Pre-allocate the memory for each entry
        for (int i = 0; i < batchSize; i++)
        {
            EntryClass *b = new EntryClass(10);
            batch[i] = b;
        }

        // batch = new EntrySet *[batchSize];
        labels = new LabelSet *[numberLabels];
    };
    ~LokiStream()
    {
        delete[] batch;
        delete[] labels;
    }
    EntryClass **batch = nullptr;
    uint8_t batchPointer = 0;

    LabelSet **labels = nullptr;
    uint8_t labelPointer = 0;

    void addLabel(String key, String val);
    void addEntry(uint64_t tsNanos, char *val);
    void resetEntries();

private:
    int _batchSize = 0;
    int _numberLabels = 0;
    String _uint64ToString(uint64_t input);
};

#endif