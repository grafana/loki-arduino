
```
docker run --rm -v ${PWD}:/tmp slimbean/nanopb-gen --proto_path=/tmp --nanopb_out=/tmp /tmp/timestamp.proto
docker run --rm -v ${PWD}:/tmp slimbean/nanopb-gen --proto_path=/tmp --nanopb_out=/tmp /tmp/logproto.proto
```

Change <pb.h> to "pb.h" in timestamp.ph.h and logproto.pb.h