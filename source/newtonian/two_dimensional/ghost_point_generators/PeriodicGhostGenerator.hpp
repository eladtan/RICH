/*! \file PeriodicGhostGenerator.hpp
\brief Class for creating computationalcells of ghost points for periodic boundaries
\author Elad Steinberg
*/

#ifndef PERIODIC_POINT_GENERATOR_HPP
#define PERIODIC_POINT_GENERATOR_HPP 1

#include "../GhostPointGenerator.hpp"

/*! \brief Class for creating computationalcells of ghost points for periodic boundaries
\author Elad Steinberg
*/
class PeriodicGhostGenerator : public GhostPointGenerator
{
public:
	boost::container::flat_map<size_t, ComputationalCell> operator() (const Tessellation& tess,
		const vector<ComputationalCell>& cells) const;

	std::pair<ComputationalCell, ComputationalCell> GetGhostGradient(const Tessellation& tess,
		const vector<ComputationalCell>& cells, const vector<std::pair<ComputationalCell, ComputationalCell> >& gradients,
		size_t ghost_index) const;
	
	Vector2D GetGhostVelocity(const Tessellation& tess, const vector<ComputationalCell>& cells,
		vector<Vector2D> const& point_veolcities, size_t ghost_index, Edge const& edge)const;
};

#endif // PERIODIC_POINT_GENERATOR_HPP