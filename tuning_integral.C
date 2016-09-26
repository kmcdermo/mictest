#include "TString.h"
#include "TStyle.h"
#include "TVirtualFitter.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

#include <fstream>
#include <iostream>

void tuning_integral()
{
  TString var   = "phi";
  Float_t range = 6.0;
  Int_t   nbins = 1001;

  bool save = true;
  ofstream yields(Form("zphi_newsimple/%sintegrals.txt",var.Data()),std::ios_base::app);

  TFile * file = TFile::Open("valtree.root");
  TTree * tree = (TTree*)file->Get("fittree");
  
  for (int layer = 3; layer < 10; layer++)
  {
    TH1F * hist = new TH1F("hist","hist",nbins,-range,range); hist->Sumw2();
    tree->Draw(Form("(%s_prop[%i]-%s_hit[%i])/sqrt(e%s_prop[%i])>>%s",var.Data(),layer,var.Data(),layer,var.Data(),layer,hist->GetName()),Form("%s_hit[%i]!=-1000",var.Data(),layer),"goff");
    
    int onesigbins[2]   = {-1,-1}; int onesigints[2]   = {-1,-1}; float onesigvals[2]   = {-99.,-99.}; bool onesigma   = false; 
    int twosigbins[2]   = {-1,-1}; int twosigints[2]   = {-1,-1}; float twosigvals[2]   = {-99.,-99.}; bool twosigma   = false; 
    int threesigbins[2] = {-1,-1}; int threesigints[2] = {-1,-1}; float threesigvals[2] = {-99.,-99.}; bool threesigma = false;

    int total = hist->Integral(0,1002);
    for (int diff = 0; diff <= 501; diff++) {
      int beg = 501 - diff;
      int end = 501 + diff;
    
      int integral = hist->Integral(beg,end);
      if ((float(integral)/float(total) > 0.6826895) && !onesigma) {
	onesigma = true;
	onesigbins[0] = end - 1; onesigbins[1] = end;
	onesigints[0] = hist->Integral(beg+1,end-1);
	onesigints[1] = integral;
	onesigvals[0] = hist->GetBinCenter(end-1);
	onesigvals[1] = hist->GetBinCenter(end);
      }
      else if ((float(integral)/float(total) > 0.9544997) && !twosigma) {
	twosigma = true;
	twosigbins[0] = end - 1; twosigbins[1] = end;
	twosigints[0] = hist->Integral(beg+1,end-1);
	twosigints[1] = integral;
	twosigvals[0] = hist->GetBinCenter(end-1);
	twosigvals[1] = hist->GetBinCenter(end);
      }
      else if ((float(integral)/float(total) > 0.9973002) && !threesigma) {
	threesigma = true;
	threesigbins[0] = end - 1; threesigbins[1] = end;
	threesigints[0] = hist->Integral(beg+1,end-1);
	threesigints[1] = integral;
	threesigvals[0] = hist->GetBinCenter(end-1);
	threesigvals[1] = hist->GetBinCenter(end);
      }
    }

    if (!save) {
      std::cout << "layer: " << layer << " range: " << range << std::endl;
      std::cout << total << std::endl << std::endl;
      std::cout << onesigbins[0] << "-" << onesigbins[1] << std::endl;
      std::cout << onesigints[0] << "-" << onesigints[1] << std::endl;
      std::cout << onesigvals[0] << "-" << onesigvals[1] << std::endl << std::endl;
      
      std::cout << twosigbins[0] << "-" << twosigbins[1] << std::endl;
      std::cout << twosigints[0] << "-" << twosigints[1] << std::endl;
      std::cout << twosigvals[0] << "-" << twosigvals[1] << std::endl << std::endl;
      
      std::cout << threesigbins[0] << "-" << threesigbins[1] << std::endl;
      std::cout << threesigints[0] << "-" << threesigints[1] << std::endl;
      std::cout << threesigvals[0] << "-" << threesigvals[1] << std::endl;
    }
    else {
      yields << layer << " "
	     << (onesigvals[0]+onesigvals[1])/2. << " "
	     << (twosigvals[0]+twosigvals[1])/2. << " "
	     << (threesigvals[0]+threesigvals[1])/2. << " "
	     << std::endl;
    }
    delete hist;
  }
  delete tree;
  delete file;
  yields.close();
}
