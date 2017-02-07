// Copyright 2017 Jeremy Letang.
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#include <proto2ts/ts_generator.hpp>

namespace p2t = proto2ts;

int main(int argc, char* argv[]) {
  p2t::ts_generator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
