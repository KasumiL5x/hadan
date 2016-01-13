#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

MStatus hadan::doIt( const MArgList& args ) {
	std::cout << "Hello there, " << args.asString(0).asChar() << std::endl;

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

	MIntArray faceCounts;
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);
	faceCounts.append(3); faceCounts.append(3);

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

	MFnMesh mesh;
	MObject dataObj = mesh.create(numVerts, numFaces, pointArray, faceCounts, faceConnects);

	return MStatus::kSuccess;
}