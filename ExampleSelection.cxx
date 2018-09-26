#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <TH2D.h>
#include <THStack.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TH1D.h>
#include <json/json.h>
#include "gallery/ValidHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "ExampleSelection.h"
#include "ExampleTools.h"
#include "core/Event.hh"

namespace ana {
  namespace ExampleAnalysis {

ExampleSelection::ExampleSelection() : SelectionBase(), fNuCount(0), fEventCounter(0) {}


void ExampleSelection::Initialize(Json::Value* config) {
  // Make a histogram
  fNuVertexXZHist = new TH2D("nu_vtx_XZ", "",
                             100, -1000, 1000, 100, -1000, 1000);

  fModeAndVertex = new TH2D("mode_at_vtx_XZ", "",
                             100, -1000, 1000, 100, -1000, 1000);

  fDaughterAndVertex = new TH2D("daughters_at_vtx_XZ", "",
                             100, -1000, 1000, 100, -1000, 1000);

  top = new TLine(-199.15, 500, 199.15, 500);
  left = new TLine(-199.15, 0, -199.15, 500);
  bottom = new TLine(-199.15, 0, 199.15, 0);
  right = new TLine(199.15, 0, 199.15, 500);

  phID1 = 0;
  phID2 = 0;
  phID3 = 0;
  phID4 = 0;
  numBins = 100;
  minBin = 0.0;
  maxBin = 2.5;
  fPHadron1 = new TH1D("PHadron1", "", numBins, minBin, maxBin);
  fPHadron2 = new TH1D("PHadron2", "", numBins, minBin, maxBin);
  fPHadron3 = new TH1D("PHadron3", "", numBins, minBin, maxBin);
  fPHadron4 = new TH1D("PHadron4", "", numBins, minBin, maxBin);

  fPHStack = new THStack("fPHStack", "Neutrino Energy Sorted by Parent Hadron");
  fLegend = new TLegend(0.7, 0.9, 0.9, 0.6);
  fLegend -> SetHeader("Parent PDG code", "C");

  // Load configuration parameters
  fMyParam = 0;
  fTruthTag = { "generator" };
  fFluxTag = { "generator" };
  fParticleTag = { "generator" };


  if (config) {
    fMyParam = { (*config)["ExampleAnalysis"].get("parameter", 0).asInt() };
    fTruthTag = { (*config)["ExampleAnalysis"].get("MCTruthTag", "generator").asString() };
    fFluxTag  = { (*config)["ExampleAnalysis"].get("MCFluxTag", "generator").asString() };
    fParticleTag = { (*config)["ExampleAnalysis"].get("MCParticleTag", "largeant").asString() };
  }

  // Add custom branches
  AddBranch("nucount", &fNuCount);
  AddBranch("myvar", &fMyVar);
  AddBranch("parentHad", &fParentHad);
  AddBranch("interactionType", &fInteractionType);
  AddBranch("interactionMode", &fMode);
  AddBranch("daughter", &fDaughter);
  // Use some library code
  hello();
}

void ExampleSelection::Finalize() {
  // Output our histograms to the ROOT file
  fOutputFile->cd();
  fNuVertexXZHist->Write();
  fModeAndVertex -> Write();
  fDaughterAndVertex -> Write();

  fPHadron1 -> SetFillColor(kCyan-7);
  fPHadron2 -> SetFillColor(kViolet-4);
  fPHadron3 -> SetFillColor(kGreen-3);
  
  fPHadron1 -> Write();
  fPHadron2 -> Write();
  fPHadron3 -> Write();
  fPHadron4 -> Write();
   
  const char *ph1char = (std::to_string(phID1)).c_str();
  const char *ph2char = (std::to_string(phID2)).c_str();
  const char *ph3char = (std::to_string(phID3)).c_str();
  const char *ph4char = (std::to_string(phID4)).c_str();

  fLegend -> AddEntry(fPHadron1, ph1char, "f");
  fLegend -> AddEntry(fPHadron2, ph2char, "f");
  fLegend -> AddEntry(fPHadron3, ph3char, "f");
  fLegend -> AddEntry(fPHadron4, ph4char, "f");
   
  fPHStack->Add(fPHadron1);
  fPHStack->Add(fPHadron2);
  fPHStack->Add(fPHadron3);
  fPHStack->Add(fPHadron4);

  fCanvas = new TCanvas("fCanvas", "", 700, 500);
  fCanvas -> cd();
  fModeAndVertex -> Draw("COLZ");
  right ->Draw("al same");
  left -> Draw("l same");
  top  -> Draw("l same");
  bottom -> Draw("l same");
  fCanvas -> Write();

  fStackCanvas = new TCanvas("fStackCanvas", "", 700, 500);
  fStackCanvas -> cd(); 
  fPHStack -> Draw();
  fLegend -> Draw();
  fStackCanvas -> Write();
}

bool ExampleSelection::ProcessEvent(const gallery::Event& ev, std::vector<Event::Interaction>& reco) {
  if (fEventCounter % 10 == 0) {
    std::cout << "ExampleSelection: Processing event " << fEventCounter << std::endl;
  }
  fEventCounter++;

  // Grab a data product from the event
  auto const& mctruths = *ev.getValidHandle<std::vector<simb::MCTruth>>(fTruthTag);
  auto const& mcfluxes = *ev.getValidHandle<std::vector<simb::MCFlux>>(fFluxTag);
  auto const& mcparticles = *ev.getValidHandle<std::vector<simb::MCParticle>>(fParticleTag);
  // Fill in the custom branches
  fNuCount = mctruths.size();  // Number of neutrinos in this event
  fMyVar = fMyParam;

  //Iterates through each neutrino
  for (size_t i=0; i<fNuCount; i++){
   auto const &mctruth = mctruths.at(i);
   auto const &mcflux = mcfluxes.at(i);
   auto const &mcparticle = mcparticles.at(i);
   fParentHad = mcflux.fptype;
   fNuEnergy = mctruth.GetNeutrino().Nu().E();//mcflux.fnenergy;

   int fRelevantBin = (fNuEnergy/maxBin)*numBins;

   std::cout << "Bin #:" << fRelevantBin << " NuEnergy: "<< fNuEnergy << std::endl;

   if (fParentHad == phID1 || phID1 == 0) { fPHadron1 -> Fill(fNuEnergy); phID1 = fParentHad; }
   else if(fParentHad == phID2 || phID2 == 0) { fPHadron2 ->  Fill(fNuEnergy); phID2 = fParentHad; }
   else if(fParentHad == phID3 || phID3 == 0) { fPHadron3 ->  Fill(fNuEnergy); phID3 = fParentHad; }
   else if(fParentHad == phID4 || phID4 == 0) { fPHadron4 ->  Fill(fNuEnergy); phID4 = fParentHad; }

   fNumDaughters = mctruth.GetNeutrino().Nu().NumberDaughters();
   fDaughter = mctruth.GetNeutrino().Nu().FirstDaughter();
/*
   for(int i = 1; i<fNumDaughters; i++){
      fDaughter = std::next (fDaughter);   
        }    
*/
   fMode = mctruth.GetNeutrino().Mode();
   int interactionNumber = (fInteractionType-1000)*100;

   //Fill neutrino vertex position histogram
   fNuVertexXZHist->Fill(mctruth.GetNeutrino().Nu().Vx(),
                        mctruth.GetNeutrino().Nu().Vz());

   fModeAndVertex -> Fill(mctruth.GetNeutrino().Nu().Vx(),
                        mctruth.GetNeutrino().Nu().Vz(), mctruth.GetNeutrino().Mode()+1);
/*   
   if(mctruth.GetNeutrino().CCNC() == 1 ){
   fDaughterAndVertex -> Fill(mctruth.GetNeutrino().Nu().Vx(),
                        mctruth.GetNeutrino().Nu().Vz(), fDaughter);
    }*/
                                                                                                                                                                            156,3         93%
 }
  return true;
}

  }  // namespace ExampleAnalysis
}  // namespace ana


// This line must be included for all selections!
DECLARE_SBN_PROCESSOR(ana::ExampleAnalysis::ExampleSelection)
                                                                                                                                                                            196,5         Bot

                                                                                                                                                                            95,5          31%
