#ifndef _validation_
#define _validation_
#include "Track.h"
#include "BinInfoUtils.h"

class Event;

class Validation {
public:
  virtual void alignTrackExtra(TrackVec&, TrackExtraVec&) {}

  virtual void resetValidationMaps() {}
  virtual void resetDebugVectors() {}

  virtual void collectSeedPairInfo(int, const std::vector<int>&) {} 
  virtual void collectSeedTripletInfo(int, int, const std::vector<int>&) {} 
  virtual void collectSeedFilteredTripletInfo(int, int, int) {} 

  virtual void collectSimTkTSVecMapInfo(int, const TSVec&) {}
  virtual void collectSeedTkCFMapInfo(int, const TrackState&) {}
  virtual void collectSeedTkTSLayerPairVecMapInfo(int, const TSLayerPairVec&) {}
  virtual void collectBranchingInfo(int, int, float, float, int, float, int,
				    int, const std::vector<int>&, const std::vector<int>&) {}
  virtual void collectFitTkCFMapInfo(int, const TrackState&) {}
  virtual void collectFitTkTSLayerPairVecMapInfo(int, const TSLayerPairVec&) {}

  virtual void collectPropTSLayerVecInfo(int, const TrackState&) {} // exclusively for debugtree
  virtual void collectChi2LayerVecInfo(int, float) {} // exclusively for debugtree
  virtual void collectUpTSLayerVecInfo(int, const TrackState&) {} // exclusively for debugtree

  virtual void setTrackExtras(Event& ev) {}
  virtual void makeSimTkToRecoTksMaps(Event&) {}
  virtual void makeSeedTkToRecoTkMaps(Event&) {}

  virtual void fillSeedInfoTree(const TripletIdxVec&, const Event&) {}
  virtual void fillSeedCountTree(const TripletIdxVec&, const TripletIdxVec&, const Event&) {}
  virtual void fillDebugTree(const Event&) {}
  virtual void fillSegmentTree(const BinInfoMap&, int) {}
  virtual void fillBranchTree(int) {}
  virtual void fillEfficiencyTree(const Event&) {}
  virtual void fillFakeRateTree(const Event&) {}
  virtual void fillGeometryTree(const Event&) {}
  virtual void fillConformalTree(const Event&) {}
  virtual void fillConfigTree() {}
  virtual void fillTimeTree(const std::vector<double> &) {}
  virtual void fillSeedTree(const Event&) {}

  virtual void saveTTrees() {}
};

#endif
