#pragma once
#ifndef MESH_H
#define MESH_H

#include "mana/core/memoryallocator.h"
//
#include <cstorage/cstorage.h>

#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/utilities/texture.h"

struct Vertex {
  vec3 position;
  vec3 normal;
  vec2 tex_coord;
  vec3 tangent;
  vec3 bit_tangent;
};

struct Mesh {
  struct Vector* vertices;
  struct Vector* indices;
  struct Vector* textures;
};

void mesh_init(struct Mesh* mesh);
void mesh_delete(struct Mesh* mesh);
void mesh_clear(struct Mesh* mesh);
void mesh_assign_vertex(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v);
void mesh_assign_vertex_full(struct Vector* vector, float x, float y, float z, float r, float g, float b, float u, float v, float tan_x, float tan_y, float tan_z, float bit_x, float bit_y, float bit_z);
void mesh_assign_indice(struct Vector* vector, uint32_t indice);
VkVertexInputBindingDescription mesh_get_binding_description();
void mesh_get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);

#endif  // MESH_H
