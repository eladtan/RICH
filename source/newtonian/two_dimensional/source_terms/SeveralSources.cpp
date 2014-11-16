#include "SeveralSources.hpp"

SeveralSources::SeveralSources(vector<SourceTerm*> forces):
  sources_(forces) {}
/*
{
  for(size_t i=0;i<forces.size();++i)
    sources_.push_back(forces[i]);
}
*/

SeveralSources::~SeveralSources(void)
{
}

Conserved SeveralSources::Calculate(Tessellation const& tess,
				    const PhysicalGeometry& pg,
	vector<Primitive> const& cells,int point,
	vector<Conserved> const& fluxes,
	vector<Vector2D> const& point_velocity,
	HydroBoundaryConditions const& hbc,
	vector<vector<double> > const &tracer_extensive,vector<double> &dtracer,
	vector<double> const& lengthes,
	double time,double dt)
{
	Conserved res;
	vector<double> dtracer_local;
	if(!dtracer.empty())
		dtracer_local.assign(dtracer.size(),0);
	for(size_t i=0;i<sources_.size();++i)
	{
	  res+=sources_[i]->Calculate(tess,pg,cells,point,fluxes,
		point_velocity,hbc,tracer_extensive,dtracer_local,lengthes,time,dt);
		if(!dtracer.empty())
			dtracer=dtracer+dtracer_local;
	}
	return res;
}