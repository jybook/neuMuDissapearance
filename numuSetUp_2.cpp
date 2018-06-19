#include <stdio.h> 
#include <math.h>

void codeTester(){
	
	double flux[];
	double crossSection[200];

	crossSection[0] = 24;
	cout <<"crossSection address: "<< crossSection << endl;

	numuSetUp(crossSection);
	cout << "numuSetUp completed" << endl;
	cout <<"crossSection address final: "<< crossSection << endl;

	cout << crossSection[0] << endl;
	cout << crossSection[0] << " , " << crossSection[100] << " , " << crossSection[185] << endl;
	

}

void numuSetUp(double *crossSecArr){
	cout << "numuSetUp entered" << endl;
	
	cout <<"crossSection address2: "<< crossSecArr << endl;

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
	//double crossSecHist[200];//crossSecHist[numBins];
	double binMean;
	double x, y, x2, y2;
	int currentPoint=0;

	for(int i=0; i<numBins; i++,currentPoint += ppb){
		crossSection->GetPoint(currentPoint, x, y);
		crossSection->GetPoint(currentPoint + ppb - 1, x2, y2);
		binMean = (y+y2)/2;
		//crossSecHist[i] = binMean;
		crossSecArr[i] = binMean;
	}
	cout << crossSecArr[0] << " , " << crossSecArr[100] << " , " << crossSecArr[185] << endl;
	cout << "numuSetUp almost done" << endl;
	
	//cout << *crossSecHist << endl;
	cout << *crossSecArr << endl;
	
	//*crossSecArr = *crossSecHist;
	//cout << "crossSecHist address: "<< crossSecHist << endl;
	cout << *crossSecArr << endl;
	
	cout << crossSecArr[100] <<endl;
	//cout << "if true, it will say "<< true<< " "<< (crossSecArr[185] == crossSecHist[185])<< endl;
	cout << "current array address: "<< crossSecArr << endl;

return;
}
