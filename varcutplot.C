void varcutplot(const TString & var, const TString & cut, const Int_t nbins = 100, 
		const Float_t xlow = 0, const Float_t xhigh = 100, 
		const Bool_t isLogy = false, const Bool_t saveCanv = false,
		const TString text = "")
{
  // get tree
  TFile * file = TFile::Open("valtree_SNB_CMSSW_TTbar_PU70_CE_CMSSWVAL.root");
  TTree * tree = (TTree*)file->Get("cmsswfrtree");
  
  // init hist
  TH1F * hist = new TH1F("hist",Form("%s{%s}",var.Data(),cut.Data()),nbins,xlow,xhigh);
  hist->Sumw2();

  // fill hist
  tree->Draw(Form("%s>>hist",var.Data()),cut.Data(),"goff");

  // scale it to unity
  hist->Scale(1.f/hist->Integral());

  // draw hist
  TCanvas * canv = new TCanvas(); 
  canv->cd();
  canv->SetLogy(isLogy);

  hist->Draw("ep");

  if (saveCanv)
  {
    canv->SaveAs(Form("%s_%s.png",var.Data(),text.Data()));
  }
}
