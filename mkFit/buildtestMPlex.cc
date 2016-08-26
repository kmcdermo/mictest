#include "buildtestMPlex.h"

#include "Matrix.h"
#include "KalmanUtils.h"
#include "Propagation.h"
#include "Simulation.h"
#include "BinInfoUtils.h"

#include "MkBuilder.h"
#include "MkBuilderEndcap.h"

#include <omp.h>

#if defined(USE_VTUNE_PAUSE)
#include "ittnotify.h"
#endif

#include <memory>

inline bool sortByHitsChi2(const std::pair<Track, TrackState>& cand1,
                           const std::pair<Track, TrackState>& cand2)
{
  if (cand1.first.nFoundHits() == cand2.first.nFoundHits())
    return cand1.first.chi2() < cand2.first.chi2();

  return cand1.first.nFoundHits() > cand2.first.nFoundHits();
}

inline bool sortByPhi(const Hit& hit1, const Hit& hit2)
{
  return std::atan2(hit1.y(),hit1.x()) < std::atan2(hit2.y(),hit2.x());
}

inline bool sortByEta(const Hit& hit1, const Hit& hit2)
{
  return hit1.eta()<hit2.eta();
}

inline bool sortTracksByEta(const Track& track1, const Track& track2)
{
  return track1.momEta() < track2.momEta();
}

inline bool sortTracksByPhi(const Track& track1, const Track& track2)
{
  return track1.momPhi() < track2.momPhi();
}

struct sortTracksByPhiStruct
{
  const std::vector<std::vector<Track>>& m_track_candidates;

  sortTracksByPhiStruct(std::vector<std::vector<Track>>* track_candidates)
    : m_track_candidates( * track_candidates)
  {}

  bool operator() (const std::pair<int,int>& track1, const std::pair<int,int>& track2)
  {
    return m_track_candidates[track1.first][track1.second].posPhi() <
           m_track_candidates[track2.first][track2.second].posPhi();
  }
};

// within a layer with a "reasonable" geometry, ordering by Z is the same as eta
inline bool sortByZ(const Hit& hit1, const Hit& hit2)
{
  return hit1.z() < hit2.z();
}

namespace
{
  MkBuilder* make_builder()
  {
    if (Config::endcapTest) return new MkBuilderEndcap;
    else                    return new MkBuilder;
  }
}

//==============================================================================
// runBuildTestPlexBestHit
//==============================================================================

double runBuildingTestPlexBestHit(Event& ev)
{
  std::unique_ptr<MkBuilder> builder_ptr(make_builder());
  MkBuilder &builder = * builder_ptr.get();

  builder.begin_event(&ev, 0, __func__);

  if   (Config::findSeeds) {builder.find_seeds();}
  else                     {builder.map_seed_hits();} // all other simulated seeds need to have hit indices line up in LOH for seed fit

  //  builder.fit_seeds_tbb();

  EventOfCandidates event_of_cands;
  //  builder.find_tracks_load_seeds(event_of_cands);

// #ifdef USE_VTUNE_PAUSE
//   __itt_resume();
// #endif

  double time = dtime();

//   builder.FindTracksBestHit(event_of_cands);

//   time = dtime() - time;

// #ifdef USE_VTUNE_PAUSE
//   __itt_pause();
// #endif
  
  if   (!Config::normal_val) {builder.quality_output_besthit(event_of_cands);}
  else                       {builder.root_val_besthit(event_of_cands);}

  builder.end_event();
  
  return time;
}


//==============================================================================
// runBuildTestPlex
//==============================================================================

double runBuildingTestPlex(Event& ev, EventTmp& ev_tmp)
{
  EventOfCombCandidates &event_of_comb_cands = ev_tmp.m_event_of_comb_cands;
  event_of_comb_cands.Reset();

  std::unique_ptr<MkBuilder> builder_ptr(make_builder());
  MkBuilder &builder = * builder_ptr.get();

  builder.begin_event(&ev, &ev_tmp, __func__);

  if   (Config::findSeeds) {builder.find_seeds();}
  else                     {builder.map_seed_hits();} // all other simulated seeds need to have hit indices line up in LOH for seed fit

  builder.fit_seeds();

  builder.find_tracks_load_seeds();

#ifdef USE_VTUNE_PAUSE
  __itt_resume();
#endif

  double time = dtime();

  builder.FindTracks();

  time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
  __itt_pause();
#endif
  
  if   (!Config::normal_val) {builder.quality_output();}
  else                       {builder.root_val();}

  builder.end_event();

  return time;
}


//==============================================================================
// runBuildTestPlexCloneEngine
//==============================================================================

double runBuildingTestPlexCloneEngine(Event& ev, EventTmp& ev_tmp)
{
  EventOfCombCandidates &event_of_comb_cands = ev_tmp.m_event_of_comb_cands;
  event_of_comb_cands.Reset();

  std::unique_ptr<MkBuilder> builder_ptr(make_builder());
  MkBuilder &builder = * builder_ptr.get();

  builder.begin_event(&ev, &ev_tmp, __func__);

  if   (Config::findSeeds) {builder.find_seeds();}
  else                     {builder.map_seed_hits();} // all other simulated seeds need to have hit indices line up in LOH for seed fit

  builder.fit_seeds();

  builder.find_tracks_load_seeds();

#ifdef USE_VTUNE_PAUSE
  __itt_resume();
#endif

  double time = dtime();

  builder.FindTracksCloneEngine();

  time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
  __itt_pause();
#endif

  if   (!Config::normal_val) {builder.quality_output();}
  else                       {builder.root_val();}

  builder.end_event();

  return time;
}


//==============================================================================
// runBuildTestPlexCloneEngineTbb
//==============================================================================

double runBuildingTestPlexTbb(Event& ev, EventTmp& ev_tmp)
{
  EventOfCombCandidates &event_of_comb_cands = ev_tmp.m_event_of_comb_cands;
  event_of_comb_cands.Reset();

  std::unique_ptr<MkBuilder> builder_ptr(make_builder());
  MkBuilder &builder = * builder_ptr.get();

  builder.begin_event(&ev, &ev_tmp, __func__);

  if   (Config::findSeeds) {builder.find_seeds();}
  else                     {builder.map_seed_hits();} // all other simulated seeds need to have hit indices line up in LOH for seed fit

  builder.fit_seeds_tbb();

  builder.find_tracks_load_seeds();

#ifdef USE_VTUNE_PAUSE
  __itt_resume();
#endif

  double time = dtime();

  builder.FindTracksCloneEngineTbb();

  time = dtime() - time;

#ifdef USE_VTUNE_PAUSE
  __itt_pause();
#endif

  if   (!Config::normal_val) {builder.quality_output();}
  else                       {builder.root_val();}

  builder.end_event();

  return time;
}
