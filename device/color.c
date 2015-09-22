#include "color.h"

#define SIMULATED_COLOR_ZERO 25
#define DEVICE_COLOR_ZERO 0

void c_determine_color(struct Jelly *jelly)
{
  // dummy implementation for now, gonna have interchangable pattern calculation in the end
  struct ProximityListNode* current_node = jelly->proximity_locations;
  bool local_proximity = false;
  while (current_node != NULL) {
    if(current_node->proximity->position->x == jelly->position->x &&
       current_node->proximity->position->y == jelly->position->y) {
      local_proximity = true;
    }

    current_node = current_node->next_node;
  }

  if (local_proximity) {
    jelly->color->red = (jelly->position->x * 255) / POSITION_X_MAX;
    jelly->color->green = 128;
    jelly->color->blue = (jelly->position->y * 255) / POSITION_Y_MAX;;
    jelly->color_invalid = true;
  } else {
#ifdef SIMULATED
    jelly->color->red = SIMULATED_COLOR_ZERO;
    jelly->color->green = SIMULATED_COLOR_ZERO;
    jelly->color->blue = SIMULATED_COLOR_ZERO;
#else
    jelly->color->red = DEVICE_COLOR_ZERO;
    jelly->color->green = DEVICE_COLOR_ZERO;
    jelly->color->blue = DEVICE_COLOR_ZERO;
#endif
    jelly->color_invalid = true;
  }
}

void c_update_color(struct Jelly *jelly)
{
  // calculate the color based on proximities
  c_determine_color(jelly);

  // actually output the color
  if (jelly->color_invalid) {
#ifdef SIMULATED
    // update the color in the render buffer
    pthread_mutex_lock(&(jelly->render_color_mutex));
    jelly->render_color->red = jelly->color->red;
    jelly->render_color->green = jelly->color->green;
    jelly->render_color->blue = jelly->color->blue;
    pthread_mutex_unlock(&(jelly->render_color_mutex));
#else
    // push the color to the neopixels
#endif
    jelly->color_invalid = false;
  }
}
