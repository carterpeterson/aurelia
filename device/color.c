#include "color.h"

void set_color(struct Jelly* jelly, struct RGBColor* color)
{
  jelly->color.red = color->red;
  jelly->color.green = color->green;
  jelly->color.blue = color->blue;
  jelly->color_invalid = true;
}
