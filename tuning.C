#include "TString.h"
#include "TStyle.h"
#include "TVirtualFitter.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

#include <fstream>

void tuning()
{
  TString var   = "phi";
  Float_t range = 5.0;
  Float_t frac  = 0.8;

  gStyle->SetOptStat("ou");
  gStyle->SetOptFit (0111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  TFile * file = TFile::Open("valtree.root");
  TTree * tree = (TTree*)file->Get("fittree");

  ofstream output(Form("zphi_newsimple/%s_output.txt",var.Data()),std::ios::app);

  for (int i = 3; i < 10; i++)
  {
    Int_t layer = i;

    TCanvas * canv = new TCanvas();
    canv->cd();

    TH1F * hist = new TH1F(Form("h_%s_diff_lay%i",var.Data(),layer),Form("%s_prop[%i]-%s_hit[%i]",var.Data(),layer,var.Data(),layer),100,-range,range); hist->Sumw2();
    tree->Draw(Form("%s_prop[%i]-%s_hit[%i]>>%s",var.Data(),layer,var.Data(),layer,hist->GetName()),Form("%s_hit[%i]!=-1000",var.Data(),layer),"goff");
    
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

    output << fit->GetParameter(2) << std::endl;

    canv->SaveAs(Form("zphi_newsimple/%s.png",hist->GetName()));

    delete fit;
    delete hist;
    delete canv;
  }
}
