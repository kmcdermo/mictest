#include "dumpVal.h"

#ifdef NO_ROOT
void resetdumpval(dumpval& vals){}
void fill_dump(const Event *){}
#else

#include "TFile.h"
#include "TTree.h"

#include <sstream>
#include <vector>

void resetdumpval(dumpval& vals)
{
  vals.minchi2=-99999.f,vals.meanchi2=-99999.f,vals.maxchi2=-99999.f;
  vals.phi=-99999.f,vals.phiMin=-99999.f,vals.phiMax=-99999.f,vals.eta=-99999.f,vals.etaMin=-99999.f,vals.etaMax=-99999.f,vals.pt=-99999.f,vals.ptMin=-99999.f,vals.ptMax=-99999.f;
  vals.nHits=-99999,vals.nHitsMin=-99999,vals.nHitsMax=-99999,vals.nHitsMatchedMin=-99999,vals.nHitsMatchedMax=-99999;
  vals.evID=-99999,vals.tkID=-99999,vals.tkIDMin=-99999,vals.tkIDMax=-99999;
}

void fill_dump(const Event * m_event)
{
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
    if (track.nUniqueLayers() >= 7) nmkFitTks7++;
  }

  for (auto& track : m_event->extRecTracks_)
  {
    if (track.nUniqueLayers() >= 7) nCMSSWTks7++;
  }

  stattree->Write();

  // full dump
  TTree * dumptree = new TTree("dumptree","dumptree");
  dumpval vals;
  dumptree->Branch("minchi2" ,&vals.minchi2);
  dumptree->Branch("meanchi2",&vals.meanchi2);
  dumptree->Branch("maxchi2" ,&vals.maxchi2);
  dumptree->Branch("phi"   ,&vals.phi);
  dumptree->Branch("phiMin",&vals.phiMin);
  dumptree->Branch("phiMax",&vals.phiMax);
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
  dumptree->Branch("evID",&vals.evID);
  dumptree->Branch("tkID"   ,&vals.tkID);
  dumptree->Branch("tkIDMin",&vals.tkIDMin);
  dumptree->Branch("tkIDMax",&vals.tkIDMax);

  // relabel recTracks
  for (int itrack = 0; itrack < m_event->extRecTracks_.size(); itrack++)
  {
    

  }
  

  for (auto& track : m_event->extRecTracks_)
  {
    std::cout << track.label() << " pt: " << track.pT() << " phi: " << track.momPhi() << " eta: " << track.momEta() << std::endl;
  }

  std::cout << "------------------------" << std::endl;

  for (auto& track : m_event->candidateTracks_)
  {
    std::cout << std::endl << std::endl;
    std::cout << track.label() << " pt: " << track.pT() << " phi: " << track.momPhi() << " eta: " << track.momEta() << std::endl;

    resetdumpval(vals);
    vals.evID = m_event->evtID();

    float tmpminchi2 = 1e6; float tmpmaxchi2 = -1e6; float tmpmeanchi2 = 0;
    int tmpminlbl = -1, tmpmaxlbl = -1;

    const SVector3 & recoParams = track.parameters().Sub<SVector3>(3);
    for (auto& cmsswtrack : m_event->extRecTracks_)
    {
      const SVector3 & simParams = cmsswtrack.parameters().Sub<SVector3>(3);
      const SMatrixSym33 & recoErrs = track.errors().Sub<SMatrixSym33>(3,3);
      const float chi2 = computeHelixChi2(simParams,recoParams,recoErrs);

      std::cout << "  " << cmsswtrack.label() << " " << chi2 << std::endl;


      if (chi2 < tmpminchi2) {tmpminchi2 = chi2; tmpminlbl = cmsswtrack.label();}
      if (chi2 > tmpmaxchi2) {tmpmaxchi2 = chi2; tmpmaxlbl = cmsswtrack.label();}
      tmpmeanchi2 += chi2;
    }

    vals.minchi2 = tmpminchi2;
    vals.meanchi2 = tmpmeanchi2 / nCMSSWTks;
    vals.maxchi2 = tmpmaxchi2;
    
    vals.tkID = track.label();
    vals.phi = track.momPhi();
    vals.eta = track.momEta();
    vals.pt = track.pT();
    vals.nHits = track.nFoundHits();

    //    std::cout << "  " << tmpminlbl << std::endl;

    if (tmpminlbl != -1) 
    {
      vals.tkIDMin = tmpminlbl;
      const Track& trackMin = m_event->extRecTracks_[vals.tkIDMin];
      vals.phiMin = trackMin.momPhi();
      vals.etaMin = trackMin.momEta();
      vals.ptMin = trackMin.pT();
      vals.nHitsMin = trackMin.nFoundHits();

      std::vector<int> mcHitIDs;
      for (int i = 0; i < track.nTotalHits(); i++)
      {
	const int lyr = track.getHitLyr(i);
	const int idx = track.getHitIdx(i);
	mcHitIDs.push_back(m_event->layerHits_[lyr][idx].mcHitID());
      }

      std::vector<int> mcHitIDsMin;
      for (int i = 0; i < trackMin.nTotalHits(); i++)
      {
	const int lyr = trackMin.getHitLyr(i);
	const int idx = trackMin.getHitIdx(i);
	mcHitIDsMin.push_back(m_event->layerHits_[lyr][idx].mcHitID());
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
    }

    //    std::cout << "  " << tmpmaxlbl << std::endl;

    if (tmpmaxlbl != -1) 
    {
      vals.tkIDMax = tmpmaxlbl;
      const Track& trackMax = m_event->extRecTracks_[vals.tkIDMax];
      vals.phiMax = trackMax.momPhi();
      vals.etaMax = trackMax.momEta();
      vals.ptMax = trackMax.pT();
      vals.nHitsMax = trackMax.nFoundHits();

      std::vector<int> mcHitIDs;
      for (int i = 0; i < track.nTotalHits(); i++)
      {
	const int lyr = track.getHitLyr(i);
	const int idx = track.getHitIdx(i);
	mcHitIDs.push_back(m_event->layerHits_[lyr][idx].mcHitID());
      }

      std::vector<int> mcHitIDsMax;
      for (int i = 0; i < trackMax.nTotalHits(); i++)
      {
	const int lyr = trackMax.getHitLyr(i);
	const int idx = trackMax.getHitIdx(i);
	mcHitIDsMax.push_back(m_event->layerHits_[lyr][idx].mcHitID());
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
    }

    dumptree->Fill();
  }

  outfile->cd();
  outfile->Write();

  delete dumptree;
  delete stattree;
  delete outfile;
}
#endif
