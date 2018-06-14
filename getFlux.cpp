//Make sure numu_CV_AV_TPC is in your directory before running!
//Make sure [event rate histogram filename] is in your directory before running!
//Somehow, check that the binning on both histograms is the same, or this won't work!

int numBins = numu_CV_AV_TPC->GetNBins();
double binFlux; //number of neutrinos produced at the energy corresponding to a particular bin
double binRate; //event rate corresponding to the energy of that bin
double binEventFlux; //number of events expected at the energy level corresponding to the energy bin

//create a histogram to hold flux x event rate, with the same axis range as numu_CV_AV_TPC
TH1 *eventFlux = new TH1(numBins, numu_CV_AV_TPC.GetXaxis()->GetXmin(), numu_CV_AV_TPC.GetXaxis()->GetXmax());

//fill eventFlux
for(int i; i < numBins; i++){
  binFlux = numu_CV_AV_TPC -> GetBinContent(i);
  binRate = /* [event rate histogram filename]->GetBinContent(i); */
  //do something with binFlux. Store somewhere? Multiply by event rate? Both.
  binEventRate = binFlux*binRate;
  eventFlux -> Fill(i, binEventRate);
}

/*
HALT!!! The event rate file is a TGraph, not a TH1!!!!!!!! Fix this!
*/
//draw the histogram to see whether it makes sense
eventFlux -> Draw();
