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

void resetdumpval(dumpval& vals)
{
  vals.minchi2=-99999.f,vals.meanchi2=-99999.f,vals.maxchi2=-99999.f;
  vals.minchi2_r=-99999.f,vals.meanchi2_r=-99999.f,vals.maxchi2_r=-99999.f;
  vals.phi=-99999.f,vals.phiMin=-99999.f,vals.phiMax=-99999.f;
  vals.eta=-99999.f,vals.etaMin=-99999.f,vals.etaMax=-99999.f;
  vals.pt=-99999.f,vals.ptMin=-99999.f,vals.ptMax=-99999.f;
  vals.diffPhiMin=-99999.f,vals.diffPhiMax=-99999.f;
  vals.nHits=-99999,vals.nHitsMin=-99999,vals.nHitsMax=-99999,vals.nHitsMatchedMin=-99999,vals.nHitsMatchedMax=-99999;
  vals.nLayers=-99999,vals.nLayersMin=-99999,vals.nLayersMax=-99999,vals.nLayersMatchedMin=-99999,vals.nLayersMatchedMax=-99999;
  vals.evID=-99999,vals.tkID=-99999,vals.tkIDMin=-99999,vals.tkIDMax=-99999;
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
  dumptree->Branch("minchi2" ,&vals.minchi2);
  dumptree->Branch("meanchi2",&vals.meanchi2);
  dumptree->Branch("maxchi2" ,&vals.maxchi2);
  dumptree->Branch("minchi2_r" ,&vals.minchi2_r);
  dumptree->Branch("meanchi2_r",&vals.meanchi2_r);
  dumptree->Branch("maxchi2_r" ,&vals.maxchi2_r);
  dumptree->Branch("phi"   ,&vals.phi);
  dumptree->Branch("phiMin",&vals.phiMin);
  dumptree->Branch("phiMax",&vals.phiMax);
  dumptree->Branch("diffPhiMin",&vals.diffPhiMin);
  dumptree->Branch("diffPhiMax",&vals.diffPhiMax);
  dumptree->Branch("eta"   ,&vals.eta);
  dumptree->Branch("etaMin",&vals.etaMin);
  dumptree->Branch("etaMax",&vals.etaMax);
  dumptree->Branch("pt"   ,&vals.pt);
  dumptree->Branch("ptMin",&vals.ptMin);
  dumptree->Branch("ptMax",&vals.ptMax);
  dumptree->Branch("nHits"   ,&vals.nHits);
  dumptree->Branch("nHitsMin",&vals.nHitsMin);
  dumptree->Branch("nHitsMax",&vals.nHitsMax);
  dumptree->Branch("nHitsMatchedMin",&vals.nHitsMatchedMin);
  dumptree->Branch("nHitsMatchedMax",&vals.nHitsMatchedMax);
  dumptree->Branch("nLayers"   ,&vals.nLayers);
  dumptree->Branch("nLayersMin",&vals.nLayersMin);
  dumptree->Branch("nLayersMax",&vals.nLayersMax);
  dumptree->Branch("nLayersMatchedMin",&vals.nLayersMatchedMin);
  dumptree->Branch("nLayersMatchedMax",&vals.nLayersMatchedMax);
  dumptree->Branch("evID",&vals.evID);
  dumptree->Branch("tkID"   ,&vals.tkID);
  dumptree->Branch("tkIDMin",&vals.tkIDMin);
  dumptree->Branch("tkIDMax",&vals.tkIDMax);

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

    float tmpminchi2 = 1e6; float tmpmaxchi2 = -1e6; float tmpmeanchi2 = 0;
    float tmpminchi2_r = 1e6; float tmpmaxchi2_r = -1e6; float tmpmeanchi2_r = 0;
    int tmpminlbl = -1, tmpmaxlbl = -1;

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
      if (chi2_r > tmpmaxchi2_r) {tmpmaxchi2_r = chi2_r; tmpmaxlbl = cmsswtrack.label(); tmpmaxchi2 = chi2;}
      tmpmeanchi2 += chi2;
      tmpmeanchi2_r += chi2_r;
    }

    vals.minchi2 = tmpminchi2;
    vals.meanchi2 = tmpmeanchi2 / nCMSSWTks;
    vals.maxchi2 = tmpmaxchi2;

    vals.minchi2_r = tmpminchi2_r;
    vals.meanchi2_r = tmpmeanchi2_r / nCMSSWTks;
    vals.maxchi2_r = tmpmaxchi2_r;

    vals.tkID = track.label();
    vals.phi = track.momPhi();
    vals.eta = track.momEta();
    vals.pt = track.pT();
    vals.nHits = track.nFoundHits();
    vals.nLayers = track.nUniqueLayers();

    //    std::cout << vals.tkID << " " << track.nTotalHits() << " " << track.nFoundHits() << " " << track.nUniqueLayers() << std::endl;
    std::vector<int> mcHitIDs;
    std::vector<int> unLayers;
    int tmplyr = -1;
    for (int i = 0; i < track.nTotalHits(); i++)
    {
      const int lyr = track.getHitLyr(i);
      const int idx = track.getHitIdx(i);
      if (idx >= 0) mcHitIDs.push_back(m_event->layerHits_[lyr][idx].mcHitID());

      if (lyr >= 0 && idx>= 0 && tmplyr != lyr) {tmplyr = lyr; unLayers.push_back(lyr);}

      //      std::cout << std::setw(3) << i << " " << lyr << " " << idx << std::endl;
    }

    // std::cout << "      ";
    // for (auto hit : mcHitIDs) std::cout << std::setw(6) << hit << " ";
    // std::cout << std::endl;

    // std::cout << "      ";
    // for (auto lyr : unLayers) std::cout << std::setw(6) << lyr << " ";
    // std::cout << std::endl << std::endl;

    if (tmpminlbl != -1) 
    {
      vals.tkIDMin = tmpminlbl;
      const Track& trackMin = m_event->extRecTracks_[vals.tkIDMin];
      vals.phiMin = trackMin.momPhi();
      vals.etaMin = trackMin.momEta();
      vals.ptMin = trackMin.pT();
      vals.nHitsMin = trackMin.nFoundHits();
      vals.nLayersMin = trackMin.nUniqueLayers();

      std::vector<int> mcHitIDsMin;
      std::vector<int> unLayersMin;
      int tmplyrMin = -1;
      for (int i = 0; i < trackMin.nTotalHits(); i++)
      {
	const int lyr = trackMin.getHitLyr(i);
	const int idx = trackMin.getHitIdx(i);
	if (idx >= 0) mcHitIDsMin.push_back(m_event->layerHits_[lyr][idx].mcHitID());

	if (lyr >= 0 && idx>=0 && tmplyrMin != lyr) {tmplyrMin = lyr; unLayersMin.push_back(lyr);}
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
      vals.diffPhiMin = diffParamsMin[diffParamsMin.kSize-2];
    }

    if (tmpmaxlbl != -1) 
    {
      vals.tkIDMax = tmpmaxlbl;
      const Track& trackMax = m_event->extRecTracks_[vals.tkIDMax];
      vals.phiMax = trackMax.momPhi();
      vals.etaMax = trackMax.momEta();
      vals.ptMax = trackMax.pT();
      vals.nHitsMax = trackMax.nFoundHits();
      vals.nLayersMax = trackMax.nUniqueLayers();

      std::vector<int> mcHitIDsMax;
      std::vector<int> unLayersMax;
      int tmplyrMax = -1;
      for (int i = 0; i < trackMax.nTotalHits(); i++)
      {
	const int lyr = trackMax.getHitLyr(i);
	const int idx = trackMax.getHitIdx(i);
	if (idx >= 0) mcHitIDsMax.push_back(m_event->layerHits_[lyr][idx].mcHitID());

	if (lyr >= 0 && idx >=0 && tmplyrMax != lyr) {tmplyrMax = lyr; unLayersMax.push_back(lyr);}
      }
      
      int tmpnHitsMatched = 0;
      for (auto mcHitID : mcHitIDs)
      {
	for (auto mcHitIDMax : mcHitIDsMax)
	{  
	  if (mcHitID == mcHitIDMax) tmpnHitsMatched++;
	}
      }
      vals.nHitsMatchedMax = tmpnHitsMatched;

      int tmpnLyrsMatched = 0;
      for (auto lyr : unLayers)
      {
	for (auto lyrMax : unLayersMax)
	{  
	  if (lyr == lyrMax) tmpnLyrsMatched++;
	}
      }
      vals.nLayersMatchedMax = tmpnLyrsMatched;

      const SVector3 & simParamsMax = trackMax.parameters().Sub<SVector3>(3);
      SVector3 diffParamsMax = simParamsMax-recoParams;
      squashPhiGeneral(diffParamsMax);
      vals.diffPhiMax = diffParamsMax[diffParamsMax.kSize-2];
    }

    dumptree->Fill();
  }

  outfile->cd();
  outfile->Write();

  delete dumptree;
  delete outfile;
}
#endif
