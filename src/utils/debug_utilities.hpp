#pragma once

// (C) 2014 - Monkeykingz Audio
// See LICENSE for more information.

#include <string>

namespace monkeykingz
{
  void dump_binary(const void* data, uint32_t data_size, bool display_header = false);
  void log_hex_dump(const char* marker_text, const void* data, uint32_t data_size);

  std::string hex_dump(const char* marker_text, const void* data, uint32_t data_size);
  std::string hex_string(const void* data, uint32_t data_size);
}
