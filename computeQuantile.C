#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TColor.h"
#include "TPaveText.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

typedef std::vector<TH1F*> TH1FV;
typedef std::vector<TH1FV> TH1FVV;

// define pt-eta bins
static const Int_t netabins = 3;
static const Int_t nptbins  = 6;

TString getEtaBinName(const Int_t etabin)
{
  if      (etabin == 0) return "BL";
  else if (etabin == 1) return "TN";
  else if (etabin == 2) return "EC";
  else                  return "BAD";
}

TString getPtBinName(const Int_t ptbin)
{
  if      (ptbin == 0) return "pt <   0.5             ";
  else if (ptbin == 1) return "pt >=  0.5 && pt <  1.0";
  else if (ptbin == 2) return "pt >=  1.0 && pt <  2.0";
  else if (ptbin == 3) return "pt >=  2.0 && pt <  5.0";
  else if (ptbin == 4) return "pt >=  5.0 && pt < 10.0";
  else if (ptbin == 5) return "pt >= 10.0             ";
  else                 return "BAD";
}

Int_t getEtaBin(const Float_t eta)
{
  if      (eta <  0.6             ) return  0;
  else if (eta >= 0.6 && eta < 1.6) return  1;
  else if (eta >= 1.6 && eta < 2.5) return  2;
  else                              return -1;
}

Int_t getPtBin(const Float_t pt)
{
  if      (pt <   0.5             ) return  0;
  else if (pt >=  0.5 && pt <  1.0) return  1;
  else if (pt >=  1.0 && pt <  2.0) return  2;
  else if (pt >=  2.0 && pt <  5.0) return  3;
  else if (pt >=  5.0 && pt < 10.0) return  4;
  else if (pt >= 10.0             ) return  5;
  else                              return -1;
}

void initHists(TH1FVV & hists, const Float_t quantile, const TString & var, 
	       const Int_t nbinsx, const Float_t xlow, const Float_t xhigh, const TString & xtitle)
{
  std::cout << Form("Initializing %s hists...",var.Data()) << std::endl;

  hists.resize(netabins);
  for (Int_t etabin = 0; etabin < netabins; etabin++)
  {
    hists[etabin].resize(nptbins);
    for (Int_t ptbin = 0; ptbin < nptbins; ptbin++)
    {
      hists[etabin][ptbin] = new TH1F(Form("%s_eta%i_pt%i",var.Data(),etabin,ptbin),
					Form("%s %s %s [quant: %4.2f]",var.Data(),getEtaBinName(etabin).Data(),getPtBinName(ptbin).Data(),quantile),
					nbinsx,xlow,xhigh);
      hists[etabin][ptbin]->GetXaxis()->SetTitle(xtitle.Data());
      hists[etabin][ptbin]->GetYaxis()->SetTitle("Fraction of tracks");
      hists[etabin][ptbin]->Sumw2();
    }
  }
}

Float_t dumpQuantile(const TH1F * hist, const Int_t etabin, const Int_t ptbin, const Float_t quantile, std::ofstream& output)
{
  Float_t sum_frac = 0.f;
  Float_t val = 0.f;
  for (Int_t ibin = 1; ibin <= hist->GetXaxis()->GetNbins(); ibin++)
  {
    const Float_t frac = hist->GetBinContent(ibin);
    sum_frac += frac;

    if (sum_frac >= quantile)
    {
      val = hist->GetXaxis()->GetBinLowEdge(ibin);
      output << getEtaBinName(etabin).Data() << " " << getPtBinName(ptbin).Data() << " : " << val << std::endl;
      break;
    }
  }
  return val;
}

void outputHists(TH1FVV & hists, const TString & var, const Float_t quantile, std::ofstream& output, const Bool_t saveHists)
{
  output << "------------------" << Form(" %s ",var.Data()) << "------------------" << std::endl;

  std::cout << Form("Computing quantile for %s...",var.Data()) << std::endl;
  for (Int_t etabin = 0; etabin < netabins; etabin++)
  {
    for (Int_t ptbin = 0; ptbin < nptbins; ptbin++)
    {
      // scale and compute quantile
      hists[etabin][ptbin]->Scale(1.f/hists[etabin][ptbin]->Integral());
      const Float_t val = dumpQuantile(hists[etabin][ptbin],etabin,ptbin,quantile,output);

      // Draw hist
      if (saveHists)
      {
	TCanvas * canv = new TCanvas();
	canv->cd();
	canv->SetLogy();
	hists[etabin][ptbin]->Draw("ep");

	TLine * line = new TLine(val,hists[etabin][ptbin]->GetMinimum(),val,hists[etabin][ptbin]->GetMaximum());
	line->SetLineColor(kRed);
	line->Draw("same");

	TPaveText * text = new TPaveText(0.55,0.8,0.75,0.9,"NDC");
	text->SetFillStyle(0);
	text->AddText(Form("Cut: %9.4f",val));
	text->Draw("same");
	
	canv->SaveAs(Form("%s_quant%4.2f.png",hists[etabin][ptbin]->GetName(),quantile));

	delete text;
	delete line;
	delete canv;
      }
    }
    output << std::endl;
  }
}

