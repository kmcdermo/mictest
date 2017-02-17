#ifndef _fittest_mplex_
#define _fittest_mplex_

#include "Event.h"
#include "Track.h"

#ifdef USE_CUDA
#include "FitterCU.h"
#endif

#include <map>

void prepFitTracks(std::vector<Track>& fittracks, std::map<int,int>& nHitsToTks);

void   make_validation_tree(const char         *fname,
                            std::vector<Track> &simtracks,
                            std::vector<Track> &rectracks);

double runFittingTestPlex(Event& ev, std::vector<Track>& rectracks);

double runFittingTestPlexSortedBuiltTracks(Event& ev, std::vector<Track>& plextracks);

#ifdef USE_CUDA
void runAllEventsFittingTestPlexGPU(std::vector<Event>& events);
double runFittingTestPlexGPU(FitterCU<float> &cuFitter, Event& ev, std::vector<Track>& rectracks);
#endif

#endif
