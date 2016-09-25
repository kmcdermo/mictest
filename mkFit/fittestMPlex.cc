#include "fittestMPlex.h"

#include "Matrix.h"
#include "KalmanUtils.h"
#include "Propagation.h"
#include "Simulation.h"
#include "Geometry.h"
//#define DEBUG
#include <Debug.h>

#include "MkFitter.h"
#if USE_CUDA
#include "FitterCU.h"
#endif

#ifndef NO_ROOT
#include "TFile.h"
#include "TTree.h"
#include <mutex>
#endif

#include <tbb/tbb.h>
#include <omp.h>

#include <iostream>
#include <memory>
#include <limits>

#if defined(USE_VTUNE_PAUSE)
#include "ittnotify.h"
#endif

inline bool isBadLabel     (const Track& track){return track.label()<0;}
inline bool isNotEnoughHits(const Track& track){return track.nFoundHits()<Config::nLayers/2;}
inline bool sortByLessNhits(const Track& track1, const Track& track2){return track1.nFoundHits()<track2.nFoundHits();}

void mergeSimTksIntoSeedTks(std::vector<Track>& simtracks, std::vector<Track>& seedtracks)
{
  seedtracks.erase(std::remove_if(seedtracks.begin(),seedtracks.end(),isBadLabel),seedtracks.end());

  for (auto&& seedtrack : seedtracks)
  {
    Track& simtrack = simtracks[seedtrack.label()];
    for (int hi = Config::nlayers_per_seed; hi < Config::nLayers; ++hi)
    {
      seedtrack.setHitIdx(hi,simtrack.getHitIdx(hi));
    }
    simtrack.setNGoodHitIdx(); // for validation
    seedtrack.setNGoodHitIdx();
  }

  if (Config::prune_tracks) seedtracks.erase(std::remove_if(seedtracks.begin(),seedtracks.end(),isNotEnoughHits),seedtracks.end()); 
}

void prepSeedTracks(std::vector<Track>& seedtracks, std::map<int,int>& nHitsToTks)
{
  std::sort(seedtracks.begin(),seedtracks.end(),sortByLessNhits);
  for (auto&& seedtrack : seedtracks){nHitsToTks[seedtrack.nFoundHits()]++;}
}

//==============================================================================

void make_validation_tree(const char         *fname,
                          std::vector<Track> &simtracks,
                          std::vector<Track> &fittracks)
{
   assert(simtracks.size() == fittracks.size());

   float pt_mc, pt_fit, pt_err, chg;
   int goodtrk = 0;

#ifndef NO_ROOT
   static std::mutex roolock;

   std::lock_guard<std::mutex> rooguard(roolock);

   TFile *file = TFile::Open(fname, "recreate");
   TTree *tree = new TTree("T", "Validation Tree for simple Kalman fitter");;

   tree->Branch("pt_mc",  &pt_mc,  "pt_mc");
   tree->Branch("pt_fit", &pt_fit, "pt_fit");
   tree->Branch("pt_err", &pt_err, "pt_err");
   tree->Branch("chg",    &chg,    "chg");
#endif

   std::vector<float> diff_vec;

   const int NT = simtracks.size();
   for (int i = 0; i < NT; ++i)
   {
      pt_mc  = simtracks[i].pT();
      pt_fit = fittracks[i].pT();
      pt_err = fittracks[i].epT() / pt_fit;
      chg = simtracks[i].charge();

#ifndef NO_ROOT
      tree->Fill();
#endif

      float pr = pt_fit/pt_mc;
      float diff = (pt_mc/pt_fit - 1) / pt_err;
      if (pr > 0.8 && pr < 1.2 && std::isfinite(diff)) {
        diff_vec.push_back(diff);
        ++goodtrk;
      } else {
        dprint("pt_mc, pt_fit, pt_err, ratio, diff " << pt_mc << " " << pt_fit << " " << pt_err << " " << pt_fit/pt_mc << " " << diff);
      }
   }
   float mean = std::accumulate(diff_vec.begin(), diff_vec.end(), 0.0)
              / diff_vec.size();

   std::transform(diff_vec.begin(), diff_vec.end(), 
                  diff_vec.begin(),  // in-place
                  [mean](float x) {return (x-mean)*(x-mean);});
                  
   float stdev = std::sqrt(
       std::accumulate(diff_vec.begin(), diff_vec.end(), 0.0)
       / diff_vec.size());

   std::cerr << goodtrk << " good tracks, mean pt pull: " << mean
             << " standard dev: " << stdev << std::endl;

#ifndef NO_ROOT
   file->Write();
   file->Close();
   delete file;
#endif
}

//==============================================================================
// runFittingTestPlex
//==============================================================================

