#ifndef DIFFUSION_HPP
#define DIFFUSION_HPP 1

#include "conj_grad_solve.hpp"

using namespace CG;

//! \brief Abstract class for calculating the needed data for diffusion
class DiffusionCoefficientCalculator
{
public:
/*!
    \brief Calculates the diffusion coefficient
    \param index The index of the cell
    \param cells The primitive variables
    \return The diffusion coefficient (default units are cm^2/sec)
*/
virtual double CalcDiffusionCoefficient(size_t const index, std::vector<ComputationalCell3D> const& cells) const = 0;
/*!
    \brief Calculates the Planck opacity
    \param index The index of the cell
    \param cells The primitive variables
    \return The planck opacity (default units are 1/cm)
*/
virtual double CalcPlanckOpacity(size_t const index, std::vector<ComputationalCell3D> const& cells) const = 0;
};

//! \brief Class for assigning boundary conditions for diffusion
class DiffusionBoundaryCalculator
{
    public:
    /*!
\brief Sets the boundary values
\param tess The tesselation
\param index The index of the cell that is adjacent to a boundary
\param outside_point The index of the cell that is outside
\param cell The primitve cells
\param key_index The index of the tracer to calculate for
\param A the value in the A matrix to change, given as input and output
\param b the value in the b vector to change, given as input and output
\param Area The area of the interface between the two cells
\param dt The time step
    */
virtual void SetBoundaryValues(Tessellation3D const& tess, size_t const index, size_t const outside_point, double const dt,
    std::vector<ComputationalCell3D> const& cells, size_t const key_index, double const Area, double& A, double &b)const = 0;
};

//! \brief Class with constant blackbody temperature on the left x side and zero flux on other sides
class DiffusionSideBoundary : public DiffusionBoundaryCalculator
{
    public:
    /*!
    \brief Class constructor
    \param T Boundary temperature, in kelvin
    */
    DiffusionSideBoundary(double const T): T_(T){}

    void SetBoundaryValues(Tessellation3D const& tess, size_t const index, size_t const outside_point, double const dt,
        std::vector<ComputationalCell3D> const& cells, size_t const key_index, double const Area, double& A, double &b)const override;
    private:
        double const T_;
};

//! \brief Class for calculating diffusion matrix data for the CG solver
class Diffusion : public CG::MatrixBuilder
{
public:
/*!
\brief Class constructor
\param D_coefficient_calcualtor Class for calcualting the diffusion coefficients
*/
    Diffusion(DiffusionCoefficientCalculator const& D_coefficient_calcualtor, DiffusionBoundaryCalculator const& boundary_calc) : D_coefficient_calcualtor_(D_coefficient_calcualtor),
        boundary_calc_(boundary_calc) {}

    void BuildMatrix(Tessellation3D const& tess, mat& A, size_t_mat& A_indeces, std::vector<ComputationalCell3D> const& cells, std::string const& key_name,
            double const dt, std::vector<double>& b, std::vector<double>& x0) const override;
private:
    DiffusionCoefficientCalculator const& D_coefficient_calcualtor_;
    DiffusionBoundaryCalculator const& boundary_calc_;
    EquationOfState const& eos_;
};

//! D=D0*rho^alpha*T^beta
class PowerLawOpacity: public DiffusionCoefficientCalculator
{
private:
    double const D0_, alpha_, beta_;
public:
    PowerLawOpacity(double const D0, double const alpha, double const beta): D0_(D0), alpha_(alpha), beta_(beta){}

    double CalcDiffusionCoefficient(size_t const index, std::vector<ComputationalCell3D> const& cells) const override;

    double CalcPlanckOpacity(size_t const index, std::vector<ComputationalCell3D> const& cells) const override;
};

#endif