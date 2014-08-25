/*! \file spatial_reconstruction.hpp
  \brief Abstract class for interpolation of the hydrodynamic variables
  \author Almog Yalinewich
*/

#ifndef SPATIAL_RECONSTRUCTION_HPP
#define SPATIAL_RECONSTRUCTION_HPP 1

#include "../common/hydrodynamic_variables.hpp"
#include "../../tessellation/tessellation.hpp"
#include "ReducedPrimitiveGradient2D.hpp"

//! \brief Type of interpolation
enum InterpolationType{InBulk,Boundary};

/*! \brief Calculates the central point of the edge
  \param edge Edge
  \return Center of the edge
*/
Vector2D CalcCentroid(Edge const& edge);

/*! \brief Spatial reconstruction of the primitive functions
  \author Almog Yalinewich
*/
class SpatialReconstruction
{
public:

  /*! \brief Calculates the slopes for all of the cells
    \param tessellation The tessellation
    \param cells The primitive cells
    \param tracers Tracers
    \param dt The time step
    \param time The simulation time
  */
  virtual void Prepare(Tessellation const& tessellation,
		       vector<Primitive> const& cells,
		       vector<vector<double> > const& tracers,
		       double dt,double time) = 0;

  /*! \brief Interpolates the hydrodynamic variables near the edge
    \param tessellation Point and edge positions
    \param cells Hydrodynamic variables
    \param dt Time step
    \param edge The face on which the Riemann problem will be solved
    \param side The side of the edge
    \param interptype Type of interpolation
    \param vface The face of the velocity
    \return The interpolated primitive
  */
  virtual Primitive Interpolate
  (Tessellation const& tessellation,
   vector<Primitive> const& cells,
   double dt,
   Edge const& edge,
   int side,
   InterpolationType interptype,
   Vector2D const& vface) const = 0;

  /*! \brief Interpolates the scalar tracers near the edge
    \param tess Point and edge positions
    \param cells Hydrodynamic variables
    \param tracers The scalar tracers
    \param dt Time step
    \param edge The face on which the Riemann problem will be solved
    \param side The side of the edge
    \param interp_type Type of interpolation
    \param vface The face of the velocity
    \return The interpolated tracer
  */
  virtual vector<double> interpolateTracers
  (Tessellation const& tess,vector<Primitive> const& cells,
   vector<vector<double> > const& tracers,double dt,Edge const& edge,int side,
   InterpolationType interp_type,Vector2D const& vface) const = 0;

  //! \brief Virtual destructor
  virtual ~SpatialReconstruction(void);
  /*!
  \brief Returns a reference to the gradients
  \return The gradients
  */
  virtual vector<ReducedPrimitiveGradient2D>& GetGradients(void)=0;
};

#endif // SPATIAL_RECONSTRUCTION_HPP