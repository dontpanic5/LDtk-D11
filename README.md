# LDtk-D11
A rendering engine for [Level Designer Toolkit](https://github.com/deepnight/ldtk) files using DirectX 11 through [DirectX Tool Kit](https://github.com/microsoft/DirectXTK). [LDtkLoader](https://github.com/Madour/LDtkLoader) is used to load LDtk files.

Any LDtk file may be referenced in the file and all layers will be rendered using referenced icon or sprite textures. Entities with patrols will move along their patrol routes.

Use CMake to build, current dependencies are:
* directxtk (https://github.com/microsoft/DirectXTK). Available on [VCPKG](vcpkg.io).
* directxmath. Availble on [VCPKG](vcpkg.io).
* LDtkLoader (https://github.com/Madour/LDtkLoader).
