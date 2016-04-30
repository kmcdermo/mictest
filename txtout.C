void txtout(){

  TString cut1 = "nHits<=3&&abs(phi_mc[0])>1&&abs(phi_mc[0])<2";
  //TString cut2 = "nHits>3&&nHits<=9";
  TString cut2 = "nHits<=3&&(abs(phi_mc[0])<1||abs(phi_mc[0])>2)";

  //TString extra = "&&chi2[nHits]>15.";
  TString extra = "&&chi2[nHits]==-99";
  //  TString extra = "&&chi2[nHits]<0&&chi2[nHits]!=-99";
  cut1.Append(extra);
  cut2.Append(extra);

  TFile * file = TFile::Open("cfseeds.root");
  TTree * tree = (TTree*)file->Get("debugtree");

  tree->Draw("phi_gen>>h1",cut1.Data(),"goff");
  tree->Draw("phi_gen>>h2",cut2.Data(),"goff");

  //  std::cout << extra.Data() << std::endl;
  std::cout << h1->Integral() << " " << h2->Integral() << std::endl;
}
