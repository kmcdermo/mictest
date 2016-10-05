#include "fittestMPlex.h"

#include "Matrix.h"
#include "PropagationMPlex.h"
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
#include <fstream>
#include <string>

#if defined(USE_VTUNE_PAUSE)
#include "ittnotify.h"
#endif

typedef std::pair<int,int> IIPair;

inline bool isBadLabel     (const Track& track){return track.label()<0;}
inline bool isNotEnoughHits(const Track& track){return track.nFoundHits()<Config::nLayers/2;}
//inline bool sortByLessNhits(const Track& track1, const Track& track2){return track1.nFoundHits()<track2.nFoundHits();}
inline bool sortByLessNhits(const IIPair& pair1, const IIPair& pair2){return pair1.second<pair2.second;}

void mergeSimTksIntoSeedTks(std::vector<Track>& simtracks, std::vector<Track>& seedtracks)
{
  seedtracks.erase(std::remove_if(seedtracks.begin(),seedtracks.end(),isBadLabel),seedtracks.end());

  for (auto&& seedtrack : seedtracks)
  {
    // since seedtrack.label() == mcTrackID
    // and simtracks indices == mcTrackID
    // just append seedtrack with hit indices AFTER the seed's hits
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

void prepSimTracks(const std::vector<Track>& simtracks, VecOfIIPairs& tkidxsTonHits, std::map<int,int>& nHitsToTks)
{
  // make map of track indices + nhits, sort it by nhits
  for (int isim = 0; isim < simtracks.size(); isim++){tkidxsTonHits[isim] = std::make_pair(isim,simtracks[isim].nFoundHits());}
  std::sort(tkidxsTonHits.begin(),tkidxsTonHits.end(),sortByLessNhits);

  // make temp map of raw nhit counts
  std::map<int,int> rawCounts;
  for (auto&& pair : tkidxsTonHits){rawCounts[pair.second]++;}

  // make map of start/end of sorted tracks by nHits (from 3 hits --only seeds, to 17 hits --all layers)
  nHitsToTks[Config::nlayers_per_seed-1] = 0;
  for (int ilay = Config::nlayers_per_seed; ilay < Config::nLayers+1; ilay++){nHitsToTks[ilay] = rawCounts[ilay] + nHitsToTks[ilay-1];}
}

void readInHits(std::vector<Track>& simtracks, int evt)
{
  std::ifstream hitpattern;

  std::string intStr = std::to_string(evt-1);
  std::string filename = "hitpatterns/ev"+intStr+".txt";
  hitpattern.open(filename.c_str(),std::ios::in);

  int hitids[Config::nLayers];
  int itk = 0;
  while (hitpattern >> hitids[0] >> hitids[1] >> hitids[2] >> hitids[3] >> hitids[4] >> hitids[5] >> hitids[6] >> hitids[7] >> hitids[8]
	 >> hitids[9] >> hitids[10] >> hitids[11] >> hitids[12] >> hitids[13] >> hitids[14] >> hitids[15] >> hitids[16]) 
  {
    auto & simtrack = simtracks[itk];
    for (int ilay = 0; ilay < Config::nLayers; ilay++)
    {
      if (hitids[ilay] == 0) simtrack.setHitIdx(ilay,-1);
    }
    itk++;
  }
  hitpattern.close();
}

void addFakeHits(std::vector<Track>& simtracks, std::vector<HitVec>& layerHits)
{
  // add fake hits to the end of the layerhits vector
  // since we propagate to the "exact" radius with R as the parameter
  // simply just make the components of the hit to be (r, 0, 0)
  // and to zero the KF gain --> make the uncertainity infinite on diagonal, zero on the off diagonal (i.e. 1e15)

  const float big = 1e15;
  for (int hi = 0; hi < Config::nLayers; hi++)
  {
    const SVector3 tmppos(Config::fRadialSpacing*(hi+1),0,0);
    SMatrixSym33   tmperr = ROOT::Math::SMatrixIdentity();
    tmperr(0,0) = big; 
    tmperr(1,1) = big; 
    tmperr(2,2) = big;
    int detid = getDetId(tmppos[0],tmppos[2]);
    Hit tmphit(tmppos,tmperr,detid,-1);
    layerHits[hi].push_back(tmphit);
  }
  
  // now make a small map of the layer sizes
  std::map<int,int> laysizes;
  for (int hi = 0; hi < Config::nLayers; hi++)
  {
    laysizes[hi] = layerHits[hi].size()-1;
  }
  
  for (auto&& simtrack : simtracks)
  {
    for (int hi = 0; hi < Config::nLayers; hi++)
    {
      if (simtrack.getHitIdx(hi) < 0) simtrack.setHitIdx(hi,laysizes[hi]);
    }
  }
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
	  mkfp->FitTracksDetIds(end - itrack, &ev);
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
  readInHits(simtracks,ev.evtID());

  // std::vector<Track>& seedtracks = ev.seedTracks_;
  // mergeSimTksIntoSeedTks(simtracks,seedtracks);
  fittracks.resize(simtracks.size());

#ifdef USE_VTUNE_PAUSE
  __itt_resume();
#endif

  double time = dtime();

  VecOfIIPairs tkidxsTonHits(simtracks.size());
  std::map<int,int> nHitsToTks;
  prepSimTracks(simtracks,tkidxsTonHits,nHitsToTks);

  tbb::parallel_for(tbb::blocked_range<int>(Config::nlayers_per_seed, Config::nLayers+1),
		    [&](const tbb::blocked_range<int>& nhits)
  {
    for (int nhit = nhits.begin(); nhit != nhits.end(); ++nhit)
    {
      const int theBeg = ((nhit != Config::nlayers_per_seed) ? nHitsToTks[nhit-1] : 0);
      const int theEnd = nHitsToTks[nhit];
      const int count  = (theEnd - theBeg + NN - 1)/NN;
    
      tbb::parallel_for(tbb::blocked_range<int>(0, count, std::max(1, Config::numSeedsPerTask/NN)),
			[&](const tbb::blocked_range<int>& i)
      {
	std::unique_ptr<MkFitter, decltype(retfitr)> mkfp(g_exe_ctx.m_fitters.GetFromPool(), retfitr);
	mkfp->SetNhits(nhit);
	for (int it = i.begin(); it < i.end(); ++it)
	{
	  int itrack = theBeg+it*NN;
	  int end = itrack + NN;

	  // copy/slurp In equivalents
	  if (theEnd < end) 
          {
	    end = theEnd;
	    mkfp->InputTrackGoodLayers(simtracks, itrack, end, tkidxsTonHits);
	    mkfp->InputSortedTracksAndHits(simtracks, ev.layerHits_, itrack, end, tkidxsTonHits);
	  } else {
	    mkfp->InputTrackGoodLayers(simtracks, itrack, end, tkidxsTonHits);
	    mkfp->SlurpInSortedTracksAndHits(simtracks, ev.layerHits_, itrack, end, tkidxsTonHits); // only safe for a full matriplex
	  }
	  mkfp->FitSortedTracks(end - itrack, &ev);
	  mkfp->OutputSortedFittedTracks(fittracks, itrack, end, tkidxsTonHits);
	}
      });
    }
  });

  time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
  __itt_pause();
#endif
  
  ev.Validate();
				     
  return time;
}


double runFittingTestPlexFakeHits(Event& ev, std::vector<Track>& fittracks)
{
  g_exe_ctx.populate(Config::numThreadsFinder);

  // get the tracks ready for processing
  std::vector<Track>& simtracks  = ev.simTracks_;
  readInHits(simtracks,ev.evtID());
  fittracks.resize(simtracks.size());
  
  // assume that fake hits already added in building
  addFakeHits(simtracks,ev.layerHits_);

  const int Nhits = Config::nLayers;
  
  int theEnd = simtracks.size();
  int count  = (theEnd + NN - 1)/NN;
  
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
	
	if (theEnd < end) 
	{
	  end = theEnd;
	  mkfp->InputTracksAndHits(simtracks, ev.layerHits_, itrack, end);
	} else {
	  mkfp->SlurpInTracksAndHits(simtracks, ev.layerHits_, itrack, end); // only safe for a full matriplex
	}
	mkfp->FitTracksDetIds(end - itrack, &ev);
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
