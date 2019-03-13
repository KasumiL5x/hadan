# 破断（はだん）

> Mayaにメッシュを破断することは難しいです。このツールでMayaにメッシュを破断することは簡単です。Fracturing meshes in Maya is difficult.  With this tool, fracturing meshes in Maya is easy.

Hadan is a fracturing tool for Autodesk Maya. Many major 3D packages, such as Luxology Modo, have either native or well-known plug-ins for fracturing geometry. Unfortunately, Maya does not (at the time of writing this project) have such tools by default. Hadan aims to solve this by introducing multiple fracture types with customizable options. For more details on the inner workings of Hadan, please refer to my [accompanying report](https://dgreen.me/portfolio_dl/hadan/hadan-report-VIS4062-dgreen.pdf).

## Status
Hadan is **not** being developed.  It is a was developed as a University project in _a couple of months_ and should be treated as such.

## Features
Development on Hadan has now ceased, as it was created for a University module that has long since finished. The [YouTube video](https://www.youtube.com/watch?v=PwdDBsPTBco) below shows features of the tool.

[![Hadan Feature Video](https://img.youtube.com/vi/PwdDBsPTBco/0.jpg)](https://www.youtube.com/watch?v=PwdDBsPTBco "Hadan Feature Video")

## Usage
Hadan is implemented as a Maya command with flags to configure its behavior. A GUI is provided for ease of use.

| Flag                    | Type    | Description                                                                                               |
|-------------------------|---------|-----------------------------------------------------------------------------------------------------------|
| _meshName/mn_           | string  | Name of the mesh to fracture                                                                              |
| _fractureType/ft_       | string  | Fracture operation: _uniform_, _cluster_, or _bezier_                                                           |
| _slicerType/st_         | string  | Slicer algorithm: _gte_ or _csgjs_                                                                            |
| _uniformCount/uc_       | integer | Number of random points to generate                                                                       |
| _primaryCount/pc_       | integer | Number of random primary points to generate; used in cluster mode                                         |
| _secondaryCount/sc_     | integer | Number of random secondary points to generate around primary points; used in cluster mode                 |
| _separationDistance/sd_ | double  | Distance to shrink chunks' vertices along their normals                                                   |
| _sampleCount/sam_       | integer | Number of samples for bezier curves                                                                       |
| _fluxPercent/flp_       | double  | Fluctuation percentage based on the size of the object's bounding box                                     |
| _randomSeed/rs_         | integer | Random seed for reproducible fractures                                                                    |
| _point/pnt_             | double  | Custom fracture location; three doubles (x, y, z)                                                         |
| _smoothingAngle/sa_     | double  | Angle used for smoothing of chunks' normals                                                               |
| _minBezierDist/mbd_     | double  | Minimum distance between randomly generated bezier endpoints as a percentage of the object's bounding box |
| _multithreaded/mt_      | boolean | Whether to enable multi-threading or not                                                                  |

When _fractureType/ft_ is set to **uniform**, _uniformCount/uc_ randomly generated points are created within the object's bounding box.

When _fractureType/ft_ is set to **cluster**, _primaryCount/pc_ randomly generated points are spawned within the object's bounding box, and then _secondaryCount/sc_ randomly generated points are added within a distance of _fluxPercent/flp_ of each primary point. _uniformCount/uc_ randomly generated points are also added for variation.

When _fractureType/ft_ is set to **bezier**, one of three cases can occur. Each case is chosen based on the number of user-provided _point/pnt_. If none are provided, all four points of the bezier are randomly generated; the first and last are on the surface, and will attempt to space themselves at least _minBezierDist/mbd_ apart (unless a maximum iteration count is hit, of which the last attempt is accepted regardless of the distance), and the two intermediate points are randomly generated within the bounding box. If two points are provided, they are assumed to be the beginning and end points respectively, and the two intermediate points are generated as before. If all four points are provided, the curve is assumed to be complete and in order, and no random generation will take place. The bezier curve is sampled _sampleCount/sam_ times along the curve at uniform intervals. The sampled points are moved [-fluxPercent/flp, fluxPercent/flp] away from the curve for variation. _uniformCount/uc_ uniform points are also added for variation.

The slicing algorithm used can be chosen using the _slicerType/st_ flag. Choosing **gte** will use a cutting algorithm derived from [Geometric Tools' ClipMesh](http://geometrictools.com/). This algorithm is very efficient, but will not work for more complex geometry, especially concave. Electing to use csgjs will instead perform boolean intersections of each cell with the source geometry using a C++ port of the [csg.js](https://github.com/evanw/csg.js/) library. This option works better with complex geometry, but comes with its own drawbacks, especially in performance. Having a selection of slicing algorithms is important, as the source geometry may not always be suited for a specific algorithm.

Multi-threading is supported and can be toggled with the _multithreaded/mt_ flag. It is enabled by default.

Each chunk has its normals generated and smoothed based on a smoothing angle. If an edge has only one face (which should seldom occur), then its edge will be hardened. If the angle between the shared faces of the edge in question is less than _smoothingAngle/sa_, it will be smoothed, or otherwise hardened.

![Hadan GUI](https://raw.githubusercontent.com/KasumiL5x/hadan/master/docs/hadan_gui.png "Hadan GUI")

Hadan's GUI is separated into three parts: Settings, Positions, and Advanced. The Settings tab allows for configuration of most of the flags above. The Positions tab is for managing custom user points. The Advanced tab is for extra configuration options. The GUI simply maps its elements to a flag in the command, and generates a command upon execution. The generated command can be seen in the Advanced tab after fracturing takes place.

## How does it work?
Hadan is implemented as a Maya command. This can be run from both MEL and Python. Upon execution, Hadan goes through three major phases. The first stage, Generate Points, stage is responsible for generating source points used to feed stage 2. The second stage, Generate Cells, has to create slicing cells that will later be used to cut the geometry. The third stage, Cut Geometry, is where the source geometry is actually decimated based on the generated cells.

For more information, please see my [accompanying report](https://dgreen.me/portfolio_dl/hadan/hadan-report-VIS4062-dgreen.pdf).

### Tech
* [Voro++](http://math.lbl.gov/voro++/) - computation of Voronoi diagrams
* [GeometricTools](http://geometrictools.com/) - large library ranging from physics to graphics
* ccmath - mathematics library