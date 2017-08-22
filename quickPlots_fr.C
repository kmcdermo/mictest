struct histinfo
{
  TString name;
  TString title;
  Int_t nbinsx;
  Float_t xlow;
  Float_t xhigh;
  TString meanpres;
};

struct cutinfo
{
  TString hlabel;
  TString llabel;
  TString cut;
};

typedef std::vector<std::vector<TFile*> > VVFile;
typedef std::vector<std::vector<TTree*> > VVTree;
typedef std::vector<std::vector<std::vector<TH1F*> > > VVVTH1F;

void quickPlots_fr()
{
  gStyle->SetOptStat(0);

  std::vector<TString> build = {"BH","STD","CE"};
  std::vector<TString> pu = {"NoPU","PU35","PU70"};
  
  VVFile files(pu.size());
  VVTree trees(pu.size());
  for (Int_t ipu = 0; ipu < pu.size(); ipu++)
  {
    files[ipu].resize(build.size());
    trees[ipu].resize(build.size());
    for (Int_t ibuild = 0; ibuild < build.size(); ibuild++)
    {
      files[ipu][ibuild] = TFile::Open(Form("valtree_SNB_CMSSW_TTbar_%s_%s.root",pu[ipu].Data(),build[ibuild].Data()));
      trees[ipu][ibuild] = (TTree*)files[ipu][ibuild]->Get("cmsswfrtree");
    }
  }

  const std::vector<Color_t> colors = {kRed+1,kBlue,kGreen+1,kViolet-1,kOrange+1,kYellow-7,kBlack,kMagenta,kAzure+10,kYellow+3}; 

  std::vector<histinfo> hinfos;
  hinfos.push_back({"fracHitsMatched","fracHitsMatched_build",22,0,1.1,"5.3"});
  hinfos.push_back({"fracHitsMatchedMC","fracHitsMatchedMC_build",22,0,1.1,"5.3"});
  hinfos.push_back({"fracHitsMatchedMCTrue","fracHitsMatchedMCTrue_build",22,0,1.1,"5.3"});
  hinfos.push_back({"fracHitsMatchedSeed","fracHitsMatchedSeed_build",22,0,1.1,"5.3"});
  hinfos.push_back({"fracHitsMatchedMC_CMSSW","fracHitsMatchedMC_build_cmssw",22,0,1.1,"5.3"});
  hinfos.push_back({"fracHitsMatchedMCTrue_CMSSW","fracHitsMatchedMCTrue_build_cmssw",22,0,1.1,"5.3"});
  hinfos.push_back({"nHits","nHits_build",30,0,30,"5.2"});
  hinfos.push_back({"deta","eta_cmssw_true-eta_build",40,-0.2,0.2,"6.5"});
  hinfos.push_back({"dinvpt","1./pt_cmssw_true-1./pt_build",40,-1,1,"6.4"});
  hinfos.push_back({"dphi","dphi_build_true",40,0.0,0.4,"6.5"});
  hinfos.push_back({"dpt","pt_cmssw_true-pt_build",40,-2,2,"6.3"});
  hinfos.push_back({"chi2","helixchi2_build",25,0,50,"5.2"});

  std::vector<TString> commoncuts = {"&&cmsswmask_build==0","&&cmsswmask_build==1"};
  std::vector<std::vector<cutinfo> > cinfos(commoncuts.size());
  for (Int_t icc = 0; icc < commoncuts.size(); icc++)
  {
    cinfos[icc].push_back({Form("pt_lt2_%s",icc?"good":"fake"),Form("p_{T} < 2 %s",icc?"Good":"Fake"),Form("(pt_build<2)%s",commoncuts[icc].Data())});
    cinfos[icc].push_back({Form("pt_2to10_%s",icc?"good":"fake"),Form("2 #leq p_{T} < 10 %s",icc?"Good":"Fake"),Form("(pt_build>=2&&pt_build<10)%s",commoncuts[icc].Data())});
    cinfos[icc].push_back({Form("pt_gt10_%s",icc?"good":"fake"),Form("p_{T} #geq 10 %s",icc?"Good":"Fake"),Form("(pt_build>=10)%s",commoncuts[icc].Data())});
  }

  for (Int_t ipu = 0; ipu < pu.size(); ipu++)
  {
    for (Int_t ibuild = 0; ibuild < build.size(); ibuild++)
    {
      VVVTH1F hists(hinfos.size());
      for (Int_t ihist = 0; ihist < hinfos.size(); ihist++)
      {
	hists[ihist].resize(cinfos.size());
	const auto & h = hinfos[ihist];
	
	Float_t min = 1e6;
	Float_t max = -1e6;
	for (Int_t icc = 0; icc < cinfos.size(); icc++)
        {
	  hists[ihist][icc].resize(cinfos[icc].size());
	  for (Int_t icut = 0; icut < cinfos[icc].size(); icut++)
	  {
	    const auto & c = cinfos[icc][icut];

	    auto & hist = hists[ihist][icc][icut];
	    hist = new TH1F(Form("%s_%s",h.name.Data(),c.hlabel.Data()),Form("%s",h.title.Data()),h.nbinsx,h.xlow,h.xhigh);
	    hist->GetXaxis()->SetTitle(Form("%s",h.title.Data()));
	    hist->GetYaxis()->SetTitle("nTracks");
	    hist->SetLineStyle(icc?1:2);
	    hist->SetLineWidth(2);
	    hist->SetLineColor(colors[icut]);

	    trees[ipu][ibuild]->Draw(Form("%s>>%s",h.title.Data(),hist->GetName()),Form("%s",c.cut.Data()),"goff");
	      
	    hist->Scale(1.f/hist->GetEntries());

	    Float_t tmpmin = 1e5;
	    Float_t tmpmax = -1e5;
	    for (Int_t ibin = 1; ibin <= hist->GetNbinsX(); ibin++)
            {
	      const Float_t content = hist->GetBinContent(ibin);
	      if (content > tmpmax) tmpmax = content;
	      if (content != 0.f && content < tmpmin) tmpmin = content;
	    }

	    if (tmpmax > max) max = tmpmax;
	    if (tmpmin < min) min = tmpmin;
	  }
	}
	
	hists[ihist][0][0]->SetMaximum(2.1);
	hists[ihist][0][0]->SetMinimum(min/1.5);

	TCanvas * canv = new TCanvas(); 
	canv->cd();
	canv->SetLogy(1);
	//canv->SetGridx(1);
	canv->SetGridy(1);
	//canv->SetTickx();
	canv->SetTicky();

	const bool isFrac = !(ihist == 0 || ihist == 1 || ihist == 2|| ihist == 3 || ihist == 4 || ihist == 5);

	TLegend * leg = new TLegend();
	leg->SetX1NDC(isFrac?0.7:0.35);
	leg->SetY1NDC(isFrac?0.8:0.73);
	leg->SetX2NDC(isFrac?1.0:0.65);
	leg->SetY2NDC(isFrac?1.0:0.93);
	for (Int_t icc = 0; icc < cinfos.size(); icc++)
	{
	  for (Int_t icut = 0; icut < cinfos[icc].size(); icut++)
          {
	    const auto & c = cinfos[icc][icut];
	    auto & hist = hists[ihist][icc][icut];
	    hist->Draw((icut+icc*cinfos.size())>0?"SAME":"");
	    leg->AddEntry(hist,Form(Form("%%s, #mu = %%%sf",h.meanpres.Data()),c.llabel.Data(),hist->GetMean()),"l");
	  }    
	}
	leg->Draw("same");
	canv->SaveAs(Form("%s/%s/%s.png",pu[ipu].Data(),build[ibuild].Data(),h.name.Data()));

	delete leg;
	delete canv;
	
	for (Int_t icc = 0; icc < cinfos.size(); icc++)
	{
	  for (Int_t icut = 0; icut < cinfos[icc].size(); icut++)
	  {
	    delete hists[ihist][icc][icut];
	  }
	}
      } // end loop over hists
    } // end loop over builds
  } // end pileup loop

  for (Int_t ipu = 0; ipu < pu.size(); ipu++)
  {
    for (Int_t ibuild = 0; ibuild < build.size(); ibuild++)
    {
      delete trees[ipu][ibuild];
      delete files[ipu][ibuild];
    }
  }
}
