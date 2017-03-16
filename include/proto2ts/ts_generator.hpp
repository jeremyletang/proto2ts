// Copyright 2017 Jeremy Letang.
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef PROTO2TS_TS_GENERATOR_09102016
#define PROTO2TS_TS_GENERATOR_09102016

#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <cstring>
#include <sstream>
#include <set>

#include </usr/local/Cellar/protobuf/3.1.0/include/google/protobuf/compiler/plugin.h>
#include </usr/local/Cellar/protobuf/3.1.0/include/google/protobuf/compiler/code_generator.h>
#include </usr/local/Cellar/protobuf/3.1.0/include/google/protobuf/io/zero_copy_stream.h>
#include </usr/local/Cellar/protobuf/3.1.0/include/google/protobuf/descriptor.h>

#include "field.hpp"

using namespace google::protobuf::compiler;
using namespace google::protobuf;
using namespace google::protobuf::io;

namespace proto2ts {

  namespace detail {

    void write_nested_type(const FileDescriptor& fd,
                           const Descriptor& desc,
                           std::ostringstream& out);

    std::tuple<bool, std::string> write_all(ZeroCopyOutputStream& stream,
                                            std::string data) {
      auto size = 0;
      char *buf;

      while (data.length() > 0) {
        // get next buffer
        if (not stream.Next(reinterpret_cast<void**>(&buf), &size)) {
          // error occured
          auto estr = std::string{"proto2ts: unable to acquire new buffer to write output"};
          return std::make_tuple(false, estr);
        }
        // copy size char from the string to the buf
        if (data.length() < static_cast<std::string::size_type>(size)) {
          std::strncpy(buf, data.c_str(), data.length());
          stream.BackUp(size - data.length());
        } else {
          std::strncpy(buf, data.c_str(), size);
        }
        // remove them from the string
        data.erase(0, size);
      }

      return std::make_tuple(true, std::string{});
    }

    std::vector<field> get_message_fields(const Descriptor& desc) {
      auto fields_count = desc.field_count();
      auto fs = std::vector<field>{};
      for (auto i = 0; i < fields_count; i+=1) {
        auto raw_field = desc.field(i);
        auto type_name = std::string{};

        // get typename if its an enum or message
        switch (raw_field->type()) {
        case FieldDescriptor::TYPE_MESSAGE:
          if (not raw_field->is_map()) {
            type_name = raw_field->message_type()->name();
          } else {
            type_name = "__MAP__SHIT__";
          }
          break;
        case FieldDescriptor::TYPE_ENUM:
          type_name = raw_field->enum_type()->name(); break;
        default: break;
        }

        fs.emplace_back(raw_field->type(),
                        type_name,
                        raw_field->label(),
                        raw_field->name());
      }
      return fs;
    }

    void write_header(const FileDescriptor& fd, std::ostringstream& out) {
      out << "// this is a generated file please do not edit\n";
      out << "// original proto file name: " << fd.name() << "\n";
      out << "// original package name: " << fd.package() << "\n";
      // out << "// protobuf syntax: " << FileDescriptor::SyntaxName(fd.syntax()) << "\n";
    }

    void write_imports(const std::string& current_type_name,
                       const std::vector<field>& fields,
                       std::ostringstream& out,
                       const std::vector<std::string> exclude_names) {
      auto print_import = false;
      auto dedup = std::set<std::string>{};
      // add to dedup list of excluded_names
      dedup.insert("__MAP__SHIT__");
      // add current type to the dedup so we do not import ourselves
      dedup.insert(current_type_name);
      for (const auto& e : exclude_names) {
        dedup.insert(e);
      }
      for (const auto& f : fields) {
        // if not already inserted
        if (f.ty == ts_type::message and dedup.find(f.ty_name) == dedup.end()) {
          out << "import { " << f.ty_name << ", I" << f.ty_name
              << " } from './" << f.ty_name << "';\n";
          print_import = true;
          dedup.insert(f.ty_name);
        } else if (f.ty == ts_type::enum_ and dedup.find(f.ty_name) == dedup.end()) {
          out << "import { " << f.ty_name << " } from './" << f.ty_name << "';\n";
          print_import = true;
          dedup.insert(f.ty_name);
        }
      }
      if (print_import) {
        out << "\n";
      }
    }

    void write_interface(const Descriptor& desc,
                         const std::vector<field>& fields,
                         std::ostringstream& out) {
      out << "export interface I" << desc.name() << " {\n";
      for (const auto& f: fields) {
        // two spaces indent
        if (f.ty_name != "__MAP__SHIT__") {
          out << "  " << to_string(f, true) << "\n";
        }
      }
      out << "}\n";
    }

