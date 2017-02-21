#include "TString.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <vector>

void makeBenchmarkPlotsSeed(bool isKNC)
{
  TString hORm  = (isKNC)?"knc":"snb";
  TString shORm = (isKNC)?"KNC":"SNB";

  std::vector<TString> tests  = {"triplets","merge","seeding","fit","full"};
  std::vector<TString> stests = {"Triplet Finding Only","Triplets to Tracks Only","Seeding Only","Seed Fit Only (CF + KF fit)","Seed Finding + Fitting"};

  float maxth = 24;
  float maxvu = 8;
  TString nth = "1"; // isMic?"60":"12"; // for multithreaded VU tests
  TString nvu = Form("%i",int(maxvu));

  TFile* f = TFile::Open(Form("benchmark_seeding_%s.root",hORm.Data()));

  for (UInt_t i = 0; i < tests.size(); i++)
  {
    // Vectorization Benchmark
    // TCanvas c1;
    // c1.cd();
    // TGraphErrors* g_1000_VU  = (TGraphErrors*) f->Get(Form("g_%s_nTk1000_VU",tests[i].Data()));
    // TGraphErrors* g_5000_VU  = (TGraphErrors*) f->Get(Form("g_%s_nTk5000_VU",tests[i].Data()));
    // TGraphErrors* g_10000_VU = (TGraphErrors*) f->Get(Form("g_%s_nTk10000_VU",tests[i].Data()));
    // g_1000_VU->SetTitle(Form("%s Vectorization Benchmark on %s [nTH="+nth+"]",stests[i].Data(),shORm[i].Data()));
    // g_1000_VU->GetXaxis()->SetTitle("Matriplex Vector Width [floats]");
    // g_1000_VU->GetYaxis()->SetTitle("Average Time per Event [s]");
    // g_1000_VU->GetXaxis()->SetRangeUser(1,maxvu);
    // g_1000_VU->GetYaxis()->SetRangeUser(0.0001,20);
    // g_1000_VU->SetLineWidth(2);
    // g_5000_VU->SetLineWidth(2);
    // g_10000_VU->SetLineWidth(2);
    // g_1000_VU->SetLineColor(kBlue);
    // g_5000_VU->SetLineColor(kGreen+1);
    // g_10000_VU->SetLineColor(kRed);
    // g_1000_VU->SetMarkerStyle(kFullCircle);
    // g_5000_VU->SetMarkerStyle(kFullCircle);
    // g_10000_VU->SetMarkerStyle(kFullCircle);
    // g_1000_VU->SetMarkerColor(kBlue);
    // g_5000_VU->SetMarkerColor(kGreen+1);
    // g_10000_VU->SetMarkerColor(kRed);
    // g_1000_VU->Draw("ALP");
    // g_5000_VU->Draw("LP");
    // g_10000_VU->Draw("LP");
    // TLegend* leg_VU = new TLegend(0.60,0.65,0.85,0.85);
    // leg_VU->SetBorderSize(0);
    // leg_VU->AddEntry(g_1000_VU,"1000 Tks/Ev","LP");
    // leg_VU->AddEntry(g_5000_VU,"5000 Tks/Ev","LP");
    // leg_VU->AddEntry(g_10000_VU,"10000 Tks/Ev","LP");
    // leg_VU->Draw();
    // c1.SetGridy();
    // c1.Update();
    // c1.SaveAs(Form("%s_%s_vu_time.png",tests[i].Data(),hORm.Data()));
    // delete leg_VU;

    // // Vectorization Speedup
    // TCanvas c2;
    // c2.cd();
    // TGraphErrors* g_1000_VU_speedup  = (TGraphErrors*) f->Get(Form("g_%s_nTk1000_VU_speedup",tests[i].Data()));
    // TGraphErrors* g_5000_VU_speedup  = (TGraphErrors*) f->Get(Form("g_%s_nTk5000_VU_speedup",tests[i].Data()));
    // TGraphErrors* g_10000_VU_speedup = (TGraphErrors*) f->Get(Form("g_%s_nTk10000_VU_speedup",tests[i].Data()));
    // g_1000_VU_speedup->SetTitle(Form("%s Vectorization Speedup on %s [nTH="+nth+"]",stests[i].Data(),shORm[i].Data()));
    // g_1000_VU_speedup->GetXaxis()->SetTitle("Matriplex Vector Width [floats]");
    // g_1000_VU_speedup->GetYaxis()->SetTitle("Speedup");
    // g_1000_VU_speedup->GetXaxis()->SetRangeUser(1,maxvu);
    // g_1000_VU_speedup->GetYaxis()->SetRangeUser(0,maxvu);
    // g_1000_VU_speedup->SetLineWidth(2);
    // g_5000_VU_speedup->SetLineWidth(2);
    // g_10000_VU_speedup->SetLineWidth(2);
    // g_1000_VU_speedup->SetLineColor(kBlue);
    // g_5000_VU_speedup->SetLineColor(kGreen+1);
    // g_10000_VU_speedup->SetLineColor(kRed);
    // g_1000_VU_speedup->SetMarkerStyle(kFullCircle);
    // g_5000_VU_speedup->SetMarkerStyle(kFullCircle);
    // g_10000_VU_speedup->SetMarkerStyle(kFullCircle);
    // g_1000_VU_speedup->SetMarkerColor(kBlue);
    // g_5000_VU_speedup->SetMarkerColor(kGreen+1);
    // g_10000_VU_speedup->SetMarkerColor(kRed);
    // g_1000_VU_speedup->Draw("ALP");
    // g_5000_VU_speedup->Draw("LP");
    // g_10000_VU_speedup->Draw("LP");
    // TLine lvu(1,1,maxvu,maxvu);
    // lvu.Draw();
    // TLegend* leg_VU_speedup = new TLegend(0.20,0.65,0.45,0.85);
    // leg_VU_speedup->SetBorderSize(0);
    // leg_VU_speedup->AddEntry(g_1000_VU_speedup,"1000 Tks/Ev","LP");
    // leg_VU_speedup->AddEntry(g_5000_VU_speedup,"5000 Tks/Ev","LP");
    // leg_VU_speedup->AddEntry(g_10000_VU_speedup,"10000 Tks/Ev","LP");
    // leg_VU_speedup->Draw();
    // c2.SetGridy();
    // c2.Update();
    // c2.SaveAs(Form("%s_%s_vu_speedup.png",tests[i].Data(),hORm.Data()));
    // delete leg_VU_speedup;
    
    // Parallelization Benchmark
    TCanvas c3; 
    c3.cd();
    TGraphErrors* g_1000_TH  = (TGraphErrors*) f->Get(Form("g_%s_nTk1000_TH",tests[i].Data()));
    TGraphErrors* g_5000_TH  = (TGraphErrors*) f->Get(Form("g_%s_nTk5000_TH",tests[i].Data()));
    TGraphErrors* g_10000_TH = (TGraphErrors*) f->Get(Form("g_%s_nTk10000_TH",tests[i].Data()));
    g_1000_TH->SetTitle(Form("%s Parallelization Benchmark on %s [nVU="+nvu+"]",stests[i].Data(),shORm[i].Data()));
    g_1000_TH->GetXaxis()->SetTitle("Number of Threads");
    g_1000_TH->GetYaxis()->SetTitle("Average Time per Event [s]");
    g_1000_TH->GetXaxis()->SetRangeUser(1,maxth);
    g_1000_TH->GetYaxis()->SetRangeUser(0.0001,20.0);
    g_1000_TH->SetLineWidth(2);
    g_5000_TH->SetLineWidth(2);
    g_10000_TH->SetLineWidth(2);
    g_1000_TH->SetLineColor(kBlue);
    g_5000_TH->SetLineColor(kGreen+1);
    g_10000_TH->SetLineColor(kRed);
    g_1000_TH->SetMarkerStyle(kFullCircle);
    g_5000_TH->SetMarkerStyle(kFullCircle);
    g_10000_TH->SetMarkerStyle(kFullCircle);
    g_1000_TH->SetMarkerColor(kBlue);
    g_5000_TH->SetMarkerColor(kGreen+1);
    g_10000_TH->SetMarkerColor(kRed);
    g_1000_TH->Draw("ALP");
    g_5000_TH->Draw("LP");
    g_10000_TH->Draw("LP");
    TLegend* leg_TH = new TLegend(0.60,0.65,0.85,0.85);
    leg_TH->SetBorderSize(0);
    leg_TH->AddEntry(g_1000_TH,"1000 Tks/Ev","LP");
    leg_TH->AddEntry(g_5000_TH,"5000 Tks/Ev","LP");
    leg_TH->AddEntry(g_10000_TH,"10000 Tks/Ev","LP");
    leg_TH->Draw();
    c3.SetGridy();
    c3.SetLogy();
    c3.Update();
    c3.SaveAs(Form("%s_%s_th_time.png",tests[i].Data(),hORm.Data()));
    delete leg_TH;

    // Parallelization Speedup
    TCanvas c4;
    c4.cd();
    TGraphErrors* g_1000_TH_speedup  = (TGraphErrors*) f->Get(Form("g_%s_nTk1000_TH_speedup",tests[i].Data()));
    TGraphErrors* g_5000_TH_speedup  = (TGraphErrors*) f->Get(Form("g_%s_nTk5000_TH_speedup",tests[i].Data()));
    TGraphErrors* g_10000_TH_speedup = (TGraphErrors*) f->Get(Form("g_%s_nTk10000_TH_speedup",tests[i].Data()));
    g_1000_TH_speedup->SetTitle(Form("%s Parallelization Speedup on %s [nVU="+nvu+"]",stests[i].Data(),shORm[i].Data()));
    g_1000_TH_speedup->GetXaxis()->SetTitle("Number of Threads");
    g_1000_TH_speedup->GetYaxis()->SetTitle("Speedup");
    g_1000_TH_speedup->GetXaxis()->SetRangeUser(1,maxth);
    g_1000_TH_speedup->GetYaxis()->SetRangeUser(0,maxth);
    g_1000_TH_speedup->SetLineWidth(2);
    g_5000_TH_speedup->SetLineWidth(2);
    g_10000_TH_speedup->SetLineWidth(2);
    g_1000_TH_speedup->SetLineColor(kBlue);
    g_5000_TH_speedup->SetLineColor(kGreen+1);
    g_10000_TH_speedup->SetLineColor(kRed);
    g_1000_TH_speedup->SetMarkerStyle(kFullCircle);
    g_5000_TH_speedup->SetMarkerStyle(kFullCircle);
    g_10000_TH_speedup->SetMarkerStyle(kFullCircle);
    g_1000_TH_speedup->SetMarkerColor(kBlue);
    g_5000_TH_speedup->SetMarkerColor(kGreen+1);
    g_10000_TH_speedup->SetMarkerColor(kRed);
    g_1000_TH_speedup->Draw("ALP");
    g_5000_TH_speedup->Draw("LP");
    g_10000_TH_speedup->Draw("LP");
    TLine lth(1,1,maxth,maxth);
    lth.Draw();
    TLegend* leg_TH_speedup = new TLegend(0.20,0.65,0.45,0.85);
    leg_TH_speedup->SetBorderSize(0);
    leg_TH_speedup->AddEntry(g_1000_TH_speedup,"1000 Tks/Ev","LP");
    leg_TH_speedup->AddEntry(g_5000_TH_speedup,"5000 Tks/Ev","LP");
    leg_TH_speedup->AddEntry(g_10000_TH_speedup,"10000 Tks/Ev","LP");
    leg_TH_speedup->Draw();
    c4.SetGridy();
    c4.Update();
c4.SaveAs(Form("%s_%s_th_speedup.png",tests[i].Data(),hORm.Data()));
    delete leg_TH_speedup;
  }
}
