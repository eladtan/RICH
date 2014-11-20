#include <iostream>
#include "source/tessellation/VoronoiMesh.hpp"
#include "source/newtonian/two_dimensional/hdsim2d.hpp"
#include "source/newtonian/two_dimensional/interpolations/pcm2d.hpp"
#include "source/newtonian/two_dimensional/spatial_distributions/uniform2d.hpp"
#include "source/newtonian/common/ideal_gas.hpp"
#include "source/newtonian/two_dimensional/geometric_outer_boundaries/SquareBox.hpp"
#include "source/newtonian/two_dimensional/hydro_boundary_conditions/RigidWallHydro.hpp"
#include "source/newtonian/common/hllc.hpp"
#include "source/newtonian/two_dimensional/source_terms/zero_force.hpp"
#include "source/newtonian/two_dimensional/point_motions/eulerian.hpp"
#include "source/newtonian/two_dimensional/point_motions/lagrangian.hpp"
#include "source/newtonian/two_dimensional/point_motions/round_cells.hpp"
#include "source/newtonian/two_dimensional/diagnostics.hpp"
#include "source/newtonian/two_dimensional/source_terms/cylinderical_geometry.hpp"
#include "source/newtonian/two_dimensional/source_terms/cylindrical_complementary.hpp"
#include "source/misc/simple_io.hpp"
#include "source/newtonian/test_2d/main_loop_2d.hpp"
#include "source/newtonian/two_dimensional/hdf5_diagnostics.hpp"
#include "source/misc/mesh_generator.hpp"
#include "source/tessellation/shape_2d.hpp"
#include "source/newtonian/test_2d/piecewise.hpp"

using namespace std;
using namespace simulation2d;

namespace {

class SimData
{
public:

  SimData(Vector2D lower_left = Vector2D(0.02,0)+Vector2D(0,0),
	  Vector2D upper_right = Vector2D(0.02,0)+Vector2D(1,1)):
    mesh_(cartesian_mesh(50,50,lower_left,upper_right)),
    outer_(lower_left, upper_right),
    tess_(),
    interpm_(),
    eos_(5./3.),
    rs_(),
    hbc_(rs_),
    raw_point_motion_(),
    point_motion_(raw_point_motion_,hbc_),
    //    force_(),
    /*
    force_(Vector2D(0,0),
	   Vector2D(0,1)),
    */
    force_(Axis(Vector2D(0,0), Vector2D(0,1))),
    sim_(mesh_,
	 tess_,
	 interpm_,
	 Uniform2D(1),
	 Piecewise(Circle(Vector2D(0,0.5),0.05),
		   Uniform2D(1e5), Uniform2D(1)),
	 Uniform2D(0),
	 Uniform2D(0),
	 eos_,
	 rs_,
	 point_motion_,
	 force_,
	 outer_,
	 hbc_),
    pg_(Vector2D(0,0), Vector2D(0,1))
  {
    sim_.changePhysicalGeometry(&pg_);
  }

  hdsim& getSim(void)
  {
    return sim_;
  }
  
private:
  const vector<Vector2D> mesh_;
  const SquareBox outer_;
  VoronoiMesh tess_;
  PCM2D interpm_;
  const IdealGas eos_;
  const Hllc rs_;
  const RigidWallHydro hbc_;
  Lagrangian raw_point_motion_;
  RoundCells point_motion_;
  //CylindericalGeometry force_;
  //  ZeroForce force_;
  CylindricalComplementary force_;
  hdsim sim_;
  const CylindricalSymmetry pg_;
};

  namespace {
    class TotalConservedHistory: public DiagnosticFunction
    {
    public:

      TotalConservedHistory(const string& fname):
	fname_(fname), times_(), values_() {}

      void operator()(const hdsim& sim)
      {
	times_.push_back(sim.GetTime());
	values_.push_back(total_conserved(sim));
      }

      ~TotalConservedHistory(void)
      {
	ofstream f(fname_.c_str());
	for(size_t i=0;i<times_.size();++i)
	  f << times_[i] << " "
	    << values_[i].Mass << " "
	    << values_[i].Momentum.x << " "
	    << values_[i].Momentum.y << " "
	    << values_[i].Energy << "\n";
	f.close();
      }

    private:
      const string fname_;
      mutable vector<double> times_;
      mutable vector<Conserved> values_;
    };
  }

  void my_main_loop(hdsim& sim)
  {
    SafeTimeTermination term_cond(0.01,1e6);
    WriteTime diag("time.txt");
    //TotalConservedHistory diag("history.txt");
    main_loop(sim,
	      term_cond,
	      &hdsim::TimeAdvance,
	      &diag);
  }
}

int main(void)
{
  SimData sim_data;
  hdsim& sim = sim_data.getSim();

  my_main_loop(sim);

  write_snapshot_to_hdf5(sim, "final.h5");

  return 0;
}