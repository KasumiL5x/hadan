#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include "Model.hpp"
#include "ClipMesh.hpp"

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

void createMayaMesh( MFnMesh& outMesh ) {
	// build points array
	MPointArray pointArray;
	const float SIZE = 1.0f;
	pointArray.append(MPoint(-0.5f*SIZE, -0.5f*SIZE,  0.5f*SIZE)); // front
	pointArray.append(MPoint( 0.5f*SIZE, -0.5f*SIZE,  0.5f*SIZE));
	pointArray.append(MPoint( 0.5f*SIZE,  0.5f*SIZE,  0.5f*SIZE));
	pointArray.append(MPoint(-0.5f*SIZE,  0.5f*SIZE,  0.5f*SIZE));
	pointArray.append(MPoint(-0.5f*SIZE, -0.5f*SIZE, -0.5f*SIZE)); // back
	pointArray.append(MPoint( 0.5f*SIZE, -0.5f*SIZE, -0.5f*SIZE));
	pointArray.append(MPoint( 0.5f*SIZE,  0.5f*SIZE, -0.5f*SIZE));
	pointArray.append(MPoint(-0.5f*SIZE,  0.5f*SIZE, -0.5f*SIZE));
	const int numVerts = pointArray.length();

	// build face count array
	MIntArray faceCounts;
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);

	// build indices array
	MIntArray faceConnects;
	faceConnects.append(0); faceConnects.append(1); faceConnects.append(2); // front
	faceConnects.append(2); faceConnects.append(3); faceConnects.append(0);
	faceConnects.append(1); faceConnects.append(5); faceConnects.append(6); // top
	faceConnects.append(6); faceConnects.append(2); faceConnects.append(1);
	faceConnects.append(7); faceConnects.append(6); faceConnects.append(5); // back
	faceConnects.append(5); faceConnects.append(4); faceConnects.append(7);
	faceConnects.append(4); faceConnects.append(0); faceConnects.append(3); // bottom
	faceConnects.append(3); faceConnects.append(7); faceConnects.append(4);
	faceConnects.append(4); faceConnects.append(5); faceConnects.append(1); // left
	faceConnects.append(1); faceConnects.append(0); faceConnects.append(4);
	faceConnects.append(3); faceConnects.append(2); faceConnects.append(6); // right
	faceConnects.append(6); faceConnects.append(7); faceConnects.append(3);

	const int numFaces = faceConnects.length() / 3;
	outMesh.create(numVerts, numFaces, pointArray, faceCounts, faceConnects);
}

void copyToModel( MFnMesh& mayaMesh, Model& model ) {
	// get points
	MPointArray points;
	mayaMesh.getPoints(points, MSpace::kPostTransform);
	for( unsigned int i = 0; i < points.length(); ++i ) {
		const MPoint& pnt = points[i];
		const float x = static_cast<float>(pnt.x);
		const float y = static_cast<float>(pnt.y);
		const float z = static_cast<float>(pnt.z);
		model.addVertex(Vertex(cc::Vec3f(x, y, z)));
	}

	// get triangles (indices)
	MIntArray triangleCounts;
	MIntArray triangleVertices;
	mayaMesh.getTriangles(triangleCounts, triangleVertices);
	for( unsigned int i = 0; i < triangleVertices.length(); ++i ) {
		model.addIndex(triangleVertices[i]);
	}
}

void debugPrintMayaMesh( MFnMesh& mayaMesh, bool verbose ) {
	MPointArray points;
	mayaMesh.getPoints(points, MSpace::kPostTransform);
	printf("Points (%d)\n", points.length());
	if( verbose ) {
		for( unsigned int i = 0; i < points.length(); ++i ) {
			printf("\t[%d] (%f, %f, %f)\n", i, points[i].x, points[i].y, points[i].z);
		}
	}

	MIntArray triangleCounts;
	MIntArray triangleVertices;
	mayaMesh.getTriangles(triangleCounts, triangleVertices);
	printf("Indices (%d)\n", triangleVertices.length());
	if( verbose ) {
		for( unsigned int i = 0; i < triangleVertices.length(); ++i ) {
			printf("\t[%d] %d\n", i, triangleVertices[i]);
		}
	}
}

bool createMayaMeshFromModel( const Model& model, MFnMesh& outMayaMesh ) {
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

	return true; // todo: check for false model etc.
}

MStatus hadan::doIt( const MArgList& args ) {
	std::cout << "Hello there, " << args.asString(0).asChar() << std::endl;

	// create maya mesh
	MFnMesh mayaMesh;
	createMayaMesh(mayaMesh);

	//
	// extract mfn mesh data into Model
	//
	Model model;
	copyToModel(mayaMesh, model);

	// build extended Model data to have access to generated triangles and edges
	model.buildExtendedData();

	// clip the model with a plane
	ClipMesh cm(model);
	cm.clip(Plane(cc::Vec3f(0.0f, 0.906307817f, -0.422618270f), 0.226576954f));
	
	// convert the clipped model into a new model
	Model clippedModel;
	if( !cm.convert(&clippedModel) ) {
		printf("ERROR: Failed to convert clipped model!\n");
	} else {
		// move the clipped model up just for visual ease
		clippedModel.translate(0.0f, 2.0f, 0.0f);

		// create a new maya mesh from the clipped model
		MFnMesh newMayaMesh;
		createMayaMeshFromModel(clippedModel, newMayaMesh);
	}

	//// test print data to ensure it copied correctly
	//printf("---BEGIN PRINT MAYA MESH---\n");
	//debugPrintMayaMesh(mayaMesh, true);
	//printf("---END PRINT MAYA MESH---\n");
	//printf("---BEGIN PRINT MODEL---\n");
	//model.printDebug(true);
	//printf("---END PRINT MODEL---\n");

	return MStatus::kSuccess;
}