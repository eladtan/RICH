#include "ConservativeForce.hpp"

namespace {
	Vector2D MassFlux(Tessellation const& tess,int point,
		vector<Conserved> const& fluxes,
		HydroBoundaryConditions const& hbc,vector<double> const& lengthes)
	{
		Vector2D dm;
		vector<int> edge_index=tess.GetCellEdges(point);
		Vector2D center=tess.GetMeshPoint(point);
		int n=(int)edge_index.size();
		Edge edge;
		for(int i=0;i<n;++i)
		{
			edge=tess.GetEdge(edge_index[i]);
			if(point==edge.neighbors.first)
				if(edge.neighbors.second>-1)
				{
					dm-=lengthes[edge_index[i]]*fluxes[edge_index[i]].Mass*
						(center-tess.GetMeshPoint(edge.neighbors.second));
				}
				else
				{
					Vector2D r=hbc.Normal(edge,tess);
					dm-=2*lengthes[edge_index[i]]*fluxes[edge_index[i]].Mass*r;
				}
			else
				if(point==edge.neighbors.second)
					if(edge.neighbors.first>-1)
					{
						dm+=lengthes[edge_index[i]]*fluxes[edge_index[i]].Mass*
							(center-tess.GetMeshPoint(edge.neighbors.first));
					}
					else
					{
						Vector2D r=hbc.Normal(edge,tess);
						dm+=2*lengthes[edge_index[i]]*fluxes[edge_index[i]].Mass*r;
					}
				else
					throw UniversalError("Error in ConservativeForce MassFlux: Cell and edge are not mutual neighbors");
		}
		return dm;
	}
}

ConservativeForce::ConservativeForce(Acceleration& acc,bool DtCalc):acc_(acc),
	DtCalc_(DtCalc),dt_(-1),first_time_(true){}

ConservativeForce::~ConservativeForce(void){}

Conserved ConservativeForce::Calculate
	(Tessellation const& tess,
	vector<Primitive> const& cells,
	int point,
	vector<Conserved> const& fluxes,
	vector<Vector2D> const& point_velocity,
	HydroBoundaryConditions const& hbc,
	vector<vector<double> > const &tracer_extensive,
	vector<double> &/*dtracer*/,vector<double> const& lengthes,
	double time,
	double dt)
{
	Conserved res;
	Vector2D acc=acc_.Calculate
		(tess,cells,point,fluxes,point_velocity,hbc,
		 tracer_extensive,
		 time, dt);
	double volume=tess.GetVolume(point);
	res.Momentum=volume*cells[point].Density*acc;
	res.Energy=cells[point].Density*volume*ScalarProd(point_velocity[point],acc)+
		0.5*ScalarProd(MassFlux(tess,point,fluxes,hbc,lengthes),acc);
	if(DtCalc_)
	{
		if(first_time_)
		{
			dt_=sqrt(tess.GetWidth(point)/abs(acc));
			first_time_=false;
		}
		else
		  dt_=std::min(dt_,sqrt(tess.GetWidth(point)/abs(acc)));
	}
	return res;
}

double ConservativeForce::GetTimeStep(void) const
{
	return dt_;
}

Acceleration::~Acceleration(void) {}