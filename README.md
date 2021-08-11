# LDtk-D11
A rendering engine for [Level Designer Toolkit](https://github.com/deepnight/ldtk) files using DirectX 11 through [DirectX Tool Kit](https://github.com/microsoft/DirectXTK). [LDtkLoader](https://github.com/Madour/LDtkLoader) is used to load LDtk files.

Pass as a command line argument a local LDTK file to render and all layers will be rendered using referenced icon or sprite textures. Entities with patrols will move along their patrol routes.



https://user-images.githubusercontent.com/242570/129111903-88727f39-9e21-45cb-bc76-a5d7975a4a9c.mp4



Use CMake to build, current dependencies are:
* directxtk (https://github.com/microsoft/DirectXTK). Available on [VCPKG](vcpkg.io).
* directxmath. Availble on [VCPKG](vcpkg.io).
* LDtkLoader (https://github.com/Madour/LDtkLoader).