#include "Pool.h"
namespace
{
  struct ExecutionContext
  {
    Pool<MkFitter>   m_fitters;

    void populate(int n_thr)
    {
      m_fitters.populate(n_thr - m_fitters.size());
    }
  };

  ExecutionContext g_exe_ctx;
  auto retfitr = [](MkFitter*   mkfp  ) { g_exe_ctx.m_fitters.ReturnToPool(mkfp);   };
}

double runFittingTestPlex(Event& ev, std::vector<Track>& fittracks)
{
   g_exe_ctx.populate(Config::numThreadsFinder);
   std::vector<Track>& simtracks  = ev.simTracks_;
   std::vector<Track>& seedtracks = ev.seedTracks_;

   const int Nhits = Config::nLayers;
   // XXX What if there's a missing / double layer?
   // Eventually, should sort track vector by number of hits!
   // And pass the number in on each "setup" call.
   // Reserves should be made for maximum possible number (but this is just
   // measurments errors, params).

   int theEnd = ( (Config::endcapTest && Config::readCmsswSeeds) ? seedtracks.size() : simtracks.size());
   int count = (theEnd + NN - 1)/NN;

#ifdef USE_VTUNE_PAUSE
   __itt_resume();
#endif

   double time = dtime();

   tbb::parallel_for(tbb::blocked_range<int>(0, count, std::max(1, Config::numSeedsPerTask/NN)),
     [&](const tbb::blocked_range<int>& i)
   {
     std::unique_ptr<MkFitter, decltype(retfitr)> mkfp(g_exe_ctx.m_fitters.GetFromPool(), retfitr);
     mkfp->SetNhits(Nhits);
     for (int it = i.begin(); it < i.end(); ++it)
     {
        int itrack = it*NN;
        int end = itrack + NN;

	if (Config::endcapTest) { 
	  //fixme, check usage of SlurpInTracksAndHits for endcapTest
	  if (Config::readCmsswSeeds) {
	    mkfp->InputSeedsTracksAndHits(seedtracks,simtracks, ev.layerHits_, itrack, end);
	  } else {
	    mkfp->InputTracksAndHits(simtracks, ev.layerHits_, itrack, end);
	  }
	  mkfp->FitTracksTestEndcap(end - itrack, &ev);
	} else { // barrel test
	  if (theEnd < end) {
	    end = theEnd;
	    mkfp->InputTracksAndHits(simtracks, ev.layerHits_, itrack, end);
	  } else {
	    mkfp->SlurpInTracksAndHits(simtracks, ev.layerHits_, itrack, end); // only safe for a full matriplex
	  }
	  if (Config::cf_fitting) mkfp->ConformalFitTracks(true, itrack, end);
	  mkfp->FitTracks(end - itrack, &ev);
	}
	mkfp->OutputFittedTracks(fittracks, itrack, end);
     }
   });

   time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
   __itt_pause();
#endif

   ev.Validate();

   return time;
}

