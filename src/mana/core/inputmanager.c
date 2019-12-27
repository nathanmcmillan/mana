#include "mana/core/inputmanager.h"

void input_manager_init(struct InputManager* input_manager) {
  for (int loop_num = 0; loop_num < KEY_LIMIT; loop_num++) {
    input_manager->keys[loop_num].state = RELEASED;
    input_manager->keys[loop_num].held = false;
  }

  input_manager->last_mouse_x_pos = 0.0;
  input_manager->last_mouse_y_pos = 0.0;
}

void input_manager_process_input(struct InputManager* input_manager, struct Window* window) {
  for (int loop_num = 0; loop_num < KEY_LIMIT; loop_num++) {
    if (glfwGetKey(window->renderer.vulkan_renderer.glfw_window, loop_num) == GLFW_PRESS) {
      input_manager->keys[loop_num].state = PRESSED;
      input_manager->keys[loop_num].held = true;
    } else if (glfwGetKey(window->renderer.vulkan_renderer.glfw_window, loop_num) == GLFW_RELEASE) {
      input_manager->keys[loop_num].state = RELEASED;
      input_manager->keys[loop_num].held = false;
    }
  }
}