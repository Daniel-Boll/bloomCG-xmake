#shader vertex
#version 330 core

//------------------------------------------------------------------//
//                         Vertex attribute                         //
//------------------------------------------------------------------//
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normals;
layout(location = 2) in vec2 textureCoordinates;

//------------------------------------------------------------------//
//                             Uniforms                             //
//------------------------------------------------------------------//
uniform mat4 uMatrixModelView;
uniform mat4 uMatrixModelViewProjection;

//------------------------------------------------------------------//
//                             Varying                              //
//------------------------------------------------------------------//
out vec2 vTextureCoordinates;
out vec3 esVertex;

void main() {
  esVertex = vec3(uMatrixModelView * position);
  gl_Position = uMatrixModelViewProjection * position;

  vTextureCoordinates = textureCoordinates;
}

#shader fragment
#version 330 core

//------------------------------------------------------------------//
//                             Uniforms                             //
//------------------------------------------------------------------//
uniform bool uUsingTexture;
uniform vec4 uColor;

//------------------------------------------------------------------//
//                             Varying                              //
//------------------------------------------------------------------//
in vec2 vTextureCoordinates;

void main() {
  /* vec4 textureColor = texture(uTexture, vTextureCoordinates); */

  if (uUsingTexture) {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    gl_FragColor = uColor;
  }
}
