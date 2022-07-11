#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normals;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  gl_Position = uProjection * uView * uModel * position;
  /* gl_Position = uView * uModel * position; */
  v_position = vec3(uModel * position);
  // TODO: probably extract this to the CPU and give as uNormalMatrix
  v_normal = mat3(transpose(inverse(uModel))) * normals;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_position;
in vec3 v_normal;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform vec3 uLightPosition;
uniform vec3 uCameraPosition;
uniform Material uMaterial;
uniform Light uLight;

// TODO: Reintroduce textures later
void main() { 
  // ==== Ambient Light ====
  vec3 ambient = uLight.ambient * uMaterial.ambient;

  // ==== Diffuse Light ====
  vec3 norm = normalize(v_normal);
  vec3 lightDirection = normalize(uLightPosition - v_position);
  
  float diff = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

  // ==== Specular Light ====
  vec3 viewDirection = normalize(uCameraPosition - v_position);
  vec3 reflectDirection = reflect(-lightDirection, norm);

  float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
  vec3 specular = uLight.specular * (spec * uMaterial.specular);

  // ==== Attenuation ====
  float distance = length(uLight.position - v_position);
  float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * (distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  vec3 result = (ambient + diffuse + specular);
  color = vec4(result, 1.0);
}
