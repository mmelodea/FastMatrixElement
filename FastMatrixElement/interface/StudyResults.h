///::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
///::::::::::::::::::::::::::::::[ To do Some Studies From the FastME Results]:::::::::::::::::::::::::::::::::
///::::::::::::::::::::::::::::::::[ Code Designer: Miqueias M. de Almeida ]:::::::::::::::::::::::::::::::::::
///::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


#ifndef StudyResults_h
#define StudyResults_h


#include "FastMatrixElement/FastMatrixElement/interface/FmeDefinitions.h"

#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TLine.h>
#include <TLegend.h>
#include <TString.h>


void StudyResults(FmeSetup UserSetup){
  //To get better colors
  Int_t palette[5];
  palette[0] = 1;
  palette[1] = 2;
  palette[2] = 3;
  palette[3] = 4;
  palette[4] = 5;
  palette[5] = 6;
  palette[6] = 7;
  palette[7] = 8;
  palette[8] = 9;
  palette[9] = 10;
  gStyle->SetPalette(10,palette);

  const Int_t Number = 3;
  Double_t Red[Number]    = { 1.00, 0.00, 0.00 };
  Double_t Green[Number]  = { 0.00, 1.00, 0.00 };
  Double_t Blue[Number]   = { 1.00, 0.00, 1.00 };
  Double_t Length[Number] = { 0.00, 0.50, 1.00 };
  Int_t nb=50;
  TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);

  TString plot_name;
  
  //Does not show plots on fly
  gROOT->SetBatch(kTRUE);
  gStyle->SetOptStat(0);
  TCanvas *c1 = new TCanvas("c1","",0,0,600,600);

  
  ///-------------- Plot discriminant from singal and background --------------
  TLegend *leg = new TLegend(0.57,0.77,0.87,0.87);
  std::vector<TH1D*> hists;
  for(Int_t i=0; i<(Int_t)UserSetup.FmeFiles.Size(); i++){
    hists[i] = new TH1D(Form("hist%i",i),"Discriminant Based on Distance",100,0,1);
    hists[i]->SetLineColor(i+1);
    //hists[i]->SetFillColor(9);
    hists[i]->SetFillStyle(3001);
    hists[i]->GetXaxis()->SetTitle("P_{SB}(Distance)");
    hists[i]->GetYaxis()->SetTitle("Events/0.01");

    ((TTree*)(TFile::Open(UserSetup.FmeFiles[0]))->Get("FastME"))->Project(Form("hist%i",i),"Global_PsbDist");
    if(i == 0) hists[i]->Draw();
    else hists[i]->Draw("same");

    leg->AddEntry(hsig,Form("file%i",i),"f");
  }
/*  
  TTree *tsig  = (TTree*)fsig->Get("FastME");
  TTree *tbkg  = (TTree*)fbkg->Get("FastME");
  
  TH1D *hsig = new TH1D("hsig","Discriminant Based on Distance",100,0,1);
  hsig->SetLineColor(9);
  hsig->SetFillColor(9);
  hsig->SetFillStyle(3001);
  hsig->GetXaxis()->SetTitle("P_{SB}(Distance)");
  hsig->GetYaxis()->SetTitle("Events/0.01");

  TH1D *hbkg = new TH1D("hbkg","Discriminant Based on Distance",100,0,1);
  hbkg->SetLineColor(2);
  hbkg->SetFillColor(2);
  hbkg->SetFillStyle(3001);

  tsig->Project("hsig","Global_PsbDist");
  hsig->Draw();
  hbkg->Draw("same");
*/  
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  leg->Draw();

  c1->Update();
  std::cout<<ansi_yellow<<"Enter name to save discriminant distribution plot: "<<ansi_reset;
  std::cin >> plot_name;
  c1->Print(UserSetup.OutPath+"/"+plot_name+".png");
  ///----------------------------------------------------------------------  
  
/*  
  ///------------- Plot ROC curve -----------------------------------------
  Double_t SGlobal_PsbDist, BGlobal_PsbDist;
  tsig->SetBranchAddress("Global_PsbDist",&SGlobal_PsbDist);
  tbkg->SetBranchAddress("Global_PsbDist",&BGlobal_PsbDist);
  
  Double_t cutoff, integral=0;
  const int discret = 1000;
  float TPR[discret], FPR[discret], TP, FP, TN, FN;
  for(int j=0; j<discret; j++){
    cutoff = j/float(discret);
    TP = FP = TN = FN = 0;
    for(int i=0; i<tsig->GetEntries(); i++){
      tsig->GetEntry(i);
      if( SGlobal_PsbDist > cutoff ) TP++;
      if( SGlobal_PsbDist < cutoff ) FN++;
      tbkg->GetEntry(i);
      if( BGlobal_PsbDist > cutoff ) FP++;
      if( BGlobal_PsbDist < cutoff ) TN++;
    }
    TPR[j] = TP/float(TP + FN);
    FPR[j] = FP/float(FP + TN);
    if(j>0)
     integral += fabs(FPR[j-1]-FPR[j])*TPR[j];
  }

  TGraph *roc = new TGraph(discret,FPR,TPR);
  roc->SetTitle(Form("ROC Plot - Area under curve = %.3f",integral));
  roc->SetMarkerStyle(4);
  roc->SetMarkerSize(0.9);
  roc->SetMarkerColor(kOrange);
  roc->GetXaxis()->SetTitle("False Positive Rate");
  roc->GetXaxis()->SetRangeUser(0,1.);
  roc->GetYaxis()->SetTitle("True Positive Rate");
  roc->GetYaxis()->SetRangeUser(0,1.1);
  
  TLine *l3 = new TLine(0,0,1,1);
  l3->SetLineColor(kGreen-6);
  l3->SetLineStyle(2);

  TLine *l50  = new TLine(0,0.5,1,0.5);		l50->SetLineStyle(2);
  TLine *l80  = new TLine(0,0.8,1,0.8);		l80->SetLineStyle(2);
  TLine *l90  = new TLine(0,0.9,1,0.9);		l90->SetLineStyle(2);
  TLine *l95  = new TLine(0,0.95,1,0.95);	l95->SetLineStyle(2);
  TLine *l100 = new TLine(0,1.,1,1.);		l100->SetLineStyle(2);
    
  roc->Draw("AP");
  l3->Draw();
  l50->Draw();
  l80->Draw();
  l90->Draw();
  l95->Draw();
  l100->Draw();
  
  c1->Update();
  std::cout<<ansi_yellow<<"Enter name to save ROC plot: "<<ansi_reset;
  std::cin >> plot_name;
  c1->Print(UserSetup.OutPath+"/"+plot_name+".png");
  ///----------------------------------------------------------------------
*/ 

  return;
}


#endif
