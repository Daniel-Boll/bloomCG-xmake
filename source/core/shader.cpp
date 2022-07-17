#include <bloomCG/core/shader.hpp>

#include "bloomCG/core/core.hpp"

namespace bloom {

  Shader::Shader(const std::string& filename) : m_filepath(filename), m_rendererID(0) {
    ShaderProgramSource source = parseShader(filename);
    m_rendererID = createShader(source.vertexSource, source.fragmentSource);
  }

  Shader::~Shader() { GLCall(glad_glDeleteProgram((m_rendererID))); }

  Shader* Shader::bind() {
    GLCall(glad_glUseProgram(m_rendererID));
    return this;
  }

  void Shader::unbind() const { GLCall(glad_glUseProgram(0)); }

  Shader* Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    GLCall(glad_glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)));
    return this;
  }

  Shader* Shader::setUniform1i(const std::string& name, int value) {
    GLCall(glad_glUniform1i(getUniformLocation(name), value));
    return this;
  }

  Shader* Shader::setUniform1f(const std::string& name, const float& value) {
    GLCall(glad_glUniform1f(getUniformLocation(name), value));
    return this;
  }

  Shader* Shader::setUniform3f(const std::string& name, const glm::vec3& value) {
    GLCall(glad_glUniform3f(getUniformLocation(name), value.x, value.y, value.z));
    return this;
  }

  Shader* Shader::setUniform4f(const std::string& name, const glm::vec4& value) {
    GLCall(glad_glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w));
    return this;
  }

  int32_t Shader::getUniformLocation(const std::string& name) {
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
      return m_uniformLocationCache[name];

    GLCall(int32_t location = glad_glGetUniformLocation(m_rendererID, name.c_str()));

    if (location == -1) {
      fmt::print("Warning: uniform '{}' doesn't exist!\n", name);
      // ASSERT(false);
    }

    m_uniformLocationCache[name] = location;
    return location;
  }

  [[nodiscard]] uint32_t Shader::compileShader(GLenum type, const std::string& source) {
    uint32_t shader = glad_glCreateShader(type);

    const char* sourceCStr = source.c_str();
    glad_glShaderSource(shader, 1, &sourceCStr,
                        nullptr);  // Replace the shader source code with the one we just created
    glad_glCompileShader(shader);  // Compile it

    int32_t success;
    glad_glGetShaderiv(shader, GL_COMPILE_STATUS,
                       &success);  // Check if the compilation was successful
    if (success != GL_TRUE) {      // If not, print the error
      int32_t length;
      glad_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glad_glGetShaderInfoLog(shader, length, &length, message);
      fmt::print("Failed to compile {} shader!\n{}\n",
                 type == GL_VERTEX_SHADER ? "vertex" : "fragment", message);
      glad_glDeleteShader(shader);
      return 0;
    }

    return shader;
  }

  uint32_t Shader::createShader(const std::string& vertexShader,
                                const std::string& fragmentShader) {
    uint32_t program = glad_glCreateProgram();
    uint32_t vertexCompiledShader = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragmentCompiledShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glad_glAttachShader(program, vertexCompiledShader);
    glad_glAttachShader(program, fragmentCompiledShader);
    glad_glLinkProgram(program);
    glad_glValidateProgram(program);

    int32_t linkStatus;
    int32_t validateStatus;
    glad_glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    glad_glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);

    if (linkStatus == GL_FALSE | validateStatus == GL_FALSE) {
      int32_t length;
      glad_glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glad_glGetShaderInfoLog(program, length, &length, message);
      fmt::print("Failed to compile {} shader!\n{}\n",
                 linkStatus == GL_FALSE ? "link (or both)" : "validate (or both)", message);

      glad_glDeleteProgram(program);
    }

    glad_glDeleteShader(vertexCompiledShader);
    glad_glDeleteShader(fragmentCompiledShader);

    return program;
  }

  [[nodiscard]] ShaderProgramSource Shader::parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    if (stream.fail()) {
      fmt::print("Failed to open file: {}\n", filepath);
      std::terminate();
    }

    std::string line;
    std::stringstream ss[2];
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
      if (line.find("#shader") != std::string::npos) {
        if (line.find("vertex") != std::string::npos) {
          type = ShaderType::VERTEX;
        } else if (line.find("fragment") != std::string::npos) {
          type = ShaderType::FRAGMENT;
        }
      } else {
        ss[(int)type] << line << '\n';
      }
    }

    return {ss[0].str(), ss[1].str()};
  }

}  // namespace bloom
