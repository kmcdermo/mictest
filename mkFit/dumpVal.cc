#include "dumpVal.h"

#ifdef NO_ROOT
void resetdumpval(dumpval& vals){}
void fill_dump(Event *){}
#else

#include "TFile.h"
#include "TTree.h"
#include "Debug.h"
#define DEBUG

#include <sstream>
#include <vector>
#include <iomanip>

void resetdumpval(dumpval& vals)
{
  vals.chi2=-99999.f,vals.chi2_d=-99999.f;
  vals.chi2_wp=-99999.f,vals.chi2_d_wp=-99999.f;
  vals.ipt_m=-99999.f,vals.ipt_c=-99999.f,vals.eipt_m=-99999.f;
  vals.phi_m=-99999.f,vals.phi_c=-99999.f,vals.diffPhi=-99999.f,vals.ephi_m=-99999.f;
  vals.eta_m=-99999.f,vals.eta_c=-99999.f,vals.eeta_m=-99999.f;
  vals.charge_m=-99999,vals.charge_c=-99999;
  vals.nHits_m=-99999,vals.nHits_c=-99999,vals.nHitsMatched=-99999;
  vals.nLayers_m=-99999,vals.nLayers_c=-99999,vals.nLayersMatched=-99999;
  vals.evID=-99999,vals.tkID_m=-99999,vals.tkID_c=-99999;
}