void computeQuantile(const Float_t quantile, const Bool_t saveHists)
{
  std::cout << "Setting branches..." << std::endl;

  // get tree + branches
  TFile * file = TFile::Open("valtree_SNB_CMSSW_TTbar_PU70_CE_CMSSWVAL.root");
  TTree * tree = (TTree*)file->Get("cmsswfrtree");

  Float_t xhit_build; tree->SetBranchAddress("xhit_build",&xhit_build);
  Float_t yhit_build; tree->SetBranchAddress("yhit_build",&yhit_build);
  Float_t zhit_build; tree->SetBranchAddress("zhit_build",&zhit_build);

  Float_t pt_build; tree->SetBranchAddress("pt_build",&pt_build);
  Float_t pt_cmssw; tree->SetBranchAddress("pt_cmssw",&pt_cmssw);

  Float_t eta_build; tree->SetBranchAddress("eta_build",&eta_build);
  Float_t eta_cmssw; tree->SetBranchAddress("eta_cmssw",&eta_cmssw);

  Float_t dphi_build; tree->SetBranchAddress("dphi_build",&dphi_build);
  Float_t helixchi2_build; tree->SetBranchAddress("helixchi2_build",&helixchi2_build);

  // define output
  std::ofstream output(Form("quantile_%4.2f.txt",quantile),std::ios_base::trunc);

  TH1FVV dphi_hists;
  initHists(dphi_hists,quantile,"dphi",10000,0,3.2,"#Delta#phi");

  TH1FVV helixchi2_hists;
  initHists(helixchi2_hists,quantile,"helixchi2",10000,0,10000.f,"Helix #chi^{2}");

  TH1FVV deta_hists;
  initHists(deta_hists,quantile,"deta",10000,0,4.f,"#Delta#eta");

  TH1FVV dinvpt_hists;
  initHists(dinvpt_hists,quantile,"dinvpt",10000,0,10.f,"#Delta1/p_{T}");

  // fill hists
  std::cout << "Filling hists..." << std::endl;
  for (UInt_t ientry = 0; ientry < tree->GetEntries(); ientry++)
  {
    // common to all
    tree->GetEntry(ientry);
    if (pt_cmssw < 0.f) continue;

    // compute radius
    const Float_t rhit_build = std::sqrt(std::pow(xhit_build,2) + std::pow(yhit_build,2));

    // compute positional eta
    const Float_t peta_build = std::abs(std::log(std::tan(std::atan(rhit_build/zhit_build)/2.f)));

    // determine eta bin
    const Int_t etabin = getEtaBin(peta_build);
    if (etabin < 0) continue;

    // determine pt bin
    const Int_t ptbin = getPtBin(pt_build);
    if (ptbin < 0) continue;    

    // fill the hists
    dphi_hists[etabin][ptbin]->Fill(dphi_build);
    helixchi2_hists[etabin][ptbin]->Fill(helixchi2_build);
    deta_hists[etabin][ptbin]->Fill(std::abs(eta_build-eta_cmssw));
    dinvpt_hists[etabin][ptbin]->Fill(std::abs(1.f/pt_build-1.f/pt_cmssw));
  }

  // now create output
  outputHists(dphi_hists,"dphi",quantile,output,saveHists);
  outputHists(helixchi2_hists,"helixchi2",quantile,output,saveHists);
  outputHists(deta_hists,"deta",quantile,output,saveHists);
  outputHists(dinvpt_hists,"dinvpt",quantile,output,saveHists);

  // delete and close out!
  std::cout << "Closing out..." << std::endl;

  output.close();

  for (Int_t etabin = 0; etabin < netabins; etabin++)
  {
    for (Int_t ptbin = 0; ptbin < nptbins; ptbin++)
    {
      delete dphi_hists[etabin][ptbin];
      delete helixchi2_hists[etabin][ptbin];
      delete deta_hists[etabin][ptbin];
      delete dinvpt_hists[etabin][ptbin];
    }
  }

  delete tree;
  delete file;
}
