#ifndef LokiStreams_H
#define LokiStreams_H

#include <Arduino.h>
#include "LokiStream.h"
#include "snappy/snappy.h"
#include "proto/pb.h"
#include "proto/pb_encode.h"
#include "proto/logproto.pb.h"
#include "Util.h"

class LokiStreams {
public:
    LokiStreams(int numStreams);
    ~LokiStreams();

    bool addStream(LokiStream* stream);
    String toJson();

    uint16_t toSnappyProto(char* output);

    const __FlashStringHelper* errmsg;

private:
    int _streamCount = 0;
    String _uint64ToString(uint64_t input);

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