# Zenit

![](https://github.com/MagiX7/Zenit/blob/main/Logo/Zenit%20Logo.png)

## Introduction

Zenit is a material editor node-based in OpenGL and C++ for the final thesis at UPC-CITM.
This tool is centered in the PBR metallic workflow, allowing to create each different textures that take part in this technique.

Zenit also supports custom FBX model loading.

The very core engine was made following [TheCherno Game Engine Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
to explore new and different workarounds from what I learnt during the course.

## Files generation

All what is needed to get the files is:
  - Cloning the repository recursively: ```git clone --recursive https://github.com/MagiX7/Zenit```
  - Running the file named GenProjects.bat

## Goals

One of the two main goals of this project is to understand the basis of procedural generation aswell as everything envolved when creating materials alongside a strong learning of OpenGL and its shading system.
The other one is to create a useful tool that can be used to generate textures and exporting them, allowing a posterior usage in other developing environments.

Besides that, another goal to achieve is to keep the tool as simple as possible, allowing the user to have a better experience when using it.


## Controls

### General
- Left Control + S to save.

### Nodes Panel
- Right click to open the creation popup.
- Left click to move nodes, multiple selection and create links.
- For moving, press the mouse wheel button and move the mouse.
- Mouse wheel to zoom in and out.
- Double left click a node name to change its name.
- Left click outside of it to exit name editing mode.
- Delete key to delete nodes or links.

### 3D Preview
- Left click to rotate the model.
- Mouse wheel to zoom in or out.


## Dependencies

The dependencies Zenit uses are:
  - [Premake](https://premake.github.io/)
  - [GLFW](https://www.glfw.org/)
  - [Glad](https://glad.dav1d.de/)
  - [glm](https://github.com/g-truc/glm)
  - [ImGui](https://github.com/ocornut/imgui)
  - [Assimp](https://github.com/assimp/assimp)
  - [spdlog](https://github.com/gabime/spdlog)
  - [stb_image](https://github.com/nothings/stb)


## Reminder

If you want to keep updated check the [github page](https://github.com/MagiX7/Zenit).


