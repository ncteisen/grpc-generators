/*
 *
 * Copyright 2015, Google Inc.
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

#include <memory>
#include <sstream>
#include <cstdlib>

#include "abstract_generator.h"

class CppGrpcClientGenerator : public AbstractGenerator {
 private:
  grpc::string GetLanguageSpecificFileExtension() const 
  { 
    return ".grpc.client.pb.cc"; 
  }

  grpc::string GetCommentPrefix() const 
  {
    return "// "; 
  }

  void PrintPackage(Printer &printer, vars_t &vars) const
  {
    // nothing to do for c++
  }

  void PrintIncludes(Printer &printer, vars_t &vars) const
  {
    // headers
    std::vector<grpc::string> headers = {
        "iostream",
        "memory",
        "string",
        "cstdint",
        "thread",
        "gflags/gflags.h",
        "grpc++/grpc++.h",
        "grpc/support/log.h",
        "grpc/support/useful.h"};
    
    for (auto i = headers.begin(); i != headers.end(); i++) {
      vars["header"] = *i;
      printer.Print(vars, "#include <$header$>\n");
    }

    printer.Print(
      vars, "\n#include \"$proto_filename_without_ext$.grpc.pb.h\"\n\n");
  }

  void PrintFlags(Printer &printer, vars_t &vars) const
  {
    printer.Print(
      "// In some distros, gflags is in the namespace "
      "google, and in some others,\n"
      "// in gflags. This hack is enabling us to find both.\n"
      "namespace google {}\n"
      "namespace gflags {}\n"
      "using namespace google;\n"
      "using namespace gflags;\n\n");

    // print the flag definitions
    printer.Print("DEFINE_bool(use_tls, false, \"Connection uses TLS if true, else plain TCP.\");\n"
        "DEFINE_string(custom_ca_file, \"\", "
            "\"The file containning the CA root cert file.\");\n"
        "DEFINE_int32(server_port, 8080, \"Server port.\");\n"
        "DEFINE_string(server_host, \"localhost\", "
            "\"Server host to connect to\");\n"
        "DEFINE_string(server_host_override, \"foo.test.google.fr\",\n"
        "\t\t\"The server name use to verify the hostname returned by TLS handshake\");\n\n");
  }

  void DoPrintMessagePopulatingFunctionDecl(
      Printer &printer, vars_t &vars) const
  {
    printer.Print(vars, "void Populate$name$($type$ &message);\n");
  }

  void DoPrintMessagePrintingFunctionDecl(
      Printer &printer, vars_t &vars) const
  {
     printer.Print(vars, "void Print$name$($type$ &message);\n");
  }

  void StartMain(Printer &printer) const
  {
    printer.Print("int main() {\n");
    printer.Indent();
  }

  void EndMain(Printer &printer) const
  {
    printer.Outdent();
    printer.Print("}\n");
  }

};


int main(int argc, char *argv[]) {
  srand(time(NULL));
  CppGrpcClientGenerator generator;
  return grpc::protobuf::compiler::PluginMain(argc, argv, &generator);
}
