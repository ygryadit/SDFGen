# SDFGen edited version

It takes as arguments an obj file without normals and textures. Be careful!

It assumes that all shapes are within bounding boxes with min dimension [-0.8 -0.8 -0.8] amd max dimension [0.8 0.8 0.8].

The gird size is set to 64 x 64 x 64.

Does not support currently different grid sizes in different diemnsions. 

# SDFGen
A simple commandline utility to generate grid-based signed distance field (level set) generator from triangle meshes, using code from Robert Bridson's website.


The MIT License (MIT)

Copyright (c) 2015, Christopher Batty

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
