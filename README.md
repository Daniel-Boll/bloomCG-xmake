# <h1 align="center">Bloom Engine</h1>

> A graphics engine initially made for the Computer Graphics course at UNIOESTE.

## Dependencies

The code dependencies are:

- [glm](https://glm.g-truc.net/)
- [GLFW](https://www.glfw.org/)
- [Glad](https://glad.dav1d.de/)
- [ImGui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [fmt](https://fmt.dev/latest/index.html)

The code is built using [`xmake`](https://xmake.io/) and the dependencies are installed automatically through
[`xrepo`](https://github.com/xmake-io/xrepo).

## Showcase

## Features

### Objects

You can add the following objects to the scene:

- [X] Sphere
- [X] Cube
- [X] Point light
- [ ] Directional light

### Shading

> ⚠️  The current version of the Bloom engine only supports local shading. 
> Amongst the shading models available, you can choose between:

- [X] Flat shading
- [X] Gouraud shading
- [X] Phong shading
- [ ] Blinn-Phong shading

> OBS: The shading model is chosen per object.

## Enhancements

- [ ] Scale window with the buttons +1 and -1.
- [ ] Base refactor
- [ ] Global shadows
- [ ] Ray casting from the camera
  - [ ] Select objects with the mouse
- [ ] Ray tracing support?
