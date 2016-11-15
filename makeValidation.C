#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <vector>

void setupcpp11()
{ // customize ACLiC's behavior ...
  TString o;
  // customize MakeSharedLib
  o = TString(gSystem->GetMakeSharedLib());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeSharedLib(o.Data());
  // customize MakeExe
  o = TString(gSystem->GetMakeExe());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeExe(o.Data());
} 

void makeValidation()
{
  setupcpp11();
  gStyle->SetOptStats(0);

  TFile * f_bh   = TFile::Open("validation_BH/validation_BH.root");
  TFile * f_comb = TFile::Open("validation_COMB/validation_COMB.root");

  std::vector<TString> dirs  = {"efficiency","fakerate","duplicaterate"};
  std::vector<TString> sORr  = {"sim","reco","reco"};
  std::vector<TString> rates = {"eff","FR","DR"};
  std::vector<TString> vars  = {"pt","phi","eta"};

  for (int i = 0; i < rates.size(); i++)
  {
    for (int j = 0; j < vars.size(); j++)
    {
      TCanvas * canv = new TCanvas();
      canv->cd();
      
      TH1F * h_bh   = (TH1F*)f_bh  ->Get(Form("%s/h_%s_%s_%s_build_%s",dirs[i].Data(),sORr[i].Data(),vars[j].Data(),rates[i].Data(),rates[i].Data()));
      TH1F * h_comb = (TH1F*)f_comb->Get(Form("%s/h_%s_%s_%s_build_%s",dirs[i].Data(),sORr[i].Data(),vars[j].Data(),rates[i].Data(),rates[i].Data()));
      
      h_bh  ->SetLineColor(kBlue);
      h_comb->SetLineColor(kRed);
      h_bh  ->SetMarkerColor(kBlue);
      h_comb->SetMarkerColor(kRed);

      h_bh  ->Draw("ep");
      h_comb->Draw("ep same");

      TLegend * leg = new TLegend(0.8,0.8,0.95,0.95);
      leg->AddEntry(h_bh  ,"Best Hit","LEP");
      leg->AddEntry(h_comb,"Combinatorial","LEP");
      leg->Draw("same");

      canv->SaveAs(Form("%s_%s.png",rates[i].Data(),vars[j].Data()));

      delete leg;
      delete h_comb;
      delete h_bh;
      delete canv;
    }
  }

  delete f_comb;
  delete f_bh;
}
