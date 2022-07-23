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

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform Material uMaterial;
uniform bool uUseLighting;

uniform PointLight uPointLights[8];

uniform int uPointLightCount;

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection) {
  // ==== Ambient Light ====
  vec3 ambient = light.ambient * uMaterial.ambient;

  vec3 lightDirection = normalize(light.position - fragmentPosition);
  
  float diff = max(dot(normal, lightDirection), 0.0);
  vec3 diffuse = light.diffuse * (diff * uMaterial.diffuse);

  // ==== Specular Light ====
  vec3 reflectDirection = reflect(-lightDirection, normal);

  float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
  vec3 specular = light.specular * (spec * uMaterial.specular);

  // ==== Attenuation ====
  float distance = length(light.position - fragmentPosition);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

void main() {
  gl_Position = uProjection * uView * uModel * position;

  vec3 position = vec3(uModel * position);
  vec3 normal = normalize(mat3(transpose(inverse(uModel))) * normals);

  // Check if uLightPosition was set
  if (uUseLighting) {

    vec3 viewDirection = normalize(uCameraPosition - position);
    vec3 result = vec3(0.0);

    // Phase 1. Calculate the directional light (only ambient and diffuse)

    // Phase 2. Calculate the point lights (diffuse and specular) (8 max) (remove ambient after)
    for (int i = 0; i < uPointLightCount; i++) {
      result += calculatePointLight(uPointLights[i], normal, position, viewDirection);
    }

    // Phase 3. Spot lights

    vLightColor = vec3(result);
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
