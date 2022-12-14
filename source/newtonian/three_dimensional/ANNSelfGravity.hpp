#ifndef ANNTREESG_HPP
#define ANNTREESG_HPP 1

#include "ConservativeForce3D.hpp"
#include <string>

//! \brief ANN based self gravity
class ANNSelfGravity : public Acceleration3D
{
private:
	const double opening_;
  #ifdef RICH_MPI
	Tessellation3D const* tproc_;
#endif // RICH_MPI
	std::string const d_name_;
  ANNSelfGravity(const ANNSelfGravity &/*other*/):opening_(0),
#ifdef RICH_MPI
						  tproc_(0),
#endif // RICH_MPI
						  d_name_("") {};
  ANNSelfGravity& operator=(const ANNSelfGravity /*other*/) 
  {
#if RICH_MPI
    this->tproc_ = 0;
#endif // RICH_MPI
    return *this; 
  }
public:
#ifdef RICH_MPI
  //! \param tproc Process tessellation
#endif // RICH_MPI
  /*! \brief Class constructor
    \param opening Opening angle
    \param debug_name Logger file name
   */
	ANNSelfGravity(double opening = 0.25,
#ifdef RICH_MPI
		       Tessellation3D const* tproc=0,
#endif // RICH_MPI
		       const std::string& debug_name = "");

	void operator()(const Tessellation3D& tess, const vector<ComputationalCell3D>& cells,
		const vector<Conserved3D>& fluxes, const double time, TracerStickerNames const& tracerstickernames,
		vector<Vector3D> &acc) const override;
	~ANNSelfGravity() {}
};

#endif //ANNTREESG_HPP

