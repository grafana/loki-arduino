#include "LokiStreams.h"

LokiStreams::LokiStreams(uint16_t numStreams, uint32_t bufferSize) : _streamCount(numStreams), _bufferSize(bufferSize) {
  _streams = new LokiStream * [numStreams];
};
LokiStreams::~LokiStreams() {
  delete[] _streams;
}

void LokiStreams::setDebug(Stream& stream) {
  _debug = &stream;
}

bool LokiStreams::addStream(LokiStream& stream) {
  errmsg = nullptr;
  if (_streamPointer >= _streamCount) {
    errmsg = "cannot add stream, max number of streams have already been added.";
    return false;
  }

  _streams[_streamPointer] = &stream;
  _streamPointer++;
  return true;
};

uint32_t LokiStreams::getBufferSize() {
  return _bufferSize;
}

int16_t LokiStreams::toSnappyProto(uint8_t* output) {
  errmsg = nullptr;
  DEBUG_PRINT("Begin serialization: ");
  PRINT_HEAP();

  uint8_t buffer[_bufferSize];
  pb_ostream_t os = pb_ostream_from_buffer(buffer, sizeof(buffer));

  StreamTuple tp = StreamTuple{
    strs: _streams,
    strCnt : _streamPointer
  };
  logproto_PushRequest p = {};
  p.streams.arg = &tp;
  p.streams.funcs.encode = &callback_encode_push_request;
  if (!pb_encode(&os, logproto_PushRequest_fields, &p)) {
    DEBUG_PRINT("Error from proto encode: ");
    DEBUG_PRINTLN(PB_GET_ERROR(&os));
    errmsg = "Error creating protobuf, enable debug logging to see more details";
    return -1;
  }

  DEBUG_PRINT("Bytes used for serialization: ");
  DEBUG_PRINTLN(os.bytes_written);

  DEBUG_PRINT("After serialization: ");
  PRINT_HEAP();

  // for (uint16_t i = 0; i < os.bytes_written; i++)
  // {
  //   if (buffer[i] < 0x10)
  //   {
  //     DEBUG_PRINT(0);
  //   }
  //   DEBUG_PRINT(buffer[i], HEX);
  // }
  // DEBUG_PRINTLN();

  snappy_env env;
  snappy_init_env(&env);
  DEBUG_PRINT("After Compression Init: ");
  PRINT_HEAP();

  size_t len = snappy_max_compressed_length(os.bytes_written);
  DEBUG_PRINT("Required buffer size for compression: ");
  DEBUG_PRINTLN(len);

  if (len > _bufferSize) {
    errmsg = "WriteRequest bufferSize is too small, enable debug logging to see required buffer size";
    return -1;
  }

  snappy_compress(&env, (char*)buffer, os.bytes_written, (char*)output, &len);
  snappy_free_env(&env);

  DEBUG_PRINT("Compressed Len: ");
  DEBUG_PRINTLN(len);

  DEBUG_PRINT("After Compression: ");
  PRINT_HEAP();
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