#include <stdio.h> 
#include <math.h>
//Get the data we need in the form we need it
void numuSetUp(){

	TGraph* crossSection = 0;//holds cross-section graph
	TFile *crossSectionFile = TFile::Open("cross_section.root");
	crossSectionFile ->GetObject("qel_cc_n", crossSection);

	//get an array holding the x values of the graph from 0 to 10 GeV
	int relevantLength = qel_cc_n -> GetN();
	cout << "number of points in the graph =" << relevantLength << endl;
	

	TH1 *flux = 0; //holds flux histogram
	TFile *fluxFile = TFile::Open("flux.root");
	fluxFile ->GetObject("numu_CV_AV_TPC", flux);

	//flux->Draw();

	int numBins = numu_CV_AV_TPC ->GetNbinsX();
	double binFlux; //number of neutrinos produced at the energy corresponding to a particular bin
	double binCSX; //cross section corresponding to the energy of that bin
	double binEventRate; //number of events expected at the energy level corresponding to the energy bin

	//Get values of cross section with the same binning as the histogram so we can multiply them together
	int ppb = relevantLength/numBins;//Get points per pin. relevantLength/numBins may not be an integer -- will this cause problems?

	//compute the mean value of the cross section for every "bin" of evergy, place it in an array
	double crossSecHist[200];//crossSecHist[numBins];
	double binMean;
	//double binSum;
	double x;
	double y;
	double x2;
	double y2;
	int currentPoint=0;

	crossSection ->Draw();
	

	for(int i=0; i<numBins; i++,currentPoint += ppb){
		//cout << "Current point: "<< currentPoint << " ";
		crossSection->GetPoint(currentPoint, x, y);
		crossSection->GetPoint(currentPoint + ppb - 1, x2, y2);
		//cout << "x values: " << x << " , " << x2 << " ";
		//cout << "y values: " << y << " , " << y2 << " ";
		binMean = (y+y2)/2;
		crossSecHist[i] = binMean;
		//cout << "the mean for bin "<< i << " is " << binMean << endl;

return;
}
