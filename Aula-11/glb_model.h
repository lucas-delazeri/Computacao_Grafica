// Copyright
// Computação Gráfica
// URI Santiago
// Professor Laurence

#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include <GL/glew.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "stb_image.h"

struct GlbTexture {
  GLuint id = 0;
  int width = 0;
  int height = 0;
};

struct GlbPosedMesh {
  std::vector<aiVector3D> vertices;
  std::vector<aiVector3D> normals;
};

struct GlbModel {
  std::unique_ptr<Assimp::Importer> importer;
  const aiScene* scene = nullptr;
  std::unordered_map<unsigned int, GlbTexture> textures;
  std::unordered_map<unsigned int, GlbPosedMesh> posed_meshes;
  std::unordered_map<unsigned int, aiColor4D> material_colors;
  std::unordered_set<unsigned int> hidden_meshes;
};

static inline bool load_texture_index_from_material(unsigned int& texture_index,
                                                    const aiMaterial* material) {
  if (!material) {
    return false;
  }

  aiString texture_path;
  if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
      material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) != AI_SUCCESS) {
    return false;
  }

  const std::string texture_path_string = texture_path.C_Str();
  if (!texture_path_string.empty() && texture_path_string[0] == '*') {
    texture_index = std::strtoul(texture_path_string.c_str() + 1, nullptr, 10);
    return true;
  }

  return false;
}

static inline GlbTexture load_texture(const uint8_t* texture_data, GLenum internal_format,
                                      int texture_width, int texture_height, GLenum input_format) {
  GLint previous_texture_bind, previous_unpack_alignment;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous_texture_bind);
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &previous_unpack_alignment);

  GlbTexture texture;
  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);
  texture.width = texture_width;
  texture.height = texture_height;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture_width, texture_height, 0, input_format,
               GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glPixelStorei(GL_UNPACK_ALIGNMENT, previous_unpack_alignment);
  glBindTexture(GL_TEXTURE_2D, previous_texture_bind);
  return texture;
}

static inline bool load_texture_embedded(GlbTexture& glb_texture, const aiTexture* assimp_texture) {
  if (!assimp_texture) {
    return false;
  }

  const uint8_t* assimp_texture_data = reinterpret_cast<const uint8_t*>(assimp_texture->pcData);

  if (assimp_texture->mHeight == 0) {
    int texture_length = assimp_texture->mWidth;
    int texture_width;
    int texture_height;
    uint8_t* glb_texture_data = stbi_load_from_memory(assimp_texture_data, texture_length,
                                                      &texture_width, &texture_height, nullptr, 4);
    if (!glb_texture_data) {
      return false;
    }

    const GLenum internal_format = GL_RGBA;
    const GLenum input_format = GL_RGBA;
    glb_texture = load_texture(glb_texture_data, internal_format, texture_width, texture_height,
                               input_format);
    stbi_image_free(glb_texture_data);
    return true;
  }

  const GLenum internal_format = GL_RGBA;
  const int texture_width = assimp_texture->mWidth;
  const int texture_height = assimp_texture->mHeight;
  const GLenum input_format = GL_BGRA;
  glb_texture = load_texture(assimp_texture_data, internal_format, texture_width, texture_height,
                             input_format);
  return true;
}

static inline bool load_texture_file(GlbTexture& texture, const std::filesystem::path& filepath) {
  int texture_width, texture_height;
  uint8_t* texture_data = stbi_load(filepath.string().c_str(), &texture_width, &texture_height,
                                    nullptr, 4);
  if (!texture_data) {
    std::cerr << "Erro ao carregar a textura do modelo: " << filepath << " ("
              << stbi_failure_reason() << ")" << std::endl;
    return false;
  }

  texture = load_texture(texture_data, GL_RGBA, texture_width, texture_height, GL_RGBA);
  stbi_image_free(texture_data);
  return true;
}

