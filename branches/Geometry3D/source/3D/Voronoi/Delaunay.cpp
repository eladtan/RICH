/*  \file Delaunay.cpp
\brief An abstract class that encapsulates the 3D Delaunay calculations
\Author Itay Zandbank
*/

#include "../Utilities/assert.hpp"
#include "Delaunay.hpp"

Delaunay::Delaunay(const std::vector<VectorRef> &points, const Tetrahedron &bigTetrahedron)
	: _points(points), _bigTetrahedron(bigTetrahedron)
{

}

void Delaunay::FillVertices()
{
	_vertices.clear();

	for (size_t i = 0; i < _tetrahedra.size(); i++)
		for (size_t j = 0; j < 4; j++)
		{
			VectorRef v = _tetrahedra[i][j];
			VertexMap::iterator existing = _vertices.find(v);
			if (existing == _vertices.end())
				_vertices[v] = vector<size_t>();

			_vertices[v].push_back(i);
		}
}

const std::vector<size_t> &Delaunay::EdgeNeighbors(const VectorRef vec1, const VectorRef vec2) const
{
	Edge edge(vec1, vec2);
	EdgeMap::const_iterator it = _edges.find(edge);
	BOOST_ASSERT(it != _edges.end());

	return it->second;
}

const std::vector<size_t> &Delaunay::VertexNeighbors(const VectorRef v) const
{
	VertexMap::const_iterator it = _vertices.find(v);
	BOOST_ASSERT(it != _vertices.end());

	return it->second;
}

void Delaunay::Run()
{
	_tetrahedra.clear();
	_edges.clear();
	_vertices.clear();
	RunDelaunay();
	BOOST_ASSERT(_edges.size() == 0 && _vertices.size() == 0); // You're not supposed to fill those
	FillVertices();
	FillEdges();
	FillNeighbors();
}

Tetrahedron Delaunay::CalcBigTetrahedron(const OuterBoundary3D &boundary)
{
	// A big tetrahedron that will contain the bounding box, as well as the 8 adjacent boundary boxes,
	// and with room to spare.

	const Vector3D &fur = boundary.FrontUpperRight();
	const Vector3D &bll = boundary.BackLowerLeft();
	Vector3D absFrontUpperRight(abs(fur.x), abs(fur.y), abs(fur.z));
	Vector3D absBackLowerLeft(abs(bll.x), abs(bll.y), abs(bll.z));

	absFrontUpperRight *= 1e10;
	absBackLowerLeft *= -1e10;

	// The top of the tetrahedron will be on the Y axis
	vector<VectorRef> tetrahedron;
	tetrahedron.push_back(Vector3D(0, absFrontUpperRight.y, 0));

	// The bottom face is parallel to the x-z plane
	double bottomY = absBackLowerLeft.y;

	// The bottom face is a triangle whose lower edge is parallel to the x axis
	double backZ = absBackLowerLeft.z;
	tetrahedron.push_back(Vector3D(absBackLowerLeft.x, bottomY, backZ));
	tetrahedron.push_back(Vector3D(absFrontUpperRight.x, bottomY, backZ));

	// The last triangle edge is on x=0
	tetrahedron.push_back(Vector3D(0, bottomY, absFrontUpperRight.z));

	return Tetrahedron(tetrahedron);
}
