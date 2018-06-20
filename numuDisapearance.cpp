#include <stdio.h> 
#include <math.h>

//in this script, it's assumed that the flux file has 200 bins with widths of 50 MeV each
int nBins = 200;

void numuDisapearance(int xbins, int ybins){
	
	TCanvas *c1 = new TCanvas("c1", "Event Rates", 1200, 600);
	c1 -> Divide(2, 1, 0.01, 0.01, 0);// divide(number of pads in the x direction, number of pads in the y direction, x-margin, y-margin, color)

	//create arrays to hold values for flux and cross section, fill them with the appropriate info
	double flux[200];
	double crossSection[200];
	numuSetUp(crossSection, flux);

	//calculate event rate at 470m
	double eventRate[200];
	calculateEventRate(eventRate, flux, crossSection);
	
	//adjust event rate for the ICARUS TPC
	TH1 *eventRateICARUS = new TH1D("eventRateICARUS", "Predicted Event Rate at 600m (no oscillations); Neutrino Energy (GeV); Event Rate", 200, 0.0, 10.1);
	double scale = (470.0*470)/(600.0*600);
	double binRate;
	for(int j=0; j<200; j++){
		binRate = scale*eventRate[j];
		eventRateICARUS -> SetBinContent(j, binRate);
	}
	//Display event rate in ICARUS for the null hypothesis
	c1 -> cd(1);//cd(2);
	eventRateICARUS -> Draw();

	//calculate event rate in ICARUS for the fourth oscillation hypothesis with a particular delta m^2, sin^2(2theta)
	TH1 *oscRateICARUS = new TH1D("oscRateICARUS", "Predicted Event Rate at 600m (with oscillations); Neutrino Energy (GeV); Event Rate", 200, 0.0, 10.1);
	
	//int xbins = 1000;//400;
	//int ybins = 100;//400;
	TH2 *dissapearanceProb = new TH2D("dissapearanceProb", "Dissapearance Chi-squareds in ICARUS; Sin^2(2 Theta); Delta m^2", xbins, 0, 1, ybins, 0, 10);
	
	for(int i=0; i<xbins; i++){
		for (int j=0; j<ybins; j++){
			double ss2t = (double)i/(double)xbins;
			double dMSquared = (double)j/(double)ybins;
			calculateOscillation(dMSquared, ss2t, 0.600, eventRateICARUS, oscRateICARUS);
			double chiSquared = calculateChiSquared(eventRateICARUS, oscRateICARUS);
			dissapearanceProb -> SetBinContent(i, j, chiSquared);
			}
	}
	
	//Display event rate in ICARUS for the fourth oscillation hypothesis
	c1 -> cd(2);//cd(3);
	c1_2 -> SetLogx(); c1_2 -> SetLogy(); c1_2 -> SetLogz();
	dissapearanceProb -> Draw("COLZ");

}

void calculateOscillation(double dMS, double ss2Th, double oscLength, TH1 *eventRate, TH1 *result){//calculate oscillation probability for a given (delta M squared, sin^2(2Theta)) pair, at L, given a particular event rate
	//General formula for P(E) numu -> numu: 1 - sin^2(2*theta) * sin^2(1.27(delta m^s)(L/E))
	//int nBins = eventRate -> GetNbinsX();
	double binWidth = 0.05; // bin width in Gev = 50 MeV
	double binProb =0;
	double arg = 0;
	double binEnergy = 0.025;
	double er = 0; //event rate for the current bin

	for(int i=0; i<nBins; i++){
		binEnergy = i*binWidth;
		arg = sin(1.27*dMS*oscLength/binEnergy);
		binProb = 1 - ss2Th*pow(arg, 2);
		er = eventRate -> GetBinContent(i);
		result -> SetBinContent(i, er*binProb);
	}

}

double calculateChiSquared(TH1 *noOsc, TH1 *osc){
	double chiSquared = 0.0;
	//int nBins = 200;//noOsc -> GetNBinsX();

	for(int i=0; i<nBins; i++){
		double nNull = noOsc -> GetBinContent(i);
		if (nNull == 0){
			chiSquared += 0;
		}
		else{
		double nOsc = osc -> GetBinContent(i);
		double increment = (pow((nNull - nOsc), 2)/nNull);
		chiSquared += increment;
		}
	
	}
	//cout << " chiSquared: " << chiSquared << endl;
	return chiSquared;

}

void calculateEventRate(double *eventRateArr, double *crossSecArr, double *fluxArr){
	for(int i=0; i<200; i++){
		eventRateArr[i] = crossSecArr[i]*fluxArr[i]*(7.176*(10^30));
	}
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
