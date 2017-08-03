// use https://github.com/kmcdermo/mictest.git
// branch: fdt-dump
// make -j 12 WITH_ROOT=yes

// CMSSW: ./mkFit/mkFit --ext-rec-tracks --geom CMS-2017 --read --file-name /data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/10muEtaLT06Pt1to10/memoryFile.fv3.recT.072617.bin --build-ce --cmssw-seeds --num-thr 8 --num-events 100 >& cmsswdump100.txt; rm *.root
// Sim: ./mkFit/mkFit --geom CMS-2017 --read --file-name /data/nfsmic/slava77/samples/2017/pass-4874f28/initialStep/10muEtaLT06Pt1to10/memoryFile.fv3.recT.072617.bin --build-ce --num-thr 8 --num-events 100 >& simdump100.txt

// need to modify dumped text file: search replace "[" and "]"
// drop the lines with chi2, as well as those with the track id info
// as well as all other lines not from dumper 

#include "TString.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TCanvas.h"

#include <map>
#include <vector>
#include <fstream>
#include <iostream>

typedef std::vector<int> vi;
typedef std::map<int,std::vector<float> > mivf;
typedef std::vector<std::vector<TGraph*> > vvtg;
typedef std::vector<TString> vts;
typedef std::vector<std::vector<TLegend*> > vvtl;

int readFile(TString, vi&, mivf&, mivf&, mivf&, mivf&, mivf&);
void initgraphs(vvtg&, const int, const vi&, const vts&, const vts&, const vts&, const vts&);
void fillgraphs(vvtg&, mivf&, mivf&, mivf&, mivf&, mivf&);
void setminmax(vvtg&);
void drawandsave(vvtg&, const bool);

void plottrajectory(const bool isCMSSW = true)
{
  vi n;
  mivf x,y,r,p,z;
  const int ntk = readFile(Form("%s/firstntks.txt",isCMSSW?"cmssw":"sim"),n,x,y,r,p,z);

  vts name   = {"xy","rphi","zr"};
  vts title  = {"Y vs X","#Phi vs R","R vs Z"};
  vts xtitle = {"X","R","Z"};
  vts ytitle = {"Y","#Phi","R"};
  vvtg gr(name.size());

  initgraphs(gr,ntk,n,name,title,xtitle,ytitle);
  fillgraphs(gr,x,y,r,p,z);
  setminmax(gr);
  drawandsave(gr,isCMSSW);
}

int readFile(TString filename, vi& n, mivf& x, mivf& y, mivf& r, mivf& p, mivf& z)
{
  std::ifstream infile(filename.Data(),std::ios_base::in);
  std::vector<TString> vars(10);
  std::vector<int> idcs(5);
  std::vector<float> vals(5);

  int idx = 100;
  int tkid = -1;
  while ( infile 
	  >> vars[0] >> idcs[0] >> vars[1] >> idcs[1] >> vars[2] >> idcs[2] >> vars[3] >> idcs[3] >> vars[4] >> idcs[4] 
	  >> vars[5] >> vals[0] >> vars[6] >> vals[1] >> vars[7] >> vals[2] >> vars[8] >> vals[3] >> vars[9] >> vals[4] )
  { 
    if (idcs[2] == -1) continue;
    if (idcs[0] < idx) {tkid++; n.push_back(0);}
    idx = idcs[0];
    n[tkid]++;
    
    x[tkid].push_back(vals[0]);
    y[tkid].push_back(vals[1]);
    r[tkid].push_back(vals[2]);
    p[tkid].push_back(vals[3]);
    z[tkid].push_back(vals[4]);
  }

  return tkid+1;
}

void initgraphs(vvtg& gr, const int ntk, const vi& n, const vts& name, const vts& title, const vts& xtitle, const vts& ytitle)
{
  for (int i = 0; i < gr.size(); i++)
  {
    gr[i].resize(ntk);
    for (int j = 0; j < gr[i].size(); j++)
    {
      gr[i][j] = new TGraph(n[j]);
      gr[i][j]->SetName(name[i].Data());
      gr[i][j]->SetTitle(Form("%s;%s;%s",title[i].Data(),xtitle[i].Data(),ytitle[i].Data()));
      gr[i][j]->SetLineColor(j%2==0?kRed:kBlue);
      gr[i][j]->SetMarkerColor(j%2==0?kRed:kBlue);
      gr[i][j]->SetMarkerStyle(j%2==0?2:5);
      gr[i][j]->SetMarkerSize(1);

    }
  }
}


void fillgraphs(vvtg& gr, mivf& x, mivf& y, mivf& r, mivf& p, mivf& z)
{
  for (int i = 0; i < gr.size(); i++)
  {
    for (int j = 0; j < gr[i].size(); j++)
    {
      for (int k = 0; k < gr[i][j]->GetN(); k++) 
      {
	if (i == 0) gr[i][j]->SetPoint(k,x[j][k],y[j][k]);
	if (i == 1) gr[i][j]->SetPoint(k,r[j][k],p[j][k]);
	if (i == 2) gr[i][j]->SetPoint(k,z[j][k],r[j][k]);
      }
    }
  }
}


void setminmax(vvtg& gr)
{
  for (int i = 0; i < gr.size(); i++)
  {
    double min;
    double max;
    for (int j = 0; j < gr[i].size(); j++)
    {
      bool id0 = (j%2==0);
      if (id0) 
      {
	min =  1e9;
	max = -1e9;
      }

      double tmpmin = gr[i][j]->GetMinimum();
      double tmpmax = gr[i][j]->GetMaximum();

      if (tmpmin < min) min = tmpmin;
      if (tmpmax > max) max = tmpmax;

      if (!id0)
      {
	gr[i][j-1]->SetMinimum(min);
	gr[i][j-1]->SetMaximum(max);
	gr[i][j]  ->SetMinimum(min);
	gr[i][j]  ->SetMaximum(max);
      }
    }
  }
}

void drawandsave(vvtg& gr, const bool isCMSSW)
{  
  TCanvas * canv = new TCanvas(); canv->cd();
  vvtl leg(gr.size());
  
  for (int i = 0; i < leg.size(); i++)
  {
    leg[i].resize(gr[i].size()/2);
    for (int j = 0; j < gr[i].size(); j++)
    {
      bool id0 = (j%2==0);
      int k = j/2;
      
      if (id0) leg[i][k] = new TLegend(0.90,0.90,0.99,0.99);
      
      gr[i][j]->Draw(id0?"AP":"P SAME");
      leg[i][k]->AddEntry(gr[i][j],id0?"mkFit":(isCMSSW?"CMSSW":"Sim"),"lp");

      if (!id0) 
      {
	leg[i][k]->Draw("SAME");
	canv->SaveAs(Form("%s/pair%i_%s.png",(isCMSSW?"cmssw":"sim"),k,gr[i][j]->GetName()));
	delete leg[i][k];
	delete gr[i][j-1];
	delete gr[i][j];
      }
    }
  }
  delete canv;
}
