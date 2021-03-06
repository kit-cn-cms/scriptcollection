
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"


#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#endif

using namespace TMVA;


void Regressiontraining_newVarTest(   )
{
  TMVA::Tools::Instance();

  // to get access to the GUI and all tmva macro
  TString tmva_dir(TString(gRootDir) + "/tmva");
  if(gSystem->Getenv("TMVASYS"))
    tmva_dir = TString(gSystem->Getenv("TMVASYS"));
  gROOT->SetMacroPath(tmva_dir + "/test/:" + gROOT->GetMacroPath() );
  gROOT->ProcessLine(".L TMVARegGui.C");


  TString outfileName( "BReg_0906_BaseLine.root" );
//  TString outfileName( "BReg_0823_JESUP.root" );
//  TString outfileName( "BReg_0823_JESDOWN.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );


   TMVA::Factory *factory = new TMVA::Factory( "BReg_0906_BaseLine", outputFile,"V:!Silent:Color:DrawProgressBar" );
//   TMVA::Factory *factory = new TMVA::Factory( "BReg_0823_JESUP", outputFile,"V:!Silent:Color:DrawProgressBar" );
//   TMVA::Factory *factory = new TMVA::Factory( "BReg_0823_JESDOWN", outputFile,"V:!Silent:Color:DrawProgressBar" );

   //Add Variables to factory
   factory->AddVariable("Jet_Pt","Jet_pt","units", 'F');
   factory->AddVariable("Jet_corr","Jet_corr","units", 'F');
   factory->AddVariable("N_PrimaryVertices","N_PrimaryVertices","units", 'F');

   factory->AddVariable("Jet_Eta","Jet_eta","units", 'F');
   factory->AddVariable("Jet_Mt","Jet_mt","units", 'F');
   factory->AddVariable("Jet_leadTrackPt","Jet_leadTrackPt","units", 'F');
   factory->AddVariable("Jet_leptonPtRel","Jet_leptonPtRel","units", 'F');
   factory->AddVariable("Jet_leptonPt","Jet_leptonPt","units", 'F');
   factory->AddVariable("Jet_leptonDeltaR","Jet_leptonDeltaR","units", 'F');
   factory->AddVariable("Jet_totHEFrac","Jet_totHEFrac","units", 'F');
   factory->AddVariable("Jet_nEmEFrac","Jet_neEmEF","units", 'F');
   //factory->AddVariable("Jet_chargedMult","Jet_chargedMult","units", 'F');
   factory->AddVariable("Jet_vtxPt","Jet_vtxPt","units", 'F');
   factory->AddVariable("Jet_vtxMass","Jet_vtxMass","units", 'F');
   factory->AddVariable("Jet_vtx3DVal","Jet_vtx3dL","units", 'F');
   factory->AddVariable("Jet_vtxNtracks","Jet_vtxNtrk","units", 'F');
   factory->AddVariable("Jet_vtx3DSig","Jet_vtx3deL","units", 'F');

   //factory->AddTarget( "Jet_MatchedPartonPt / Jet_Pt" );
   factory->AddTarget( "Jet_MatchedGenJetwNuPt / Jet_Pt" );
   //factory->AddTarget( "Jet_MatchedGenJetwNuPt" );
   //factory->AddTarget( "Jet_MatchedPartonPt" );

   /*
   factory->AddSpectator( "Jet_MatchedPartonPt");
   factory->AddSpectator( "Jet_MatchedPartonFlav" );
   factory->AddSpectator( "Jet_PartonFlav" );
   factory->AddSpectator( "Jet_Flav" );
   factory->AddSpectator( "Evt_Odd" );
   factory->AddSpectator("Evt_Rho");
   */

   std::cout << "before fname" << endl;
   //Root file for Training
   TFile *input(0);
   TString fname = "/nfs/dust/cms/user/kschweig/JetRegression/training/trees0823/TreeforbReg_nominal_0823.root";
//   TString fname = "/nfs/dust/cms/user/kschweig/JetRegression/training/trees0823/TreeforbReg_JESUP_0823.root";
//   TString fname = "/nfs/dust/cms/user/kschweig/JetRegression/training/trees0823/TreeforbReg_JESDOWN_0823.root";
   //TString fname = "/nfs/dust/cms/user/kschweig/JetRegression/trees0619_ttbar/ttbarforbReg.root";
   //   TString fname = "/nfs/dust/cms/user/kschweig/JetRegression/trees0113/ttbar_nominal.root";
   std::cout << "after fname" << endl;

   if (!gSystem->AccessPathName( fname )) {
     input = TFile::Open( fname ); // check if file in local directory exists
   }
   if (!input) {
     std::cout << "ERROR: could not open data file" << std::endl;
     exit(1);
   }

   std::cout << "--- TMVARegression           : Using input file: " << input->GetName() << std::endl;

   TTree *Tree = (TTree*)input->Get("bRegTree");
   Double_t regWeight = 1.0;

   std::cout << "Add RegressionTree" << endl;
   factory->AddRegressionTree( Tree, regWeight );

   std::cout << "Add weight expression" << endl;
   factory->SetWeightExpression("Weight_PU * Weight_CSV * Weight_ElectronSFID * Weight_ElectronSFIso * Weight_MuonSFID * Weight_MuonSFTrigger * Weight_MuonSFIso * Weight_ElectronSFGFS * Weight_ElectronSFTrigger","Regression");

   //Cut on on samples
   TCut mycut = "Evt_Odd == 1 && abs(Jet_Flav) == 5 && Jet_Pt > 25 && Jet_Pt < 600  && Jet_Eta <= 2.4 && Jet_MatchedGenJetwNuPt > 0 && Jet_MatchedGenJetwNuPt < 600 && abs(Jet_MatchedPartonFlav) == 5";
   //TCut mycut = "Evt_Odd == 1 && abs(Jet_Flav) == 5 && abs(Jet_MatchedPartonFlav) == 5 && Jet_Eta <= 2.4";

   std::cout << "Prepare Training" << endl;
   factory->PrepareTrainingAndTestTree( mycut, "V:VerboseLevel=Debug:nTrain_Regression=200000:nTest_Regression=250000:SplitMode=Random:NormMode=NumEvents:!V" );
   //factory->PrepareTrainingAndTestTree( mycut, "nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V" );

   bool usebdt = true;

   if (usebdt) {
     std::cout << "Book BTDG" << endl;
     //factory->BookMethod( TMVA::Types::kBDT, "BDTG","!H:V:VerbosityLevel=Debug:NTrees=1200::BoostType=Grad:Shrinkage=0.1:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=30:MaxDepth=5:PruneMethod=costcomplexity:PruneStrength=30:VarTransform=U(Jet_leptonDeltaR)" );
     factory->BookMethod( TMVA::Types::kBDT, "BDTG","!H:V:VerbosityLevel=Debug:NTrees=1200::BoostType=Grad:Shrinkage=0.075:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=30:MaxDepth=3:PruneMethod=costcomplexity:PruneStrength=30:VarTransform=U(Jet_leptonDeltaR):MinNodeSize=0.6" );
     //factory->BookMethod( TMVA::Types::kBDT, "BDTG","!H:!V:NTrees=200:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:MaxDepth=3:PruneMethod=CostComplexity:PruneStrength=30" );
     }
   else {
     std::cout << "Book ANN" << endl;
     factory->BookMethod( TMVA::Types::kMLP, "MLP_ANN", "");
   }

   // Train MVAs using the set of training events
   std::cout << "Train MVA" << endl;
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   std::cout << "Test MVA" << endl;
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   std::cout << "Eval MVA" << endl;
   factory->EvaluateAllMethods();

   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVARegression is done!" << std::endl;

   delete factory;

}
