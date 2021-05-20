#include "LokiStreams.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char* __brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

LokiStreams::LokiStreams(int numStreams) : _streamCount(numStreams) {
  _streams = new LokiStream * [numStreams];
};
LokiStreams::~LokiStreams() {
  delete[] _streams;
}

bool LokiStreams::addStream(LokiStream* stream) {
  if (_streamPointer >= _streamCount) {
    errmsg = "cannot add stream, max number of streams have already been added.";
    return false;
  }

  _streams[_streamPointer] = stream;
  _streamPointer++;
};

String LokiStreams::toJson() {
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

uint16_t LokiStreams::estimateProtoBuffSize() {
  /*
  ❯ echo 0A270A0B7B666F6F3D22626172227D12180A0C08F9A0F4840610D0C9F9E402120873747265616D31200A270A0B7B666F6F3D22626172227D12180A0C08F9A0F4840610D0C9F9E402120873747265616D32200A270A0B7B666F6F3D22626172227D12180A0C08F9A0F4840610D0C9F9E402120873747265616D3320 | xxd -r -p | protoc --decode_raw
    1 {
      1: "{foo=\"bar\"}"
      2 {
        1 {
          1: 1620906105
          2: 748578000
        }
        2: "stream1 "
      }
    }
    1 {
      1: "{foo=\"bar\"}"
      2 {
        1 {
          1: 1620906105
          2: 748578000
        }
        2: "stream2 "
      }
    }
    1 {
      1: "{foo=\"bar\"}"
      2 {
        1 {
          1: 1620906105
          2: 748578000
        }
        2: "stream3 "
      }
    }
  */

  // for (int i=0; i<_streamCount; i++) {
  //   uint16_t streamEntryLength = _streams[i]->_batchSize*_streams[i]->_maxEntryLength;
  //   streamEntryLength + 
  // }



}

uint16_t LokiStreams::toSnappyProto(char* output) {
  LOKI_DEBUG_PRINT("Begin To Proto Free Mem:");
  LOKI_DEBUG_PRINTLN(freeMemory());

  uint8_t buffer[512];
  pb_ostream_t os = pb_ostream_from_buffer(buffer, sizeof(buffer));

  StreamTuple tp = StreamTuple{
    strs: _streams,
    strCnt : _streamPointer
  };
  logproto_PushRequest p = {};
  p.streams.arg = &tp;
  p.streams.funcs.encode = &callback_encode_push_request;
  bool status = pb_encode(&os, logproto_PushRequest_fields, &p);

  if (!status) {
    errmsg = PB_GET_ERROR(&os);
    return false;
  }
  LOKI_DEBUG_PRINT("After Proto Free Mem:");
  LOKI_DEBUG_PRINTLN(freeMemory());
  LOKI_DEBUG_PRINT("Message Length: ");
  LOKI_DEBUG_PRINTLN(os.bytes_written);

  // LOKI_DEBUG_PRINT("Message: ");

  // for (uint8_t i = 0; i < os.bytes_written; i++)
  // {
  //   if (buffer[i] < 0x10)
  //   {
  //     LOKI_DEBUG_PRINT(0);
  //   }
  //   LOKI_DEBUG_PRINT(buffer[i], HEX);
  //   // LOKI_DEBUG_PRINT(F(":"));
  // }
  // LOKI_DEBUG_PRINTLN();

  snappy_env env;
  snappy_init_env(&env);
  LOKI_DEBUG_PRINT("After Init_env Free Mem:");
  LOKI_DEBUG_PRINTLN(freeMemory());
  size_t len = snappy_max_compressed_length(os.bytes_written);
  snappy_compress(&env, (char*)buffer, os.bytes_written, output, &len);
  snappy_free_env(&env);

  LOKI_DEBUG_PRINT("After Compression Free Mem:");
  LOKI_DEBUG_PRINTLN(freeMemory());

  LOKI_DEBUG_PRINT("Compressed Length: ");
  LOKI_DEBUG_PRINTLN(len);
  return len;
}

bool LokiStreams::callback_encode_push_request(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg)
{
  StreamTuple* tp = (StreamTuple*)*arg;
  for (int i = 0; i < tp->strCnt; i++)
  {
    if (!pb_encode_tag_for_field(ostream, field)) {
      return false;
    }
    LokiStream* stream = tp->strs[i];
    logproto_StreamAdapter sa = {};
    sa.labels.arg = stream;
    sa.labels.funcs.encode = &callback_encode_labels;
    sa.entries.arg = stream;
    sa.entries.funcs.encode = &callback_encode_entry_adapter;
    if (!pb_encode_submessage(ostream, logproto_StreamAdapter_fields, &sa)) {
      return false;
    }
  }
  return true;
};

bool LokiStreams::callback_encode_entry_adapter(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg)
{
  LokiStream* stream = (LokiStream*)*arg;
  if (!stream->_batchPointer) {
    return true;
  }

  for (int i = 0; i < stream->_batchPointer; i++)
  {
    if (!pb_encode_tag_for_field(ostream, field)) {
      return false;
    }
    logproto_EntryAdapter ea = {};
    ea.has_timestamp = true;
    ea.timestamp.seconds = stream->_batch[i]->tsNanos / 1000000000;
    ea.timestamp.nanos = stream->_batch[i]->tsNanos - (ea.timestamp.seconds * 1000000000);
    ea.line.arg = stream->_batch[i];
    ea.line.funcs.encode = &callback_encode_line;
    if (!pb_encode_submessage(ostream, logproto_EntryAdapter_fields, &ea)) {
      return false;
    }
  }
  return true;
}

bool LokiStreams::callback_encode_labels(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg)
{
  LokiStream* stream = (LokiStream*)*arg;
  if (!pb_encode_tag_for_field(ostream, field)) {
    return false;
  }
  if (!pb_encode_string(ostream, ((const uint8_t*)stream->_labels), strlen(stream->_labels))) {
    return false;
  }
  return true;
}

bool LokiStreams::callback_encode_line(pb_ostream_t* ostream, const pb_field_t* field, void* const* arg)
{
  LokiStream::EntryClass* s = (LokiStream::EntryClass*)*arg;
  if (!pb_encode_tag_for_field(ostream, field)) {
    return false;
  }
  if (!pb_encode_string(ostream, ((const uint8_t*)s->val), strlen(s->val))) {
    return false;
  }
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