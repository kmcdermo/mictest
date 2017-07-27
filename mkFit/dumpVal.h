#ifndef _dumpvals_
#define _dumpvals_

#include "Event.h"
#include "Track.h"

// dump in mplex
struct dumpval
{
  float minchi2=0.f,meanchi2=0.f,maxchi2=0.f;
  float minchi2_r=0.f,meanchi2_r=0.f,maxchi2_r=0.f;
  float phi=0.f,phiMin=0.f,phiMax=0.f,eta=0.f,etaMin=0.f,etaMax=0.f,pt=0.f,ptMin=0.f,ptMax=0.f;
  float diffPhiMin=0.f,diffPhiMax=0.f;
  int nHits=0,nHitsMin=0,nHitsMax=0,nHitsMatchedMin=0,nHitsMatchedMax=0;
  int nLayers=0,nLayersMin=0,nLayersMax=0,nLayersMatchedMin=0,nLayersMatchedMax=0;
  int evID=0,tkID=0,tkIDMin=0,tkIDMax=0;
};

void resetdumpval(dumpval& vals);
void fill_dump(Event *);

#endif
