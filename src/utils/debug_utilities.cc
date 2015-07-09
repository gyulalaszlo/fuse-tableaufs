#include "debug_utilities.hpp"

// (C) 2014 - Monkeykingz Audio
// See LICENSE for more information.

#include <cstdio>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace monkeykingz;

namespace
{
  enum { kCharsPerSlice = 16 };
  // helper to dump the hex part for a slice
  void dump_hex_part(const char* data, uint32_t data_size)
  {
    for (uint32_t i = 0; i < data_size; i++) {
      if (i % 4 == 0) {
        printf("  ");
      }
      const unsigned char* c = (const unsigned char*)&data[i];
      unsigned char cv = *c;
      printf("%02x ", cv);
    }
  }
  //
  // helper to dump the string part for a slice
  void dump_string_part(const char* data, uint32_t data_size)
  {
    for (uint32_t i = 0; i < data_size; i++) {
      const char* c = (const char*)&data[i];
      if (*c > ' ' && *c < '~') {
        printf("%1c", *c);
      } else {
        printf(" ");
      }
    }
  }

  //
  void dump_data_slice(const void* base, uint32_t slice_idx,
                       const uint32_t data_size)
  {
    const char* slice = ((const char*)base) + (slice_idx * kCharsPerSlice);
    // const void ptr* = data[
    dump_hex_part(slice, data_size);
    printf("  |  ");
    dump_string_part(slice, data_size);
  }
}

namespace monkeykingz
{
  // Implementations
  // ----------------------------------------
  void dump_binary(const void* data, uint32_t data_size, bool display_header)
  {
    // unsigned char is necessary because %02x causes
    // printf to print it as an unsigned int, not a signed one.
    if (display_header) {
      printf(
          "  %8d bytes @ 0x%016lx:\n--------------------------------------\n",
          data_size, (uintptr_t)data);
    }

    const uint32_t slice_count = data_size / kCharsPerSlice;
    const uint32_t last_slice_len = data_size % kCharsPerSlice;

    for (uint32_t i = 0; i < slice_count; ++i) {
      dump_data_slice(data, i, kCharsPerSlice);
      printf("\n");
    }
    // dump the final part
    if (last_slice_len > 0) {
      dump_data_slice(data, slice_count, last_slice_len);
    }
    printf("\n");
  }

  std::string hex_string(const void* data, uint32_t data_size)
  {
    using std::hex;
    using std::dec;
    using std::setw;
    using std::setfill;
    std::stringstream cout;
    char* cursor = (char*)data;
    char* start = cursor;
    char* end = start + data_size;

    int j = 0;
    cout << hex << setfill('0');
    while (cursor < end) {
      char buf = *cursor;
      cursor++;
      j++;
      //
      if (j % 4 == 0) cout << ' ';
      cout << setw(2) << (0x000000ff & (uint32_t)buf);
    }
    cout << " | ";
    cursor = (char*)data;
    j = 0;
    while (cursor < end) {
      char buf = *cursor;
      cursor++;
      j++;
      if (buf < 32 && buf != '\"' && buf != '\\')
        cout << '.';
      else
        cout << buf;
    }
    return cout.str();
  }

  std::string hex_dump(const char* marker_text, const void* data,
                       uint32_t data_size)
  {
    unsigned long address = 0;

    using std::hex;
    using std::dec;
    using std::setw;
    using std::setfill;
    std::stringstream cout;

    cout << dec << setfill(' ');
    cout << "----- " << setw(20) << marker_text << " ----- ";

    cout << hex << setfill('0');
    cout << setw(16) << (uintptr_t)data << " ------";

    cout << dec << setfill('-');
    cout << setw(10) << data_size << " bytes ---\n";
    cout << hex << setfill('0');
    char* cursor = (char*)data;
    char* start = cursor;
    char* end = start + data_size;

    while (cursor < end) {
      enum { PerRow = 16 };
      int nread = 0;
      char buf[PerRow];

      // copy a row of bytes
      while (cursor < end && nread < PerRow) {
        buf[nread] = *cursor;
        cursor++;
        nread++;
      }

      // Show the address
      cout << setw(8) << address;
      // cout << setw(8) << address;

      // Show the hex codes
      for (int j = 0; j < PerRow; j++) {
        if (j % 8 == 0) cout << ' ';
        if (j < nread)
          cout << ' ' << setw(2) << (0x000000ff & (uint32_t)buf[j]);
        else
          cout << "   ";
      }

      cout << "  ";
      for (int j = 0; j < nread; j++) {
        if (buf[j] < 32)
          cout << '.';
        else
          cout << buf[j];
      }

      cout << "\n";
      address += 16;
    }
    return cout.str();
  }

  void log_hex_dump(const char* marker_text, const void* data,
                    uint32_t data_size)
  {
    std::cout << hex_dump(marker_text, data, data_size);
  }
}

namespace monkeykingz
{
  // Interfaces
  // ----------------------------------------
}
