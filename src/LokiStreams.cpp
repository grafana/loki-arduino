#include "LokiStreams.h"

// // Empty implementations of these, they are auto generated because we defined the callback_datatype on the proto but instead use the above callback functions ^^
// bool logproto_EntryAdapter_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field){
//   LOKI_DEBUG_PRINTLN("CALLBACK CALLED ENTRY");
//   return true;
// };
// bool logproto_StreamAdapter_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field){
//   LOKI_DEBUG_PRINTLN("CALLBACK CALLED STREAM");
//   return true;
// };

void LokiStreams::addStream(LokiStream *stream)
{
  streams[streamPointer] = stream;
  streamPointer++;
};

String LokiStreams::toJson(){
    //FIXME this probably needs to be bigger or dynamic
    // StaticJsonDocument<384> doc;
    // JsonObject jstreams = doc["streams"].createNestedObject();
    // for (int i = 0; i < _streamCount; i++)
    // {
    //   JsonObject jstream = jstreams.createNestedObject("stream");
    //   LokiStream *stream = streams[i];
    //   for (int j = 0; j < stream->labelPointer; j++)
    //   {
    //     jstream[stream->labels[j]->key] = stream->labels[j]->val;
    //   };
    //   JsonArray vals = jstreams["values"].createNestedArray();
    //   for (int j = 0; j < stream->batchPointer; j++)
    //   {
    //     vals.add(_uint64ToString(stream->batch[j]->tsNanos));
    //     vals.add(stream->batch[j]->val);
    //   }
    //   String out;
    //   serializeJson(doc, out);
    //   return out;
    // };
};

bool LokiStreams::toProto(char *output, size_t length)
{

  for (int i = 0; i < streamPointer; i++)
  {
    for (int j = 0; j < streams[i]->batchPointer; j++)
    {
      LOKI_DEBUG_PRINTLN(streams[i]->batch[j]->val);
    }
  }

  LOKI_DEBUG_PRINTLN("1");
  uint8_t buffer[256];
  pb_ostream_t os = pb_ostream_from_buffer(buffer, sizeof(buffer));
  StreamTuple tp = StreamTuple{
    strs : streams,
    strCnt : streamPointer
  };
  logproto_PushRequest p = {};
  p.streams.arg = &tp;
  p.streams.funcs.encode = &callback_encode_push_request;
  bool status = pb_encode(&os, logproto_PushRequest_fields, &p);

  // logproto_TestMessage t = {};
  // t.test = 34;
  // bool status = pb_encode(&os, logproto_TestMessage_fields, &t);
  LOKI_DEBUG_PRINTLN("4");

  if (!status)
  {
    Serial.println("Failed to encode");
    return false;
  }

  LOKI_DEBUG_PRINT("Message Length: ");
  LOKI_DEBUG_PRINTLN(os.bytes_written);

  LOKI_DEBUG_PRINT("Message: ");

  for (uint8_t i = 0; i < os.bytes_written; i++)
  {
    if (buffer[i] < 0x10)
    {
      LOKI_DEBUG_PRINT(0);
    }
    LOKI_DEBUG_PRINT(buffer[i], HEX);
    // LOKI_DEBUG_PRINT(F(":"));
  }
  LOKI_DEBUG_PRINTLN();

  // for (int i = 0; i < os.bytes_written; i++)
  // {
  //   LOKI_DEBUG_PRINTF("%02X", buffer[i]);
  // }

  // logproto_StreamAdapter stream = logproto_StreamAdapter_init_zero;
  // stream.labels = "{foo=\"bar\"}";

  // logproto_PushRequest p = logproto_PushRequest_init_zero;
  // p.streams =

  // snappy_env env;
  // snappy_init_env(&env);
  // size_t len = snappy_max_compressed_length(strlen(_out.c_str()));
  // char *compressed[len];
  // snappy_compress(&env, _out.c_str(), strlen(_out.c_str()), *compressed, &len);
  return true;
}

static bool callback_encode_push_request(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
  LOKI_DEBUG_PRINTLN("2");
  StreamTuple *tp = (StreamTuple *)*arg;
  for (int i = 0; i < tp->strCnt; i++)
  {
    LOKI_DEBUG_PRINT("2-");
    LOKI_DEBUG_PRINT(field->tag);
    LOKI_DEBUG_PRINT("-");
    LOKI_DEBUG_PRINTLN(i);
    pb_encode_tag_for_field(ostream, field);
    LokiStream *stream = tp->strs[i];
    logproto_StreamAdapter sa = {};
    sa.labels.arg = stream;
    sa.labels.funcs.encode = &callback_encode_labels;

    // sa.labels = "{foo=\"bar\"}";
    sa.entries.arg = stream;
    sa.entries.funcs.encode = &callback_encode_entry_adapter;
    if (!pb_encode_submessage(ostream, logproto_StreamAdapter_fields, &sa))
    {
      LOKI_DEBUG_PRINTF("Encoding failed: %s\n", PB_GET_ERROR(ostream));
    }
  }
  return true;
};

static bool callback_encode_entry_adapter(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
  LOKI_DEBUG_PRINTLN("3");
  LOKI_DEBUG_PRINT("Tag: ");
  LOKI_DEBUG_PRINT(field->tag);
  LOKI_DEBUG_PRINTLN();
  LokiStream *stream = (LokiStream *)*arg;
  if (!stream->batchPointer)
  {
    LOKI_DEBUG_PRINTLN("NO BATCHES")
    return true;
  }

  for (int i = 0; i < stream->batchPointer; i++)
  {
    LOKI_DEBUG_PRINT("3-");
    LOKI_DEBUG_PRINTLN(i);
    pb_encode_tag_for_field(ostream, field);
    logproto_EntryAdapter ea = {};
    ea.has_timestamp = true;
    ea.timestamp.seconds = stream->batch[i]->tsNanos / 1000000000;
    ea.timestamp.nanos = stream->batch[i]->tsNanos - (ea.timestamp.seconds * 1000000000);
    ea.line.arg = stream->batch[i];
    ea.line.funcs.encode = &callback_encode_line;
    pb_encode_submessage(ostream, logproto_EntryAdapter_fields, &ea);
  }
  return true;
}

static bool callback_encode_labels(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
  LokiStream *stream = (LokiStream *)*arg;
  pb_encode_tag_for_field(ostream, field);
  const char *str = "{foo=\"bar\"}";
  pb_encode_string(ostream, ((const uint8_t *)str), strlen(str));
  return true;
}

static bool callback_encode_line(pb_ostream_t *ostream, const pb_field_t *field, void *const *arg)
{
  EntrySet *s = (EntrySet *)*arg;
  pb_encode_tag_for_field(ostream, field);
  LOKI_DEBUG_PRINT("Tag: ");
  LOKI_DEBUG_PRINT(field->tag);
  LOKI_DEBUG_PRINTLN();
  LOKI_DEBUG_PRINTLN(s->val);
  pb_encode_string(ostream, ((const uint8_t *)s->val), strlen(s->val));
  // LOKI_DEBUG_PRINTLN(s->val)
  return true;
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