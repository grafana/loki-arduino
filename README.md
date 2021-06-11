# GrafanaLoki

An Arduino library for sending logs directly to Loki.

## Overview

This library is built to make it easy to send logs to [Grafana Loki](https://github.com/grafana/loki). If you find yourself wanting to send text data to a time series database built for easy long term storage in object stores like S3/GCS, then this library and Loki are right for you!

Loki can do a lot more than store logs, if you structure your data in [json](https://arduinojson.org/) or [logfmt](https://brandur.org/logfmt), Loki can easily parse and extract this data into metrics and graphs using Prometheus' PromQL query language adapted for metrics and much more with Loki's LogQL.

There are many use cases where storing your sensor data in a structured log line could be advantageous, but also make sure you check out the [Prometheus remote write library](https://github.com/grafana/prometheus-arduino) for metric data.  

## Dependencies

Not all of these libraries need to be installed for every board but it may just be easier to install them all.

* **SnappyProto** For creating snappy compressed protobufs
* **PromLokiTransport** Handles underlying connections for connecting to networks and sending data
  * **ArduinoBearSSL** For SSL connections
  * **ArduinoECCX08** For devices that have hardware support for SSL connections
  * **MKRGSM** For MKRGSM1400 board
  * **WiFiNINA** for MKRGSM1010 board
* **ArduinoHttpClient**


## Streams

### Stream

Loki stores log data in a stream, a stream is defined by the unique set of labels sent with the stream. Streams are essentially the same as series in Prometheus however loki does not have a `metric name`, otherwise they are essentially the same.

Here is an example of two log streams:

```
{job="esp-32", type="location"}
{job="esp-32", type="message"}
```
Streams can have the same labels with different values, the combination of _all_ labels and values defines the stream.

Some examples of queries could for these entries could be 

```
{job="esp-32"} # Returns entries from both streams
{type="location"} # Returns entries from only the first stream.
```

A stream is created with the following constructor:

```
LokiStream(uint8_t batchSize, uint8_t maxEntryLength, const char* labels);
```

Multiple entries can be appended into a stream up to the `batchSize`

Entries can have a maxium length of `maxEntryLength` and the labels are defined at construction as well.

Arduino devices are very memory constrained and also tend to suffer from [Heap Fragmentation](https://cpp4arduino.com/2018/11/06/what-is-heap-fragmentation.html) if you allocated and deallocate memory too frequently, as a result we use `batchSize` and `maxEntryLength` to pre-allocated space on the heap when creating a stream. As a result we need to put a limit on the max length of a log line and how many we allow per stream.

Example:

```
LokiStream s1(5, 30, "{job=\"mkr\",stream=\"1\"}");
```

This stream would allocate 5*30+26=176 bytes on the heap (the +26 bytes is the number of bytes in the label string)

More info on the label string format is in the [Labels section](#labels) below.

### Streams

A stream is grouped with others into a `Streams` object as multiple streams can be sent in one push to Loki.

```
LokiStreams(uint16_t numStreams, uint32_t bufferSize = 512);
```

Example:

```
LokiStreams streams(2); // Default buffer size of 512 bytes
LokiStreams streams(2,1024) // Custom buffer size
```

More details on the `bufferSize` are in the [Buffer section](#buffers) below.

Add your `stream` to your `streams` object like so:

```
streams.addStream(stream1);
```

## Labels

Labels are important to Loki as they are the only thing indexed, Loki does not index the log line content. You might be thinking then that you should use lots of labels to make queries faster, **this is mistake :)** instead you should keep your labelset as simple as possible.

Just like how the labelset above requires memory when creating your stream, more labels leads to more memory in Loki's index too. One of the keys to Loki's success is keeping a small index, instead it has other tricks up its sleeve for performance such as parallelization.

So stick to just a couple labels to make it easy to identify your logs/events, and instead favor putting more data in the log line in a structured format to be parsed at query time.

Looking again at the example from above:

```
LokiStream s1(5, 30, "{job=\"mkr\",stream=\"1\"}");
```

There is a good reason for the kind of ugly format lables are defned.  It was the easiest :)

This is the exact way the labels are sent in the protobuf to Loki so rather than write a bunch of code to turn a set of strings into that format unfortunately you have to for now...

The format is fairly simple however: 

```
"{name=\"value\"}"
```

It's essentially name="value" with the double quotes escaped, wrapped in `{}` with a comma separating multiple labels.

The [prometheus rules for labels](https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels) apply to Loki as well.

## Batching

HTTP requests have a lot of overhead and if you are generating a lot of log entries it's much more efficient to batch them.

When defining a `Stream` object you must specify a batch size, this then allocates memory on the heap for storing batches.

You do not need to fill a batch to send it.

It is not required to batch, but it will improve efficieny a little and reduce bandwidth consumption.

**NOTE** You must reset the batch after every send with the `resetSamples()` function.

## Buffers

There are 2 important buffers used to serialize the data to send to Loki, one for creating the proto and one for Snappy compressing it.

Unfortunately it's not possible to use the same buffer for both, as you need to read the proto buffer to create the Snappy compressed output in a new buffer.

**By default this library sets both of these buffers to 512 bytes.**

The size for both of these buffers can be set when initializing the Streams object, see the example above.

These buffers are allocated on the stack during a send, meaning you need 2x this buffer size value in stack space to send.

It's possible, but difficult to estimate how big the buffer needs to be, mainly because protos do a good job of reducing the size of numeric values, however in a Loki push request most of the data is a string so you can estimate this like what we did above:

```
LokiStream s1(5, 30, "{job=\"mkr\",stream=\"1\"}");
```

This stream would allocate 5*30+26=176 bytes, the actual proto also includes a nanosecond timestamp for each entry, so as a starting point make sure your buffer is at least as big as this space plus some bytes for a timestamp, maybe 10? (sorry I'm not actually sure), instead we can do this empircally:


## How to size the buffer

For most devices the "free memory" is printed out during this process, you can use this to understand if you have enough stack space to perform a send without crashing your stack. _Make sure you have as much free mem before starting to serialize as 2x the buffer size._

Enable debug logging on your Streams object, 

```
streams.setDebug(Serial);
```

Send your data and look at the logs, if you get an error on `toSnappyProto` and you see `stream full` in the debug output then the buffer is undersized for serializing to protobuf, try increasing it:

```
Streams streams(2,1024);
```

Compression can actually result in a larger object in some cases, usually around small objects with a lot of strings.

The debug logging will tell you the size needed for compression:

```
Required buffer size for compression: 754
```

Make sure your buffer size is bigger than this.

If you are working on a very memory constrained device, keep your `batchSize`, `maxEntryLength` and labels as small as possible.


## SSL

TLS/SSL configuration is difficult with Arduinos as the hardware is barely powerful enough (or not powerful enough at all in some cases) to do SSL negotiations, and there are several types of hardware and software libraries for doing SSL.

The [PromLokiTransport](https://github.com/grafana/arduino-prom-loki-transport) library attempts to simplify this by providing the libraries and code for making SSL connections, check out that project for more information, or just look at the `loki_02_grafana_cloud.ino` example to see how to load a CA certificate and perform SSL connections.


## License Notes

This libray is licensed under the Apache2 license however it uses several libraries which use different licenses:

* Arduino API and libraries are released under the LGPL, this [note describes requirements for LGPL code](https://support.arduino.cc/hc/en-us/articles/360018434279-I-have-used-Arduino-for-my-project-do-I-need-to-release-my-source-code-)
* [SnappyProto](https://github.com/grafana/arduino-snappy-proto) ports two libraries both released under permissive, but custom licenses. See the project for more info.
* [PromLokiTransport](https://github.com/grafana/arduino-prom-loki-transport) which is Apache2 but contains some MIT licensed files from [BearSSL](https://bearssl.org/)