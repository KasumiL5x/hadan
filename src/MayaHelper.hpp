#ifndef __mayahelper__
#define __mayahelper__

#include <string>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshEdge.h>
#include <maya/MPointArray.h>
#include <maya/MItMeshVertex.h>
#include <maya/MGlobal.h>
#include "Model.hpp"
#include "MTLog.hpp"

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

	static void copyMFnMeshToModel( MDagPath& mayaMeshDagPath, Model& outModel ) {
		MFnMesh mayaMesh(mayaMeshDagPath);
		// does the mesh have uvs?
		MStringArray uvSetNames;
		mayaMesh.getUVSetNames(uvSetNames);
		const bool hasUvs = (uvSetNames.length() > 0) && (mayaMesh.numUVs(uvSetNames[0]) > 0);

		MItMeshVertex it(mayaMeshDagPath);
		while( !it.isDone() ) {
			// this is our vertex, not maya's
			Vertex vtx;

			// get position of the vertex
			MStatus status;
			MPoint pos = it.position(MSpace::kWorld, &status);
			if( status.error() || status != MS::kSuccess ) {
				MTLog::instance()->log("Failed to get pos.\n");
			}
			vtx.position = cc::Vec3f(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));

			// get the texcoord, if it has one
			if( hasUvs ) {
				int uvCount = 0;
				it.numUVs(uvCount, &uvSetNames[0]);
				if( uvCount > 0 ) {
					float2 uvs;
					it.getUV(uvs, &uvSetNames[0]);
					vtx.texcoord = cc::Vec2f(uvs[0], uvs[1]);
				}
			}

			// append vertex to our mesh
			outModel.addVertex(vtx);

			it.next();
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

	static bool hasMesh( const MDagPath& path ) {
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

	static bool isMeshFullyClosed( const MDagPath& meshPath ) {
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

	static void softenMeshEdge( const std::string& meshName ) {
		// harden edges
		const std::string harden = "polySoftEdge -a 0 -ch 1 " + meshName + ";";
		MGlobal::executeCommand(harden.c_str());
		
		// set normal angle to 30
		const std::string normalAngle = "polySoftEdge -angle 30 -ch 1 " + meshName + ";";
		MGlobal::executeCommand(normalAngle.c_str());
	}

	static void centerPivot( const std::string& objName ) {
		const std::string cp = "xform -cpc " + objName + ";";
		MGlobal::executeCommand(cp.c_str());
	}

	static void moveVerticesAlongNormal( MObject mesh, double amount, bool invert ) {
		MItMeshVertex it(mesh);
		while( !it.isDone() ) {
			MVector normal;
			it.getNormal(normal, MSpace::kObject);
			if( invert ) {
				normal = -normal;
			}
			const MPoint position = it.position(MSpace::kObject);
			it.setPosition(position + normal * amount, MSpace::kObject);
			it.next();
		}
	}
} // mayahelper

#endif /* __mayahelper__ */