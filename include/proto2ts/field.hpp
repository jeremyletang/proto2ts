// Copyright 2017 Jeremy Letang.
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef PROTO2TS_FIELD_09102016
#define PROTO2TS_FIELD_09102016

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>

using namespace google::protobuf::compiler;
using namespace google::protobuf;
using namespace google::protobuf::io;

namespace proto2ts {

  enum class ts_type {
    number = 0,
      string,
      boolean,
      enum_,
      message,
      unknown
      };

  std::string ts_type_to_string(const ts_type& t) {
    auto s = std::string{};
    switch (t) {
    case ts_type::number: s = "number"; break;
    case ts_type::string: s = "string"; break;
    case ts_type::boolean: s = "boolean"; break;
    case ts_type::enum_: s = "enum"; break;
    case ts_type::message: s = "message"; break;
    case ts_type::unknown: s = "unknown"; break;
    }
    return s;
  }

  template<typename CharT,
           typename Traits>
  std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                                const ts_type& t) {
    os << ts_type_to_string(t);
    return os;
  }

  ts_type proto_type_to_ts(const FieldDescriptor::Type& t) {
    switch (t) {
    case FieldDescriptor::TYPE_DOUBLE: return ts_type::number;
    case FieldDescriptor::TYPE_FLOAT: return ts_type::number;
    case FieldDescriptor::TYPE_INT64: return ts_type::number;
    case FieldDescriptor::TYPE_UINT64: return ts_type::number;
    case FieldDescriptor::TYPE_INT32: return ts_type::number;
    case FieldDescriptor::TYPE_FIXED64: return ts_type::number;
    case FieldDescriptor::TYPE_FIXED32: return ts_type::number;
    case FieldDescriptor::TYPE_BOOL: return ts_type::boolean;
    case FieldDescriptor::TYPE_STRING: return ts_type::string;
    case FieldDescriptor::TYPE_GROUP: return ts_type::unknown;
    case FieldDescriptor::TYPE_MESSAGE: return ts_type::message;
    case FieldDescriptor::TYPE_BYTES: return ts_type::string;
    case FieldDescriptor::TYPE_UINT32: return ts_type::number;
    case FieldDescriptor::TYPE_ENUM: return ts_type::enum_;
    case FieldDescriptor::TYPE_SFIXED32: return ts_type::number;
    case FieldDescriptor::TYPE_SFIXED64: return ts_type::number;
    case FieldDescriptor::TYPE_SINT32: return ts_type::number;
    case FieldDescriptor::TYPE_SINT64: return ts_type::number;
    }
    return ts_type::unknown;
  }

  enum class label {
    optional = 0,
      required,
      repeated,
      unknown
      };

  label proto_label_to_ts(const FieldDescriptor::Label& l) {
    switch (l) {
    case FieldDescriptor::LABEL_OPTIONAL: return label::optional;
    case FieldDescriptor::LABEL_REQUIRED: return label::required;
    case FieldDescriptor::LABEL_REPEATED: return label::repeated;
    }
    return label::unknown;
  }

  struct field {
    ts_type ty;
    std::string ty_name;
    label labl;
    std::string name;

    field(const FieldDescriptor::Type& t,
          const std::string& ty_name,
          const FieldDescriptor::Label& l,
          const std::string& name)
      : ty(proto_type_to_ts(t)),
        ty_name(ty_name),
        labl(proto_label_to_ts(l)),
        name(name) {
      if (this->ty_name.length() == 0) {
        this->ty_name = ts_type_to_string(this->ty);
      }
    }
  };


  std::string to_string(const field& f, bool is_interface = false) {
    auto os = std::ostringstream{};
    // name
    os << f.name;
    // is optional ?
    if (f.labl == label::optional) {
      os << "?";
    }
    os << ": ";
    // print type
    if (f.ty == ts_type::enum_ or f.ty == ts_type::message) {
      if (is_interface && f.ty == ts_type::message) {
        os << "I";
      }
      os << f.ty_name;
    } else {
      os << f.ty;
    }
    // is array ?
    if (f.labl == label::repeated) {
      os << "[]";
    }

    // add | undefined if object + optional
    if ((f.ty == ts_type::enum_ or f.ty == ts_type::message)and f.labl == label::optional) {
      os << " | undefined";
    }

    // terminate line
    os << ";";

    return os.str();
  }

} // proto2ts

#endif // PROTO2TS_FIELD_09102016
