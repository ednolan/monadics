// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lib_c.hpp"

int main() { return integration::combined_result() == 25 ? 0 : 1; }