static inline bool load_model(
    GlbModel& model, const std::string& filepath,
    const std::unordered_map<std::string, std::string>& material_texture_overrides = {}) {
  model.importer = std::make_unique<Assimp::Importer>();
  model.scene = model.importer->ReadFile(
      filepath.c_str(), aiProcess_Triangulate | aiProcess_GenNormals |
                            aiProcess_ImproveCacheLocality | aiProcess_JoinIdenticalVertices |
                            aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
  if (!model.scene) {
    std::cerr << "Erro ao carregar modelo: " << filepath << std::endl;
    return false;
  }

  model.textures.clear();
  model.material_colors.clear();

  for (unsigned int material_index = 0; material_index < model.scene->mNumMaterials;
       material_index++) {
    const aiMaterial* material = model.scene->mMaterials[material_index];
    if (!material) {
      continue;
    }

    aiString material_name;
    material->Get(AI_MATKEY_NAME, material_name);
    const auto texture_override = material_texture_overrides.find(material_name.C_Str());
    std::string texture_path_string;
    if (texture_override != material_texture_overrides.end()) {
      texture_path_string = texture_override->second;
    } else {
      if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
        continue;
      }
      aiString texture_path;
      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) != AI_SUCCESS) {
        continue;
      }
      texture_path_string = texture_path.C_Str();
    }

    GlbTexture texture;
    bool texture_loaded = false;
    if (!texture_path_string.empty() && texture_path_string[0] == '*') {
      const unsigned long texture_index =
          std::strtoul(texture_path_string.c_str() + 1, nullptr, 10);
      if (texture_index < model.scene->mNumTextures) {
        texture_loaded =
            load_texture_embedded(texture, model.scene->mTextures[texture_index]);
      }
    } else {
      const std::filesystem::path model_directory = std::filesystem::path(filepath).parent_path();
      std::filesystem::path texture_filepath = model_directory / texture_path_string;
      if (!std::filesystem::exists(texture_filepath)) {
        texture_filepath = model_directory / std::filesystem::path(texture_path_string).filename();
      }
      texture_loaded = load_texture_file(texture, texture_filepath);
    }

    if (texture_loaded) {
      model.textures[material_index] = texture;
    }
  }

  return true;
}

static inline bool set_model_material_color(GlbModel& model, const std::string& material_name,
                                            const aiColor4D& color) {
  if (!model.scene) {
    return false;
  }

  for (unsigned int material_index = 0; material_index < model.scene->mNumMaterials;
       material_index++) {
    aiString current_material_name;
    if (model.scene->mMaterials[material_index]->Get(AI_MATKEY_NAME, current_material_name) ==
            AI_SUCCESS &&
        material_name == current_material_name.C_Str()) {
      model.material_colors[material_index] = color;
      return true;
    }
  }

  std::cerr << "Material nao encontrado no modelo: " << material_name << std::endl;
  return false;
}

static inline bool hide_model_mesh(GlbModel& model, const std::string& mesh_name) {
  if (!model.scene) {
    std::cerr << "Nao foi possivel ocultar a malha sem um modelo carregado." << std::endl;
    return false;
  }

  for (unsigned int mesh_index = 0; mesh_index < model.scene->mNumMeshes; mesh_index++) {
    if (mesh_name == model.scene->mMeshes[mesh_index]->mName.C_Str()) {
      model.hidden_meshes.insert(mesh_index);
      return true;
    }
  }

  std::cerr << "Malha nao encontrada no modelo: " << mesh_name << std::endl;
  return false;
}

static inline const aiNodeAnim* find_animation_channel(const aiAnimation* animation,
                                                       const aiString& node_name) {
  for (unsigned int channel_index = 0; channel_index < animation->mNumChannels;
       channel_index++) {
    const aiNodeAnim* channel = animation->mChannels[channel_index];
    if (channel->mNodeName == node_name) {
      return channel;
    }
  }
  return nullptr;
}

