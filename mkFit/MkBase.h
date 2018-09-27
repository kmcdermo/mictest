#ifndef MkBase_h
#define MkBase_h

#include "Matrix.h"

#include "PropagationMPlex.h"

#include <mutex>
#include <iostream>

static std::mutex mtx_;

//run --cmssw-n2seeds --sim-val --input-file /data2/slava77/samples/2017/pass-c93773a/initialStep/PU70HS/10224.0_TTbar_13+TTbar_13TeV_TuneCUETP8M1_2017PU_GenSimFullINPUT+DigiFullPU_2017PU+RecoFullPU_2017PU+HARVESTFullPU_2017PU/memoryFile.fv3.clean.writeAll.recT.082418-25daeda.bin --build-std --backward-fit-pca --num-thr 64 --num-thr-ev 32

namespace mkfit {

//==============================================================================
// MkBase
//==============================================================================

class MkBase
{
public:
  MPlexLS    Err[2];
  MPlexLV    Par[2];

  MPlexQI    Chg;

  static constexpr int iC = 0; // current
  static constexpr int iP = 1; // propagated

  float getPar(int itrack, int i, int par) const { return Par[i].ConstAt(itrack, par, 0); }

  //----------------------------------------------------------------------------

  MkBase() {}

  //----------------------------------------------------------------------------

  void PropagateTracksToR(float r, const int N_proc, const PropagationFlags pf)
  {
    MPlexQF msRad;
#pragma omp simd
    for (int n = 0; n < NN; ++n)
    {
      msRad.At(n, 0, 0) = r;
    }

    propagateHelixToRMPlex(Err[iC], Par[iC], Chg, msRad,
                           Err[iP], Par[iP], N_proc, pf);
  }

  void PropagateTracksToHitR(const MPlexHV& par, const int N_proc, const PropagationFlags pf)
  {
    MPlexQF msRad;
#pragma omp simd
    for (int n = 0; n < NN; ++n)
    {
      msRad.At(n, 0, 0) = std::hypot(par.ConstAt(n, 0, 0), par.ConstAt(n, 1, 0));
    }

    propagateHelixToRMPlex(Err[iC], Par[iC], Chg, msRad,
                           Err[iP], Par[iP], N_proc, pf);
  }

  //----------------------------------------------------------------------------

  void PropagateTracksToZ(float z, const int N_proc, const PropagationFlags pf)
  {
    MPlexQF msZ;
#pragma omp simd
    for (int n = 0; n < NN; ++n)
    {
      msZ.At(n, 0, 0) = z;
    }

    propagateHelixToZMPlex(Err[iC], Par[iC], Chg, msZ,
                           Err[iP], Par[iP], N_proc, pf);
  }

  void PropagateTracksToHitZ(const MPlexHV& par, const int N_proc, const PropagationFlags pf)
  {
    MPlexQF msZ;
#pragma omp simd
    for (int n = 0; n < NN; ++n)
    {
      msZ.At(n, 0, 0) = par.ConstAt(n, 2, 0);
    }

    propagateHelixToZMPlex(Err[iC], Par[iC], Chg, msZ,
                           Err[iP], Par[iP], N_proc, pf);
  }

  void PropagateTracksToPCAZ(const int N_proc, const PropagationFlags pf)
  {
    MPlexQF msZ; // PCA z-coordinate
#pragma omp simd
    for (int n = 0; n < NN; ++n)
    {
      const float slope = std::tan(Par[iC].ConstAt(n, 5, 0));
      //      msZ.At(n, 0, 0) = ( Config::beamspotz0 + slope * ( Config::beamspotr0 - std::hypot(Par[iC].ConstAt(n, 0, 0), Par[iC].ConstAt(n, 1, 0))) + slope * slope * Par[iC].ConstAt(n, 2, 0) ) / ( 1+slope*slope); // PCA w.r.t. z0, r0
      msZ.At(n, 0, 0) = (slope * (slope * Par[iC].ConstAt(n, 2, 0) - std::hypot(Par[iC].ConstAt(n, 0, 0), Par[iC].ConstAt(n, 1, 0)))) / (1 + slope * slope); // PCA to origin
    } 

    propagateHelixToZMPlex(Err[iC], Par[iC], Chg, msZ,
                           Err[iP], Par[iP], N_proc, pf);
  }

};

} // end namespace mkfit
#endif
