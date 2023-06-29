# Zenit

## Trailer

<a href="http://www.youtube.com/watch?feature=player_embedded&v=XyL9dlqSCmY"
target="_blank"><img src="http://img.youtube.com/vi/XyL9dlqSCmY/0.jpg" 
alt="Zenit Trailer" width="1040" height="660" border="10"/></a>


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

## Custom 3D Model and HDRI loading
In order to texture your own models, all that is required is to drag and drop them in the file explorer, under the folder "Models" or "Skybox", both located in "Assets".
Once this is done, go back to the application and in the Main Menu Bar, under 3D Model, or Skybox, click "Refresh", and you are ready to go!

Note: the 3D model format MUST be FBX.

## Dependencies

The dependencies Zenit uses are:
  - [Premake](https://premake.github.io/)
  - [GLFW](https://www.glfw.org/)
  - [Glad](https://glad.dav1d.de/)
  - [glm](https://github.com/g-truc/glm)
  - [ImGui](https://github.com/ocornut/imgui)
  - [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
  - [Assimp](https://github.com/assimp/assimp)
  - [spdlog](https://github.com/gabime/spdlog)
  - [stb_image](https://github.com/nothings/stb)
  - [parson](https://github.com/kgabis/parson)


## Reminder

If you want to keep updated check the [github page](https://github.com/MagiX7/Zenit).


