#pragma once

#include <bloomCG/core/common.hpp>
#include <bloomCG/core/shader.hpp>
#include <functional>

enum class ShaderType { Object = 0, Light = 1 };
enum class LightModel { Flat = 2, Gouraud = 3, Phong = 4 };

namespace bloom {
  enum class ShaderType { Object = 0, Light = 1 };
  enum class LightModel { Flat = 2, Gouraud = 3, Phong = 4 };

  typedef std::pair<ShaderType, LightModel> Name;

  struct hash_name {
    size_t operator()(const Name& name) const {
      return std::hash<ShaderType>()(name.first) ^ std::hash<LightModel>()(name.second);
    }
  };

  std::unordered_map<Name, bloom::Shader*, hash_name> shaders;
  struct ShaderMap {
    ShaderType type;
    LightModel model;

    template <ShaderType Type, LightModel Model = LightModel::Phong> bloom::Shader* get() {
      return shaders[Name(Type, Model)];
    }

    template <ShaderType Type, LightModel Model = LightModel::Phong>
    ShaderMap* registerShader(const std::string& path) {
      shaders[Name(Type, Model)] = new bloom::Shader(path);
      return this;
    }
  };

}  // namespace bloom
