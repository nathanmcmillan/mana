#include "mana/graphics/utilities/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int texture_init(struct Texture *texture, struct VulkanRenderer *vulkan_renderer, char *path) {
  // Note: Extra 0 needed to ensure end of string
  int path_length = strlen(path);
  texture->path = malloc(path_length + 1);
  memset(texture->path, 0, path_length);
  strcpy(texture->path, path);

  // Todo: Extract filetype and detect pixel bit
  int type_length = 4;
  texture->type = malloc(type_length + 1);
  memset(texture->type, 0, type_length + 1);
  strcpy(texture->type, ".png");

  // Note: Something like this could be useful for optimizing but not needed as stbi will correctly convert up/down bits
  //int pixel_bit = 16;
  //int tex_width, tex_height, tex_channels;
  //void *pixels;
  //VkDeviceSize image_size;
  //if (pixel_bit == 8) {
  //  pixels = (void *)stbi_load(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4;
  //} else if (pixel_bit == 16) {
  //  pixels = (void *)stbi_load_16(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4 * 2;
  //} else if (pixel_bit == 32) {
  //  pixels = (void *)stbi_load_32(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  //  image_size = tex_width * tex_height * 4 * 2 * 2;
  //} else
  //  return -1;

  int tex_width, tex_height, tex_channels;
  stbi_us *pixels = stbi_load_16(texture->path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  VkDeviceSize image_size = tex_width * tex_height * 4 * 2;

  if (!pixels) {
    printf("failed to load texture image!\n");
    return -1;
  }

  VkBuffer staging_buffer = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);

  void *data;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, image_size);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  stbi_image_free(pixels);

  uint32_t mip_levels = (uint32_t)(floor(log2(MAX(tex_width, tex_height))));

  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, tex_width, tex_height, mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->texture_image, &texture->texture_image_memory);

  graphics_utils_transition_image_layout(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);
  graphics_utils_copy_buffer_to_image(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, &staging_buffer, &texture->texture_image, tex_width, tex_height);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  graphics_utils_generate_mipmaps(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, tex_width, tex_height, mip_levels);

  graphics_utils_create_image_view(vulkan_renderer->device, texture->texture_image, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mip_levels, &texture->texture_image_view);
  graphics_utils_create_sampler(vulkan_renderer->device, &texture->texture_sampler, mip_levels);

  return 0;
}

void texture_delete(struct VulkanRenderer *vulkan_renderer, struct Texture *texture) {
  vkDestroySampler(vulkan_renderer->device, texture->texture_sampler, NULL);
  vkDestroyImageView(vulkan_renderer->device, texture->texture_image_view, NULL);

  vkDestroyImage(vulkan_renderer->device, texture->texture_image, NULL);
  vkFreeMemory(vulkan_renderer->device, texture->texture_image_memory, NULL);

  free(texture->path);
  free(texture->type);
}
