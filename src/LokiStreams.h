#ifndef LokiStreams_H
#define LokiStreams_H

#include <Arduino.h>
#include "LokiStream.h"
#include <snappy.h>
#include <pb.h>
#include "proto/logproto.pb.h"
#include "LokiDebug.h"

class LokiStreams {
public:
    LokiStreams(uint16_t numStreams, uint32_t bufferSize = 512);
    ~LokiStreams();

    void setDebug(Stream& stream);

    bool addStream(LokiStream& stream);

    int16_t toSnappyProto(uint8_t* output);

    uint32_t getBufferSize();

    char* errmsg;

private:
    uint16_t _streamCount = 0;
    uint32_t _bufferSize = 0;

    Stream* _debug = nullptr;

    LokiStream** _streams = nullptr;
    uint8_t _streamPointer = 0;

    static bool callback_encode_push_request(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg);
    static bool callback_encode_entry_adapter(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg);
    static bool callback_encode_labels(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg);
    static bool callback_encode_line(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg);
};

struct StreamTuple {
    LokiStream** strs;
    uint8_t strCnt;
};

#endif