static inline aiVector3D sample_animation_vector(const aiVectorKey* keys, unsigned int key_count,
                                                double time, const aiVector3D& default_value) {
  if (key_count == 0) {
    return default_value;
  }
  if (time <= keys[0].mTime) {
    return keys[0].mValue;
  }
  if (time >= keys[key_count - 1].mTime) {
    return keys[key_count - 1].mValue;
  }

  for (unsigned int key_index = 0; key_index + 1 < key_count; key_index++) {
    if (time <= keys[key_index + 1].mTime) {
      const double interval = keys[key_index + 1].mTime - keys[key_index].mTime;
      const ai_real factor = static_cast<ai_real>((time - keys[key_index].mTime) / interval);
      return keys[key_index].mValue * (1.0f - factor) + keys[key_index + 1].mValue * factor;
    }
  }
  return default_value;
}

static inline aiQuaternion sample_animation_rotation(const aiQuatKey* keys,
                                                     unsigned int key_count, double time,
                                                     const aiQuaternion& default_value) {
  if (key_count == 0) {
    return default_value;
  }
  if (time <= keys[0].mTime) {
    return keys[0].mValue;
  }
  if (time >= keys[key_count - 1].mTime) {
    return keys[key_count - 1].mValue;
  }

  for (unsigned int key_index = 0; key_index + 1 < key_count; key_index++) {
    if (time <= keys[key_index + 1].mTime) {
      const double interval = keys[key_index + 1].mTime - keys[key_index].mTime;
      const ai_real factor = static_cast<ai_real>((time - keys[key_index].mTime) / interval);
      aiQuaternion rotation;
      aiQuaternion::Interpolate(rotation, keys[key_index].mValue, keys[key_index + 1].mValue,
                                factor);
      rotation.Normalize();
      return rotation;
    }
  }
  return default_value;
}

static inline void build_animation_transforms(
    const aiNode* node, const aiAnimation* animation, double time,
    const aiMatrix4x4& parent_transform, const aiMatrix4x4& bind_parent_transform,
    std::unordered_map<std::string, aiMatrix4x4>& global_transforms,
    std::unordered_map<unsigned int, aiMatrix4x4>& mesh_transforms,
    std::unordered_map<unsigned int, aiMatrix4x4>& bind_mesh_transforms) {
  aiMatrix4x4 local_transform = node->mTransformation;
  const aiNodeAnim* channel = find_animation_channel(animation, node->mName);
  if (channel) {
    aiVector3D scaling, position;
    aiQuaternion rotation;
    node->mTransformation.Decompose(scaling, rotation, position);
    scaling = sample_animation_vector(channel->mScalingKeys, channel->mNumScalingKeys, time,
                                      scaling);
    position = sample_animation_vector(channel->mPositionKeys, channel->mNumPositionKeys, time,
                                       position);
    rotation = sample_animation_rotation(channel->mRotationKeys, channel->mNumRotationKeys, time,
                                         rotation);
    local_transform = aiMatrix4x4(scaling, rotation, position);
  }

  const aiMatrix4x4 global_transform = parent_transform * local_transform;
  const aiMatrix4x4 bind_global_transform = bind_parent_transform * node->mTransformation;
  global_transforms[node->mName.C_Str()] = global_transform;
  for (unsigned int mesh_index = 0; mesh_index < node->mNumMeshes; mesh_index++) {
    mesh_transforms[node->mMeshes[mesh_index]] = global_transform;
    bind_mesh_transforms[node->mMeshes[mesh_index]] = bind_global_transform;
  }
  for (unsigned int child_index = 0; child_index < node->mNumChildren; child_index++) {
    build_animation_transforms(node->mChildren[child_index], animation, time, global_transform,
                               bind_global_transform, global_transforms, mesh_transforms,
                               bind_mesh_transforms);
  }
}

