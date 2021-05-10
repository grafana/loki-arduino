#ifndef Streams_H
#define Streams_H

#include "LokiStream.h"

class LokiStreams
{
public:
    LokiStreams(int numStreams) : _streamCount(numStreams)
    {
        streams = new LokiStream *[numStreams];
    };
    ~LokiStreams()
    {
        delete[] streams;
    }
    LokiStream **streams = nullptr;
    uint8_t streamPointer = 0;

    void addStream(LokiStream &stream);
    String toJson();

private:
    int _streamCount = 0;
    String _uint64ToString(uint64_t input);
};

#endif