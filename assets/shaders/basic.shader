#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_textureCoordinates;

uniform mat4 uMVP; // Model view projection

void main() { 
  gl_Position = uMVP * position; 
  v_textureCoordinates = textureCoordinates;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_textureCoordinates;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uUsingTexture;

void main() { 
  vec4 textureColor = texture(uTexture, v_textureCoordinates);
  if (uUsingTexture) {
    color = textureColor;
  } else {
    color = uColor;
  }
}

// vim: set ft=glsl:
