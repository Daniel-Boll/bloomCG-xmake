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

<details>
  <summary>
    Initial scene
  </summary>
  <p align="center">
    <br />
    <img src="https://user-images.githubusercontent.com/43689101/180625023-f5761172-dd05-4b33-95fd-194b90b070f2.png" width=1200 />
  </p>
</details>

<details>
  <summary>
    Hierarchy
  </summary>
  
  <p>This is where the entities are listed and created.</p>
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625080-94e50f32-a03b-44b5-b5eb-b495b643d5ca.png" width=150 />
  </p>
  
  <br />
  <p>When you right click on the hierarchy it opens this pane where you can create entities and control the scene</p>
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625148-a32a5dd0-29ea-444c-88c0-5780b2cfecf6.png" width=300 />
  </p>
  
  <br />
  <p>Creating entities opens a modal in the center of the screen with the inputs for that specific entity</p>
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625183-e122a622-16bf-42dc-b9e1-8a375d4b2af1.png" width=300 />
  </p>
</details>

<details>
  <summary>
    Inspector
  </summary>
  
  <br />
  <p>When you click in a element from the hierarchy it will display into the inspector it's information</p>
  
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625221-65c29d7b-9aa6-4ff8-abe6-58a6358b49d8.png" width=300 />
  </p>
  
  <br />
  <p>Here are some of the inspectors for different objects</p>
  
  | Cube                   |  Sphere | Point Light | Ambient Light |
  :-------------------------:|:-------------------------:|:----:|:-----:
  | ![image](https://user-images.githubusercontent.com/43689101/180625225-42e569b8-5491-4559-aa7d-90c234c50d5f.png)|![image](https://user-images.githubusercontent.com/43689101/180625228-3f1997e8-f6fe-4dfd-a0dd-1335fd05c3a2.png)|![image](https://user-images.githubusercontent.com/43689101/180625236-8f51d7a3-0072-4d09-abbc-ec6b653c6360.png)|![image](https://user-images.githubusercontent.com/43689101/180625329-c11d0ba1-4548-4a52-ae15-e41703b2545a.png) |
  | This cube is the floor entity in the hierarchy, you can see in the transform section that he is indeed scaled in X and Z to a large amount to give the impression of a huge floor | This is the main sphere of the scene, located at (0, 0, 0), shaded through **phong**, it's material given the red color and the amount of sectors and stacks needed to render it that way | This is one of the point lights of the scene you can control it's intensity and the coeficients of the attenuation formula | This is the ambient light, which you cannot create a new one as it works as a global source of light that affects every object in all directions, because of that it doesn't has a position |
</details>

<details>
  <summary>
    Viewport
  </summary>  
  
  <br />
  <p>The viewport is the main area of render, this is were OpenGL draw frames</p>
  
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625472-031d819b-f6ae-4d04-8efd-1cdefc33eadc.png" width=1200 />
  </p>
  
  <p>In this scene the red, green, yellow and blue objects are all spheres with different values for stacks and sectors, shading model, radius, position, etc. There are also two cubes, one bellow white as the floor, and the other purple rotated and scaled.</p>

</details>

<details>
  <summary>
    Gizmos
  </summary>  
  
  <br />
  <p>You can control elements through the gizmos changing their position. You can also notice the scene orientation through the mapped cube in the top right.</p>
  
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625549-26a18747-95da-46df-83b9-a31c26a65c03.png" width=500 />
  </p>

</details>


<details>
  <summary>
    Controllers
  </summary>  
  
  <br />
  <p>You can also control whether to show or not the movement gizmo, scale the scene, clear the elements all through this controller section in the top of the engine.</p>
  
  <p align="center">
    <img src="https://user-images.githubusercontent.com/43689101/180625571-941fa314-c290-409b-bb50-da12f01c6f84.png" width=300 />
  </p>
  
</details>

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