void fill_dump(Event * m_event)
{
  const bool debug = false;
  const bool diagonly = true;
  
  std::ostringstream filename;
  filename << "dump_" << m_event->evtID() << ".root";
  TFile * outfile = TFile::Open(filename.str().c_str(),"recreate");

  // simple stats
  TTree * stattree = new TTree("stattree","stattree");
  int evID=-1,nmkFitTks=0,nCMSSWTks=0,nmkFitTks11=0,nCMSSWTks11=0;
  stattree->Branch("evID",&evID);
  stattree->Branch("nmkFitTks",&nmkFitTks);  
  stattree->Branch("nCMSSWTks",&nCMSSWTks);
  stattree->Branch("nmkFitTks11",&nmkFitTks11);  
  stattree->Branch("nCMSSWTks11",&nCMSSWTks11);
  
  evID = m_event->evtID();

  nmkFitTks = m_event->candidateTracks_.size();
  nCMSSWTks = m_event->extRecTracks_.size();

  for (auto& track : m_event->candidateTracks_)
  {
    track.sortHitsByLayer();
    if (track.nUniqueLayers() >= 11) nmkFitTks11++;
  }

  for (auto& track : m_event->extRecTracks_)
  {
    track.sortHitsByLayer();
    if (track.nUniqueLayers() >= 11) nCMSSWTks11++;
  }
  stattree->Fill();

  outfile->cd();
  stattree->Write();
  delete stattree;

  // full dump
  TTree * dumptree = new TTree("dumptree","dumptree");
  dumpval vals;
  dumptree->Branch("chi2"  ,&vals.chi2);
  dumptree->Branch("chi2_d",&vals.chi2_d);
  dumptree->Branch("chi2_wp"  ,&vals.chi2_wp);
  dumptree->Branch("chi2_d_wp",&vals.chi2_d_wp);
  dumptree->Branch("ipt_m" ,&vals.ipt_m);
  dumptree->Branch("ipt_c" ,&vals.ipt_c);
  dumptree->Branch("eipt_m",&vals.eipt_m);
  dumptree->Branch("phi_m" ,&vals.phi_m);
  dumptree->Branch("phi_c" ,&vals.phi_c);
  dumptree->Branch("diffPhi",&vals.diffPhi);
  dumptree->Branch("ephi_m",&vals.ephi_m);
  dumptree->Branch("eta_m" ,&vals.eta_m);
  dumptree->Branch("eta_c" ,&vals.eta_c);
  dumptree->Branch("eeta_m",&vals.eeta_m);
  dumptree->Branch("charge_m",&vals.charge_m);
  dumptree->Branch("charge_c",&vals.charge_c);
  dumptree->Branch("nHits_m",&vals.nHits_m);
  dumptree->Branch("nHits_c",&vals.nHits_c);
  dumptree->Branch("nHitsMatched",&vals.nHitsMatched);
  dumptree->Branch("nLayers_m",&vals.nLayers_m);
  dumptree->Branch("nLayers_c",&vals.nLayers_c);
  dumptree->Branch("nLayersMatched",&vals.nLayersMatched);
  dumptree->Branch("evID"  ,&vals.evID);
  dumptree->Branch("tkID_m",&vals.tkID_m);
  dumptree->Branch("tkID_c",&vals.tkID_c);

  // relabel recTracks
  for (int itrack = 0; itrack < m_event->extRecTracks_.size(); itrack++)
  {
    m_event->extRecTracksExtra_.emplace_back(m_event->extRecTracks_[itrack].label());
    m_event->extRecTracks_[itrack].setLabel(itrack);
  }

  // _d == diagonal
  // _m == mkFit
  // _c == CMSSW
  for (auto&& mkfittrack : m_event->candidateTracks_)
  {
    resetdumpval(vals);
    vals.evID = m_event->evtID();

    // mkFit parameters
    const SVector3 & mkfitParams = mkfittrack.parameters().Sub<SVector3>(3);
    //    const SMatrixSym33 & mkfitErrs = mkfittrack.errors().Sub<SMatrixSym33>(3,3);
    SMatrixSym33 mkfitErrs = mkfittrack.errors().Sub<SMatrixSym33>(3,3);
    mkfitErrs[1][1] += 0.00002739;

    vals.tkID_m = mkfittrack.label();
    vals.ipt_m  = mkfitParams[0];
    vals.eipt_m = mkfitErrs[0][0];
    vals.phi_m  = mkfitParams[1];
    vals.ephi_m = mkfitErrs[1][1];
    vals.eta_m  = mkfitParams[2];
    vals.eeta_m = mkfitErrs[2][2];
    vals.charge_m  = mkfittrack.charge();
    vals.nHits_m   = mkfittrack.nFoundHits();
    vals.nLayers_m = mkfittrack.nUniqueLayers();

    // temps need for chi2
    // SVector2 mkfitParamsR;
    // mkfitParamsR[0] = mkfitParams[0];
    // mkfitParamsR[1] = mkfitParams[2];
    
    // SMatrixSym22 mkfitErrsR;
    // mkfitErrsR[0][0] = mkfitErrs[0][0];
    // mkfitErrsR[1][1] = mkfitErrs[2][2];
    // mkfitErrsR[0][1] = mkfitErrs[0][2];
    // mkfitErrsR[1][0] = mkfitErrs[2][0];
    
    const float x = mkfittrack.x(), y = mkfittrack.y();
    float chi2 = 1e6, chi2_d = 1e6;
    int   tkID_c = -1;
    for (auto&& cmsswtrack : m_event->extRecTracks_)
    {
      SVector3 cmsswParams = cmsswtrack.parameters().Sub<SVector3>(3);
      cmsswParams[1] = cmsswtrack.swimPhiToR(x,y);

      const float tmpchi2   = computeHelixChi2(cmsswParams,mkfitParams,mkfitErrs,false);
      const float tmpchi2_d = computeHelixChi2(cmsswParams,mkfitParams,mkfitErrs,true);

      // SVector2 cmsswParams;
      // cmsswParams[0] = cmsswParams[0];
      // cmsswParams[1] = cmsswParams[2];
      
      // const float tmpchi2   = computeHelixChi2(cmsswParams,mkfitParamsR,mkfitErrsR,false);
      // const float tmpchi2_d = computeHelixChi2(cmsswParams,mkfitParamsR,mkfitErrsR,true);
      
      if (tmpchi2 < chi2) {chi2 = tmpchi2; tkID_c = cmsswtrack.label(); chi2_d = tmpchi2_d;}
      //if (tmpchi2_d < chi2_d) {chi2 = tmpchi2; tkID_c = cmsswtrack.label(); chi2_d = tmpchi2_d;}
    }

    // store both chi2s for now
    vals.chi2   = chi2;
    vals.chi2_d = chi2_d;

    if (tkID_c != -1) // extra info from a "good" match
    {
      const Track& cmsswtrack = m_event->extRecTracks_[tkID_c];
      
      // CMSSW parameters
      SVector3 cmsswParams = cmsswtrack.parameters().Sub<SVector3>(3);
      cmsswParams[1] = cmsswtrack.swimPhiToR(x,y);

      vals.tkID_c = cmsswtrack.label();
      vals.ipt_c  = cmsswParams[0];
      vals.phi_c  = cmsswParams[1];
      vals.diffPhi= squashPhiGeneral(vals.phi_m-vals.phi_c);
      vals.eta_c  = cmsswParams[2];
      vals.charge_c  = cmsswtrack.charge();
      vals.nHits_c   = cmsswtrack.nFoundHits();
      vals.nLayers_c = cmsswtrack.nUniqueLayers();

      // TEMPORARY
      vals.chi2_wp   = computeHelixChi2(cmsswParams,mkfitParams,mkfitErrs,false);
      vals.chi2_d_wp = computeHelixChi2(cmsswParams,mkfitParams,mkfitErrs,true);
      
      // count mkFit good hits
      std::vector<int> mcHitIDs_m;
      std::vector<int> unLayers_m;
      int tmplyr_m = -1;
      for (int i = 0; i < mkfittrack.nTotalHits(); i++)
      {
	const int lyr = mkfittrack.getHitLyr(i);
	const int idx = mkfittrack.getHitIdx(i);
	int mcHitID = -1;
	float x=-999,y=-999,z=-999;
	if (idx >= 0) 
	{
	  const auto & hit = m_event->layerHits_[lyr][idx];
	  x = hit.x(); y = hit.y(); z = hit.z();
	  mcHitID = m_event->layerHits_[lyr][idx].mcHitID(); 
	  mcHitIDs_m.push_back(mcHitID);
	}
	
	if (lyr >= 0 && idx>= 0 && tmplyr_m != lyr) {tmplyr_m = lyr; unLayers_m.push_back(lyr);}
      }

      // count CMSSW good hits
      std::vector<int> mcHitIDs_c;
      std::vector<int> unLayers_c;
      int tmplyr_c = -1;
      for (int i = 0; i < cmsswtrack.nTotalHits(); i++)
      {
	const int lyr = cmsswtrack.getHitLyr(i);
	const int idx = cmsswtrack.getHitIdx(i);
	int mcHitID = -1;
	float x=-999,y=-999,z=-999;
	if (idx >= 0) 
	{
	  const auto & hit = m_event->layerHits_[lyr][idx];
	  x = hit.x(); y = hit.y(); z = hit.z();
	  mcHitID = m_event->layerHits_[lyr][idx].mcHitID(); 
	  mcHitIDs_c.push_back(mcHitID);
	}

	if (lyr >= 0 && idx>=0 && tmplyr_c != lyr) {tmplyr_c = lyr; unLayers_c.push_back(lyr);}
      }
      
      int nHitsMatched = 0;
      for (auto mcHitID_m : mcHitIDs_m)
      {
	for (auto mcHitID_c : mcHitIDs_c)
	{  
	  if (mcHitID_m == mcHitID_c) nHitsMatched++;
	}
      }      
      vals.nHitsMatched = nHitsMatched;

      int nLyrsMatched = 0;
      for (auto lyr_m : unLayers_m)
      {
	for (auto lyr_c : unLayers_c)
	{  
	  if (lyr_m == lyr_c) nLyrsMatched++;
	}
      }
      vals.nLayersMatched = nLyrsMatched;

    }

    dumptree->Fill();
  }

  outfile->cd();
  outfile->Write();

  delete dumptree;
  delete outfile;
}
#endif
