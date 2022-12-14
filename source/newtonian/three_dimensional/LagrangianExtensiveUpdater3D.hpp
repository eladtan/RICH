#ifndef LEUPDATE3D_HPP
#define LEUPDATE3D_HPP 1
#include "ConditionExtensiveUpdater3D.hpp"
#include "ConditionActionFlux1.hpp"

//! \brief Lagrangian cell updater
class LagrangianExtensiveUpdater3D : public ExtensiveUpdater3D
{
public:
  /*! \brief Class constructor
    \param lflux Lagrangian flux calculator
    \param sequence Condition - action sequence
   */
	LagrangianExtensiveUpdater3D(LagrangianFlux3D const& lflux, const vector<pair<const ConditionExtensiveUpdater3D::Condition3D*, 
		const ConditionExtensiveUpdater3D::Action3D*> >& sequence);

	void operator()(const vector<Conserved3D>& fluxes, const Tessellation3D& tess,
		const double dt, const vector<ComputationalCell3D>& cells, vector<Conserved3D>& extensives, double time,
		TracerStickerNames const& tracerstickernames, const vector<Vector3D>& edge_velocities,
		std::vector<std::pair<ComputationalCell3D, ComputationalCell3D> > const& interp_values) const override;
private:
	LagrangianFlux3D const& lflux_;
  //EquationOfState const& eos_;
  //	Ghost3D const& ghost_;
	vector<pair<const ConditionExtensiveUpdater3D::Condition3D*, const ConditionExtensiveUpdater3D::Action3D*> >
		const& sequence_;
};
#endif //LEUPDATE_HPP3D
