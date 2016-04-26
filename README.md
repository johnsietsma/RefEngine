# AIEOpenGL

An OpenGL game engine with QT and GLFW bindings.

It purpose is to demonstrate OpenGL concepts and code layout.

## Building
Run the shell scripts in the root directory to generate project files for Windows or Mac.

## Dependencies
* [CMake](https://cmake.org/)
Must be installed and in your path.

* [FBX SDK](http://usa.autodesk.com/adsk/servlet/pc/item?id=24746731&siteID=123112)
Download "FBX SDK 2016.1.2 VS2015" and install in the default location.

* [Visual Leak Detector](https://vld.codeplex.com/)
Download and install in the default location.


## Design Notes

### OpenGL Resource Creation and Lifetime

OpenGL resource classes such as ```Texture``` and ```Program``` always provide a create and destroy functions. This is so:

1. Lifetime can be explicitly managed outside of the lifetime of the variable. There are order dependant steps to create and destroy resources, so we want to control when things happen.
1. Resource classes can be copyable. OpenGL handles are simple integers, if we don't use the constructor to create and destroy OpenGL resources we can copy Resources objects without
 having to implement move or copy constructors. The burden is on the creator to make sure resources are cleaned up explicitly.
1. We can verify that creation succeeded by return true of false from the create function.


### Supporting New Vertex Types

You can add new Vertex types by adding a new struct to Vertex.h and then specialising the ```Setup<T>()``` template function to load the correct vertex attributes for your new vertex.


