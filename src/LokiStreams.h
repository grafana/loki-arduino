#ifndef Streams_H
#define Streams_H

#include "LokiStream.h"
#include "snappy/snappy.h"
#include "proto/pb.h"
#include "proto/pb_encode.h"
#include "proto/logproto.pb.h"


static bool callback_encode_push_request(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg);
static bool callback_encode_stream_adapter(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg);
static bool callback_encode_entry_adapter(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg);

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
    bool toProto(char *output,  size_t length);

private:
    int _streamCount = 0;
    String _uint64ToString(uint64_t input);
};

#endif