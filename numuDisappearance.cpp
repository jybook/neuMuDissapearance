#include <stdio.h> 
#include <math.h>

//in this script, it's assumed that the flux file has 200 bins with widths of 50 MeV each
int nBins = 200;

void numuDisappearance(int xbins, int ybins, double xmax, double ymax){

	if(xmax > 1) {xmax = 1;}

	double flux[200];
	double crossSection[200];
	numuSetUp(crossSection, flux);

	TH1 *eventRateICARUS = new TH1D("eventRateICARUS", "Predicted Event Rate at 600m (no oscillations); Neutrino Energy (GeV); Event Rate", 200, 0.0, 10.01);
	double distance = 600.0;
	calculateEventRate(eventRateICARUS, flux, crossSection, distance);

	TH1 *oscRateICARUS = new TH1D("oscRateICARUS", "Predicted Event Rate at 600m (with oscillations); Neutrino Energy (GeV); Event Rate", 200, 0.0, 10.01);
	TH2 *chiSquaredPlot = new TH2D("chiSquaredPlot", "Dissapearance Chi-squareds in ICARUS; Sin^2(2 Theta); Delta m^2", xbins, 0.0, xmax, ybins, 0.0, ymax);
	
	double xres = xmax/xbins;
	double yres = ymax/ybins;

	double ss2t;
	double dMSquared;
	double chiSquared;

	double x[20000];
	double y[20000];
	for(int i = 0; i<20000; i++){
		x[i] = 0;
		y[i] = 0;
	}

	int n = 0;

	for(int i=0; i<xbins; i++){
		for (int j=0; j<ybins; j++){
			ss2t = xres*i;
			dMSquared = yres*j;
			calculateOscillation(dMSquared, ss2t, 0.600, eventRateICARUS, oscRateICARUS);
			chiSquared = calculateChiSquared(eventRateICARUS, oscRateICARUS);
			chiSquaredPlot -> SetBinContent(i, j, chiSquared);
			
			
			if(chiSquared <= 2.0 && chiSquared >= 1.0){
				x[n] = ss2t;
				y[n] = dMSquared;
				//cout << "point "<< n <<" set: (" << x[n] << ", " << y[n] << "): ss2t: "<< ss2t << " dms: " << dMSquared << endl;
				n++;
			} 
			else { 
				if(chiSquared <= 8.0 && chiSquared >= 6.6){
					x[n] = ss2t;
					y[n] = dMSquared;
					n++; 
					else {
						if(chiSquared <= 30.0 && chiSquared >= 20.0){
							x[n] = ss2t;
							y[n] = dMSquared;
							n++;
						}

					} 
				}

			} 

		}
	}

	TCanvas *c1 = new TCanvas("c1", "NuMu Dissapearance Sensitivities", 1200, 600);
	c1 -> Divide(2, 1, 0.01, 0.01, 0);// divide(number of pads in the x direction, number of pads in the y direction, x-margin, y-margin, color)
	
	TGraph *contours = new TGraph(n, x, y);
	c1 -> cd(2); 
	c1_2 -> SetLogx(); c1_2 -> SetLogy(); //c1_2 -> SetLogz();
	contours -> Draw("ap");
	
	
	c1 -> cd(1); 
	c1_1 -> SetLogx(); c1_1 -> SetLogy(); c1_1 -> SetLogz();
	chiSquaredPlot -> Draw("COLZ");

}

void calculateOscillation(double dMS, double ss2Th, double oscLength, TH1 *eventRate, TH1 *result){
//calculate oscillation probability for a given (delta M squared (GeV), sin^2(2Theta)) pair, at L (km), given a particular event rate
//General formula for P(E) numu -> numu: 1 - sin^2(2*theta) * sin^2(1.27(delta m^s)(L/E))

	double binWidth = 0.05; // bin width in Gev = 50 MeV
	double binProb =0;
	double arg = 0;
	double binEnergy = 0.025; //use the middle of each bin
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
	double nOsc = 0.0;
	double nNull = 0.0;
	double increment = 0.0;

	for(int i=0; i<nBins; i++){
		nNull = noOsc -> GetBinContent(i);
		if (nNull == 0){
			chiSquared += 0;
		}
		else{
		nOsc = osc -> GetBinContent(i);
		increment = (pow((nNull - nOsc), 2)/nNull);
		chiSquared += increment;
		}
	
	}
	
	return chiSquared;

}



void printArr(double *a1){
	int length = 200;
	for(int i=0; i<length; i++){
		cout << a1[i] << ", ";
	}
	cout << endl;
}


void calculateEventRate(TH1 *eventRate, double *crossSecArr, double *fluxArr, double distance){
	double scale = (470.0*470.0)/(distance*distance);
	double binRate;
	for(int i=0; i<200; i++){
		binRate = scale*crossSecArr[i]*fluxArr[i]*(7.176*(100)*6);
		eventRate -> SetBinContent(i, binRate);
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
