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

