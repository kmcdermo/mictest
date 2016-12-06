void makeBenchmarkPlots(bool isMic = false, bool isCMSSW = false, bool isEndcap = false)
{

  TString hORm = "host";
  if (isMic) hORm = "mic";

  TString label = "Xeon";
  if (isMic) label+=" Phi";

  if (isEndcap) {
    hORm+="_endcap";
    label+=" (endcap)";
  }

  float maxvu = 8;
  if (isMic) maxvu = 16;
  float maxth = 21;
  if (isMic) maxth = 210;

  TFile* f_omp = TFile::Open("benchmark_"+hORm+"_omp.root");
  TFile* f_tbb = TFile::Open("benchmark_"+hORm+"_tbb.root");
  {
  TCanvas c1;
  TGraph* g_OMP_VU = (TGraph*) f_omp->Get("g_OMP_VU");
  TGraph* g_TBB_VU = (TGraph*) f_tbb->Get("g_TBB_VU");
  g_OMP_VU->SetTitle("Vectorization benchmark on "+label);
  g_OMP_VU->GetXaxis()->SetTitle("Vector Width");
  g_OMP_VU->GetYaxis()->SetTitle("Time for 10 events x 20k tracks [s]");
  if (isCMSSW) g_OMP_VU->GetYaxis()->SetTitle("Time for 100 TTbarPU35 events [s]");
  g_OMP_VU->GetXaxis()->SetRangeUser(1,maxvu);
  g_OMP_VU->GetYaxis()->SetRangeUser(0,20);
  if (isMic) g_OMP_VU->GetYaxis()->SetRangeUser(0,200);
  if (isEndcap) g_OMP_VU->GetYaxis()->SetRangeUser(0,60);
  g_OMP_VU->SetLineWidth(2);
  g_TBB_VU->SetLineWidth(2);
  g_OMP_VU->SetLineColor(kBlue);
  g_TBB_VU->SetLineColor(kRed);
  g_OMP_VU->SetMarkerStyle(kFullCircle);
  g_TBB_VU->SetMarkerStyle(kFullCircle);
  g_OMP_VU->SetMarkerColor(kBlue);
  g_TBB_VU->SetMarkerColor(kRed);
  g_OMP_VU->Draw("ALP");
  g_TBB_VU->Draw("LP");
  TLegend* leg_VU = new TLegend(0.60,0.60,0.85,0.85);
  leg_VU->SetBorderSize(0);
  leg_VU->AddEntry(g_OMP_VU,"OMP Standard","LP");
  leg_VU->AddEntry(g_TBB_VU,"TBB Standard","LP");
  leg_VU->Draw();
  c1.SetGridy();
  c1.Update();
  if (isCMSSW) c1.SaveAs("cmssw_"+hORm+"_vu_time.png");
  else c1.SaveAs(hORm+"_vu_time.png");
  } {
  TCanvas c2;
  TGraph* g_OMP_VU_speedup = (TGraph*) f_omp->Get("g_OMP_VU_speedup");
  TGraph* g_TBB_VU_speedup = (TGraph*) f_tbb->Get("g_TBB_VU_speedup");
  g_OMP_VU_speedup->SetTitle("Vectorization speedup on "+label);
  g_OMP_VU_speedup->GetXaxis()->SetTitle("Vector Width");
  g_OMP_VU_speedup->GetYaxis()->SetTitle("Speedup");
  g_OMP_VU_speedup->GetXaxis()->SetRangeUser(1,maxvu);
  g_OMP_VU_speedup->GetYaxis()->SetRangeUser(0,maxvu);
  g_OMP_VU_speedup->SetLineWidth(2);
  g_TBB_VU_speedup->SetLineWidth(2);
  g_OMP_VU_speedup->SetLineColor(kBlue);
  g_TBB_VU_speedup->SetLineColor(kRed);
  g_OMP_VU_speedup->SetMarkerStyle(kFullCircle);
  g_TBB_VU_speedup->SetMarkerStyle(kFullCircle);
  g_OMP_VU_speedup->SetMarkerColor(kBlue);
  g_TBB_VU_speedup->SetMarkerColor(kRed);
  g_OMP_VU_speedup->Draw("ALP");
  g_TBB_VU_speedup->Draw("LP");
  TLine lvu(1,1,maxvu,maxvu);
  lvu.Draw();
  TLegend* leg_VU_speedup = new TLegend(0.20,0.60,0.45,0.85);
  leg_VU_speedup->SetBorderSize(0);
  leg_VU_speedup->AddEntry(g_OMP_VU_speedup,"OMP Standard","LP");
  leg_VU_speedup->AddEntry(g_TBB_VU_speedup,"TBB Standard","LP");
  leg_VU_speedup->Draw();
  c2.SetGridy();
  c2.Update();
  if (isCMSSW) c2.SaveAs("cmssw_"+hORm+"_vu_speedup.png");
  else c2.SaveAs(hORm+"_vu_speedup.png");
  } {
  TCanvas c3;
  TGraph* g_OMP_TH = (TGraph*) f_omp->Get("g_OMP_TH");
  TGraph* g_TBB_TH = (TGraph*) f_tbb->Get("g_TBB_TH");
  g_OMP_TH->SetTitle("Parallelization benchmark on "+label);
  g_OMP_TH->GetXaxis()->SetTitle("Number of Threads");
  g_OMP_TH->GetYaxis()->SetTitle("Time for 10 events x 20k tracks [s]");
  if (isCMSSW) g_OMP_TH->GetYaxis()->SetTitle("Time for 100 TTbarPU35 events [s]");
  g_OMP_TH->GetXaxis()->SetRangeUser(1,maxth);
  g_OMP_TH->GetYaxis()->SetRangeUser(0,10);
  if (isMic) g_OMP_TH->GetYaxis()->SetRangeUser(0.1,100);
  if (isEndcap) g_OMP_TH->GetYaxis()->SetRangeUser(0.1,40);
  g_OMP_TH->SetLineWidth(2);
  g_TBB_TH->SetLineWidth(2);
  g_OMP_TH->SetLineColor(kBlue);
  g_TBB_TH->SetLineColor(kRed);
  g_OMP_TH->SetMarkerStyle(kFullCircle);
  g_TBB_TH->SetMarkerStyle(kFullCircle);
  g_OMP_TH->SetMarkerColor(kBlue);
  g_TBB_TH->SetMarkerColor(kRed);
  g_OMP_TH->Draw("ALP");
  g_TBB_TH->Draw("LP");
  TLegend* leg_TH = new TLegend(0.60,0.60,0.85,0.85);
  leg_TH->SetBorderSize(0);
  leg_TH->AddEntry(g_OMP_TH,"OMP Standard","LP");
  leg_TH->AddEntry(g_TBB_TH,"TBB Standard","LP");
  leg_TH->Draw();
  c3.SetGridy();
  if (isMic) c3.SetLogy();
  c3.Update();
  if (isCMSSW) c3.SaveAs("cmssw_"+hORm+"_th_time.png");
  else c3.SaveAs(hORm+"_th_time.png");
  } {
  TCanvas c4;
  TGraph* g_OMP_TH_speedup = (TGraph*) f_omp->Get("g_OMP_TH_speedup");
  TGraph* g_TBB_TH_speedup = (TGraph*) f_tbb->Get("g_TBB_TH_speedup");
  g_OMP_TH_speedup->SetTitle("Parallelization speedup on "+label);
  g_OMP_TH_speedup->GetXaxis()->SetTitle("Number of Threads");
  g_OMP_TH_speedup->GetYaxis()->SetTitle("Speedup");
  g_OMP_TH_speedup->GetXaxis()->SetRangeUser(1,maxth);
  g_OMP_TH_speedup->GetYaxis()->SetRangeUser(0,maxth);
  g_OMP_TH_speedup->SetLineWidth(2);
  g_TBB_TH_speedup->SetLineWidth(2);
  g_OMP_TH_speedup->SetLineColor(kBlue);
  g_TBB_TH_speedup->SetLineColor(kRed);
  g_OMP_TH_speedup->SetMarkerStyle(kFullCircle);
  g_TBB_TH_speedup->SetMarkerStyle(kFullCircle);
  g_OMP_TH_speedup->SetMarkerColor(kBlue);
  g_TBB_TH_speedup->SetMarkerColor(kRed);
  g_OMP_TH_speedup->Draw("ALP");
  g_TBB_TH_speedup->Draw("LP");
  TLine lth(1,1,maxth,maxth);
  lth.Draw();
  TLegend* leg_TH_speedup = new TLegend(0.20,0.60,0.45,0.85);
  leg_TH_speedup->SetBorderSize(0);
  leg_TH_speedup->AddEntry(g_OMP_TH_speedup,"OMP Standard","LP");
  leg_TH_speedup->AddEntry(g_TBB_TH_speedup,"TBB Standard","LP");
  leg_TH_speedup->Draw();
  c4.SetGridy();
  c4.Update();
  if (isCMSSW) c4.SaveAs("cmssw_"+hORm+"_th_speedup.png");
  else c4.SaveAs(hORm+"_th_speedup.png");
  }
}
