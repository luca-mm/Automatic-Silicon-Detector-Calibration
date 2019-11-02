	#include "TH1.h"
	#include "TF1.h"
	void calibration() {
	
	Double_t par[18];
	Int_t ch = 19;


	UShort_t SQX_R[32];
	TFile *inF = new TFile("SQR_front_alpha_0001.root", "READ");
	TTree *inT = (TTree*)inF->Get("AnalysisxTree");
	inT->SetMakeClass(1);
	inT->SetBranchAddress("NeEvent.SQX_R[32]", SQX_R);
	

	TH1F *h = new TH1F("h","Recorded spectrum",500,0,500);
	TH1F *h_lim = new TH1F("h_lim","Recorded spectrum",300,200,500);	
	Long64_t nEntries = inT->GetEntriesFast();

	for (Long64_t i=0;i<nEntries;i++) {
		inT->GetEntry(i);
		// fill the histogram with the destep entry
		h->Fill(SQX_R[ch]);
		h_lim->Fill(SQX_R[ch]);
	}


	//Finding the peak locations
	TSpectrum *s = new TSpectrum(4);
	Int_t nfound = s->Search(h_lim,5,"new",0.05);
	Double_t *xpeaks = s->GetPositionX();
	for(Int_t j=0;j<4;j++) printf("%f \n",xpeaks[j]);

	//Sorting the array
	for (Int_t c=0 ; c<3; c++)
	{
    	for (Int_t d=0 ; d<3-c; d++)
    	{
    		if (xpeaks[d] > xpeaks[d+1]) /* For decreasing order use < */
      		{
        		Double_t swap;
				swap       = xpeaks[d];
        		xpeaks[d]   = xpeaks[d+1];
        		xpeaks[d+1] = swap;
      		}
    	}
	}

	TF1 *g0    = new TF1("g1","gaus",50.0,150.0);
	TF1 *g1    = new TF1("g1","gaus",xpeaks[0]-15,xpeaks[0]+15);
	TF1 *g2_1  = new TF1("g2","gaus",xpeaks[1]-12,xpeaks[1]-7);
	TF1 *g2_2  = new TF1("g2","gaus",xpeaks[1]-6,xpeaks[1]+6);
	TF1 *g3    = new TF1("g3","gaus",xpeaks[2]-15,xpeaks[2]+15);
	TF1 *g4    = new TF1("g4","gaus",xpeaks[3]-15,xpeaks[3]+15);
	
	h->Fit(g0,"R");
	h->Fit(g1,"R+");
	h->Fit(g2_1,"R+");
	h->Fit(g2_2,"R+");
	h->Fit(g3,"R+");
	h->Fit(g4,"R+");

	g0->GetParameters(&par[0]);
	g1->GetParameters(&par[3]);
	g2_1->GetParameters(&par[6]);
	g2_2->GetParameters(&par[9]);
	g3->GetParameters(&par[12]);
	g4->GetParameters(&par[15]);	


	Int_t n = 6;
	Double_t x[n], y[n];
	
	x[0]=par[1];        y[0]=0;
	x[1]=par[4];        y[1]=4.784;
	x[2]=par[7];        y[2]=5.304;
	x[3]=par[10];       y[3]=5.489;
	x[4]=par[13];       y[4]=6.002;
	x[5]=par[16];       y[5]=7.686; 
   
	// create graph
	TGraph *gr  = new TGraph(n,x,y);
	TCanvas *ec1 = new TCanvas("c2","Calibration function",500,10,600,400);

	TF1 *f1 = new TF1("f1","[0]+[1]*x",1,3);
	gr->Fit("f1","");

	// draw phe graph with axis, continuous line, and put
	// a * at each point
	gr->Draw("A*");       
}