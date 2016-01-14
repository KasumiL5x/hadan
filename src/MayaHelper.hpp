#ifndef __mayahelper__
#define __mayahelper__

#include <string>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshEdge.h>
#include <maya/MPointArray.h>
#include "Model.hpp"

namespace MayaHelper {
	static bool getObjectFromString( const std::string& path, MDagPath& outDagPath ) {
		if( path.empty() ) {
			return false; // disregard empty string
		}
		MSelectionList selectionList;
		if( selectionList.add(path.c_str()) != MS::kSuccess ) {
			return false; // object not found
		}
		selectionList.getDagPath(0, outDagPath);
		return true;
	}
	
	static bool doesMeshHaveHoles( MFnMesh& mesh ) {
		MIntArray holeInfoArray;
		MIntArray holeVertexArray;
		return mesh.getHoles(holeInfoArray, holeVertexArray) != 0;
	}

	static void copyMFnMeshToModel( MFnMesh& mayaMesh, Model& outModel ) {
		// get points
		MPointArray points;
		mayaMesh.getPoints(points, MSpace::kWorld);
		for( unsigned int i = 0; i < points.length(); ++i ) {
			const MPoint& pnt = points[i];
			const float x = static_cast<float>(pnt.x);
			const float y = static_cast<float>(pnt.y);
			const float z = static_cast<float>(pnt.z);
			outModel.addVertex(Vertex(cc::Vec3f(x, y, z)));
		}

		// get triangles (indices)
		MIntArray triangleCounts;
		MIntArray triangleVertices;
		mayaMesh.getTriangles(triangleCounts, triangleVertices);
		for( unsigned int i = 0; i < triangleVertices.length(); ++i ) {
			outModel.addIndex(triangleVertices[i]);
		}
	}

	static bool copyModelToMFnMesh( Model& model, MFnMesh& outMayaMesh ) {
		if( 0 == model.getVertices().size() || 0 == model.getIndices().size() ) {
			return false;
		}

		// convert vertices
		const std::vector<Vertex>& vertices = model.getVertices();
		const int numVerts = static_cast<int>(vertices.size());
		MPointArray pointArray;
		for( unsigned int i = 0; i < static_cast<unsigned int>(vertices.size()); ++i ) {
			const Vertex& vtx = vertices[i];
			pointArray.append(MPoint(vtx.position.x, vtx.position.y, vtx.position.z));
		}

		// convert indices
		const std::vector<int>& indices = model.getIndices();
		MIntArray faceConnects;
		for( unsigned int i = 0; i < static_cast<unsigned int>(indices.size()); ++i ) {
			faceConnects.append(indices[i]);
		}

		// make up face count array
		const int numFaces = static_cast<int>(indices.size()) / 3;
		MIntArray faceCounts;
		for( int i = 0 ; i < numFaces; ++i ) {
			faceCounts.append(3);
		}

		outMayaMesh.create(numVerts, numFaces, pointArray, faceCounts, faceConnects);
		return false;
	}

	static bool hasMesh( MDagPath& path ) {
		return path.hasFn(MFn::kMesh);
	}

	static void debugPrintMFnMesh( MFnMesh& mesh, bool verbose ) {
		MPointArray points;
		mesh.getPoints(points, MSpace::kWorld);
		printf("Points (%d)\n", points.length());
		if( verbose ) {
			for( unsigned int i = 0; i < points.length(); ++i ) {
				printf("\t[%d] (%f, %f, %f)\n", i, points[i].x, points[i].y, points[i].z);
			}
		}

		MIntArray triangleCounts;
		MIntArray triangleVertices;
		mesh.getTriangles(triangleCounts, triangleVertices);
		printf("Indices (%d)\n", triangleVertices.length());
		if( verbose ) {
			for( unsigned int i = 0; i < triangleVertices.length(); ++i ) {
				printf("\t[%d] %d\n", i, triangleVertices[i]);
			}
		}
	}

	static bool isMeshFullyClosed( MDagPath& meshPath ) {
		MItMeshEdge it(meshPath);

		int faceCount = 0;
		while( !it.isDone() ) {
			it.numConnectedFaces(faceCount);
			if( faceCount != 2 ) {
				return false;
			}
			it.next();
		}
		
		return true;
	}
} // mayahelper

#endif /* __mayahelper__ */