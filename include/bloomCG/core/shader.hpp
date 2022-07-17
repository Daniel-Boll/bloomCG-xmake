#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {

  struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
  };

  class Shader {
  private:
    std::string m_filepath;
    uint32_t m_rendererID;
    std::unordered_map<std::string, int32_t> m_uniformLocationCache;

  public:
    Shader(const std::string &filename);
    ~Shader();

    Shader *bind();
    void unbind() const;

    // Set uniforms
    Shader *setUniformMat4f(const std::string &name, const glm::mat4 &matrix);
    Shader *setUniform1f(const std::string &name, const float &value);
    Shader *setUniform3f(const std::string &name, const glm::vec3 &value);
    Shader *setUniform4f(const std::string &name, const glm::vec4 &value);
    Shader *setUniform1i(const std::string &name, int value);

  private:
    [[nodiscard]] uint32_t compileShader(GLenum type, const std::string &source);
    [[nodiscard]] uint32_t createShader(const std::string &vertexShader,
                                        const std::string &fragmentShader);
    [[nodiscard]] ShaderProgramSource parseShader(const std::string &filepath);

    [[nodiscard]] int32_t getUniformLocation(const std::string &name);
  };
}  // namespace bloom
