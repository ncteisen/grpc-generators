/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef GRPC_INTERNAL_GENERATOR_CPP_CLIENT_GENERATOR_H
#define GRPC_INTERNAL_GENERATOR_CPP_CLIENT_GENERATOR_H

// cpp_client generator.h/.cc do not directly depend on GRPC/ProtoBuf, such that
// they can be used to generate code for other serialization systems, such as
// FlatBuffers.

#include <memory>
#include <vector>

#include "config.h"

#ifndef GRPC_CUSTOM_STRING
#include <string>
#define GRPC_CUSTOM_STRING std::string
#endif

namespace grpc {

typedef GRPC_CUSTOM_STRING string;

}  // namespace grpc

namespace grpc_cpp_client_generator {

// Contains all the parameters that are parsed from the command line.
struct Parameters {
  // Puts the service into a namespace
  grpc::string services_namespace;
  // Use system includes (<>) or local includes ("")
  bool use_system_headers;
  // Prefix to any grpc include
  grpc::string grpc_search_path;
};

// forward decl
struct Message;

struct Enum {
  virtual ~Enum() {}
  virtual std::string name() const = 0;
  virtual std::string type_name() const = 0;
  virtual std::string random_value_type() const = 0;

};

struct Field {
  virtual ~Field() {}

  // Specifies the C++ data type used to represent the field.  There is a
  // fixed mapping from Type to CppType where each Type maps to exactly one
  // CppType.  0 is reserved for errors.
  enum Type {
    INT32       = 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
    INT64       = 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
    UINT32      = 3,     // TYPE_UINT32, TYPE_FIXED32
    UINT64      = 4,     // TYPE_UINT64, TYPE_FIXED64
    DOUBLE      = 5,     // TYPE_DOUBLE
    FLOAT       = 6,     // TYPE_FLOAT
    BOOL        = 7,     // TYPE_BOOL
    ENUM        = 8,     // TYPE_ENUM
    STRING      = 9,     // TYPE_STRING, TYPE_BYTES
    MESSAGE     = 10,    // TYPE_MESSAGE, TYPE_GROUP
  };

  virtual std::unique_ptr<const Message> message_type() const = 0;
  virtual std::unique_ptr<const Enum> enum_type() const = 0;
  virtual grpc::string name() const = 0;
  virtual bool is_repeated () const = 0;
  virtual grpc::string type_name() const = 0;
  virtual Type type() const = 0;

};

struct Message {
  virtual ~Message() {}

  virtual grpc::string type_name() const = 0;
  virtual grpc::string name() const = 0;
  virtual int field_count() const = 0;
  virtual std::unique_ptr<const Field> field(int i) const = 0;

};

// An abstract interface representing a method.
struct Method {
  virtual ~Method() {}

  virtual grpc::string name() const = 0;

  virtual grpc::string input_type_name() const = 0;
  virtual grpc::string output_type_name() const = 0;

  virtual std::unique_ptr<const Message> input_message() const = 0;
  virtual std::unique_ptr<const Message> output_message() const = 0;

  virtual bool NoStreaming() const = 0;
  virtual bool ClientOnlyStreaming() const = 0;
  virtual bool ServerOnlyStreaming() const = 0;
  virtual bool BidiStreaming() const = 0;
};


// An abstract interface representing a service.
struct Service {
  virtual ~Service() {}

  virtual grpc::string name() const = 0;

  virtual int method_count() const = 0;
  virtual std::unique_ptr<const Method> method(int i) const = 0;
};

struct Printer {
  virtual ~Printer() {}

  virtual void Print(const std::map<grpc::string, grpc::string> &vars,
                     const char *template_string) = 0;
  virtual void Print(const char *string) = 0;
  virtual void Indent() = 0;
  virtual void Outdent() = 0;
};

// An interface that allows the source generated to be output using various
// libraries/idls/serializers.
struct File {
  virtual ~File() {}

  virtual grpc::string filename() const = 0;
  virtual grpc::string filename_without_ext() const = 0;
  virtual grpc::string message_header_ext() const = 0;
  virtual grpc::string service_header_ext() const = 0;
  virtual grpc::string package() const = 0;
  virtual grpc::string package_with_colons() const = 0;
  virtual std::vector<grpc::string> package_parts() const = 0;
  virtual grpc::string additional_headers() const = 0;

  virtual int service_count() const = 0;
  virtual std::unique_ptr<const Service> service(int i) const = 0;

  virtual std::unique_ptr<Printer> CreatePrinter(grpc::string *str) const = 0;
};

// Return the prologue of the generated header file.
grpc::string GetClientPrologue(File *file, const Parameters &params);

// Return the includes needed for generated header file.
grpc::string GetClientIncludes(File *file, const Parameters &params);

// Return the services for generated header file.
grpc::string GetClientServices(File *file, const Parameters &params);

// Return the epilogue of the generated header file.
grpc::string GetClientEpilogue(File *file, const Parameters &params);

}  // namespace grpc_cpp_client_generator

#endif  // GRPC_INTERNAL_GENERATOR_CPP_CLIENT_GENERATOR_H