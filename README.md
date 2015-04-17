# xgine
XGine is multi-purpose 3d graphics engine, written in C++ from ground up by me. It is based on DirectX 9.0 and has a plugin architecture, so it is possible to replace one component with another (of course, the same interface is a must). I decided to implement a seperate library (dll) called Kernel, which offers some low-level tasks to be done, e.g:  logging, loading files (in background) from disk or cache file (archive) and testing stuff like memory manager, profiler, timer.

Engine itself is a result of over six years of work (approx. 20 000 lines of engine code + 6 000 lines of gpu shaders). During that time I have changed the critical architecture elements several times to find the most suitable and efficient design. Now it is built upon a dynamic scene managment system, material system with support of multishaders and optimized deferred renderer with hdr post-process effects, such as: dynamic tone mapping + bright pass, dynamic depth of field (with eye adaptaion) and well-known volumetric light shafts done in screen space.

I have also implemented specialized entities: hdr sky-dome with sun & clouds, water (updating heights/normal maps on gpu), height-mapped terrain and vegetation. Of course every entity can use level of detail to improve performance.

*** Please note that this repository is not up to date and does not contain all necessary resources to compile and execute engine ***

## Detailed engine features

- Written in C++/WinAPI/DirectX
- Kernel
 - Logger
 - Virtual File System ( file buffering, background loading, packing & compression )
 - Cache system
 - Console ( simple script system, config file loading )
 - Memory Manager ( statistics, finding memory leaks )
 - Timer
 - Profiler
 - Tokenizer ( parsing utility )
- Audio module ( Loading, playing sounds & music, 3d system )
- Input system
- Plug-in system ( Ease of replacing components )
- ResourcesManager ( Resource manipulation, background loading on demand )
- Renderer 2D
 - GUI ( Label, Window, WindowPopup, Button, TextBox, ScrollBar, CheckBox, ~Console Input/Output )
 - 2D shapes rendering, font
- Renderer 3D
 - Camera Module
 - Entities ( Particles ( + Soft Particles ), Billboards, SkyDome, Terrain (Height-Map), Vegetation (Trees, grass, ...), Simple Water, High Quality Water, RenderableModel )
 - Scene Management ( LOD, Culling ( Frustum, Occlusion ), Octree, Quadtree, Scene-Nodes )
 - RenderList ( Material-based sorting, instancing )
 - Materials ( Multishader based ) :
  - Lighs ( Directional, Point, Spot )
  - ColorMap, NormalMap ( Normal Mapping ), AlbedoMap, CubeMap
  - Normal Mapping, Parallax Mapping, Specular
  - Relief Texture Mapping, Parallax Occlusion Mapping
  - Terrain ( Texture-Splatting )
  - Vegetation ( Animated )
 - Shaders Management
 - RenderTarget Utility ( MRT, RenderTexture, RenderCubeTexture, + BoxBlur/GuassianBlur Filter )
 - Shadow Mapping ( Plain-fast, PCF3x3, PCF5x5, PCF7x7, + randomly jittered )
 - Deferred Shading
- Postprocess Effects:
 - Atmosphere, Fog
 - Bloom, Sepia
 - Depth Of Field + Dynamic Eye Adaptaion
 - HDR Rendering + Bright Pass + Tone Mapping
 - Heat & Haze
 - Volumetric Light Scattering