static inline bool apply_animation_pose(GlbModel& model, double time_seconds) {
  if (!model.scene || !model.scene->mRootNode || model.scene->mNumAnimations == 0) {
    std::cerr << "O modelo nao contem uma animacao aplicavel." << std::endl;
    return false;
  }

  const aiAnimation* animation = model.scene->mAnimations[0];
  const double ticks_per_second =
      animation->mTicksPerSecond > 0 ? animation->mTicksPerSecond : 25.0;
  double animation_time = time_seconds * ticks_per_second;
  if (animation->mDuration > 0) {
    animation_time = std::fmod(animation_time, animation->mDuration);
  }

  std::unordered_map<std::string, aiMatrix4x4> global_transforms;
  std::unordered_map<unsigned int, aiMatrix4x4> mesh_transforms;
  std::unordered_map<unsigned int, aiMatrix4x4> bind_mesh_transforms;
  build_animation_transforms(model.scene->mRootNode, animation, animation_time, aiMatrix4x4(),
                             aiMatrix4x4(), global_transforms, mesh_transforms,
                             bind_mesh_transforms);

  model.posed_meshes.clear();
  for (unsigned int mesh_index = 0; mesh_index < model.scene->mNumMeshes; mesh_index++) {
    const aiMesh* mesh = model.scene->mMeshes[mesh_index];
    GlbPosedMesh posed_mesh;
    posed_mesh.vertices.resize(mesh->mNumVertices);
    posed_mesh.normals.resize(mesh->mNumVertices);
    std::vector<ai_real> total_weights(mesh->mNumVertices, 0);
    aiMatrix4x4 inverse_bind_mesh_transform;
    const auto bind_mesh_transform = bind_mesh_transforms.find(mesh_index);
    if (bind_mesh_transform != bind_mesh_transforms.end()) {
      inverse_bind_mesh_transform = bind_mesh_transform->second;
      inverse_bind_mesh_transform.Inverse();
    }

    const auto mesh_transform = mesh_transforms.find(mesh_index);
    if (mesh->mNumBones == 0) {
      aiMatrix4x4 pose_transform;
      if (mesh_transform != mesh_transforms.end()) {
        pose_transform = inverse_bind_mesh_transform * mesh_transform->second;
      }
      const aiMatrix3x3 normal_transform(pose_transform);
      for (unsigned int vertex_index = 0; vertex_index < mesh->mNumVertices; vertex_index++) {
        posed_mesh.vertices[vertex_index] = pose_transform * mesh->mVertices[vertex_index];
        if (mesh->HasNormals()) {
          posed_mesh.normals[vertex_index] = normal_transform * mesh->mNormals[vertex_index];
          posed_mesh.normals[vertex_index].Normalize();
        }
      }
      model.posed_meshes[mesh_index] = std::move(posed_mesh);
      continue;
    }

    for (unsigned int bone_index = 0; bone_index < mesh->mNumBones; bone_index++) {
      const aiBone* bone = mesh->mBones[bone_index];
      const auto bone_transform = global_transforms.find(bone->mName.C_Str());
      if (bone_transform == global_transforms.end()) {
        std::cerr << "Osso sem transformacao na animacao: " << bone->mName.C_Str() << std::endl;
        continue;
      }

      const aiMatrix4x4 skin_transform =
          inverse_bind_mesh_transform * bone_transform->second * bone->mOffsetMatrix;
      const aiMatrix3x3 normal_transform(skin_transform);
      for (unsigned int weight_index = 0; weight_index < bone->mNumWeights; weight_index++) {
        const aiVertexWeight& vertex_weight = bone->mWeights[weight_index];
        const unsigned int vertex_index = vertex_weight.mVertexId;
        const ai_real weight = vertex_weight.mWeight;
        posed_mesh.vertices[vertex_index] +=
            (skin_transform * mesh->mVertices[vertex_index]) * weight;
        if (mesh->HasNormals()) {
          posed_mesh.normals[vertex_index] +=
              (normal_transform * mesh->mNormals[vertex_index]) * weight;
        }
        total_weights[vertex_index] += weight;
      }
    }

    for (unsigned int vertex_index = 0; vertex_index < mesh->mNumVertices; vertex_index++) {
      if (total_weights[vertex_index] > 0) {
        posed_mesh.vertices[vertex_index] /= total_weights[vertex_index];
        if (mesh->HasNormals()) {
          posed_mesh.normals[vertex_index] /= total_weights[vertex_index];
          posed_mesh.normals[vertex_index].Normalize();
        }
      } else {
        posed_mesh.vertices[vertex_index] = mesh->mVertices[vertex_index];
        if (mesh->HasNormals()) {
          posed_mesh.normals[vertex_index] = mesh->mNormals[vertex_index];
        }
      }
    }
    model.posed_meshes[mesh_index] = std::move(posed_mesh);
  }
  return true;
}

