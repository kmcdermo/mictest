#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"

#include <vector>

typedef std::vector<TString> vts;
typedef std::vector<TFile*> vtf;
typedef std::vector<TTree*> vtt;
typedef std::vector<TH1F*> vth1;
typedef std::vector<Color_t> vtc;
typedef std::vector<Bool_t> vb;

struct HistInfo
{
  HistInfo(TString def, TString n, TString t, TString xt, 
	   TString yt, Int_t nbx, Float_t xl, Float_t xh) : 
    definition(def),name(n),title(t),xtitle(xt),
    ytitle(yt),nbinsx(nbx),xlow(xl),xhigh(xh) {}
  TString definition = "";
  TString name = "";
  TString title = "";
  TString xtitle = "";
  TString ytitle = "";
  Int_t nbinsx = -1;
  Float_t xlow = -1;
  Float_t xhigh = -1;
  Color_t color = kBlack;
};
typedef std::vector<HistInfo> vhi;

TH1F * createHist(const HistInfo & hi, const TString label, const Color_t color)
{
  TH1F * hist = new TH1F(Form("%s_%s",hi.name.Data(),label.Data()),hi.title.Data(),hi.nbinsx,hi.xlow,hi.xhigh);
  hist->GetXaxis()->SetTitle(hi.definition.Data());
  hist->GetYaxis()->SetTitle(hi.ytitle.Data());
  hist->SetLineColor(color);
  hist->SetMarkerColor(color);
  hist->Sumw2();

  return hist;
}

void getMinMax(const vth1 & hists, const Int_t nbinsx, const Bool_t isLogy, Float_t& min, Float_t& max)
{
  for (auto&& hist : hists)
  {
    for (Int_t ibin = 1; ibin <= nbinsx; ibin++)
    {
      const Float_t content = hist->GetBinContent(ibin);
      if (content > max) max = content;
      if (isLogy)
      {
	if (content != 0.f && content < min) min = content;
      }
      else
      {
	if (content < min) min = content;
      }
    }
  }
}

void chi2plots()
{
  const TString dir = "full_phiErr";
  const TString cut = "nHits_m>=11";

  gStyle->SetOptStat(0);
  const vtc colors = {kRed+1,kViolet-1,kBlack,kGreen+1,kBlue,kMagenta,kAzure+10,kOrange+1,kYellow-7,kYellow+3};

  const vts regions = {"ECN2","ECN1","BRL","ECP1","ECP2"};
  vtf files(regions.size());
  vtt trees(regions.size());
  for (Int_t i = 0; i < regions.size(); i++)
  {
    files[i] = TFile::Open(Form("%s/dump%s.root",dir.Data(),regions[i].Data()));
    files[i] -> cd();
    trees[i] = (TTree*)files[i]->Get("dumptree");
  }

  const vhi histinfos = 
  {
    HistInfo("pow(ipt_m-ipt_c,2)/eipt_m","chi2_iptonly",Form("1/p_{T} Diagonal #chi^{2} Only [%s]",cut.Data()),"","nTracks",100,0,50),
    HistInfo("pow(eta_m-eta_c,2)/eeta_m","chi2_etaonly",Form("#eta Diagonal #chi^{2} Only [%s]",cut.Data()),"","nTracks",100,0,25),
    HistInfo("pow(phi_m-phi_c,2)/ephi_m","chi2_phionly",Form("#phi Diagonal #chi^{2} Only [%s]",cut.Data()),"","nTracks",100,0,50),
    HistInfo("chi2","chi2",Form("Full #chi^{2} [%s]",cut.Data()),"","nTracks",100,0,100),
    HistInfo("ipt_m-ipt_c","ipt_only",Form("1/p_{T} residual [%s]",cut.Data()),"","nTracks",100,-0.1,0.1),
    HistInfo("eipt_m","eipt_only",Form("1/p_{T} variance [%s]",cut.Data()),"","nTracks",100,0,5e-4),
    HistInfo("(ipt_c-ipt_m)/(ipt_m*ipt_c)","pt_only",Form("p_{T} residual [%s]",cut.Data()),"","nTracks",100,-0.5,1.0)
  };
  
  for (auto&& hi : histinfos)
  {
    vth1 hists(regions.size());

    for (Int_t i = 0; i < regions.size(); i++)
    {
      hists[i] = createHist(hi,regions[i],colors[i]);
      trees[i]->Draw(Form("%s>>%s",hi.definition.Data(),hists[i]->GetName()),Form("%s",cut.Data()),"goff");
      hists[i]->Scale(1.f/hists[i]->Integral());
    }

    vb isLogys = {false,true};
    for (auto&& isLogy : isLogys)
    {
      Float_t max = -1e6, min = 1e6;    
      getMinMax(hists,hi.nbinsx,isLogy,min,max);

      hists[0]->SetMaximum(isLogy?max*2.f:max*1.1);
      hists[0]->SetMinimum(isLogy?min/2.f:min/1.1);

      TCanvas * canv = new TCanvas(); 
      canv->cd(); canv->SetLogy(isLogy);

      TLegend * leg = new TLegend(0.7,0.7,0.9,0.9);
      for (Int_t i = 0; i < regions.size(); i++)
      {
	hists[i]->Draw(i>0?"EP SAME":"EP");
	TString label;
	if (hi.title.Contains("variance") || hi.title.Contains("residual"))
	{
	  label = Form("%s, #mu: %f",regions[i].Data(),hists[i]->GetMean());
	}
	else 
	{
	  label = Form("%s, #mu: %5.3f",regions[i].Data(),hists[i]->GetMean());
	}
	leg->AddEntry(hists[i],label.Data(),"epl");
      }
      leg->Draw("SAME");
      canv->SaveAs(Form("%s/%s_%s.png",dir.Data(),hi.name.Data(),(isLogy?"log":"lin")));

      delete leg;
      delete canv;
    }

    for (auto& hist : hists) delete hist;
  }

  for (auto& tree : trees) delete tree;
  for (auto& file : files) delete file;
}
