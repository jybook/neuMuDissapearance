#include <stdio.h> 
#include <math.h>

void codeTester(){
	
	double flux[200];
	double crossSection[200];

	crossSection[0] = 24;
	cout <<"crossSection address: "<< crossSection << endl;

	numuSetUp(crossSection, flux);
	cout << "numuSetUp completed" << endl;
	cout <<"crossSection address final: "<< crossSection << endl;
	cout << "the flux is: ";
	printArr(flux);
	cout << "the cross-section is: ";
	printArr(crossSection);

}

void numuSetUp(double *crossSecArr, double *fluxArr){
	
	TGraph* crossSection = 0;//holds cross-section graph
	TFile *crossSectionFile = TFile::Open("cross_section.root");
	crossSectionFile ->GetObject("qel_cc_n", crossSection);

	//Create an array holding the mean x values of the graph from 0 to 10 GeV at 50 meV intervals
	int relevantLength = qel_cc_n -> GetN();

	TH1 *flux = 0; //holds flux histogram
	TFile *fluxFile = TFile::Open("flux.root");
	fluxFile ->GetObject("numu_CV_AV_TPC", flux);

	int numBins = numu_CV_AV_TPC ->GetNbinsX();
	
	//Get values of cross section with the same binning as the histogram so we can multiply them together
	int ppb = relevantLength/numBins;//Get points per pin. relevantLength/numBins may not be an integer -- will this cause problems?

	//compute the mean value of the cross section for every "bin" of evergy, place it in an array
	double binMean;
	double x, y, x2, y2;
	int currentPoint=0;

	for(int i=0; i<numBins; i++,currentPoint += ppb){
		crossSection->GetPoint(currentPoint, x, y);
		crossSection->GetPoint(currentPoint + ppb - 1, x2, y2);
		binMean = (y+y2)/2;
		crossSecArr[i] = binMean;
	}

	TH1 *flux = 0; //holds flux histogram
	TFile *fluxFile = TFile::Open("flux.root");
	fluxFile ->GetObject("numu_CV_AV_TPC", flux);

	for(int i=0; i<200; i++){
		fluxArr[i] = flux -> GetBinContent(i);
	}

return;

}

void printArr(double *a1){
	int length = 200;
	for(int i=0; i<length; i++){
		cout << a1[i] << ", ";
	}
	cout << endl;
}
