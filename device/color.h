#ifnded __COLOR_H
#define __COLOR_H

// standard c headers
#include <stdint.h>
#include <stdbool.h>

// project specific headers
#include "jelly.h"

struct RGBColor {
  uint8_t red, green, blue;
};

void set_color(struct Jelly*, struct RGBColor*);

#endif
