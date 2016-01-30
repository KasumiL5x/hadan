#ifndef __model__
#define __model__

#include <vector>
#include <set>
#include "Vertex.hpp"
#include "BoundingBox.hpp"

class Model {
public:
	struct Edge {
		int idx[2];
		int face[2];

		Edge();
	};

	struct Triangle {
		int idx[3];
		std::set<int> edges;

		Triangle();
	};

public:
	Model();
	~Model();
	Model( const Model& rhs );
	Model operator=( const Model& rhs );

	void addVertex( const Vertex& vertex );
	void addIndex( const int index );
	void buildExtendedData();
	void printDebug( bool verbose ) const;
	void translate( float x, float y, float z );

	const std::vector<Vertex>& getVertices() const;
	const std::vector<int>& getIndices() const;
	const std::vector<Triangle>& getTriangles() const;
	const std::vector<Edge>& getEdges() const;

	BoundingBox computeBoundingBox() const;

private:
	std::vector<Vertex> _vertices;
	std::vector<int> _indices;
	std::vector<Triangle> _triangles;
	std::vector<Edge> _edges;
};

#endif /* __model__ */