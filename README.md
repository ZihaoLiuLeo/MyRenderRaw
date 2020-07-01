# MyRenderRaw
My first render restructured.
Able to render .obj file with triangle facets. Able to draw the mesh of an .obj file.
Used Phong Model to render.
Matrix calculation by metaprogramming.
Added global normal map and tangent space normal map.
Used dumb triangle rasterization, assuming the .obj is triangle based.
Learned and reused lots of helpful code from https://github.com/ssloy/tinyrenderer/wiki and PBRT.
Performance improved by metaprgramming from book GameProgrammingGem, 1st Edition
Set the objViewer as the startup project and application.exe if using MSVC. Set other project as static library.
Create a directory /obj in the root directory to store the obj file.
