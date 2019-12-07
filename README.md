# GreenOrange

Raymarching fragment shader generator.

Proof of concept for a demoscene intro creation tool. WYSIWYG 3D scene editor that generates GLSL fragment shaders. The shaders will perform raymarching on the distance field of the created scene. 

Scenes are created by composing basic objects and CSG operators on a tree. A hierarchy of transforms can also be applied to objects. The output shader is real-time rendered on a quad on the preview panel. 

# Current features:
* Support for multiple scenes.
* Objects: spheres and boxes.
* CSG operators: union, subtraction and intersection.
* Transforms: translation, rotation and custom code.
* PBR material and shading models.
* Point and directional lights.
* Global illumination by using probes to bake irradiance maps from objects and lights marked as static.
* Premake build system.

### This is an work in progress proof of concept.

# Gallery

![GreenOrange](http://www.bmlourenco.com/public/images/greenorange/1.jpg)

![GreenOrange](http://www.bmlourenco.com/public/images/greenorange/2.jpg)