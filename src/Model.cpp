#include "Model.hpp"
#include <map>

Model::Model() {
}

Model::~Model() {
}

Model::Model( const Model& rhs ) {
	this->_vertices = rhs._vertices;
	this->_indices = rhs._indices;
}

Model Model::operator=( const Model& rhs ) {
	this->_vertices = rhs._vertices;
	this->_indices = rhs._indices;
	return *this;
}

void Model::addVertex( const Vertex& vertex ) {
	_vertices.push_back(vertex);
}

void Model::addIndex( const int index ) {
	_indices.push_back(index);
}

void Model::buildExtendedData() {
	// requires triangles
	if( _indices.size() % 3 != 0 ) {
		return;
	}

	// clear old data
	_triangles.clear();
	_edges.clear();

	// gather all triangles
	for( int i = 0; i < _indices.size(); i += 3 ) {
		Triangle tri;
		tri.idx[0] = _indices[i];
		tri.idx[1] = _indices[i+1];
		tri.idx[2] = _indices[i+2];
		_triangles.push_back(tri);
	}

	// gather all unique edges
	std::map<std::pair<int, int>, int> edgeMap;
	for( int i = 0; i < _triangles.size(); ++i ) {
		const Model::Triangle& currTri = _triangles[i];
		// edge 0->1
		std::pair<int, int> e01(currTri.idx[0], currTri.idx[1]);
		std::pair<int, int> e02(currTri.idx[1], currTri.idx[0]);
		if( edgeMap.find(e02) != edgeMap.end() ) { // if has reversed...
			_edges[edgeMap[e02]].face[1] = i; // add second triangle
		} else if( edgeMap.find(e01) != edgeMap.end() ) { // if has normal...
			_edges[edgeMap[e01]].face[1] = i; // add second triangle
		} else {
			Edge e;
			e.idx[0] = e01.first;
			e.idx[1] = e01.second;
			e.face[0] = i; // add first triangle
			_edges.push_back(e);
			edgeMap[e01] = static_cast<int>(_edges.size())-1;
		}
		// edge 1->2
		std::pair<int, int> e11(currTri.idx[1], currTri.idx[2]);
		std::pair<int, int> e12(currTri.idx[2], currTri.idx[1]);
		if( edgeMap.find(e12) != edgeMap.end() ) {
			_edges[edgeMap[e12]].face[1] = i;
		} else if( edgeMap.find(e11) != edgeMap.end() ) {
			_edges[edgeMap[e11]].face[1] = i;
		} else {
			Edge e;
			e.idx[0] = e11.first;
			e.idx[1] = e11.second;
			e.face[0] = i;
			_edges.push_back(e);
			edgeMap[e11] = static_cast<int>(_edges.size())-1;
		}
		// edge 2->0
		std::pair<int, int> e21(currTri.idx[2], currTri.idx[0]);
		std::pair<int, int> e22(currTri.idx[0], currTri.idx[2]);
		if( edgeMap.find(e22) != edgeMap.end() ) {
			_edges[edgeMap[e22]].face[1] = i;
		} else if( edgeMap.find(e21) != edgeMap.end() ) {
			_edges[edgeMap[e21]].face[1] = i;
		} else {
			Edge e;
			e.idx[0] = e21.first;
			e.idx[1] = e21.second;
			e.face[0] = i;
			_edges.push_back(e);
			edgeMap[e21] = static_cast<int>(_edges.size())-1;
		}
	}

	// link triangles' edges (edges are already linked to triangles)
	for( int i = 0; i < _edges.size(); ++i ) {
		const Edge& currEdge = _edges[i];
		_triangles[currEdge.face[0]].edges.insert(i);
		_triangles[currEdge.face[1]].edges.insert(i);
	}
}

void Model::printDebug( bool verbose ) const {
	printf("Vertices (%zd)\n", _vertices.size());
	if( verbose ) {
		for( unsigned int i = 0; i < _vertices.size(); ++i ) {
			printf("\t[%d] (%f, %f, %f)\n", i, _vertices[i].position.x, _vertices[i].position.y, _vertices[i].position.z);
		}
	}

	printf("Indices (%zd)\n", _indices.size());
	if( verbose ) {
		for( unsigned int i = 0; i < _indices.size(); ++i ) {
			printf("\t[%d] %d\n", i, _indices[i]);
		}
	}

	printf("Edges (%zd)\n", _edges.size());
	if( verbose ) {
		for( unsigned int i = 0; i < _edges.size(); ++i ) {
			printf("\t[%d] idx(%d->%d); face(%d, %d)\n", i, _edges[i].idx[0], _edges[i].idx[1], _edges[i].face[0], _edges[i].face[1]);
		}
	}

	printf("Triangles (%zd)\n", _triangles.size());
	if( verbose ) {
		for( unsigned int i = 0; i < _triangles.size(); ++i ) {
			printf("\t[%d] idx(%d->%d->%d); %zd edges\n", i, _triangles[i].idx[0], _triangles[i].idx[1], _triangles[i].idx[2], _triangles[i].edges.size());

			std::set<int>::const_iterator iter = _triangles[i].edges.cbegin();
			const std::set<int>::const_iterator end = _triangles[i].edges.cend();
			for( unsigned int counter = 0; iter != end; ++iter, ++counter ) {
				printf("\t\t[%d] %d\n", counter, *iter);
			}
		}
	}
}

void Model::translate( float x, float y, float z ) {
	for( unsigned int i = 0; i < _vertices.size(); ++i ) {
		Vertex& vtx = _vertices[i];
		vtx.position.x += x;
		vtx.position.y += y;
		vtx.position.z += z;
	}
}

const std::vector<Vertex>& Model::getVertices() const {
	return _vertices;
}

const std::vector<int>& Model::getIndices() const {
	return _indices;
}

const std::vector<Model::Triangle> Model::getTriangles() const {
	return _triangles;
}

const std::vector<Model::Edge> Model::getEdges() const {
	return _edges;
}

Model::Edge::Edge() {
	idx[0]=idx[1] = -1;
	face[0]=face[1] = -1;
}

Model::Triangle::Triangle() {
	idx[0]=idx[1]=idx[2] = -1;
}