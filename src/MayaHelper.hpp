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
#include "BoundingBox.hpp"
#include <maya/MBoundingBox.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <cc/TriMath.hpp>
#include <maya/MItMeshFaceVertex.h>

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

	static BoundingBox getBoundingBox( const MFnMesh& mayaObj ) {
		BoundingBox bbox;

		// get parent transform node (all meshes definitely have one)
		MFnTransform parent(mayaObj.parent(0));
		// get the mesh's bounding box
		MBoundingBox mayaBox = mayaObj.boundingBox();
		// transform the bounding box by the parent's transformation matrix
		mayaBox.transformUsing(parent.transformationMatrix());
		// get the transformed center
		const MPoint center = mayaBox.center();
		bbox.setCenter(cc::Vec3f(static_cast<float>(center.x), static_cast<float>(center.y), static_cast<float>(center.z)));
		// get the half dimensions
		const float halfWidth = static_cast<float>(mayaBox.width()) * 0.5f;
		const float halfHeight = static_cast<float>(mayaBox.height()) * 0.5f;
		const float halfDepth = static_cast<float>(mayaBox.depth()) * 0.5f;
		bbox.setHalfExtents(cc::Vec3f(halfWidth, halfHeight, halfDepth));

		return bbox;
	}

	static void copyMFnMeshToModel( MDagPath& mayaMeshDagPath, Model& outModel ) {
		MFnMesh mayaMesh(mayaMeshDagPath);

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

		// create the actual mesh (lock w/ mutex because create function is not thread safe)
		static std::mutex CreateMeshMutex;
		{
			std::unique_lock<std::mutex> lock(CreateMeshMutex);
			outMayaMesh.create(numVerts, numFaces, pointArray, faceCounts, faceConnects);

			// custom polySoftEdge in C++
			MItMeshEdge edgeIt(outMayaMesh.object()); // BUG: Does NOT work with DAG paths!
			while( !edgeIt.isDone() ) {
				// if not 2 edges, harden
				int faceCount = 0;
				edgeIt.numConnectedFaces(faceCount);
				if( faceCount != 2 ) {
					edgeIt.setSmoothing(false);
				} else {
					// get connected faces (2)
					MIntArray faceList;
					edgeIt.getConnectedFaces(faceList);
					// get normals of faces
					MVector nrmA;
					outMayaMesh.getPolygonNormal(faceList[0], nrmA);
					MVector nrmB;
					outMayaMesh.getPolygonNormal(faceList[1], nrmB);
					const cc::Vec3f cc_a = cc::Vec3f(nrmA.x, nrmA.y, nrmA.z).normalized();
					const cc::Vec3f cc_b = cc::Vec3f(nrmB.x, nrmB.y, nrmB.z).normalized();
					const float theta = cc_a.dot(cc_b);
					const float angle = cc::math::RAD_TO_DEG * acosf(theta);

					if( angle >= 30.0f ) { // todo: expose this
						edgeIt.setSmoothing(false);
					} else {
						edgeIt.setSmoothing(true);
					}
				}

				edgeIt.next();
			}
			outMayaMesh.cleanupEdgeSmoothing();
			outMayaMesh.updateSurface();
		}

		return true;
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