double runFittingTestPlexSortedTracks(Event& ev, std::vector<Track>& fittracks)
{
  g_exe_ctx.populate(Config::numThreadsFinder);

  // prepare track collections
  std::vector<Track>& simtracks  = ev.simTracks_;
  std::vector<Track>& seedtracks = ev.seedTracks_;
  mergeSimTksIntoSeedTks(simtracks,seedtracks);

#ifdef USE_VTUNE_PAUSE
  __itt_resume();
#endif
  
  double time = dtime();

  std::map<int,int> nHitsToTks;
  prepSeedTracks(seedtracks,nHitsToTks);
  fittracks.resize(seedtracks.size());

  std::map<int,int> lbl2idx;
  int lbl = 18029;
  for (int iseed = 0; iseed < seedtracks.size(); iseed++)
  {
    const auto& seedtrack = seedtracks[iseed];
    const int tklbl = seedtrack.label();
    lbl2idx[tklbl] = iseed;
    //    if (tkidx == 64 || tkidx == 86 || tkidx == 52 || tkidx == 88)
    if (tklbl == lbl)
    {
      std::cout << tklbl << " (" << iseed << "): ";
      for (int il = 0; il < Config::nLayers; il++)
      {
      	std::cout << seedtrack.getHitIdx(il) << " ";
      }
      std::cout << std::endl;
    }
  }

  std::vector<Track> onetrack; 
  onetrack.push_back(seedtracks[lbl2idx[lbl]]);
  MkFitter * mkf1 = new MkFitter();
  mkf1->SetNhits(Config::nLayers);

  // const float    big = std::numeric_limits<float>::infinity();
  // const SVector3 tmppos(-5.44678,24.9934,-18.3289);
  // SMatrixSym33   tmperr = ROOT::Math::SMatrixIdentity();
  // tmperr(0,0) = big; tmperr(0,1) = big; tmperr(0,2) = big;
  // tmperr(1,0) = big; tmperr(1,1) = big; tmperr(1,2) = big;
  // tmperr(2,0) = big; tmperr(2,1) = big; tmperr(2,2) = big;
  // Hit tmphit(tmppos,tmperr,-1);
  // ev.layerHits_[4].push_back(tmphit);
  // onetrack[0].setHitIdx(4,ev.layerHits_[4].size()-1);

  //  mkf1->InputTrackGoodLayers(seedtracks, lbl2idx[lbl], lbl2idx[lbl]+1);
  mkf1->InputTracksAndHits(onetrack, ev.layerHits_, 0, 1);
  mkf1->FitOneTrack(&ev);
  mkf1->OutputFittedTracks(onetrack, 0, 1);

  delete mkf1;

  exit(0);

  int previdx = 0;
  for (auto&& indexinfo : nHitsToTks)
  {
    const int theLocalEnd  = indexinfo.second;
    const int theGlobalEnd = previdx+theLocalEnd;
    const int count = (theLocalEnd + NN - 1)/NN;
    
    tbb::parallel_for(tbb::blocked_range<int>(0, count, std::max(1, Config::numSeedsPerTask/NN)),
      [&](const tbb::blocked_range<int>& i)
    {
      std::unique_ptr<MkFitter, decltype(retfitr)> mkfp(g_exe_ctx.m_fitters.GetFromPool(), retfitr);
      mkfp->SetNhits(indexinfo.first);
      for (int it = i.begin(); it < i.end(); ++it)
      {
	int itrack = previdx+it*NN;
	int end = itrack + NN;

	// "compactify" matriplexes first with only the relevant layers
	// even though tracks now grouped by nHits
	// distribution of hits on layers different between tracks!

	// copy/slurp In equivalents
       	if (theGlobalEnd < end) {
	  end = theGlobalEnd;
	  mkfp->InputTrackGoodLayers(seedtracks, itrack, end); 
	  mkfp->InputSortedTracksAndHits(seedtracks, ev.layerHits_, itrack, end);
	} else {
	  mkfp->InputTrackGoodLayers(seedtracks, itrack, end); 
	  mkfp->SlurpInSortedTracksAndHits(seedtracks, ev.layerHits_, itrack, end); // only safe for a full matriplex
	}
	
	// do the fit over the block and then output the compactified mplexes
	mkfp->FitSortedTracks(end - itrack, &ev);
	mkfp->OutputSortedFittedTracks(fittracks, itrack, end);
      }
    });
    previdx += theLocalEnd;
  }

  exit(0);

  time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
  __itt_pause();
#endif
  
  ev.Validate();
				     
  return time;
}

#ifdef USE_CUDA
double runFittingTestPlexGPU(FitterCU<float> &cuFitter, 
    Event& ev, std::vector<Track>& fittracks)
{

   std::vector<Track>& simtracks = ev.simTracks_;

   const int Nhits = MAX_HITS;
   // XXX What if there's a missing / double layer?
   // Eventually, should sort track vector by number of hits!
   // And pass the number in on each "setup" call.
   // Reserves should be made for maximum possible number (but this is just
   // measurments errors, params).

   // NOTE: MkFitter *MUST* be on heap, not on stack!
   // Standard operator new screws up alignment of ALL MPlex memebrs of MkFitter,
   // even if one adds attr(aligned(64)) thingy to every possible place.

   // MkFitter *mkfp = new (_mm_malloc(sizeof(MkFitter), 64)) MkFitter(Nhits);

   MkFitter* mkfp_arr = new (_mm_malloc(sizeof(MkFitter), 64)) MkFitter(Nhits);

   int theEnd = simtracks.size();
   double time = dtime();
   int Nstride = NN;

   for (int itrack = 0; itrack < theEnd; itrack += Nstride)
   {
      int end = std::min(itrack + Nstride, theEnd);

      MkFitter *mkfp = mkfp_arr;

      //double time_input = dtime();
      mkfp->InputTracksAndHits(simtracks, ev.layerHits_, itrack, end);
      //std::cerr << "Input time: " << (dtime() - time_input)*1e3 << std::endl;

      cuFitter.FitTracks(mkfp->Chg,
                         mkfp->GetPar0(),
                         mkfp->GetErr0(),
                         mkfp->msPar,
                         mkfp->msErr,
                         Nhits,
                         simtracks, itrack, end, ev.layerHits_);

      double time_output = dtime();
      mkfp->OutputFittedTracks(fittracks, itrack, end);
      std::cerr << "Output time: " << (dtime() - time_output)*1e3 << std::endl;
   }

   time = dtime() - time;

   _mm_free(mkfp_arr);

   return time;
}
#endif
