# 破断（はだん）

Mayaにメッシュを破断は難しいです。このツールでMayaにメッシュを破断は簡単です。\
Fracturing meshes in Maya is difficult.  With this tool, fracturing meshes in Maya is easy.

### Version
0.0.1-dev

### Tech
* [Voro++][voropp] - computation of Voronoi diagrams
* [GeometricTools][geometrictools] - large library ranging from physics to graphics

### Todos
 - Create Maya plugin and test creation of arbitrary meshes
 - Test predefined mesh slicing in Maya
 - Incorporate actual Maya meshes
 - Use 3D Voronoi as the source of the cutting planes
 - Implement many point sampling patterns for varying Voronoi creations
 - Much error checking, especially of input geometry

### License
MIT

[//]: References
[voropp]: <http://math.lbl.gov/voro++/>
[geometrictools]: <http://geometrictools.com/>
