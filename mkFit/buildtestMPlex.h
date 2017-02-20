#ifndef _buildtest_mplex_
#define _buildtest_mplex_

#include "Event.h"
#include "EventTmp.h"
#include "Track.h"

#include <fstream>

class MkBuilder;

double runBuildingTestPlexBestHit(Event& ev, MkBuilder& builder, std::ofstream& times);
double runBuildingTestPlexStandard(Event& ev, EventTmp& ev_tmp, MkBuilder& builder);
double runBuildingTestPlexCloneEngine(Event& ev, EventTmp& evtmp, MkBuilder& builder);

#if USE_CUDA
double runAllBuildingTestPlexBestHitGPU(std::vector<Event> &events);
#endif

#endif
