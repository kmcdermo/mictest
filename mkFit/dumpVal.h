#ifndef _dumpvals_
#define _dumpvals_

#include "Event.h"
#include "Track.h"

// dump in mplex
struct dumpval
{
  float chi2=-99999.f,chi2_d=-99999.f;
  float ipt_m=-99999.f,ipt_c=-99999.f,eipt_m=-99999.f;
  float phi_m=-99999.f,phi_c=-99999.f,ephi_m=-99999.f;
  float eta_m=-99999.f,eta_c=-99999.f,eeta_m=-99999.f;
  int charge_m=-99999,charge_c=-99999;
  int nHits_m=-99999,nHits_c=-99999,nHitsMatched=-99999;
  int nLayers_m=-99999,nLayers_c=-99999,nLayersMatched=-99999;
  int evID=-99999,tkID_m=-99999,tkID_c=-99999;
};

void resetdumpval(dumpval& vals);
void fill_dump(Event *);

#endif
