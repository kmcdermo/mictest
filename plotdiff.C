void plotdiff(){

  gStyle->SetOptStat(0);

  TString plot = "1./pt_mc[0]-1./pt_cf";
  TString name = "inv_pt3";

  TString cut1 = "nHits<=3&&abs(phi_mc[0])>1&&abs(phi_mc[0])<2";
  //  TString cut2 = "!(nHits<=3&&abs(phi_mc[0])>1&&abs(phi_mc[0])<2)";
  TString cut2 = "nHits<=3&&(abs(phi_mc[0])<1||abs(phi_mc[0])>2)";
  //  TString cut2 = "nHits>3&&nHits<=9";

  TFile * file = TFile::Open("cfseeds.root");
  TTree * tree = (TTree*)file->Get("debugtree");

  TCanvas * c1 = new TCanvas();

  //  Float_t posx = 1.5;
  //  Float_t negx = -posx;

  //  TH1F * h1 = new TH1F("h1","h1",100,posx,negx);
  //  TH1F * h2 = new TH1F("h2","h2",100,posx,negx);

  tree->Draw(Form("%s>>h1(100,-3,3)",plot.Data()),cut1.Data());
  tree->Draw(Form("%s>>h2(100,-3,3)",plot.Data()),cut2.Data());
  //tree->Draw(Form("%s>>h2(100,-1.5,1.5)",plot.Data()));

  h1->Scale(1.0/h1->Integral());
  h2->Scale(1.0/h2->Integral());

  h1->SetLineColor(kRed);
  h1->SetTitle(plot.Data());
  h2->SetTitle(plot.Data());

  h2->Draw();
  h1->Draw("same");

  //TLegend * leg = new TLegend(0.78,0.77,0.98,0.94);
    TLegend * leg = new TLegend(0.58,0.77,0.78,0.94);
  leg->AddEntry(h1,"3 Hit Zoom","l");
  leg->AddEntry(h2,"3 Hit, Other phi","l");
  //  leg->AddEntry(h2,"gt 3, lte 9","l");

  leg->Draw("same");
  
  c1->SetLogy(1.0);
  c1->SaveAs(Form("%s.pdf",name.Data()));

}
