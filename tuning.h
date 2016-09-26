#include "TStyle.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TVirtualFitter.h"
#include "TF1.h"
#include "TFormula.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TSystem.h"

#include <array>

typedef std::array<Double_t, 3> arr3;  

inline Double_t rad2 (Double_t x, Double_t y){return x*x + y*y;}
void tuning();
void fittingCore(TH1F *& hist);
void doFit(TH1F *& hist, TF1 *& fit, arr3 & temps);
void getFitParams(TF1 *& fit, Double_t & mean, Double_t & emean, Double_t & sigma, Double_t & esigma);
void dumpFit(TF1*& fit, arr3 & temps);
void drawFit(TF1 *& fit, TCanvas *& canv, TF1 *& sub1, TF1 *& sub2, TF1 *& sub3);
void drawStats(TF1 *& fit, TPaveText *& stats, TCanvas *& canv);
void deleteAll(TF1 *& fit, TF1 *& sub1, TF1 *& sub2, TF1 *& sub3, TPaveText *& text, TCanvas *& lincanv, TPaveText *& stats, TCanvas *& logcanv);
