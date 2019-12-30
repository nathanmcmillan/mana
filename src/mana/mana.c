#include "mana/mana.h"

int mana_init(struct Mana* mana) {
  int engine_error = engine_init(&mana->engine);
  switch (engine_error) {
    default:
      break;
    case (ENGINE_GLFW_ERROR):
      printf("Error initializing GLFW!\n");
      return MANA_ENGINE_ERROR;
    case (ENGINE_VULKAN_SUPPORT_ERROR):
      printf("Vulkan support not found!\n");
      return MANA_ENGINE_ERROR;
  }

  return MANA_SUCCESS;
}

int mana_new_window(struct Mana* mana, int width, int height) {
  if (window_init(&mana->engine.window, width, height) != MANA_SUCCESS)
    return MANA_WINDOW_ERROR;

  return MANA_SUCCESS;
}

void mana_close_window(struct Mana* mana) {
  window_delete(&mana->engine.window);
}

void mana_cleanup(struct Mana* mana) {
  engine_delete(&mana->engine);
}
