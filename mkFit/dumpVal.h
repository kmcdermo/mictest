#ifndef _dumpvals_
#define _dumpvals_

#include "Event.h"
#include "Track.h"

// dump in mplex
struct dumpval
{
  float minchi2=0.f,minchi2_r=0.f;
  float phi=0.f,phiMin=0.f,phi0=0.f,phiMinatR=0.f,ephi=0.f;
  float eta=0.f,etaMin=0.f,pt=0.f,ptMin=0.f;
  float diffphiMin=0.f,diffphi0=0.f,diffphiMinatR=0.f;
  int nHits=0,nHitsMin=0,nHitsMatchedMin=0;
  int nLayers=0,nLayersMin=0,nLayersMatchedMin=0;
  int evID=0,tkID=0,tkIDMin=0;
};

void resetdumpval(dumpval& vals);
void fill_dump(Event *);

#endif
