#ifndef __COLOR_H
#define __COLOR_H

// standard c headers
#include <stdint.h>
#include <stdbool.h>

// project specific headers
#include "jelly.h"

#ifdef SIMULATED
#include "../simulator/render.h"
#endif

struct RGBColor {
  uint8_t red, green, blue;
};

void c_update_color(struct Jelly *jelly);

#endif
