#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "TCanvas.h"

#include <fstream>

void hitres()
{
  // initial config + input
  TFile * file = TFile::Open("valtree.root");
  TTree * tree = (TTree*)file->Get("fittree");

  Float_t width = 0.9;

  Int_t nlayers_per_seed = 3;
  Int_t nLayers = 17;

  Float_t z_prop    [nLayers]; TBranch *b_z_prop;     tree->SetBranchAddress("z_prop",z_prop,&b_z_prop);
  Float_t ez_prop   [nLayers]; TBranch *b_ez_prop;    tree->SetBranchAddress("ez_prop",ez_prop,&b_ez_prop);
  Float_t z_hit     [nLayers]; TBranch *b_z_hit;      tree->SetBranchAddress("z_hit",z_hit,&b_z_hit);
  Float_t ez_hit    [nLayers]; TBranch *b_ez_hit;     tree->SetBranchAddress("ez_hit",ez_hit,&b_ez_hit);
  Float_t pphi_prop [nLayers]; TBranch *b_pphi_prop;  tree->SetBranchAddress("pphi_prop",pphi_prop,&b_pphi_prop);
  Float_t epphi_prop[nLayers]; TBranch *b_epphi_prop; tree->SetBranchAddress("epphi_prop",epphi_prop,&b_epphi_prop);
  Float_t pphi_hit  [nLayers]; TBranch *b_pphi_hit;   tree->SetBranchAddress("pphi_hit",pphi_hit,&b_pphi_hit);
  Float_t epphi_hit [nLayers]; TBranch *b_epphi_hit;  tree->SetBranchAddress("epphi_hit",epphi_hit,&b_epphi_hit);

  // setup histograms
  TH1F * z_reso[nLayers];
  TH1F * z_pull[nLayers];
  TH1F * phi_reso[nLayers];
  TH1F * phi_pull[nLayers];
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    z_reso[ilay]   = new TH1F(Form("z_reso_%i",ilay),Form("z Resolution Layer %i",ilay),1001,-20.f,20.f);
    z_pull[ilay]   = new TH1F(Form("z_pull_%i",ilay),Form("z Pull Layer %i",ilay),1001,-5.f,5.f);
    phi_reso[ilay] = new TH1F(Form("phi_reso_%i",ilay),Form("#phi Resolution Layer %i",ilay),1001,-0.1,0.1);  
    phi_pull[ilay] = new TH1F(Form("phi_pull_%i",ilay),Form("#phi Pull Layer %i",ilay),1001,-5.f,5.f);
  }

  // loop over events
  for (UInt_t entry = 0; entry < tree->GetEntries(); entry++)
  {
    tree->GetEntry(entry);

    // loop over layers
    for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
    {
      if (z_hit[ilay] != -1000.f)
      {
	z_reso[ilay]->Fill(z_hit[ilay]-z_prop[ilay]);
	Float_t z_err = std::sqrt(std::pow(ez_prop[ilay],2)+std::pow(ez_hit[ilay],2));
	z_pull[ilay]->Fill((z_hit[ilay]-z_prop[ilay])/z_err);
      }
      if (pphi_hit[ilay] != -1000.f)
      {
	phi_reso[ilay]->Fill(pphi_hit[ilay]-pphi_prop[ilay]);
	Float_t pphi_err = std::sqrt(std::pow(epphi_prop[ilay],2)+std::pow(epphi_hit[ilay],2));
	phi_pull[ilay]->Fill((pphi_hit[ilay]-pphi_prop[ilay])/pphi_err);
      } 
    }
  }
  
  Float_t z_reso_width  [nLayers];
  Float_t phi_reso_width[nLayers];

  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    // z
    {
      Int_t total  = z_reso[ilay]->Integral(0,z_reso[ilay]->GetNbinsX()+1); // include UF and OF
      Int_t midbin = (z_reso[ilay]->GetNbinsX()/2)+(z_reso[ilay]->GetNbinsX()%2); 
      
      for (Int_t ibin = 0; ibin <= midbin; ibin++)
      {
	Int_t beg = midbin - ibin;
	Int_t end = midbin + ibin;
	
	Int_t integral = z_reso[ilay]->Integral(beg,end);
	Float_t frac = Float_t(integral)/Float_t(total);
	if (frac > width)
	{
	  z_reso_width[ilay] = z_reso[ilay]->GetBinCenter(end);
	  break;
	}
      }
    }

    // phi
    {
      Int_t total  = phi_reso[ilay]->Integral(0,phi_reso[ilay]->GetNbinsX()+1); // include UF and OF
      Int_t midbin = (phi_reso[ilay]->GetNbinsX()/2)+(phi_reso[ilay]->GetNbinsX()%2); 
      
      for (Int_t ibin = 0; ibin <= midbin; ibin++)
      {
	Int_t beg = midbin - ibin;
	Int_t end = midbin + ibin;
	
	Int_t integral = phi_reso[ilay]->Integral(beg,end);
	Float_t frac = Float_t(integral)/Float_t(total);
	if (frac > width)
	{
	  phi_reso_width[ilay] = phi_reso[ilay]->GetBinCenter(end);
	  break;
	}
      }
    }
  }

  // save ranges
  std::ofstream outtxt;
  outtxt.open("res.txt",std::ios::trunc);

  outtxt << "layer," ;
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    outtxt << ilay << ",";
  }
  outtxt << std::endl;
  
  outtxt << "z reso," ;
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    outtxt << z_reso_width[ilay] << ",";
  }
  outtxt << std::endl;

  outtxt << "phi reso," ;
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    outtxt << phi_reso_width[ilay] << ",";
  }
  outtxt << std::endl;

  outtxt.close();

  // save histograms
  TCanvas * canv = new TCanvas(); canv->cd(); canv->SetLogy(1);
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    z_reso[ilay]->Draw();
    canv->SaveAs(Form("%s.png",z_reso[ilay]->GetName()));
    
    phi_reso[ilay]->Draw();
    canv->SaveAs(Form("%s.png",phi_reso[ilay]->GetName()));
  }

  // delete everything like a normal person
  delete canv;
  for (Int_t ilay = nlayers_per_seed; ilay < nLayers; ilay++)
  {
    delete z_reso[ilay];
    delete z_pull[ilay];
    delete phi_reso[ilay];
    delete phi_pull[ilay];
  }
  delete tree;
  delete file;
}
