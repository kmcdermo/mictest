#ifndef _fittest_mplex_
#define _fittest_mplex_

#include "Event.h"
#include "Track.h"

#ifdef USE_CUDA
#include "FitterCU.h"
#endif

#include <map>

void   mergeSimTksIntoSeedTks(std::vector<Track>& simtracks,
			      std::vector<Track>& seedtracks);
void   prepSeedTracks(const std::vector<Track>& seedtracks,
		      VecOfIIPairs& tkidxsTonHits,
		      std::map<int,int>& nHitsToTks);
void   make_validation_tree(const char         *fname,
                            std::vector<Track> &simtracks,
                            std::vector<Track> &fittracks);
double runFittingTestPlex(Event& ev, std::vector<Track>& fittracks);
double runFittingTestPlexSortedTracks(Event& ev, std::vector<Track>& fittracks);

#ifdef USE_CUDA
double runFittingTestPlexGPU(FitterCU<float> &cuFitter, Event& ev, std::vector<Track>& fittracks);
#endif

#endif
