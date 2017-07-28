#ifndef MkBuilder_h
#define MkBuilder_h

#include <vector>

//------------------------------------------------------------------------------

#include "CandCloner.h"
#include "MkFitter.h"
#include "MkFinder.h"
#include "SteeringParams.h"

#include <functional>
#include <mutex>

#include "dumpVal.h"

#include "Pool.h"

class TrackerInfo;
class LayerInfo;

//------------------------------------------------------------------------------

struct ExecutionContext
{
  Pool<CandCloner> m_cloners;
  Pool<MkFitter>   m_fitters;
  Pool<MkFinder>   m_finders;

  void populate(int n_thr)
  {
    m_cloners.populate(n_thr - m_cloners.size());
    m_fitters.populate(n_thr - m_fitters.size());
    m_finders.populate(n_thr - m_finders.size());
  }
};

extern ExecutionContext g_exe_ctx;

//==============================================================================
// The usual
//==============================================================================

class Event;

class MkBuilder
{
protected:
  void fit_one_seed_set(TrackVec& simtracks, int itrack, int end, MkFitter *mkfttr,
                        const bool is_brl[], const SteeringParams &st_par);

  Event                 *m_event;
  EventOfHits            m_event_of_hits;
  EventOfCombCandidates  m_event_of_comb_cands;

  int m_cnt=0, m_cnt1=0, m_cnt2=0, m_cnt_8=0, m_cnt1_8=0, m_cnt2_8=0, m_cnt_nomc=0;

  SteeringParams   m_steering_params[5];
  std::vector<int> m_brl_ecp_regions;

public:
  typedef std::vector<std::pair<int,int>> CandIdx_t;

  MkBuilder();
  ~MkBuilder();

  // --------

  static MkBuilder* make_builder();

  void begin_event(Event* ev, const char* build_type);
  void end_event();

  void create_seeds_from_sim_tracks();
  void import_seeds();
  void find_seeds();
  void fit_seeds();

  // --------

  void map_seed_hits(); // m_event->layerHits_ -> m_event_of_hits.m_layers_of_hits (seeds only)
  void remap_seed_hits(); // m_event_of_hits.m_layers_of_hits -> m_event->layerHits_ (seeds only)
  void remap_cand_hits(); // m_event_of_hits.m_layers_of_hits -> m_event->layerHits_ (cands only)
  void align_recotracks(); // ensure that all track collections have labels equal to index inside vector

  void quality_output_BH();
  void quality_output_COMB();
  void quality_reset();
  void quality_process(Track& tkcand);
  void quality_print();

  void quality_store_tracks_COMB();

  void dump_rec_tracks();

  void root_val_BH();
  void root_val_COMB();
  void init_track_extras();

  // --------

  void find_tracks_load_seeds_BH(); // for FindTracksBestHit
  void find_tracks_load_seeds();
  int  find_tracks_unroll_candidates(std::vector<std::pair<int,int>> & seed_cand_vec,
                                     int start_seed, int end_seed, int layer);
  void find_tracks_in_layers(CandCloner &cloner, MkFinder *mkfndr,
                             int start_seed, int end_seed, int region);

  // --------

  void PrepareSeeds();

  void FindTracksBestHit();
  void FindTracksStandard();
  void FindTracksCloneEngine();

#ifdef USE_CUDA
  const Event* get_event() const { return m_event; }
  const EventOfHits& get_event_of_hits() const { return m_event_of_hits; }
#endif
};

#endif
