#include "TString.h"
#include "TStyle.h"
#include "TVirtualFitter.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

void tuning()
{
  TString var   = "phi";
  Int_t   layer = 16;
  Float_t range = 0.01;
  Float_t frac  = 0.8;
  //  Float_t nsig  = 3.0;

  gStyle->SetOptStat("ou");
  gStyle->SetOptFit (0111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  TFile * file = TFile::Open("cmsswvaltree.root");
  TTree * tree = (TTree*)file->Get("fittree");

  TH1F * hist = new TH1F(Form("h_%s_diff_lay%i",var.Data(),layer),Form("%s_prop[%i]-%s_hit[%i]",var.Data(),layer,var.Data(),layer),100,-range,range); hist->Sumw2();
  tree->Draw(Form("%s_prop[%i]-%s_hit[%i]>>%s",var.Data(),layer,var.Data(),layer,hist->GetName()),Form("%s_hit[%i]!=-1000",var.Data(),layer),"goff");
  
//   TH1F * hist = new TH1F(Form("h_%s_pull_lay%i",var.Data(),layer),Form("(%s_prop[%i]-%s_hit[%i])/#sigma_{%s[%i]}",var.Data(),layer,var.Data(),layer,var.Data(),layer),100,-range,range); hist->Sumw2();
//   tree->Draw(Form("(%s_prop[%i]-%s_hit[%i])/sqrt(e%s_prop[%i])>>%s",var.Data(),layer,var.Data(),layer,var.Data(),layer,hist->GetName()),Form("%s_hit[%i]!=-1000",var.Data(),layer),"goff");
 
  TCanvas * canv = new TCanvas();
  canv->cd();
  hist->Scale(1.0/hist->Integral());
  hist->SetMinimum(0.000001);
  hist->Draw("EP");

  TF1 * fit = new TF1("fit","gaus(0)",-range*frac,range*frac);
  fit->SetParName(0,"N");
  fit->SetParName(1,"#mu");
  fit->SetParName(2,"#sigma");
  fit->SetParLimits(2,0,10);
  hist->Fit("fit","R0Q"); // for real testing, do not suppress output --> drop q

  fit->Draw("same");

  canv->SaveAs(Form("zphicmssw/%s.png",hist->GetName()));
}
