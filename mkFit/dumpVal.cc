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
  vals.minchi2=-99999.f,vals.minchi2_r=-99999.f;
  vals.phi=-99999.f,vals.phiMin=-99999.f,vals.phi0=-99999.f,vals.phiMinatR=-99999.f,vals.ephi=-99999.f;
  vals.diffphiMin=-99999.f,vals.diffphiMinatR=-99999.f,vals.diffphi0=-99999.f;
  vals.eta=-99999.f,vals.etaMin=-99999.f;
  vals.pt=-99999.f,vals.ptMin=-99999.f;
  vals.nHits=-99999,vals.nHitsMin=-99999,vals.nHitsMatchedMin=-99999;
  vals.nLayers=-99999,vals.nLayersMin=-99999,vals.nLayersMatchedMin=-99999;
  vals.evID=-99999,vals.tkID=-99999,vals.tkIDMin=-99999;
}

void fill_dump(Event * m_event)
{
  bool debug = false;

  std::ostringstream filename;
  filename << "dump_" << m_event->evtID() << ".root";
  TFile * outfile = TFile::Open(filename.str().c_str(),"recreate");

  // simple stats
  TTree * stattree = new TTree("stattree","stattree");
  int evID=-1,nmkFitTks=0,nCMSSWTks=0,nmkFitTks7=0,nCMSSWTks7=0;
  stattree->Branch("evID",&evID);
  stattree->Branch("nmkFitTks",&nmkFitTks);  
  stattree->Branch("nCMSSWTks",&nCMSSWTks);
  stattree->Branch("nmkFitTks7",&nmkFitTks7);  
  stattree->Branch("nCMSSWTks7",&nCMSSWTks7);
  
  evID = m_event->evtID();

  nmkFitTks = m_event->candidateTracks_.size();
  nCMSSWTks = m_event->extRecTracks_.size();

  for (auto& track : m_event->candidateTracks_)
  {
    track.sortHitsByLayer();
    if (track.nUniqueLayers() >= 7) nmkFitTks7++;
  }

  for (auto& track : m_event->extRecTracks_)
  {
    track.sortHitsByLayer();
    if (track.nUniqueLayers() >= 7) nCMSSWTks7++;
  }
  stattree->Fill();

  outfile->cd();
  stattree->Write();
  delete stattree;

  // full dump
  TTree * dumptree = new TTree("dumptree","dumptree");
  dumpval vals;
  dumptree->Branch("minchi2"  ,&vals.minchi2);
  dumptree->Branch("minchi2_r",&vals.minchi2_r);
  dumptree->Branch("phi"   ,&vals.phi);
  dumptree->Branch("phiMin",&vals.phiMin);
  dumptree->Branch("phi0"   ,&vals.phi0);
  dumptree->Branch("phiMinatR",&vals.phiMinatR);
  dumptree->Branch("ephi"   ,&vals.ephi);
  dumptree->Branch("diffphiMin",&vals.diffphiMin);
  dumptree->Branch("diffphi0",&vals.diffphi0);
  dumptree->Branch("diffphiMinatR",&vals.diffphiMinatR);
  dumptree->Branch("eta"   ,&vals.eta);
  dumptree->Branch("etaMin",&vals.etaMin);
  dumptree->Branch("pt"   ,&vals.pt);
  dumptree->Branch("ptMin",&vals.ptMin);
  dumptree->Branch("nHits"   ,&vals.nHits);
  dumptree->Branch("nHitsMin",&vals.nHitsMin);
  dumptree->Branch("nHitsMatchedMin",&vals.nHitsMatchedMin);
  dumptree->Branch("nLayers"   ,&vals.nLayers);
  dumptree->Branch("nLayersMin",&vals.nLayersMin);
  dumptree->Branch("nLayersMatchedMin",&vals.nLayersMatchedMin);
  dumptree->Branch("evID",&vals.evID);
  dumptree->Branch("tkID"   ,&vals.tkID);
  dumptree->Branch("tkIDMin",&vals.tkIDMin);

  // std::cout << "DUMP HITS" << std::endl;
  // for (int ilyr = 0; ilyr < 18; ilyr++)
  // {
  //   std::cout << "LAYER: " << ilyr << std::endl;
  //   for (int ihit = 0; ihit < m_event->layerHits_[ilyr].size(); ihit++)
  //   {
  //     const auto & hit = m_event->layerHits_[ilyr][ihit];
  //     std::cout << std::setw(5) << ihit << " [" << hit.mcHitID() << "] " << hit.x() << " " << hit.y() << " " << hit.z() << std::endl;
  //   }
  // }
  // std::cout << "===============================" << std::endl;

  // relabel recTracks
  for (int itrack = 0; itrack < m_event->extRecTracks_.size(); itrack++)
  {
    m_event->extRecTracksExtra_.emplace_back(m_event->extRecTracks_[itrack].label());
    m_event->extRecTracks_[itrack].setLabel(itrack);
  }
  
  for (auto& track : m_event->candidateTracks_)
  {
    resetdumpval(vals);
    vals.evID = m_event->evtID();

    float tmpminchi2 = 1e6;
    float tmpminchi2_r = 1e6;
    int tmpminlbl = -1;

    const SVector3 & recoParams = track.parameters().Sub<SVector3>(3);
    SMatrixSym33 recoErrs = track.errors().Sub<SMatrixSym33>(3,3);
    diagonalOnly(recoErrs);
    int invFail(0);
    const SMatrixSym33 & recoErrsI = recoErrs.InverseFast(invFail);
    for (auto& cmsswtrack : m_event->extRecTracks_)
    {
      const SVector3 & simParams = cmsswtrack.parameters().Sub<SVector3>(3);
      
      SVector3 diffParams = recoParams - simParams;
      squashPhiGeneral(diffParams);

      float chi2_r = 0;
      for (int iparam = 0; iparam < diffParams.kSize; iparam++)
      {
	const float chi2_i = diffParams[iparam]*diffParams[iparam] / recoErrs[iparam][iparam];
	if (iparam != diffParams.kSize-2) chi2_r += chi2_i;
      }

      const float chi2 = computeHelixChi2(simParams,recoParams,recoErrs);

      // normalize it
      chi2_r /= diffParams.kSize-2;
    
      if (chi2_r < tmpminchi2_r) {tmpminchi2_r = chi2_r; tmpminlbl = cmsswtrack.label(); tmpminchi2 = chi2;}
    }

    vals.minchi2 = tmpminchi2;
    vals.minchi2_r = tmpminchi2_r;

    vals.tkID = track.label();
    vals.phi = track.momPhi();
    vals.ephi= std::pow(track.emomPhi(),2);
    vals.eta = track.momEta();
    vals.pt = track.pT();
    vals.nHits = track.nFoundHits();
    vals.nLayers = track.nUniqueLayers();

    if (tmpminlbl != -1 && vals.nHits >= 11 && vals.minchi2_r < 20.f)
    {
      std::cout << "mkFit id: " << vals.tkID << " nTH: " << track.nTotalHits() << " nFH: " << track.nFoundHits() << " nUL: " << track.nUniqueLayers() 
		<< " pT: " << vals.pt << " mom. eta: " << vals.eta << " mom. phi: " << vals.phi 
		<< std::endl;
     
      std::vector<int> mcHitIDs;
      std::vector<int> unLayers;
      int tmplyr = -1;
      for (int i = 0; i < track.nTotalHits(); i++)
      {
	const int lyr = track.getHitLyr(i);
	const int idx = track.getHitIdx(i);
	int mcHitID = -1;
	int mcTrackID = -1;
	float x=-999,y=-999,z=-999;
	if (idx >= 0) 
	{
	  const auto & hit = m_event->layerHits_[lyr][idx];
	  x = hit.x(); y = hit.y(); z = hit.z();
	  mcHitID = m_event->layerHits_[lyr][idx].mcHitID(); 
	  mcHitIDs.push_back(mcHitID);
	  mcTrackID = m_event->simHitsInfo_[mcHitID].mcTrackID();
	}
	
	if (lyr >= 0 && idx>= 0 && tmplyr != lyr) {tmplyr = lyr; unLayers.push_back(lyr);}
	std::cout << " i: " << std::setw(2) << i 
		  << " lyr: " << std::setw(2) << lyr 
		  << " idx: " << std::setw(2) << idx 
		  << " [mcHitID: " << std::setw(3) << mcHitID 
		  << " mcTkID: " << std::setw(2) << mcTrackID
		  << "] x: " << std::setw(8) << std::setprecision(5) << x 
		  << " y: " << std::setw(8) << std::setprecision(5) << y
		  << " [r: " << std::setw(8) << std::setprecision(5) << getHypot(x,y) 
		  << " [phi: " << std::setw(8) << std::setprecision(5) << getPhi(x,y) 
		  << "] z: " << std::setw(8) << std::setprecision(5) << z << std::endl;
      }
      
      vals.tkIDMin = tmpminlbl;
      const Track& trackMin = m_event->extRecTracks_[vals.tkIDMin];
      vals.phiMin = trackMin.momPhi();
      vals.etaMin = trackMin.momEta();
      vals.ptMin = trackMin.pT();
      vals.nHitsMin = trackMin.nFoundHits();
      vals.nLayersMin = trackMin.nUniqueLayers();

      std::cout << "reduced chi2: " << vals.minchi2_r << std::endl;

      std::cout << "cmssw id: " << vals.tkIDMin << " nTH: " << trackMin.nTotalHits() << " nFH: " << trackMin.nFoundHits() << " nUL: " << trackMin.nUniqueLayers() 
		<< " pT: " << vals.ptMin << " mom. eta: " << vals.etaMin << " mom. phi: " << vals.phiMin 
		<< std::endl;
      std::vector<int> mcHitIDsMin;
      std::vector<int> unLayersMin;
      int tmplyrMin = -1;
      for (int i = 0; i < trackMin.nTotalHits(); i++)
      {
	const int lyr = trackMin.getHitLyr(i);
	const int idx = trackMin.getHitIdx(i);
	int mcHitID = -1;
	int mcTrackID = -1;
	float x=-999,y=-999,z=-999;
	if (idx >= 0) 
	{
	  const auto & hit = m_event->layerHits_[lyr][idx];
	  x = hit.x(); y = hit.y(); z = hit.z();
	  mcHitID = m_event->layerHits_[lyr][idx].mcHitID(); 
	  mcHitIDsMin.push_back(mcHitID);
	  mcTrackID = m_event->simHitsInfo_[mcHitID].mcTrackID();
	}

	if (lyr >= 0 && idx>=0 && tmplyrMin != lyr) {tmplyrMin = lyr; unLayersMin.push_back(lyr);}
	std::cout << " i: " << std::setw(2) << i 
		  << " lyr: " << std::setw(2) << lyr 
		  << " idx: " << std::setw(2) << idx 
		  << " [mcHitID: " << std::setw(3) << mcHitID 
		  << " mcTkID: " << std::setw(2) << mcTrackID
		  << "] x: " << std::setw(8) << std::setprecision(5) << x 
		  << " y: " << std::setw(8) << std::setprecision(5) << y
		  << " [r: " << std::setw(8) << std::setprecision(5) << getHypot(x,y) 
		  << " [phi: " << std::setw(8) << std::setprecision(5) << getPhi(x,y) 
		  << "] z: " << std::setw(8) << std::setprecision(5) << z << std::endl;
      }
      
      int tmpnHitsMatched = 0;
      for (auto mcHitID : mcHitIDs)
      {
	for (auto mcHitIDMin : mcHitIDsMin)
	{  
	  if (mcHitID == mcHitIDMin) tmpnHitsMatched++;
	}
      }      

      vals.nHitsMatchedMin = tmpnHitsMatched;

      int tmpnLyrsMatched = 0;
      for (auto lyr : unLayers)
      {
	for (auto lyrMin : unLayersMin)
	{  
	  if (lyr == lyrMin) tmpnLyrsMatched++;
	}
      }
      vals.nLayersMatchedMin = tmpnLyrsMatched;

      const SVector3 & simParamsMin = trackMin.parameters().Sub<SVector3>(3);
      SVector3 diffParamsMin = simParamsMin-recoParams;
      squashPhiGeneral(diffParamsMin);
      vals.diffphiMin = diffParamsMin[diffParamsMin.kSize-2];

      // compute phi extrapolations
      const float dr = getHypot(track.x()-trackMin.x(),track.y()-trackMin.y()); 
      const float recodphi = std::asin(dr/176./vals.pt);

      SVector3 recophi0;

      const float px = track.px(); const float py = track.py(); const float x = track.x(); const float y = track.y();
      const float a = 2.*(-0.1499)*3.8;
      const float T = std::sqrt(vals.pt*vals.pt - 2*a*(x*py-y*px) + a*a*(x*x+y*y));
      const float phi0 = std::acos( (px + a*y)/ T );

      recophi0[recophi0.kSize-2] = phi0; //vals.phi + recodphi;
      squashPhiGeneral(recophi0);
      vals.phi0 = recophi0[recophi0.kSize-2];

      SVector3 drecophi0;
      drecophi0[drecophi0.kSize-2] = vals.phi0 - vals.phiMin;
      squashPhiGeneral(drecophi0);
      vals.diffphi0 = drecophi0[drecophi0.kSize-2];

      const float simdphi = std::asin(dr/176./vals.ptMin);

      SVector3 simphiatR;
      simphiatR[simphiatR.kSize-2] = vals.phiMin - simdphi;
      squashPhiGeneral(simphiatR);
      vals.phiMinatR = simphiatR[simphiatR.kSize-2];

      SVector3 dsimphiatR;
      dsimphiatR[dsimphiatR.kSize-2] = vals.phi - vals.phiMinatR;
      squashPhiGeneral(dsimphiatR);
      vals.diffphiMinatR = dsimphiatR[dsimphiatR.kSize-2];
    }

    std::cout << "--------------------------------------------" << std::endl;

    dumptree->Fill();
  }

  outfile->cd();
  outfile->Write();

  delete dumptree;
  delete outfile;
}
#endif