    void write_class(const Descriptor& desc,
                     const std::vector<field>& fields,
                     std::ostringstream& out) {
      out << "export class " << desc.name() << " {\n";
      for (const auto& f: fields) {
        // two spaces indent
        if (f.ty_name != "__MAP__SHIT__") {
          out << "  " << to_string(f) << "\n";
        }
      }

      // generate constructor
      out << "\n";
      out << "  constructor(data: I" << desc.name() << ") {\n";
      for (const auto& f: fields) {
        if (f.ty_name != "__MAP__SHIT__") {
          out << "    this." << f.name << " = ";
          if (f.labl == label::repeated and f.ty == ts_type::message) {
            out << "(data." << f.name << " && data." << f.name << ".length > 0) ";
            out << "? data." << f.name << ".map((v: I" << f.ty_name << ") => new " << f.ty_name <<
              "(v)) ";
            out << ": [];\n";
          } else {
            if (f.ty == ts_type::message) {
              if (f.labl == label::optional) {
                out << "data." << f.name << " ? new " << f.ty_name << "(data." <<
                  f.name << ") : undefined;\n";
              } else {
                out << "new " << f.ty_name << "(data." << f.name << ");\n";
              }
            } else {
              out << "data." << f.name << ";\n";
            }
          }
        }
      }

      out << "  }\n}\n";
    }

    void write_enum(const EnumDescriptor& enum_desc,
                    std::ostringstream& out) {
      out << "export enum " << enum_desc.name() << " {\n";
      for (auto i = 0; i < enum_desc.value_count(); i += 1) {
        auto value = enum_desc.value(i);
        out << "  " << value->name() << " = " << value->number() << ",\n";
      }
      out << "}\n";
    }

    std::vector<std::string> get_nested_type_names(const Descriptor& desc) {
      auto out = std::vector<std::string>{};
      for (auto i = 0; i < desc.enum_type_count(); i += 1) {
        out.push_back(desc.enum_type(i)->name());
      }
      return out;
    }

    std::string generate_one_message_ts(const FileDescriptor&fd,
                                        const Descriptor& desc,
                                        bool should_write_header = true) {
      auto out = std::ostringstream{};
      auto fields = get_message_fields(desc);
      if (should_write_header) {
        write_header(fd, out);
        out << "\n";
      }
      auto nested = get_nested_type_names(desc);
      write_imports(desc.name(), fields, out, nested);
      write_nested_type(fd, desc, out);
      write_interface(desc, fields, out);
      out << "\n";
      write_class(desc, fields, out);
      return out.str();
    }

    std::string generate_one_enum_ts(const FileDescriptor&fd,
                                     const EnumDescriptor& desc,
                                     bool should_write_header = true) {
      auto out = std::ostringstream{};
      if (should_write_header) {
        write_header(fd, out);
        out << "\n";
      }
      write_enum(desc, out);
      return out.str();
    }

    void write_nested_type(const FileDescriptor& fd,
                           const Descriptor& desc,
                           std::ostringstream& out) {
      for (auto i = 0; i < desc.enum_type_count(); i += 1) {
        auto enum_ = desc.enum_type(i);
        auto body = generate_one_enum_ts(fd, *enum_, false);
        out << body << "\n";
      }
    }

    std::tuple<bool, std::string> generate_ts(const FileDescriptor& fd,
                                              GeneratorContext& generator_context) {
      auto message_count = fd.message_type_count();
      for (auto i = 0; i < message_count; i+=1) {
        auto message = fd.message_type(i);
        auto file_body = generate_one_message_ts(fd, *message);
        auto stream = generator_context.Open(message->name() + ".ts");
        auto res = detail::write_all(*stream, file_body);
        if (not std::get<0>(res)) {
          return res;
        }
      }
      auto enum_count = fd.enum_type_count();
      for (auto i = 0; i < enum_count; i+=1) {
        auto enum_ = fd.enum_type(i);
        auto file_body = generate_one_enum_ts(fd, *enum_);
        auto stream = generator_context.Open(enum_->name() + ".ts");
        auto res = detail::write_all(*stream, file_body);
        if (not std::get<0>(res)) {
          return res;
        }
      }
      return std::make_tuple(true, std::string{});
    }

    std::string to_ts_filename(const std::string& proto_filename) {
      auto pos = proto_filename.find_last_of(".");
      auto name = proto_filename.substr(0, pos);
      return name + ".ts";
    }

  } // detail

  class ts_generator: public CodeGenerator {
  public:
    ts_generator() = default;
    virtual ~ts_generator() = default;

    virtual bool Generate(const FileDescriptor* fd,
                          const std::string&,
                          GeneratorContext* generator_context,
                          std::string* error) const {

      auto res = detail::generate_ts(*fd, *generator_context);
      if (not std::get<0>(res)) {
        error->swap(std::get<1>(res));
        return false;
      }

      return true;
    }

    virtual bool GenerateAll(const std::vector<const FileDescriptor *>& fds,
                             const std::string& wtf,
                             GeneratorContext* generator_context,
                             std::string *error) const {
      auto ret = true;
      auto err = std::string{};
      for (const auto& fd : fds) {
        auto tmp_err = std::string{};
        ret |= this->Generate(fd, wtf, generator_context, &tmp_err);
        if (tmp_err.size() not_eq 0) {
          err += "\n" + tmp_err;
        }
      }

      if (not ret) {
        error->swap(err);
      }

      return ret;
    }

    virtual bool HasGenerateAll() const {
      return true;
    }
  };
} // proto2ts

#endif // PROTO2TS_TS_GENERATOR_09102016
