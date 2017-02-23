proto2ts
========

This is a plugin for protoc compiler.
I wrote this in order to generate typescript definitions file from protobuf defintions.
This is only written to answer my needs, and will probably not work for you.

## how to build the plugin

You require only a C++14 compiler, libprotobuf (should be on your system if you installed protoc), and cmake.
I expect you to run the following command from the root of the repository:

```Shell
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release .. 
$ make
```

## how to use it with protoc

You just need to load it as a plugin for the protoc compiler

```Shell
$ protoc --plugin=$PROTO2TS_REPOSITORY/build/protoc-gen-proto2ts --proto_path=$PROTOPATH --proto2ts_out=$OUTPUT_DIR
```
