// Copyright 2021, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <fstream>
#include <iostream>
#include <string>

#ifndef VIXL_TASM_ERRORS_H
#define VIXL_TASM_ERRORS_H

class ErrorHandler {
 public:
  size_t line_num{0};

  // Print error message to the std::cerr.
  void Err(std::string message) {
    std::cerr << "Error in line " << line_num << ": " << message << std::endl;
    std::cerr << "Input line: " << input_line << std::endl;

    std::cerr << std::endl;
    general_err = true;
    line_err = true;
  }

  // Get general error flag - err_flag will be set to true if at least one
  // error was raised during program assembly.
  bool GetErr() { return general_err; }

  // Get last line error flag - line_err will be set to true if at least one
  // error was raised in the last parsed instruction line.
  bool GetLineErr() { return line_err; }

  void IncrementLineNumber(std::string input_line) {
    line_err = false;
    line_num++;

    // Trim leading and trailing whitespaces from the input line string
    const char* w_spaces = " \t\n\r\f\v";
    input_line = input_line.erase(0, input_line.find_first_not_of(w_spaces));
    input_line = input_line.erase(input_line.find_last_not_of(w_spaces) + 1);
    this->input_line = input_line;
  }

 private:
  std::string input_line;
  bool general_err{false};
  bool line_err{false};
};

#endif