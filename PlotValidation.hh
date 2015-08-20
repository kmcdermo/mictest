#include "TFile.h"
#include "TH1F.h"
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TDirectory.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

typedef std::vector<UInt_t> *   UIntVecRef;
typedef std::vector<UIntVecRef> UIntVecRefVec;

typedef std::vector<Float_t> *    FltVecRef;
typedef std::vector<FltVecRef>    FltVecRefVec;
typedef std::vector<FltVecRefVec> FltVecRefVecVec;

typedef std::vector<TString> TStrVec;

typedef std::vector<UInt_t>  UIntVec;

typedef std::vector<Int_t>   IntVec;
typedef std::vector<IntVec>  IntVecVec;

typedef std::vector<Float_t> FltVec;
typedef std::vector<FltVec>  FltVecVec;

typedef std::vector<TH1F *>        TH1FRefVec;
typedef std::vector<TH1FRefVec>    TH1FRefVecVec;
typedef std::vector<TH1FRefVecVec> TH1FRefVecVecVec;

class PlotValidation
{
public:
  PlotValidation(TString inName, TString outName, TString outType);
  ~PlotValidation();
  void Validation(Bool_t mvInput = false);
  void PlotBranching();
  void PlotTiming();
  void PlotSimGeo();
  void PlotNHits();
  void PlotCFResidual();
  void PlotCFResolutionPull();
  void PlotPosResolutionPull();
  void PlotMomResolutionPull();
  void PlotEfficiency();
  void PlotFakeRate();
  void PlotDuplicateRate();
  void PrintTotals();

  void MakeSubDirectory(TDirectory *& subdir, const TString subdirname);

  void ComputeResidual      (const Float_t mcvar_val, const Float_t recovar_val, Float_t & var_out);
  void ComputeResolutionPull(const Float_t mcvar_val, const Float_t recovar_val, const Float_t recovar_err, FltVec & var_out);

  void ComputeRatioPlot(const TH1F * numer, const TH1F * denom, TH1F *& ratioPlot);

  void ZeroSuppressPlot(TH1F *& histo);

  void WriteTH2FPlot        (TDirectory *& subdir, TH2F *& hist);
  void DrawWriteSaveTH2FPlot(TDirectory *& subdir, TH2F *& histo, const TString subdirname, const TString plotName);

  void WriteTH1FPlot           (TDirectory *& subdir, TH1F *& histo);
  void DrawWriteSaveTH1FPlot   (TDirectory *& subdir, TH1F *& histo, const TString subdirname, const TString plotName, const Bool_t setLogy);
  void DrawWriteFitSaveTH1FPlot(TDirectory *& subdir, TH1F *& histo, const TString subdirname, const TString plotName, const Float_t fitrange, const Bool_t setLogy);

  void MoveInput();

private:
  TString fInName;
  TFile * fInRoot;
  TString fOutType;
  TString fOutName;
  TFile * fOutRoot;
  TCanvas * fTH1Canv;
  TCanvas * fTH2Canv;
};
