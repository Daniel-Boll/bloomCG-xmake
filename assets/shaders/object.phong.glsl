#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normals;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uW2V;

void main() {
  gl_Position = uW2V * uProjection * uView * uModel * position;
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

struct PointLight {
  vec3 position;

  vec3 intensity;

  float constant;
  float linear;
  float quadratic;
};

struct AmbientLight {
  vec3 intensity;
};

uniform vec3 uCameraPosition;
uniform Material uMaterial;
uniform bool uUseLighting;

#define MAX_POINT_LIGHTS 8
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform AmbientLight uAmbientLight;

uniform int uPointLightCount;

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 viewDirection) {
  // ==== Diffuse Light ====
  vec3 lightDirection = normalize(light.position - fragmentPosition);
  
  float diff = max(dot(normal, lightDirection), 0.0);
  vec3 diffuse = light.intensity * (diff * uMaterial.diffuse);

  // ==== Specular Light ====
  vec3 reflectDirection = reflect(-lightDirection, normal);

  float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
  vec3 specular = light.intensity * (spec * uMaterial.specular);

  // ==== Attenuation ====
  float distance = length(light.position - v_position);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  diffuse *= attenuation;
  specular *= attenuation;

  return (diffuse + specular);
}

void main() { 
  if (uUseLighting) {

    vec3 normal = normalize(v_normal);
    vec3 viewDirection = normalize(uCameraPosition - v_position);
    vec3 result = vec3(0.0);

    // Phase 1. Calculate the ambient light (only ambient)
    result += uAmbientLight.intensity * uMaterial.ambient;

    // Phase 2. Calculate the point lights (diffuse and specular) (8 max)
    for (int i = 0; i < uPointLightCount; i++) {
      result += calculatePointLight(uPointLights[i], normal, v_position, viewDirection);
    }

    // Phase 3. Spot lights

    color = vec4(result, 1.0);
  } else {
    color = vec4(0.0);
  }
}
