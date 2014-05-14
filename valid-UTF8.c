#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static bool is_valid_utf8(const char *c, int size);
static bool invalid_trailing_byte(const char c);
static int number_of_bytes(const char c);

struct Tester {
  const char* string;
  bool valid;
};

int
main (void) {

  struct Tester tests[] = { 
                           {"\xc3\xb1", true},
                           {"\xe2\x28\xa1", false},
                           {"Hello, World!", true},
                           {"\xf0\x28\x8c\x28", false},
                           {"\xE2\x98\x82\xE2\x98\x83", true},
                           {"\xf0\x28\x8c\xbc", false},
                           {"\xe2\x28\x28", false},
                           {"\xc3\x28" , false},
                           {"", true},
                           {"   ", true},
                           {"\x80", false}
                         };

  int end = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < end; i++) {

    const char *string = tests[i].string;
    int size = strlen(string);

    const char *result = (is_valid_utf8(string, size) == tests[i].valid) ? "Pass" : "Fail";
    printf("%d: %s\n", i, result);
  }

  return 0;
}

static bool
is_valid_utf8(const char *c, int size) {

  for (int i = 0; i < size; i++) {
    switch(number_of_bytes(c[i])) {
      case 0:
        return false;
      case 1 :
        break;
      case 2 :
        if (i + 1 > size) { return false; }
        if (invalid_trailing_byte(c[++i])) { return false; }
        break;
      case 3 :
        for (int temp = 0; temp < 2; temp++) {
          if (i + 1 > size) { return false; }
          if (invalid_trailing_byte(c[++i])) { return false; }
        }
        break;
      case 4 :
        for (int temp = 0; temp < 3; temp++) {
          if (i + 1 > size) { return false; }
          if (invalid_trailing_byte(c[++i])) { return false; }
        }
        break;
      default :
        return false;
    }
  }
  return true;
}

static bool
invalid_trailing_byte(const char c) {
// After the leading byte, we have n-1 occurrences of 10xxxxxx bytes
  return ((c & 0xC0) == 0x80) ? false : true;
}

static int
number_of_bytes(const char c) {
  // High bit set to 0 only on single byte sequences (same as ASCII).
  if ((c & 0x80) == 0) { return 1; }
  // All others have the first n bits set to 1 and the next 0.
  // e.g. 2 byte sequences have the first byte start with 110.
  else if ((c & 0xE0) == 0xC0) { return 2; }
  else if ((c & 0xF0) == 0xE0) { return 3; }
  else if ((c & 0xF8) == 0xF0) { return 4; }
  // Not a valid UTF-8 sequence.
  else { return 0; }
}