static inline void draw_model(const GlbModel& model, const aiNode* node) {
  if (!node) {
    return;
  }

  glPushMatrix();
  {
    aiMatrix4x4 matrix = node->mTransformation;
    matrix.Transpose();
    glMultMatrixf(&matrix.a1);

    for (unsigned int mesh_index = 0; mesh_index < node->mNumMeshes; mesh_index++) {
      const unsigned int scene_mesh_index = node->mMeshes[mesh_index];
      if (model.hidden_meshes.count(scene_mesh_index) > 0) {
        continue;
      }
      const aiMesh* mesh = model.scene->mMeshes[scene_mesh_index];
      const auto posed_mesh = model.posed_meshes.find(scene_mesh_index);
      const aiVector3D* vertices = posed_mesh == model.posed_meshes.end()
                                       ? mesh->mVertices
                                       : posed_mesh->second.vertices.data();
      const aiVector3D* normals = posed_mesh == model.posed_meshes.end()
                                      ? mesh->mNormals
                                      : posed_mesh->second.normals.data();

      glColor3ub(255, 255, 255);
      const auto iterator = model.textures.find(mesh->mMaterialIndex);
      if (iterator != model.textures.end() && iterator->second.id) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, iterator->second.id);

      } else {
        glDisable(GL_TEXTURE_2D);
        const auto color_override = model.material_colors.find(mesh->mMaterialIndex);
        if (color_override != model.material_colors.end()) {
          const aiColor4D& color = color_override->second;
          glColor4f(color.r, color.g, color.b, color.a);
        } else if (mesh->mMaterialIndex < model.scene->mNumMaterials) {
          aiColor4D diffuse_color;
          if (model.scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,
                                                                 diffuse_color) == AI_SUCCESS) {
            glColor4f(diffuse_color.r, diffuse_color.g, diffuse_color.b, diffuse_color.a);
          }
        }
      }

      glBegin(GL_TRIANGLES);
      for (unsigned int face_index = 0; face_index < mesh->mNumFaces; face_index++) {
        const aiFace& face = mesh->mFaces[face_index];
        for (unsigned int vertex_index = 0; vertex_index < face.mNumIndices; vertex_index++) {
          const unsigned current_vertex_index = face.mIndices[vertex_index];

          if (mesh->HasNormals()) {
            const aiVector3D& normal = normals[current_vertex_index];
            glNormal3f(normal.x, normal.y, normal.z);
          }

          if (mesh->HasTextureCoords(0)) {
            const aiVector3D& texture_uv = mesh->mTextureCoords[0][current_vertex_index];
            glTexCoord2f(texture_uv.x, texture_uv.y);
          }

          const aiVector3D& point = vertices[current_vertex_index];
          glVertex3f(point.x, point.y, point.z);
        }
      }
      glEnd();
    }

    for (unsigned int child_index = 0; child_index < node->mNumChildren; child_index++) {
      draw_model(model, node->mChildren[child_index]);
    }
  }
  glPopMatrix();
}

static inline void draw_model(const GlbModel& model) {
  if (!model.scene) {
    return;
  }
  if (!model.scene->mRootNode) {
    return;
  }

  draw_model(model, model.scene->mRootNode);
}

static inline void destroy_model(GlbModel& model) {
  for (auto& model_texture : model.textures) {
    if (model_texture.second.id) {
      glDeleteTextures(1, &model_texture.second.id);
      model_texture.second.id = 0;
    }
  }
  model.textures.clear();
  model.posed_meshes.clear();
  model.scene = nullptr;
  model.importer.reset();
}

#endif
