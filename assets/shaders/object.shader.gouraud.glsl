#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normals;

out vec3 vLightColor; // Result Gouraud color

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uCameraPosition;

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

uniform Material uMaterial;
uniform Light uLight;
uniform bool uUseLighting;

void main() {
  gl_Position = uProjection * uView * uModel * position;

  vec3 position = vec3(uModel * position);
  vec3 normal = mat3(transpose(inverse(uModel))) * normals;

  // Check if uLightPosition was set
  if (uUseLighting) {
    // ==== Ambient Light ====
    vec3 ambient = uLight.ambient * uMaterial.ambient;

    // ==== Diffuse Light ====
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(uLight.position - position);
    
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

    // ==== Specular Light ====
    vec3 viewDirection = normalize(uCameraPosition - position);
    vec3 reflectDirection = reflect(-lightDirection, norm);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
    vec3 specular = uLight.specular * (spec * uMaterial.specular);

    // ==== Attenuation ====
    float distance = length(uLight.position - position);
    float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular);
    vLightColor = result;
  } else {
    vLightColor = vec3(1.0);
  }
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 vLightColor;

uniform vec3 uObjectColor;

void main() {
  color = vec4(vLightColor * uObjectColor, 1.0);
}
