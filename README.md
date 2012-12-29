MSc
===

A public version of the code I've written during my MSc program at the University of Toronto.

The code is can be built using SCons with the qt4 plugin installed.
http://www.scons.org/wiki/Qt4Tool

Interesting sections
===
The interesting parts of the code are following:
src/solvers is a collection of linear and quadratic solvers that I've written using policies

src/geometry/levelset/mac is the core of the my simulation code, where I define different classes of grids, how to size them by default, how to differentiate them, and how to loop through them. (2D and 3D)

src/simulation contains a few not-novel simulation methods that I've implemented.  This directory will probably fail on compilation because I've shelled researchy code out

src/viewers/viewerbase contains some starter qt/opengl code that I've written with some generic glsl shell code

src/viewers/macviewer and src/viewers/macviewer2d are build to depend on all of my fluid solvers at once so they will probably break without slight modification (due to solvers being removed)
