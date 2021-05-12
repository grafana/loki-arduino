#include "LokiStreams.h"

String LokiStreams::toJson()
{
  //FIXME this probably needs to be bigger or dynamic
  StaticJsonDocument<384> doc;
  JsonObject jstreams = doc["streams"].createNestedObject();
  for (int i = 0; i < _streamCount; i++)
  {
    JsonObject jstream = jstreams.createNestedObject("stream");
    LokiStream *stream = streams[i];
    for (int j = 0; j < stream->labelPointer; j++)
    {
      jstream[stream->labels[j]->key] = stream->labels[j]->val;
    };
    JsonArray vals = jstreams["values"].createNestedArray();
    for (int j = 0; j < stream->batchPointer; j++)
    {
      vals.add(_uint64ToString(stream->batch[j]->tsNanos));
      vals.add(stream->batch[j]->val);
    }
    String out;
    serializeJson(doc, out);
    return out;
  };
};

bool LokiStreams::toProto(char *output, size_t length)
{
  pb_ostream_t os = pb_ostream_from_buffer(NULL, NULL);
  logproto_PushRequest p = {};
  p.streams.arg = streams;
  p.streams.funcs.encode = &callback_encode_push_request;
  pb_encode_delimited(&os, logproto_PushRequest_fields, &p);

  // logproto_StreamAdapter stream = logproto_StreamAdapter_init_zero;
  // stream.labels = "{foo=\"bar\"}";

  // logproto_PushRequest p = logproto_PushRequest_init_zero;
  // p.streams =

  // snappy_env env;
  // snappy_init_env(&env);
  // size_t len = snappy_max_compressed_length(strlen(_out.c_str()));
  // char *compressed[len];
  // snappy_compress(&env, _out.c_str(), strlen(_out.c_str()), *compressed, &len);
}

static bool callback_encode_push_request(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{

  LokiStream **streams = (LokiStream **)arg;
  //FIXME 10
  for (int i = 0; i < 10; i++)
  {
    LokiStream *stream = streams[i];
    logproto_StreamAdapter sa = {};
    sa.labels = {};
    sa.entries.arg = stream;
    sa.entries.funcs.encode = &callback_encode_entry_adapter;
    pb_encode_delimited(ostream, logproto_StreamAdapter_fields, &sa);
  }
  return true;
};

static bool callback_encode_stream_adapter(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
  LokiStream *stream = (LokiStream *)arg;

  for (int i = 0; i < stream->batchPointer; i++)
  {
    logproto_EntryAdapter ea = {};
    ea.timestamp.nanos = stream->batch[i]->tsNanos;
    ea.line = stream->batch[i]->val;
    pb_encode_delimited(ostream, logproto_EntryAdapter_fields, &ea);
  }
  return true;
}

static bool callback_encode_entry_adapter(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
}

String _uint64ToString(uint64_t input)
{
  String result = "";
  uint8_t base = 10;

  do
  {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}
