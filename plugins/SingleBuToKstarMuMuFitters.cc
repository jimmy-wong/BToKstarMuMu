// vim: set sw=4 sts=4 filetype=cpp fdm=marker et: 

// -----------------------------------------------
//       Author: Xin Shi <Xin.Shi@cern.ch> 
//       Created:   [2013-08-15 Thu 14:54] 
// -----------------------------------------------
#include <stdio.h>
#include <sstream>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
//#include <regex> //c++11 feature should be fine using gcc491.

#include <TSystem.h>
#include <TStyle.h>
#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TMinuit.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TPad.h> 
#include <TCanvas.h> 
#include <TChain.h> 
#include <TChainElement.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TString.h>
#include <TGraphAsymmErrors.h>
#include <TLorentzVector.h>

#include <RooConstVar.h>
#include <RooRealVar.h>
#include <RooAbsPdf.h>
#include <RooAddPdf.h>
#include <RooGaussian.h>
#include <RooBifurGauss.h>
#include <RooChebychev.h> 
#include <RooGenericPdf.h> 
#include <RooExponential.h>
#include <RooBernstein.h>
#include <RooPolynomial.h>
#include <RooExtendPdf.h>
#include <RooProdPdf.h>
#include <RooDataHist.h>
#include <RooDataSet.h>
#include <RooAbsData.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooAddition.h>
#include <RooProduct.h>

#include "tools.h" 

using namespace std; 
using namespace RooFit;

// Tags configration
bool is7TeVCheck = false; // Using 2011 efficiency map.
int isCDFcut = 0; // 0 for off, 1 for on;
TChain *ch=new TChain("tree");
//Constants, Fit results for efficiency, etc.. //{{{
char genQ2range[10][32] = {"genQ2 < 2.00 && genQ2 > 1.00",
                           "genQ2 < 4.30 && genQ2 > 2.00",
                           "genQ2 < 8.68 && genQ2 > 4.30",
                           "genQ2 <10.09 && genQ2 > 8.68",
                           "genQ2 <12.86 && genQ2 >10.09",
                           "genQ2 <14.18 && genQ2 >12.86",
                           "genQ2 <16.00 && genQ2 >14.18",
                           "genQ2 <19.00 && genQ2 >16.00",
                           "genQ2 <19.00 && genQ2 > 1.00",
                           "genQ2 < 6.00 && genQ2 > 1.00"};
char q2range[10][32] = {"Q2 < 2.00 && Q2 > 1.00",
                        "Q2 < 4.30 && Q2 > 2.00",
                        "Q2 < 8.68 && Q2 > 4.30",
                        "Q2 <10.09 && Q2 > 8.68",
                        "Q2 <12.86 && Q2 >10.09",
                        "Q2 <14.18 && Q2 >12.86",
                        "Q2 <16.00 && Q2 >14.18",
                        "Q2 <19.00 && Q2 >16.00",
                        "Q2 <19.00 && Q2 > 1.00",
                        "Q2 < 6.00 && Q2 > 1.00"};
double q2rangedn[10] = {1.00 , 2.00 , 4.30 , 8.68  , 10.09 , 12.86 , 14.18 , 16.00 ,  1.00 , 1.00};
double q2rangeup[10] = {2.00 , 4.30 , 8.68 , 10.09 , 12.86 , 14.18 , 16.00 , 19.00 , 19.00 , 6.00};
char mumuMassWindow[7][512] = { "Mumumass > 0",
                                "(Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)",
                                "(Mumumass < 3.096916+3*Mumumasserr && Mumumass > 3.096916-5*Mumumasserr) || (Mumumass < 3.686109+3*Mumumasserr && Mumumass > 3.686109-3*Mumumasserr)",
                                "Mumumass < 3.096916+3*Mumumasserr && Mumumass > 3.096916-5*Mumumasserr",
                                "Mumumass < 3.686109+3*Mumumasserr && Mumumass > 3.686109-3*Mumumasserr",
                                "(Mumumass*Mumumass<8.68 && Bmass-Mumumass>2.182+0.16 && Bmass-Mumumass<2.182-0.16) || (Mumumass*Mumumass>10.09 && Mumumass*Mumumass<12.86 && Bmass-Mumumass>1.593+0.06 && Bmass-Mumumass<1.593-0.06) || (Mumumass*Mumumass>14.18 && Bmass-Mumumass>1.593+0.06 && Bmass-Mumumass<1.593-0.06)",
                                "(Mumumass*Mumumass < 8.68 && ( Bmass-Mumumass < 2.182+0.16 || Bmass-Mumumass > 2.182-0.16)) || (Mumumass*Mumumass>8.68 && Mumumass*Mumumass<10.09) || (Mumumass*Mumumass > 10.09 && Mumumass < 12.86 && ( Bmass-Mumumass < 1.593+0.06 || Bmass-Mumumass > 1.593-0.06)) || (Mumumass*Mumumass>12.86 && Mumumass*Mumumass<14.08) || (Mumumass*Mumumass > 14.08 && (Bmass-Mumumass > 1.593-0.06 || Bmass-Mumumass < 1.593+0.06))"
};//None, sig, bkg, Jpsi, Psi2S, CDF, anti-CDF
double genAfb[8]={-0.160,-0.066,0.182,0.317,0.374,0.412,0.421,0.376};
double genAfberr[8]={0.000434,0.000285,0.000223,0.000383,0.000277,0.000421,0.000395,0.000422};
double genFl [8]={0.705,0.791,0.649,0.524,0.454,0.399,0.369,0.341};
double genFlerr[8]={0.000568,0.000409,0.000271,0.000420,0.000287,0.000415,0.000369,0.000361};
double arrRecPar2011   [8][20] = {
            {0.0017687,-8.73499e-06,-0.000935262,-0.000787674,
            -0.00529644,0.00155626,0.00356542,0.000171319,
            0,0,0,0,
            0.00551706,-0.00339678,-0.00463866,0.00240596,
            -0.00197731,0.00184597,0.00200529,-0.00178549},
            {0.00157318,7.37446e-05,-0.000749493,-0.000781159,
            -0.00288927,0.000261854,0.00187545,0.000883533,
            0,0,0,0,
            0.00132593,-0.000331488,-0.00112174,-0.000108788,
            0,0,0,0},
            {0.00118453,-1.23943e-05,-0.000454793,-0.000541742,
            -2.12267e-05,0.000714674,-9.69142e-05,-0.000768923,
            -7.59472e-05,0.000229203,5.36592e-05,-6.0488e-05,
            -0.00106615,-0.000896725,0.000710503,0.00156238,
            0,0,0,0},
            {0.00106852,-7.35469e-05,-0.000403409,-0.00035673,
            0.000637779,0.000294361,-0.000303145,-0.000885132,
            5.60979e-05,8.91354e-05,-0.000103992,-5.73688e-05,
            -0.00112888,9.3696e-05,0.000728873,0.000650714,
            0,0,0,0},
            {0.00102525,-0.000118438,-0.000346987,-0.000315243,
            0.000741172,0.000462559,-0.000313513,-0.00064603,
            -0.0007614,0.00153595,0.000840568,-0.0014875,
            -1.66019e-05,-0.00190496,-0.000546402,0.00220778,
            0,0,0,0},
            {0.00108772,0.000130305,-0.000457955,-0.000514284,
            0.000225576,-0.00108671,0.000283481,0.000592928,
            4.66766e-05,-0.00017112,4.27933e-05,0.000247925,
            7.59011e-05,0.000852078,-0.000514188,-8.52169e-05,
            0,0,0,0},
            {0.000964546,4.77019e-05,-0.000249802,-0.00035122,
            0.00120212,-0.0013481,-0.00081191,0.00109161,
            -0.000838523,0.00118858,0.000579591,-0.00101538,
            0,0,0,0,
            0,0,0,0},
            {0.00103644,7.27679e-05,-0.000366081,-0.000162294,
            0.000571208,-0.00089218,-1.99667e-05,0.00066918,
            -0.000423471,0.000360459,0.000228834,-0.000207738,
            0,0,0,0,
            0,0,0,0}};

double arrRecParErr2011[8][20] = {
            {9.77352e-06,1.45679e-05,1.44267e-05,1.71613e-05,
            1.7851e-05,2.42707e-05,2.66439e-05,2.89112e-05,
            0,0,0,0,
            2.06968e-05,2.87918e-05,3.2287e-05,3.52259e-05,
            1.71744e-05,2.50134e-05,2.75925e-05,3.13877e-05},
            {1.93048e-05,4.54403e-05,3.19836e-05,6.34448e-05,
            6.97046e-05,0.000175846,0.000127026,0.000252818,
            0,0,0,0,
            5.60232e-05,0.000145372,0.000105324,0.000210834,
            0,0,0,0},
            {1.20764e-05,2.99275e-05,2.12433e-05,4.24699e-05,
            5.88911e-05,0.000155115,0.0001176,0.000229393,
            1.73594e-05,3.28355e-05,3.60904e-05,5.2698e-05,
            5.76856e-05,0.000147459,0.000126591,0.000229798,
            0,0,0,0},
            {1.07973e-05,2.65747e-05,1.89528e-05,3.78338e-05,
            8.26138e-05,0.000208233,0.000148573,0.000297846,
            1.46533e-05,4.07354e-05,3.25714e-05,6.24532e-05,
            0.000102431,0.000261492,0.00018865,0.000376784,
            0,0,0,0},
            {1.43812e-05,3.69822e-05,2.68571e-05,5.34828e-05,
            0.000109918,0.000289434,0.000212286,0.000422439,
            4.35946e-05,9.76997e-05,7.60719e-05,0.000139613,
            0.000142318,0.000374508,0.000278563,0.000548987,
            0,0,0,0},
            {4.57551e-05,0.000116528,7.84802e-05,0.000164459,
            0.000350888,0.000882504,0.000628837,0.00126522,
            6.95431e-05,0.000192566,0.000150432,0.000293712,
            0.000438904,0.00111066,0.000809841,0.00160928,
            0,0,0,0},
            {1.41088e-05,3.73896e-05,2.85699e-05,5.60619e-05,
            5.72502e-05,0.000142174,0.000109448,0.000209836,
            5.66788e-05,0.000139079,0.000107494,0.00020466,
            0,0,0,0,
            0,0,0,0},
            {1.32107e-05,3.6017e-05,2.71503e-05,5.4467e-05,
            3.99561e-05,0.000108688,8.82189e-05,0.000169816,
            3.68927e-05,0.000100146,8.35791e-05,0.000158447,
            0,0,0,0,
            0,0,0,0}};

std::string f_accXrecoEff_ord0[8] = { // default values
    "11627.982364*((1.195422e-04*exp(-0.5*((CosThetaL-(-1.727343e-01))/2.021796e-01)**2)+1.156964e-04*exp(-0.5*((CosThetaL-(2.507083e-01))/2.478225e-01)**2)+4.629809e-05*exp(-0.5*((CosThetaL-(-5.148565e-01))/1.407258e-01)**2))*(7.165504e-05-2.621913e-05*CosThetaK+1.453609e-04*CosThetaK**2+2.274953e-05*CosThetaK**3-2.398253e-04*CosThetaK**4-4.428545e-05*CosThetaK**5+9.677067e-05*CosThetaK**6))",
    "9916.540629*((7.278431e-05*exp(-0.5*((CosThetaL-(-4.905860e-01))/1.878949e-01)**2)+7.448700e-05*exp(-0.5*((CosThetaL-(5.058518e-01))/2.003984e-01)**2)+1.425194e-04*exp(-0.5*((CosThetaL-(1.313125e-02))/2.957232e-01)**2))*(8.311598e-05-2.316101e-05*CosThetaK+1.476586e-04*CosThetaK**2-2.367362e-05*CosThetaK**3-1.683845e-04*CosThetaK**4+5.042865e-06*CosThetaK**5+1.243843e-05*CosThetaK**6))",
    "8.353802e+03*((1.344021e-04+1.980409e-05*CosThetaL+4.029664e-05*CosThetaL**2+1.560540e-05*CosThetaL**3-2.131400e-04*CosThetaL**4-3.310795e-05*CosThetaL**5+5.462426e-05*CosThetaL**6)*(1.136958e-04-3.718097e-05*CosThetaK+6.443598e-05*CosThetaK**2+5.683602e-05*CosThetaK**3-4.802073e-05*CosThetaK**4-7.413557e-05*CosThetaK**5-3.107261e-05*CosThetaK**6))",
    "4.392777e+04*((2.114489e-05+2.400662e-06*CosThetaL+2.759247e-05*CosThetaL**2+1.100568e-06*CosThetaL**3-4.538219e-05*CosThetaL**4-2.412249e-06*CosThetaL**5+5.307765e-06*CosThetaL**6)*(2.406814e-05-7.583489e-06*CosThetaK-9.968329e-06*CosThetaK**2+1.463576e-05*CosThetaK**3+3.247851e-05*CosThetaK**4-1.619795e-05*CosThetaK**5-2.949584e-05*CosThetaK**6))",
    "6.506619e+03*((1.349742e-04+1.528919e-05*CosThetaL+8.605597e-05*CosThetaL**2+1.312572e-05*CosThetaL**3-2.948919e-05*CosThetaL**4-9.566140e-06*CosThetaL**5-5.879247e-05*CosThetaL**6)*(1.581494e-04-3.384666e-05*CosThetaK-1.447583e-05*CosThetaK**2+3.758161e-05*CosThetaK**3+6.777260e-05*CosThetaK**4-5.585069e-05*CosThetaK**5-8.495213e-05*CosThetaK**6))",
    "4.625695e+04*((1.803216e-05+6.423635e-07*CosThetaL+9.704679e-06*CosThetaL**2+1.065779e-05*CosThetaL**3-1.658277e-06*CosThetaL**4-1.799046e-05*CosThetaL**5+6.089049e-06*CosThetaL**6)*(2.270524e-05-9.322913e-06*CosThetaK-1.587276e-05*CosThetaK**2+2.152708e-05*CosThetaK**3+5.615584e-05*CosThetaK**4-1.901528e-05*CosThetaK**5-4.887378e-05*CosThetaK**6))",
    "5.118383e+03*((1.668313e-04+1.911185e-05*CosThetaL+1.716389e-05*CosThetaL**2-3.192265e-05*CosThetaL**3+2.000329e-04*CosThetaL**4+1.783316e-05*CosThetaL**5-1.334724e-04*CosThetaL**6)*(2.056593e-04-4.151040e-05*CosThetaK-6.658669e-05*CosThetaK**2+3.742139e-05*CosThetaK**3+1.666491e-04*CosThetaK**4-5.072888e-05*CosThetaK**5-1.492963e-04*CosThetaK**6))",
    "3.837453e+03*((2.362599e-04-4.438020e-06*CosThetaL+3.318080e-05*CosThetaL**2+1.313482e-05*CosThetaL**3+7.878926e-05*CosThetaL**4-3.939653e-06*CosThetaL**5-2.211163e-05*CosThetaL**6)*(2.669904e-04-4.272653e-05*CosThetaK+1.487773e-05*CosThetaK**2+1.983652e-05*CosThetaK**3-9.317172e-05*CosThetaK**4-3.937610e-05*CosThetaK**5+4.831201e-05*CosThetaK**6))"
};
// Lumi = Nreco/(cross section*branch factor*filter efficiency), cross section is 49.59e9 [pb] for 8TeV and 48.44e9 [pb] for 7TeV.
// BF_BuToK*MuMu = 1.07E-6, 1.12E-6(2014)
// BF_BuToK*Jpsi = 1.43E-3, 1.44E-3(2014)
// BF_BuToK*Psi2S = 6.7E-4, 6.7 E-4(2014)
// BF_K*ToK0Pi  = 1  (K* decays to Kpi always)
// BF_K0ToKs  = 1/2
// BF_KsToPiPi = 2/3
double datasetLumi[5] = {19.4,26070.9,114.868,124.823,10.};//data, BuToKstarMuMu(11361.5+14709.4), BuToKstarJpsi(42.305+72.563), BuToKstarPsi2S(42.996,81.826), JpsiX
//}}}

double readParam(int iBin, const char parName[], int iColumn, double defVal=0., double forceReturn=999.)
{//{{{
    // Remark: first value is at iColumn=0.
    if (forceReturn != 999.) return forceReturn;

    std::vector<double> output;
    char lineBuff[1024];
    char *valBuff;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("fitParameters%d.txt",iBin),"r");
    if (!fp){
        printf("WARNING: readParam, missing parameter files, by default return %f.",defVal);
        return defVal;
    }
    while(fgets(lineBuff,1024,fp) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            printf("INFO: readParam, matched %s!\n",valBuff);
            valBuff = strtok(NULL," ");
            while(valBuff != NULL){
                //output.push_back(stof(valBuff));//stof if c++11 function, use other function
                if (strcmp(valBuff,"nan") == 0 || strcmp(valBuff,"inf") == 0 ){
                    output.push_back(defVal);
                }else{
                    output.push_back(std::atof(valBuff));
                }
                valBuff = strtok(NULL," ");
            }
            break;
        }
        memset(lineBuff,' ',1024*sizeof(char));
    }
    fclose(fp);
    
    if (iColumn < output.size() ){
        printf("INFO: readParam, get %s[%d]=%e\n",parName,iColumn,output.at(iColumn));
        return output.at(iColumn);
    }else{
        printf("WARNING: readParam, empty column! Return %s[%d]=defVal=%f.\n",parName,iColumn,defVal);
        return defVal;
    }
}//}}}
std::string readParam(int iBin, const char parName[], string defVal="", string forceReturn="defaultForceReturn")
{//{{{
    // Remark: first value is at iColumn=0.
    if (forceReturn != "defaultForceReturn") return forceReturn;

    string output;
    char lineBuff[1024];
    char *valBuff;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("fitParameters%d.txt",iBin),"r");
    if (!fp){
        printf("WARNING: readParam, missing parameter files, by default return %s.",defVal.c_str());
        return defVal;
    }
    while(fgets(lineBuff,1024,fp) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            printf("INFO: readParam, matched %s!\n",valBuff);
            valBuff = strtok(NULL,"\n");
            output=string(valBuff);
            break;
        }
        memset(lineBuff,' ',1024*sizeof(char));
    }
    fclose(fp);
    
    if (output != ""){
        printf("INFO: readParam, get %s=%s\n",parName,output.c_str());
        return output;
    }else{
        printf("WARNING: readParam, empty item! Return %s=defVal=%s.\n",parName, defVal.c_str());
        return defVal;
    }
}//}}}
void writeParam(int iBin, const char parName[], double *val, int nVal=2, bool overwrite=true)
{//{{{
    if ( !overwrite ) return;

    struct stat fiBuff;
    FILE *fi = 0;
    if (stat(TString::Format("fitParameters%d.txt",iBin),&fiBuff) == 0){
        rename(TString::Format("fitParameters%d.txt",iBin),TString::Format("fitParameters%d.txt.temp",iBin));
        fi = fopen(TString::Format("fitParameters%d.txt.temp",iBin),"r");
    }else{
        fi = fopen(TString::Format("fitParameters%d.txt.temp",iBin),"w");
    }
    
    bool parExist = false;
    char lineBuff[1024];
    char *valBuff = 0;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("fitParameters%d.txt",iBin),"w");
    while(fgets(lineBuff,1024,fi) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            fprintf(fp,"%s",parName);
            int iVal = 0;
            while(iVal < nVal){
                fprintf(fp," %e",val[iVal]);
                iVal++;
            }
            fprintf(fp,"\n");
            parExist = true;
        }else{
            fprintf(fp,"%s",lineBuff);
            valBuff = strtok(NULL," ");
            while( valBuff != NULL ){
                fprintf(fp," %s",valBuff);
                valBuff = strtok(NULL," ");
            }
        }
        memset(lineBuff,' ',512*sizeof(char));
    }
    if (parExist == false){
        fprintf(fp,"%s",parName);
        int iVal = 0;
        while(iVal < nVal){
            fprintf(fp," %e",val[iVal]);
            iVal++;
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    fclose(fi);
    remove(TString::Format("fitParameters%d.txt.temp",iBin));
}//}}}
void writeParam(int iBin, const char parName[], string instring, bool overwrite=true)
{//{{{
    if ( !overwrite ) return;

    struct stat fiBuff;
    FILE *fi = 0;
    if (stat(TString::Format("fitParameters%d.txt",iBin),&fiBuff) == 0){
        rename(TString::Format("fitParameters%d.txt",iBin),TString::Format("fitParameters%d.txt.temp",iBin));
        fi = fopen(TString::Format("fitParameters%d.txt.temp",iBin),"r");
    }else{
        fi = fopen(TString::Format("fitParameters%d.txt.temp",iBin),"w");
    }
    
    bool parExist = false;
    char lineBuff[1024];
    char *valBuff = 0;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("fitParameters%d.txt",iBin),"w");
    while(fgets(lineBuff,1024,fi) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            fprintf(fp,"%s %s\n", parName, instring.c_str());
            parExist = true;
        }else{
            fprintf(fp,"%s",lineBuff);
            valBuff = strtok(NULL," ");
            while( valBuff != NULL ){
                fprintf(fp," %s",valBuff);
                valBuff = strtok(NULL," ");
            }
        }
        memset(lineBuff,' ',512*sizeof(char));
    }
    if (parExist == false){
        fprintf(fp,"%s %s\n", parName, instring.c_str());
    }
    fclose(fp);
    fclose(fi);
    remove(TString::Format("fitParameters%d.txt.temp",iBin));
}//}}}

TF2  *f2_fcn = NULL;
double model_2D(double *x, double *par)
{//{{{
    double xx = x[0];
    double yy = x[1];
    for (int i = 0; i < f2_fcn->GetNpar(); i++) f2_fcn->SetParameter(i,par[i]);
    return f2_fcn->Eval(xx,yy);
}//}}}

TH2F *h2_fcn = NULL;
// nParameters, ???, return fcn value, parameter array, strategy
void fcn_binnedChi2_2D(int &npar, double *gin, double &f, double *par, int iflag)
{//{{{
    f=0;
    for (int i = 1; i <= h2_fcn->GetNbinsX(); i++) {
        for (int j = 1; j <= h2_fcn->GetNbinsY(); j++) {
            int gBin = h2_fcn->GetBin(i,j);
            double x[2] = {h2_fcn->GetXaxis()->GetBinCenter(i),h2_fcn->GetYaxis()->GetBinCenter(j)};
            double measure  = h2_fcn->GetBinContent(gBin);
            double error    = h2_fcn->GetBinError(gBin);
            
            //// Naively test using center value
            //double func     = model_2D(x, par);//Take center value
            //double delta    = (measure-func)/error;
            //if (measure != 0) 
            //    f+=delta*delta;
            
            //// Real run using integral
            for (int k = 0; k < f2_fcn->GetNpar(); k++){//nPar MUST be the same value as f2_fcn
                f2_fcn->SetParameter(k,par[k]);
            }
            double xi = h2_fcn->GetXaxis()->GetBinLowEdge(i);
            double xf = h2_fcn->GetXaxis()->GetBinUpEdge(i);
            double yi = h2_fcn->GetYaxis()->GetBinLowEdge(j);
            double yf = h2_fcn->GetYaxis()->GetBinUpEdge(j);
            //f2_fcn->SetRange(xi,xf,yi,yf);
            //double minX, minY;
            //f2_fcn->GetMinimumXY(minX,minY);
            //if (f2_fcn->Eval(minX,minY) < 0){
            //    f += 100;
            //}else{
                f += pow( (f2_fcn->Integral(xi,xf,yi,yf)/(xf-xi)/(yf-yi)-measure)/error,2);
            //}
        }
    }
    //printf("FCN in calls = %f\n",f);
}//}}}

//_________________________________________________________________________________

void bmass(int iBin, const char outfile[] = "bmass")
{//{{{
  bool test = false; 
  
  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("Bmass"         , 1);
  ch->SetBranchStatus("Mumumass"      , 1);
  ch->SetBranchStatus("Mumumasserr"   , 1);
  ch->SetBranchStatus("Q2"            , 1);
  RooRealVar Bmass("Bmass", "B^{+/-} mass(GeV/c^{2})", 5.27953-0.28, 5.27953+0.28) ;
  RooRealVar Q2("Q2","q^{2}",0.5,20.);
  RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
  RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
  RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, Bmass, Mumumass, Mumumasserr),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[1]),0);

  data->Print();

  // Create model and dataset
  // -------------------------------------------------------------------------
  // Gaussian signal 
  RooRealVar mean("mean","mean of gaussians", 5.27, 5.23, 5.32) ;
  RooRealVar sigma1("sigma1","width of Gaussian1", 0.0285, 0.01, 0.05) ;
  RooRealVar sigma2("sigma2","width of Gaussian2", 0.08, 0.05, 0.35) ;
  RooRealVar sigM_frac("sigM_frac","fraction of Gaussians",0,0.,1.);
  RooGaussian sigGauss1("siggauss1","Signal component", Bmass, mean, sigma1) ;  
  RooGaussian sigGauss2("siggauss2","Signal component", Bmass, mean, sigma2) ;  
  RooAddPdf sig("sig","sig",RooArgList(sigGauss1,sigGauss2),RooArgList(sigM_frac));

  // Build Chebychev polynomial p.d.f.  
  RooRealVar a0("a0", "constant", 0.5, -1, 1) ;
  RooRealVar a1("a1", "linear", 0.6, -1, 1) ;
  RooRealVar a2("a2", "quadratic", 0.1, -1, 1) ;
  RooChebychev bkg("bkg", "Background", Bmass, RooArgSet(a0, a1, a2)) ;

  // Construct signal+background PDF
  RooRealVar nsig("nsig", "number of signal events", 4648, 0, 1E8); 
  RooRealVar nbkg("nbkg", "number of background events", 21472, 0, 1E8);
  RooAddPdf  model("model", "g+c", RooArgList(bkg, sig), RooArgList(nbkg, nsig)) ;
  
  // Print structure of composite p.d.f.
  model.Print("t") ;

  // Fit model to data, save fitresult 
  // ------------------------------------------------------------------------
  RooFitResult* fitres; 
  if (! test) {
    fitres = model.fitTo(*data, Extended(kTRUE), Minos(kTRUE), Save(kTRUE)) ;
    fitres->Print("v"); 
  }
  
  // Plot model 
  // ---------------------------------------------------------
  TString title = "B^{+/-} mass";
  int nbins = 20; 
  RooPlot* frame = Bmass.frame(Title(title), Bins(nbins));
  data->plotOn(frame) ;
  model.plotOn(frame) ;

  // Overlay the background component of model with a dashed line
  model.plotOn(frame,Components("bkg"), LineStyle(kDashed), LineColor(2)) ;

  // Draw the frame on the canvas
  TCanvas *c = new TCanvas("c"); 
  set_root_style(); 
  c->UseCurrentStyle() ;

  gPad->SetLeftMargin(0.15) ;
  frame->GetYaxis()->SetTitleOffset(1.7) ; 
  frame->Draw();

  TPaveText* paveText = new TPaveText(0.75, 0.82, 1., 1., "NDC"); 
  paveText->SetBorderSize(0);
  paveText->SetFillColor(kWhite);
  paveText->AddText(Form("nsig = %.0f #pm %.0f " , nsig.getVal()  , nsig.getError())); 
  paveText->AddText(Form("nbkg = %.0f #pm %.0f " , nbkg.getVal()  , nbkg.getError())); 
  paveText->AddText(Form("mean = %.3f #pm %.3f " , mean.getVal()  , mean.getError())); 
  paveText->AddText(Form("sigma1 = %.3f #pm %.3f ", sigma1.getVal(), sigma1.getError())); 
  paveText->AddText(Form("sigma2 = %.3f #pm %.3f ", sigma2.getVal(), sigma2.getError())); 
  paveText->AddText(Form("frac = %.3f #pm %.3f ", sigM_frac.getVal(), sigM_frac.getError())); 
  paveText->Draw(); 

  c->Print(TString::Format("./plots/%s_bin%d.pdf",outfile,iBin));
  
  // Persist fit result in root file 
  // -------------------------------------------------------------
  //TFile resf(TString::Format("./plots/%s.root",outfile), "RECREATE") ;
  //gPad->Write("plot"); 
  //if (! test) fitres->Write("fitres") ;
  //resf.Close() ;

  // In a clean ROOT session retrieve the persisted fit result as follows:
  // RooFitResult* r = gDirectory->Get("fitres") ;
  
  delete paveText; 
  delete c;

}//}}}

//_________________________________________________________________________________

std::vector<double> fl_gen_bin(int iBin, const char outfile[] = "fl_gen")
{//{{{
  // From fomula (2) in LHCb 2012 PRL108, 181806(2012)
  // integrated over theta_l and phi: 
  // 
  // 1/Gamma * d^2 Gamma/d cos(theta_K) dq^2 = 3/2 * F_L cos^2(theta_K)
  // + 3/4(1-F_L)(1-cos^2theta_K)
  // 

  bool test = false;

  RooRealVar genCosThetaK("genCosThetaK", "cos#theta_{K}", -1, 1);
  RooRealVar genQ2("genQ2","q^{2}",0.5,20.);
  RooRealVar Q2("Q2","q^{2}",0.5,20.);
  RooRealVar fl("fl", "F_{L}", 0.5, -0.5, 1.5);

  RooGenericPdf f("f", "1.5*fl*genCosThetaK*genCosThetaK+0.75*(1-fl)*(1-genCosThetaK*genCosThetaK)", RooArgSet(genCosThetaK,fl));
  RooDataSet* data;
  
  if (test){
      fl.setVal(0.5);
      data = f.generate(RooArgSet(genCosThetaK,Q2), 10000);
  }else{
      data = new RooDataSet("data","data",ch,RooArgSet(genCosThetaK,genQ2),genQ2range[iBin],0);
  }
  
  f.fitTo(*data,Extended(kTRUE), Minos(kTRUE)); 

  RooPlot* framecosk = genCosThetaK.frame(); 
  data->plotOn(framecosk); 
  f.plotOn(framecosk); 

  // Draw the frame on the canvas
  TCanvas *c = new TCanvas("c"); 
  framecosk->SetTitle("");
  framecosk->Draw();

  TLatex *t1 = new TLatex();
  t1->SetNDC();
  t1->DrawLatex(.40,.85,TString::Format("%s",genQ2range[iBin]));
  t1->DrawLatex(.40,.79,TString::Format("F_{L}=%5.3f#pm%5.3f",fl.getVal(),fl.getError()));

  c->Print(TString::Format("./plots/%s_bin%d.pdf",outfile,iBin));

  delete c;
  delete t1;
  delete data;

  std::vector<double> outvect;
  outvect.push_back(fl.getVal());
  outvect.push_back(fl.getError());
  return outvect;
}//}}}

void fl_gen(const char outfile[] = "fl_gen")
{//{{{

    TCanvas *c = new TCanvas();
    TH2F *frame = new TH2F("frame","",18,1,19,10,0,1.5);
    frame->SetStats(kFALSE);
    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("F_{L}");
    frame->Draw();

    double x[8]={1.5,3.15,6.49,9.385,11.475,13.52,15.09,17.5};
    double xerr[8]={0.5,1.15,2.09,0.705,1.385,0.66,0.91,1.5};
    double yfl[8],yerrfl[8];

    std::vector<double> vbin;
    for(int ibin = 0; ibin < 8; ibin++){
        vbin = fl_gen_bin(ibin);
        yfl[ibin]       =vbin.at(0);
        yerrfl[ibin]    =vbin.at(1);
    }
    
    // Check input data
    for(int ibin = 0; ibin < 8; ibin++){
        printf("yfl [%d]=%6.4f +- %6.4f\n",ibin,yfl[ibin],yerrfl[ibin]);
    }

    TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(8,x,yfl,xerr,xerr,yerrfl,yerrfl);
    g_fl->Draw("P*");
    c->Print(TString::Format("./plots/%s.pdf",outfile));

    delete g_fl;
    delete frame;
    delete c;
}//}}}

//_________________________________________________________________________________

std::vector<double> angular_gen_bin(int iBin, const char outfile[] = "angular_gen")
{//{{{

    RooRealVar genCosThetaK("genCosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar genCosThetaL("genCosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar genQ2("genQ2","q^{2}",0.5,20.);
    RooRealVar fl("fl", "F_{L}", genFl[iBin], 0.2, 0.9);
    RooRealVar afb("afb", "A_{FB}", genAfb[iBin], -0.3, 0.5);
    RooRealVar fs("fs","F_{S}",0.,-0.1,0.1); //Very close to 0.
    RooRealVar as("as","A_{S}",0.01,-1,1.);
    //fs.setConstant(kTRUE);
    //as.setConstant(kTRUE);

    RooRealVar nsig("nsig","nsig",1E6,1E2,1E9);
    RooRealVar nbkg("nbkg","nbkg",10,0.1,1E4);
    
    RooGenericPdf f_sig("f_sig", "9/16*((2/3*fs+4/3*as*genCosThetaK)*(1-genCosThetaL*genCosThetaL)+(1-fs)*(2*fl*genCosThetaK*genCosThetaK*(1-genCosThetaL*genCosThetaL)+1/2*(1-fl)*(1-genCosThetaK*genCosThetaK)*(1+genCosThetaL*genCosThetaL)+4/3*afb*(1-genCosThetaK*genCosThetaK)*genCosThetaL))", RooArgSet(genCosThetaK,genCosThetaL,fl,afb,fs,as));
    RooExtendPdf f("f","",f_sig,nsig);
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(genCosThetaK,genCosThetaL,genQ2),genQ2range[iBin],0);

    RooFitResult *f_fitresult = f.fitTo(*data,Extended(kTRUE),Save(kTRUE),Minimizer("Minuit"),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* framecosk = genCosThetaK.frame(); 
    data->plotOn(framecosk,Binning(100)); 
    f.plotOn(framecosk); 

    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();

    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = -0.5;
    if (iBin < 5) fixNDC = 0.;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",genQ2range[iBin]));
    t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L}=%5.3f#pm%8.6f",fl.getVal(),fl.getError()));
    t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{FB}=%5.3f#pm%8.6f",afb.getVal(),afb.getError()));
    c->Print(TString::Format("./plots/%s_cosk_bin%d.pdf",outfile,iBin));

    //
    RooPlot* framecosl = genCosThetaL.frame(); 
    data->plotOn(framecosl,Binning(100)); 
    f.plotOn(framecosl); 

    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();

    fixNDC = -0.5;
    if (iBin > 4) fixNDC = 0.;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",genQ2range[iBin]));
    t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L}=%5.3f#pm%8.6f",fl.getVal(),fl.getError()));
    t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{FB}=%5.3f#pm%8.6f",afb.getVal(),afb.getError()));
    c->Update();
    c->Print(TString::Format("./plots/%s_cosl_bin%d.pdf",outfile,iBin));

    // Make 2-D plot
    TH1 *h1 = data->createHistogram("genCosThetaL,genCosThetaK", 100, 100);
    h1->Draw("LEGO2");
    c->Update();
    c->Print(TString::Format("./plots/%s_2D_bin%d.pdf",outfile,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    //write output
    std::vector<double> output;
    output.push_back(fl.getVal());
    output.push_back(fl.getError());
    output.push_back(afb.getVal());
    output.push_back(afb.getError());
    return output;

}//}}}

void angular_gen(const char outfile[] = "angular_gen")
{//{{{
    bool refit = true; // Turn to true if you want to fit again.

    TCanvas *c = new TCanvas();
    TH2F *frame = new TH2F("frame","",18,1,19,10,-1,1);
    frame->SetStats(kFALSE);
    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("F_{L}");
    frame->SetAxisRange(0,1,"Y");
    frame->Draw();

    double x[8]={1.5,3.15,6.49,9.385,11.475,13.52,15.09,17.5};
    double xerr[8]={0.5,1.15,2.09,0.705,1.385,0.66,0.91,1.5};
    double yfl[8]       ={0.705,0.791,0.649,0.524,0.454,0.399,0.369,0.341};
    double yerrfl[8]    ={0.000565,0.000410,0.000273,0.000428,0.000296,0.000413,0.000367,0.000359};
    double yafb[8]      ={-0.160,-0.066,0.182,0.317,0.374,0.412,0.421,0.376};
    double yerrafb[8]   ={0.000432,0.000284,0.000224,0.000390,0.000286,0.000419,0.000393,0.420};

    if (refit){ // Turn to true if you want to fit again.
        std::vector<double> vbin;
        for(int ibin = 0; ibin < 8; ibin++){
            vbin = angular_gen_bin(ibin);
            yfl[ibin]       =vbin.at(0);
            yerrfl[ibin]    =vbin.at(1);
            yafb[ibin]      =vbin.at(2);
            yerrafb[ibin]   =vbin.at(3);
        }
    }
    
    // Check input data
    for(int ibin = 0; ibin < 8; ibin++){
        printf("yafb[%d]=%6.4f +- %8.6f\n",ibin,yafb[ibin],yerrafb[ibin]);
        printf("yfl [%d]=%6.4f +- %8.6f\n",ibin,yfl[ibin],yerrfl[ibin]);
    }
    
    // plotting

    TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(8,x,yfl,xerr,xerr,yerrfl,yerrfl);
    g_fl->SetFillColor(2);
    g_fl->SetFillStyle(3001);
    g_fl->Draw("P2");
    c->Print(TString::Format("./plots/%s_fl.pdf",outfile));
    c->Clear();

    frame->SetTitle("");
    frame->SetYTitle("A_{FB}");
    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetAxisRange(-1,1,"Y");
    frame->Draw();
    TGraphAsymmErrors *g_afb = new TGraphAsymmErrors(8,x,yafb,xerr,xerr,yerrafb,yerrafb);
    g_afb->SetFillColor(2);
    g_afb->SetFillStyle(3001);
    g_afb->Draw("P2");
    c->Print(TString::Format("./plots/%s_afb.pdf",outfile));
}//}}}

//_________________________________________________________________________________
//Fit parameters of acceptance and efficiency using TMinuit instead of RooFit.

std::vector<double> acceptance(int iBin) // acceptance. Not used in standard procedure, just for check...
{//{{{

    double accUpperBound = 0.09;
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double gmuppt = 0;
    double gmupeta= 0;
    double gmumpt = 0;
    double gmumeta= 0;

    ch->SetBranchStatus("*",0);
    ch->SetBranchStatus("genQ2"         , 1);
    ch->SetBranchStatus("genCosTheta*"  , 1);
    ch->SetBranchStatus("genMu*"        , 1);
    ch->SetBranchAddress("genQ2"        , &gQ2);
    ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    ch->SetBranchAddress("genMupPt"     , &gmuppt);
    ch->SetBranchAddress("genMupEta"    , &gmupeta);
    ch->SetBranchAddress("genMumPt"     , &gmumpt);
    ch->SetBranchAddress("genMumEta"    , &gmumeta);

    // Fill histograms
    int nbinsL = 20;
    int nbinsK = 20;
    TH2F h2_ngen("h2_ngen","h2_ngen",nbinsL,-1.,1,nbinsK,-1,1);
    TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,nbinsL,-1,1,nbinsK,-1,1); 
    // Read data
    for (int entry = 0; entry < ch->GetEntries(); entry++) {
        ch->GetEntry(entry);
        if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
        h2_ngen.Fill(gCosThetaL,gCosThetaK);
        if ( fabs(gmumeta) < 2.3 && fabs(gmupeta) < 2.3 && gmumpt > 2.8 && gmuppt > 2.8 ) h2_nacc.Fill(gCosThetaL,gCosThetaK);
    }
    
    // Calculate acceptance
    TH2F h2_acc("h2_acc","",nbinsL,-1,1,nbinsK,-1,1);
    h2_acc.SetAxisRange(0.,1.,"Z");
    for (int i = 1; i <= nbinsL; i++) {
        for (int j = 1; j <= nbinsK; j++) {
            // Generate toy model
            //double ii = -1. + i*2./nbinsL - 1./nbinsL;
            //double jj = -1. + j*2./nbinsK - 1./nbinsK;
            //TF2 f2_gen("f2_gen","(0.06+0.02*(3*y**2-1)/2)-(0.03+0.03*(3*y**2-1)/2)*x**2+(0.005+0.003*(3*y**2-1)/2)*x**4",-1.,1.,-1.,1.);
            //h2_ngen.SetBinContent(i,j,400);
            //h2_nacc.SetBinContent(i,j,400*f2_gen.Eval(ii,jj));
            
            // Fill acceptance
            if (h2_ngen.GetBinContent(i,j) == 0) {
                printf("WARNING: Acceptance(%d,%d)=%f/%f\n",i,j,h2_nacc.GetBinContent(i,j),h2_ngen.GetBinContent(i,j));
                h2_acc.SetBinContent(i,j,0.);
                h2_acc.SetBinError(i,j,1.);
            }else{
                h2_acc.SetBinContent(i,j,h2_nacc.GetBinContent(i,j)/h2_ngen.GetBinContent(i,j));
                if (h2_nacc.GetBinContent(i,j) != 0){
                    h2_acc.SetBinError(i,j,sqrt(h2_acc.GetBinContent(i,j)*(1.-h2_acc.GetBinContent(i,j))/h2_ngen.GetBinContent(i,j)));
                }else{
                    h2_acc.SetBinError(i,j,sqrt(0.05/h2_ngen.GetBinContent(i,j)));
                }
                //printf("INFO: Angular bin(%d,%d)= %f +- %f ( %f / %f).\n",i,j,h2_acc.GetBinContent(i,j),h2_acc.GetBinError(i,j),h2_nacc.GetBinContent(i,j),h2_ngen.GetBinContent(i,j));
            }
        }
    }
    printf("INFO: h2_acc built.\n");
    
    // Using pure TMinuit
    int nPar = 20;
    TMinuit *gMinuit = new TMinuit(nPar);
    h2_fcn = &h2_acc;
    gMinuit->SetFCN(fcn_binnedChi2_2D);
    
    TF2 f2_model("f2_model","([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6",-1.,1.,-1.,1.);
    f2_fcn = &f2_model;
    gMinuit->DefineParameter( 0, "k0l0",  .06,  1E-3,    -1E+0, 1E+2);
    gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 4, "k0l2",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(10, "k2l3",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(11, "k3l3",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(12, "k0l4",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(13, "k1l4",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(14, "k2l4",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(15, "k3l4",   0.,  1E-3,    -1E+1, 1E+2);
    gMinuit->DefineParameter(16, "k0l6",   0.,  1E-3,    -1E+2, 1E+4);
    gMinuit->DefineParameter(17, "k1l6",   0.,  1E-3,    -1E+2, 1E+4);
    gMinuit->DefineParameter(18, "k2l6",   0.,  1E-3,    -1E+2, 1E+4);
    gMinuit->DefineParameter(19, "k3l6",   0.,  1E-3,    -1E+2, 1E+4);
    if (iBin == 0) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
    }else if (iBin == 1) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else if (iBin > 1 && iBin < 6 ) {
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else{
        gMinuit->Command("SET PARM 13 0");
        gMinuit->Command("SET PARM 14 0");
        gMinuit->Command("SET PARM 15 0");
        gMinuit->Command("SET PARM 16 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 13");
        gMinuit->Command("FIX 14");
        gMinuit->Command("FIX 15");
        gMinuit->Command("FIX 16");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }
    
    gMinuit->Command("MINI");
    gMinuit->Command("MINI");
    gMinuit->Command("IMPROVE");
    gMinuit->Command("MINOS");

    double arrPar[nPar];
    double arrParErr[nPar];
    for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
    for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);

    // Prepare draw
    double chi2Val=0;
    fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
    printf("Chi2=%f \n",chi2Val);
    
    TCanvas canvas("canvas");
    TLatex *latex = new TLatex();
    latex->SetNDC();
    
    // Draw efficiency
    h2_acc.SetStats(0);
    h2_acc.SetMinimum(0.);
    h2_acc.SetMaximum(accUpperBound);
    h2_acc.SetTitleOffset(2,"XY");
    h2_acc.SetXTitle("genCosThetaL");
    h2_acc.SetYTitle("genCosThetaK");
    h2_acc.SetZTitle("Acceptance");
    h2_acc.Draw("LEGO2");
    latex->DrawLatex(0.35,0.95,TString::Format("Acceptance in Bin%d",iBin));
    
    // Draw FitResult
    f2_model.SetTitle("");
    f2_model.SetMaximum(accUpperBound);
    f2_model.SetLineWidth(1);
    //latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
    //latex->DrawLatex(0.01,0.90,TString::Format("DoF = %d",nbinsK*nbinsL-gMinuit->GetNumFreePars()));
    //f2_model.Draw("SURF SAME ");
    canvas.Print(TString::Format("./plots/acceptance_2D_bin%d.pdf",iBin));

    //// Draw compare
    TH2F h2_compFit("h2_compFit","",nbinsL,-1,1,nbinsK,-1,1);
    for (int i = 1; i <= nbinsL; i++) {//thetaL
        for (int j = 1; j <= nbinsK; j++) {//thetaK
            if (h2_acc.GetBinContent(i,j) != 0){
                h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_acc.GetXaxis()->GetBinCenter(i),h2_acc.GetYaxis()->GetBinCenter(j))/h2_acc.GetBinContent(i,j));
            }else{
                h2_compFit.SetBinContent(i,j,0.);
            }
        }
    }
    h2_compFit.SetMinimum(0.);
    h2_compFit.SetStats(0);
    h2_compFit.SetTitleOffset(2,"XY");
    h2_compFit.SetXTitle("genCosThetaL");
    h2_compFit.SetYTitle("genCosThetaK");
    h2_compFit.Draw("LEGO2");
    latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
    latex->DrawLatex(0.01,0.90,TString::Format("DoF = %d",nbinsK*nbinsL-gMinuit->GetNumFreePars()));
    latex->DrawLatex(0.30,0.95,TString::Format("acceptance_{measured} / acceptance_{fit} in Bin%d",iBin));
    canvas.Update();
    //canvas.Print(TString::Format("./plots/acceptance_compFit_2D_bin%d.pdf",iBin));
    
    // Draw significance
    TH1F h_pull("Deviation/Error","",15,-3.,3.);
    h_pull.SetXTitle("Significance of deviation");
    h_pull.SetYTitle("Angular bins");
    for (int i = 1; i <= nbinsL; i++) {//thetaL
        for (int j = 1; j <= nbinsK; j++) {//thetaK
            double _xlo = h2_acc.GetXaxis()->GetBinLowEdge(i);
            double _xhi = h2_acc.GetXaxis()->GetBinUpEdge(i);
            double _ylo = h2_acc.GetYaxis()->GetBinLowEdge(j);
            double _yhi = h2_acc.GetYaxis()->GetBinUpEdge(j);
            if (h2_nacc.GetBinContent(i,j) != 0){
                h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_acc.GetBinContent(i,j))/h2_acc.GetBinError(i,j));
            }
        }
    }
    h_pull.Draw();
    canvas.Update();
    //canvas.Print(TString::Format("./plots/acceptance_sigma_bin%d.pdf",iBin));
    
    // Clear
    delete latex;
    delete gMinuit;

    //prepare output
    std::vector<double> output;
    for (int iPar = 0; iPar < nPar; iPar++){
        output.push_back(arrPar[iPar]);
        output.push_back(arrParErr[iPar]);
        
        printf("%f,",arrPar[iPar]);
        if (iPar+1 >= nPar) printf("\n");
    }
    for (int i = 0; i < output.size(); i=i+2) {
        printf("%f,",output[i+1]);
        if (i+2 >= output.size()) printf("\n");
    }
    return output;
}//}}}

std::vector<double> recoEff(int iBin) // reconstruction efficiency. Not used in standard procedure, just for check...
{//{{{
    printf("Evaluate reconstruction efficiency for bin#%d\n",iBin);
    double effUpperBound = 0.5;
    double BMass = 0;
    double Mumumass = 0;
    double Mumumasserr = 0;
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double gmuppt = 0;
    double gmupeta= 0;
    double gmumpt = 0;
    double gmumeta= 0;

    ch->SetBranchStatus("*",0);
    ch->SetBranchStatus("Bmass"         , 1);
    ch->SetBranchStatus("Mumumass"      , 1);
    ch->SetBranchStatus("Mumumasserr"   , 1);
    ch->SetBranchStatus("genQ2"         , 1);
    ch->SetBranchStatus("genCosTheta*"  , 1);
    ch->SetBranchStatus("genMu*"        , 1);
    ch->SetBranchAddress("Bmass"        , &BMass);
    ch->SetBranchAddress("Mumumass"     , &Mumumass);
    ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
    ch->SetBranchAddress("genQ2"        , &gQ2);
    ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    ch->SetBranchAddress("genMupPt"     , &gmuppt);
    ch->SetBranchAddress("genMupEta"    , &gmupeta);
    ch->SetBranchAddress("genMumPt"     , &gmumpt);
    ch->SetBranchAddress("genMumEta"    , &gmumeta);

    // Fill histograms
    float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};
    float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};
    TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
    TH2F h2_nreco("h2_nreco","h2_nreco",6,thetaLBins,5,thetaKBins);
    for (int entry = 0; entry < ch->GetEntries(); entry++) {
        ch->GetEntry(entry);
        if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
        if ( fabs(gmumeta) < 2.3 && fabs(gmupeta) < 2.3 && gmumpt > 2.8 && gmuppt > 2.8 ) h2_nacc.Fill(gCosThetaL,gCosThetaK);
        if (BMass != 0 && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-     3.5*Mumumasserr)) ){
            h2_nreco.Fill(gCosThetaL,gCosThetaK);
        }
    }
    
    // Calculate efficiency
    TH2F h2_rec("h2_rec","",6,thetaLBins,5,thetaKBins);
    h2_rec.SetMinimum(0.);
    h2_rec.SetTitleOffset(2,"XY");
    h2_rec.SetXTitle("genCosThetaL");
    h2_rec.SetYTitle("genCosThetaK");
    for (int i = 1; i <= 6; i++) {
        for (int j = 1; j <= 5; j++) {
            // Build from MC samples
            if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
                printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
                h2_rec.SetBinContent(i,j,0.);
                h2_rec.SetBinError(i,j,1.);
            }else{
                h2_rec.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j));
                h2_rec.SetBinError(i,j,sqrt(h2_rec.GetBinContent(i,j)*(1-h2_rec.GetBinContent(i,j))/h2_nacc.GetBinContent(i,j)));
            }

            // Build a toy sample
            //h2_rec.SetBinContent(i,j,fabs(sin((thetaLBins[i]+thetaLBins[i-1])*3.1415926/4)/(20.-2*j))+0.1);
            //h2_rec.SetBinError(i,j,sqrt(h2_rec.GetBinContent(i,j)*(1-h2_rec.GetBinContent(i,j))/20));
        }
    }

    // Using pure TMinuit
    int nPar = 20;
    TMinuit *gMinuit = new TMinuit(nPar);
    h2_fcn = &h2_rec;
    gMinuit->SetFCN(fcn_binnedChi2_2D);
    
    // Use Legendre polynomial for better convergance
    // 1,x,(3x^2-1)/2,(5x^3-3x)/2
    //TF2 f2_model("f2_model","([0]+[1]*y+[2]*y**2+[3]*y**3)+([4]+[5]*y+[6]*y**2+[7]*y**3)*x**2+([8]+[9]*y+[10]*y**2+[11]*y**3)*x**3+([12]+[13]*y+[14]*y**2+[15]*y**3)*x**4+([16]+[17]*y+[18]*y**2+[19]*y**3)*x**6",-1.,1.,-1.,1.);
    TF2 f2_model("f2_model","([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6",-1.,1.,-1.,1.);
    f2_fcn = &f2_model;
    gMinuit->DefineParameter( 0, "k0l0",  .01,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 4, "k0l2", 1E-2,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(10, "k2l3",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(11, "k3l3",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(12, "k0l4",-1E-2,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(13, "k1l4",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(14, "k2l4",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(15, "k3l4",   0.,  1E-3,    -1E+1, 1E+1);
    gMinuit->DefineParameter(16, "k0l6", 1E-2,  1E-3,    -1E+2, 1E+3);
    gMinuit->DefineParameter(17, "k1l6",   0.,  1E-3,    -1E+2, 1E+3);
    gMinuit->DefineParameter(18, "k2l6",   0.,  1E-3,    -1E+2, 1E+3);
    gMinuit->DefineParameter(19, "k3l6",   0.,  1E-3,    -1E+2, 1E+3);

    if (iBin == 0) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
    }else if (iBin == 1) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else if (iBin > 1 && iBin < 6 ) {
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else{
        gMinuit->Command("SET PARM 13 0");
        gMinuit->Command("SET PARM 14 0");
        gMinuit->Command("SET PARM 15 0");
        gMinuit->Command("SET PARM 16 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 13");
        gMinuit->Command("FIX 14");
        gMinuit->Command("FIX 15");
        gMinuit->Command("FIX 16");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }
    
    gMinuit->Command("MINI");
    gMinuit->Command("MINI");
    gMinuit->Command("IMPROVE");
    gMinuit->Command("MINOS");

    double arrPar[nPar];
    double arrParErr[nPar];
    for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
    for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);
    
    // Prepare draw
    TCanvas canvas("canvas");
    TLatex *latex = new TLatex();
    latex->SetNDC();
    
    // Draw efficiency
    h2_rec.SetStats(0);
    h2_rec.SetMaximum(effUpperBound);
    h2_rec.Draw("LEGO2");
    latex->DrawLatex(0.35,0.95,TString::Format("#varepsilon_{RECO} in Bin%d",iBin));
    
    // Draw FitResult
    f2_model.SetTitle("");
    f2_model.SetMaximum(effUpperBound);
    f2_model.SetLineWidth(1);
    f2_model.Draw("SURF SAME ");
    canvas.Print(TString::Format("./plots/recoEff_2D_bin%d.pdf",iBin));

    //// Draw compare
    double chi2Val=0;
    fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
    printf("Chi2(Bin center)=%f \n",chi2Val);
    
    TH2F h2_compFit("h2_compFit","",6,thetaLBins,5,thetaKBins);
    h2_compFit.SetTitleOffset(2,"XY");
    h2_compFit.SetXTitle("genCosThetaL");
    h2_compFit.SetYTitle("genCosThetaK");
    for (int i = 1; i <= 6; i++) {//thetaL
        for (int j = 1; j <= 5; j++) {//thetaK
            if (h2_rec.GetBinContent(i,j) != 0){
                h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_rec.GetXaxis()->GetBinCenter(i),h2_rec.GetYaxis()->GetBinCenter(j))/h2_rec.GetBinContent(i,j));
            }else{
                h2_compFit.SetBinContent(i,j,0.);
            }
        }
    }
    h2_compFit.SetMinimum(0.);
    h2_compFit.SetStats(0);
    h2_compFit.Draw("LEGO2");
    latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
    latex->DrawLatex(0.3,0.95,TString::Format("#varepsilon_{RECO,fit} / #varepsilon_{RECO,measured} in Bin%d",iBin));
    canvas.Update();
    canvas.Print(TString::Format("./plots/recoEff_compFit_2D_bin%d.pdf",iBin));

    // Draw significance of deviation
    TH1F h_pull("Deviation/Error","",15,-3.,3.);
    h_pull.SetXTitle("Significance of deviation");
    h_pull.SetYTitle("Angular bins");
    for (int i = 1; i <= 6; i++) {//thetaL
        for (int j = 1; j <= 5; j++) {//thetaK
            double _xlo = h2_rec.GetXaxis()->GetBinLowEdge(i);
            double _xhi = h2_rec.GetXaxis()->GetBinUpEdge(i);
            double _ylo = h2_rec.GetYaxis()->GetBinLowEdge(j);
            double _yhi = h2_rec.GetYaxis()->GetBinUpEdge(j);
            if (h2_rec.GetBinContent(i,j) != 0){
                h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_rec.GetBinContent(i,j))/h2_rec.GetBinError(i,j));
            }
        }
    }
    h_pull.Draw();
    canvas.Update();
    canvas.Print(TString::Format("./plots/recoEff_sigma_bin%d.pdf",iBin));

    // Clear
    delete latex;
    delete gMinuit;

    //prepare output
    std::vector<double> output;
    for (int iPar = 0; iPar < nPar; iPar++){
        output.push_back(arrPar[iPar]);
        output.push_back(arrParErr[iPar]);
        
        printf("%f,",arrPar[iPar]);
        if (iPar+1 >= nPar) printf("\n");
    }
    for (int i = 0; i < output.size(); i=i+2) {
        printf("%f,",output[i+1]);
        if (i+2 >= output.size()) printf("\n");
    }
    return output;
}//}}}

void createAccptanceHist() // create acceptance histogram from UNFILTERED GEN.
{//{{{
    double accUpperBound = 0.09;
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double gmuppt = 0;
    double gmupeta= 0;
    double gmupphi= 0;
    double gmumpt = 0;
    double gmumeta= 0;
    double gmumphi= 0;

    TChain *treein=ch;
    if (treein == NULL) gSystem->Exit(0);
    treein->SetBranchStatus("*",0);
    treein->SetBranchStatus("genQ2"         , 1);
    treein->SetBranchStatus("genCosTheta*"  , 1);
    treein->SetBranchStatus("genMu*"        , 1);
    treein->SetBranchAddress("genQ2"        , &gQ2);
    treein->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    treein->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    treein->SetBranchAddress("genMupPt"     , &gmuppt);
    treein->SetBranchAddress("genMupEta"    , &gmupeta);
    treein->SetBranchAddress("genMupPhi"    , &gmupphi);
    treein->SetBranchAddress("genMumPt"     , &gmumpt);
    treein->SetBranchAddress("genMumEta"    , &gmumeta);
    treein->SetBranchAddress("genMumPhi"    , &gmumphi);

    // Create histograms
    TFile *fout = new TFile("acceptance_8TeV.root","RECREATE");
    float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};
    float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};
    TH2F *h2_ngen[10];
    TH2F *h2_nacc[10];
    TH2F *h2_acc[10];
    TH2F *h2_ngen_fine[10];
    TH2F *h2_nacc_fine[10];
    TH2F *h2_acc_fine[10];
    TH1F *h_ngenL_fine[10];
    TH1F *h_naccL_fine[10];
    TH1F *h_accL_fine[10];
    TH1F *h_ngenK_fine[10];
    TH1F *h_naccK_fine[10];
    TH1F *h_accK_fine[10];
    for(int iBin = 0; iBin < 10; iBin++){
        h2_ngen[iBin] = new TH2F(TString::Format("h2_ngen_bin%d",iBin),"h2_ngen",6,thetaLBins,5,thetaKBins);
        h2_nacc[iBin] = new TH2F(TString::Format("h2_nacc_bin%d",iBin) ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
        h2_acc [iBin] = new TH2F(TString::Format("h2_acc_bin%d",iBin),"",6,thetaLBins,5,thetaKBins);
        h2_ngen_fine[iBin] = new TH2F(TString::Format("h2_ngen_fine_bin%d",iBin),"h2_ngen",20,-1,1,20,-1,1);
        h2_nacc_fine[iBin] = new TH2F(TString::Format("h2_nacc_fine_bin%d",iBin) ,"h2_nacc" ,20,-1,1,20,-1,1); 
        h2_acc_fine[iBin]  = new TH2F(TString::Format("h2_acc_fine_bin%d",iBin),"",20,-1,1,20,-1,1);
        h_ngenL_fine[iBin] = new TH1F(TString::Format("h_ngenL_fine_bin%d",iBin),"h_ngenL",20,-1,1);
        h_naccL_fine[iBin] = new TH1F(TString::Format("h_naccL_fine_bin%d",iBin) ,"h_naccL" ,20,-1,1); 
        h_accL_fine[iBin]  = new TH1F(TString::Format("h_accL_fine_bin%d",iBin),"",20,-1,1);
        h_ngenK_fine[iBin] = new TH1F(TString::Format("h_ngenK_fine_bin%d",iBin),"h_ngenK",20,-1,1);
        h_naccK_fine[iBin] = new TH1F(TString::Format("h_naccK_fine_bin%d",iBin) ,"h_naccK" ,20,-1,1); 
        h_accK_fine[iBin]  = new TH1F(TString::Format("h_accK_fine_bin%d",iBin),"",20,-1,1);
        h2_ngen[iBin]->SetTitleOffset(2,"XYZ");
        h2_ngen[iBin]->SetXTitle("genCosThetaL");
        h2_ngen[iBin]->SetYTitle("genCosThetaK");
        h2_ngen[iBin]->SetZTitle("Generated events");
        h2_nacc[iBin]->SetTitleOffset(2,"XY");
        h2_nacc[iBin]->SetXTitle("genCosThetaL");
        h2_nacc[iBin]->SetYTitle("genCosThetaK");
        h2_nacc[iBin]->SetZTitle("Events in acceptance");
        h2_acc [iBin]->SetStats(0);
        h2_acc [iBin]->SetMinimum(0.);
        h2_acc [iBin]->SetMaximum(accUpperBound);
        h2_acc [iBin]->SetTitleOffset(2,"XY");
        h2_acc [iBin]->SetXTitle("genCosThetaL");
        h2_acc [iBin]->SetYTitle("genCosThetaK");
        h2_acc [iBin]->SetZTitle("Acceptance");
        h2_ngen_fine[iBin]->SetTitleOffset(2,"XYZ");
        h2_ngen_fine[iBin]->SetXTitle("genCosThetaL");
        h2_ngen_fine[iBin]->SetYTitle("genCosThetaK");
        h2_ngen_fine[iBin]->SetZTitle("Generated events");
        h2_nacc_fine[iBin]->SetTitleOffset(2,"XY");
        h2_nacc_fine[iBin]->SetXTitle("genCosThetaL");
        h2_nacc_fine[iBin]->SetYTitle("genCosThetaK");
        h2_nacc_fine[iBin]->SetZTitle("Events in acceptance");
        h2_acc_fine [iBin]->SetStats(0);
        h2_acc_fine [iBin]->SetMinimum(0.);
        h2_acc_fine [iBin]->SetMaximum(accUpperBound);
        h2_acc_fine [iBin]->SetTitleOffset(2,"XY");
        h2_acc_fine [iBin]->SetXTitle("genCosThetaL");
        h2_acc_fine [iBin]->SetYTitle("genCosThetaK");
        h2_acc_fine [iBin]->SetZTitle("Acceptance");
        h_ngenL_fine[iBin]->SetXTitle("genCosThetaL");
        h_ngenL_fine[iBin]->SetZTitle("Generated events");
        h_naccL_fine[iBin]->SetXTitle("genCosThetaL");
        h_naccL_fine[iBin]->SetZTitle("Events in acceptance");
        h_accL_fine [iBin]->SetStats(0);
        h_accL_fine [iBin]->SetMinimum(0.);
        h_accL_fine [iBin]->SetMaximum(accUpperBound);
        h_accL_fine [iBin]->SetXTitle("genCosThetaL");
        h_accL_fine [iBin]->SetZTitle("Acceptance");
        h_ngenK_fine[iBin]->SetXTitle("genCosThetaK");
        h_ngenK_fine[iBin]->SetZTitle("Generated events");
        h_naccK_fine[iBin]->SetXTitle("genCosThetaK");
        h_naccK_fine[iBin]->SetZTitle("Events in acceptance");
        h_accK_fine [iBin]->SetStats(0);
        h_accK_fine [iBin]->SetMinimum(0.);
        h_accK_fine [iBin]->SetMaximum(accUpperBound);
        h_accK_fine [iBin]->SetXTitle("genCosThetaK");
        h_accK_fine [iBin]->SetZTitle("Acceptance");
    }
    
    // Fill histograms
        // Read data
    for (int entry = 0; entry < treein->GetEntries(); entry++) {
        treein->GetEntry(entry);
        for(int iBin = 0; iBin < 10; iBin++){
            if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
            h2_ngen[iBin]->Fill(gCosThetaL,gCosThetaK);
            h2_ngen_fine[iBin]->Fill(gCosThetaL,gCosThetaK);
            h_ngenL_fine[iBin]->Fill(gCosThetaL);
            h_ngenK_fine[iBin]->Fill(gCosThetaK);
            if ( fabs(gmupeta) < 2.3 && gmuppt > 2.8 && fabs(gmumeta) < 2.3 && gmumpt > 2.8){
                h2_nacc[iBin]->Fill(gCosThetaL,gCosThetaK);
                h2_nacc_fine[iBin]->Fill(gCosThetaL,gCosThetaK);
                h_naccL_fine[iBin]->Fill(gCosThetaL);
                h_naccK_fine[iBin]->Fill(gCosThetaK);
            }
        }
    }
        
    for(int iBin = 0; iBin < 10; iBin++){
        // Calculate acceptance
        h2_acc[iBin]->SetAxisRange(0.,1.,"Z");
        for (int i = 1; i <= 6; i++) {
            for (int j = 1; j <= 5; j++) {
                // Fill acceptance
                if (h2_ngen[iBin]->GetBinContent(i,j) == 0) {
                    printf("WARNING: Acceptance(%d,%d)=%f/%f\n",i,j,h2_nacc[iBin]->GetBinContent(i,j),h2_ngen[iBin]->GetBinContent(i,j));
                    h2_acc[iBin]->SetBinContent(i,j,0.);
                    h2_acc[iBin]->SetBinError(i,j,1.);
                }else{
                    h2_acc[iBin]->SetBinContent(i,j,h2_nacc[iBin]->GetBinContent(i,j)/h2_ngen[iBin]->GetBinContent(i,j));
                    if (h2_nacc[iBin]->GetBinContent(i,j) != 0){
                        h2_acc[iBin]->SetBinError(i,j,sqrt(h2_acc[iBin]->GetBinContent(i,j)*(1.-h2_acc[iBin]->GetBinContent(i,j))/h2_ngen[iBin]->GetBinContent(i,j)));
                    }else{
                        h2_acc[iBin]->SetBinError(i,j,0.);
                    }
                }
            }
        }
        printf("INFO: h2_acc_bin%d built.\n",iBin);
        
        h2_acc_fine[iBin]->SetAxisRange(0.,1.,"Z");
        for (int i = 1; i <= 20; i++) {//L
            for (int j = 1; j <= 20; j++) {//K
                // Fill acceptance
                if (h2_ngen_fine[iBin]->GetBinContent(i,j) == 0) {
                    h2_acc_fine[iBin]->SetBinContent(i,j,0.);
                    h2_acc_fine[iBin]->SetBinError(i,j,1.);
                }else{
                    h2_acc_fine[iBin]->SetBinContent(i,j,h2_nacc_fine[iBin]->GetBinContent(i,j)/h2_ngen_fine[iBin]->GetBinContent(i,j));
                    if (h2_nacc_fine[iBin]->GetBinContent(i,j) != 0){
                        h2_acc_fine[iBin]->SetBinError(i,j,sqrt(h2_acc_fine[iBin]->GetBinContent(i,j)*(1.-h2_acc_fine[iBin]->GetBinContent(i,j))/h2_ngen_fine[iBin]->GetBinContent(i,j)));
                    }else{
                        h2_acc_fine[iBin]->SetBinError(i,j,0.);
                    }
                }
                
            }
            
            // 1-D
            if (h_ngenL_fine[iBin]->GetBinContent(i) == 0) {
                h_accL_fine[iBin]->SetBinContent(i,0.);
                h_accL_fine[iBin]->SetBinError(i,1.);
            }else{
                h_accL_fine[iBin]->SetBinContent(i,h_naccL_fine[iBin]->GetBinContent(i)/h_ngenL_fine[iBin]->GetBinContent(i));
                if (h_naccL_fine[iBin]->GetBinContent(i) != 0){
                    h_accL_fine[iBin]->SetBinError(i,sqrt(h_accL_fine[iBin]->GetBinContent(i)*(1.-h_accL_fine[iBin]->GetBinContent(i))/h_ngenL_fine[iBin]->GetBinContent(i)));
                }else{
                    h_accL_fine[iBin]->SetBinError(i,0.);
                }
            }
            
        }
        for (int i = 1; i <= 20; i++) {//K
            // 1-D
            if (h_ngenK_fine[iBin]->GetBinContent(i) == 0) {
                h_accK_fine[iBin]->SetBinContent(i,0.);
                h_accK_fine[iBin]->SetBinError(i,1.);
            }else{
                h_accK_fine[iBin]->SetBinContent(i,h_naccK_fine[iBin]->GetBinContent(i)/h_ngenK_fine[iBin]->GetBinContent(i));
                if (h_naccK_fine[iBin]->GetBinContent(i) != 0){
                    h_accK_fine[iBin]->SetBinError(i,sqrt(h_accK_fine[iBin]->GetBinContent(i)*(1.-h_accK_fine[iBin]->GetBinContent(i))/h_ngenK_fine[iBin]->GetBinContent(i)));
                }else{
                    h_accK_fine[iBin]->SetBinError(i,0.);
                }
            }
        }
        printf("INFO: h2_acc_fine_bin%d built.\n",iBin);
    }
    fout->Write();
    fout->Close();
}//}}}

void createRecoEffHist(int iBin) // create reco efficiency histogram from official MC sample.
{//{{{
    printf("Evaluate reconstruction efficiency for bin#%d\n",iBin);
    double effUpperBound = 0.03;
    double BMass = 0;
    double Mumumass = 0;
    double Mumumasserr = 0;
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double gmuppt = 0;
    double gmupeta= 0;
    double gmumpt = 0;
    double gmumeta= 0;

    ch->SetBranchStatus("*",0);
    ch->SetBranchStatus("Bmass"         , 1);
    ch->SetBranchStatus("Mumumass"      , 1);
    ch->SetBranchStatus("Mumumasserr"   , 1);
    ch->SetBranchStatus("genQ2"         , 1);
    ch->SetBranchStatus("genCosTheta*"  , 1);
    ch->SetBranchStatus("genMu*"        , 1);
    ch->SetBranchAddress("Bmass"        , &BMass);
    ch->SetBranchAddress("Mumumass"     , &Mumumass);
    ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
    ch->SetBranchAddress("genQ2"        , &gQ2);
    ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    ch->SetBranchAddress("genMupPt"     , &gmuppt);
    ch->SetBranchAddress("genMupEta"    , &gmupeta);
    ch->SetBranchAddress("genMumPt"     , &gmumpt);
    ch->SetBranchAddress("genMumEta"    , &gmumeta);

    // Fill histograms
    const int nKBins = 20;
    const int nLBins = 20;
    //float thetaKBins[nKBins]={-1,-0.7,0.,0.4,0.8,1};//nKBins=5
    //float thetaLBins[nLBins]={-1,-0.7,-0.3,0.,0.3,0.7,1};//nLBins=6
    //TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
    //TH2F h2_nreco("h2_nreco","h2_nreco",6,thetaLBins,5,thetaKBins);
    TH2F h2_nacc("h2_nacc","h2_nacc",nLBins,-1,1,nKBins,-1,1);
    TH2F h2_nreco("h2_nreco","h2_nreco",nLBins,-1,1,nKBins,-1,1);
    for (int entry = 0; entry < ch->GetEntries(); entry++) {
        ch->GetEntry(entry);
        if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
        if ( fabs(gmumeta) < 2.3 && fabs(gmupeta) < 2.3 && gmumpt > 2.8 && gmuppt > 2.8 ) h2_nacc.Fill(gCosThetaL,gCosThetaK);
        if (BMass != 0 && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-     3.5*Mumumasserr)) ){
            h2_nreco.Fill(gCosThetaL,gCosThetaK);
        }
    }
    
    // Calculate efficiency
    //TH2F h2_rec("h2_rec","",6,thetaLBins,5,thetaKBins);
    TH2F h2_rec("h2_rec","",nLBins,-1,1,nKBins,-1,1);
    for (int i = 1; i <= nLBins; i++) {
        for (int j = 1; j <= nKBins; j++) {
            // Build from MC samples
            if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
                printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
                h2_rec.SetBinContent(i,j,0.);
                h2_rec.SetBinError(i,j,1.);
            }else{
                h2_rec.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j));
                h2_rec.SetBinError(i,j,sqrt(h2_rec.GetBinContent(i,j)*(1-h2_rec.GetBinContent(i,j))/h2_nreco.GetBinContent(i,j)));
                printf("INFO: Efficiency(%d,%d)=%f +- %f.\n",i,j,h2_rec.GetBinContent(i,j),h2_rec.GetBinError(i,j));
            }
        }
    }
    h2_rec.SetTitleOffset(2,"XY");
    h2_rec.SetXTitle("CosThetaL");
    h2_rec.SetYTitle("CosThetaK");
    h2_rec.SetStats(0);
    h2_rec.SetMinimum(0.);
    
    //TH1F h_recL("h_recL","",6,thetaLBins,5,thetaKBins);
    TH1F h_recL("h_recL","",nLBins,-1,1);
    for (int i = 1; i <= nLBins; i++) {
        double nacc = 0;
        double nreco = 0;
        for (int j = 1; j <= nKBins; j++) {
            nacc+= h2_nacc.GetBinContent(i,j);
            nreco+= h2_nreco.GetBinContent(i,j);
        }
        if (nacc !=0 ){
            h_recL.SetBinContent(i,nreco/nacc);
            h_recL.SetBinError(i,sqrt(h_recL.GetBinContent(i)*(1-h_recL.GetBinContent(i))/nacc));
        }else{
            h_recL.SetBinContent(i,0);
            h_recL.SetBinError(i,1);
        }
    }
    h_recL.SetStats(0);
    h_recL.SetMinimum(0.);
    h_recL.SetXTitle("CosThetaL");
    
    //TH1F h_recK("h_recK","",6,thetaLBins,5,thetaKBins);
    TH1F h_recK("h_recK","",nKBins,-1,1);
    for (int i = 1; i <= nKBins; i++) {
        double nacc = 0;
        double nreco = 0;
        for (int j = 1; j <= nLBins; j++) {
            nacc+= h2_nacc.GetBinContent(j,i);
            nreco+= h2_nreco.GetBinContent(j,i);
        }
        if (nacc !=0 ){
            h_recK.SetBinContent(i,nreco/nacc);
            h_recK.SetBinError(i,sqrt(h_recK.GetBinContent(i)*(1-h_recK.GetBinContent(i))/nacc));
        }else{
            h_recK.SetBinContent(i,0);
            h_recK.SetBinError(i,1);
        }
    }
    h_recK.SetStats(0);
    h_recK.SetMinimum(0.);
    h_recK.SetXTitle("CosThetaK");

    // Print
    TCanvas canvas("canvas");
    h2_rec.Draw("LEGO2 TEXT");
    canvas.Print(TString::Format("./plots/recoEff_2D_bin%d.pdf",iBin));
    h_recL.Draw("TEXT");
    canvas.Update();
    canvas.Print(TString::Format("./plots/recoEff_cosl_bin%d.pdf",iBin));
    h_recK.Draw("TEXT");
    canvas.Update();
    canvas.Print(TString::Format("./plots/recoEff_cosk_bin%d.pdf",iBin));
}//}}}

std::string accXrecoEff2(int iBin) // acceptance*reconstruction efficiency
{//{{{
    printf("Evaluate reconstruction efficiency for bin#%d\n",iBin);
        // cut3-8TeV
    double effUpperBound = 0.00045;
    if (iBin == 3 || iBin == 5) effUpperBound = 4e-5;
    double BMass = 0;
    double Mumumass = 0;
    double Mumumasserr = 0;
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double gmuppt = 0;
    double gmupeta= 0;
    double gmupphi= 0;
    double gmumpt = 0;
    double gmumeta= 0;
    double gmumphi= 0;

    ch->SetBranchStatus("*",0);
    ch->SetBranchStatus("Bmass"         , 1);
    ch->SetBranchStatus("Mumumass"      , 1);
    ch->SetBranchStatus("Mumumasserr"   , 1);
    ch->SetBranchStatus("genQ2"         , 1);
    ch->SetBranchStatus("genCosTheta*"  , 1);
    ch->SetBranchStatus("genMu*"        , 1);
    ch->SetBranchAddress("Bmass"        , &BMass);
    ch->SetBranchAddress("Mumumass"     , &Mumumass);
    ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
    ch->SetBranchAddress("genQ2"        , &gQ2);
    ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    ch->SetBranchAddress("genMupPt"     , &gmuppt);
    ch->SetBranchAddress("genMupEta"    , &gmupeta);
    ch->SetBranchAddress("genMupPhi"    , &gmupphi);
    ch->SetBranchAddress("genMumPt"     , &gmumpt);
    ch->SetBranchAddress("genMumEta"    , &gmumeta);
    ch->SetBranchAddress("genMumPhi"    , &gmumphi);

    // Load acceptance
    TFile f_acc("acceptance_8TeV.root");
    TH2F *h2_acc = (TH2F*)f_acc.Get(TString::Format("h2_acc_bin%d",iBin));
    TH1F *h_accL = (TH1F*)f_acc.Get(TString::Format("h_accL_fine_bin%d",iBin));
    TH1F *h_accK = (TH1F*)f_acc.Get(TString::Format("h_accK_fine_bin%d",iBin));
    TH2F *h2_ngen = (TH2F*)f_acc.Get(TString::Format("h2_ngen_bin%d",iBin));
    TH1F *h_ngenL = (TH1F*)f_acc.Get(TString::Format("h_ngenL_fine_bin%d",iBin));
    TH1F *h_ngenK = (TH1F*)f_acc.Get(TString::Format("h_ngenK_fine_bin%d",iBin));

    // Fill histograms
    float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};
    float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};
    TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
    TH2F h2_nreco("h2_nreco","h2_nreco",6,thetaLBins,5,thetaKBins);
    int nLBins = 20;// The same value as h_acc
    int nKBins = 20;
    TH1F h_naccL("h_naccL" ,"h_naccL" ,nLBins,-1,1); 
    TH1F h_nrecoL("h_nrecoL","h_nrecoL",nLBins,-1,1);
    TH1F h_naccK("h_naccK" ,"h_naccK" ,nKBins,-1,1); 
    TH1F h_nrecoK("h_nrecoK","h_nrecoK",nKBins,-1,1);
    h_naccL.SetStats(0);
    h_naccL.SetMinimum(0.);
    h_naccL.SetXTitle("CosThetaL");
    h_naccL.SetYTitle("#Events/0.2");
    h_nrecoL.SetStats(0);
    h_nrecoL.SetMinimum(0.);
    h_nrecoL.SetXTitle("CosThetaL");
    h_nrecoL.SetYTitle("#Events/0.2");
    h_naccK.SetStats(0);
    h_naccK.SetMinimum(0.);
    h_naccK.SetXTitle("CosThetaK");
    h_naccK.SetYTitle("#Events/0.2");
    h_nrecoK.SetStats(0);
    h_nrecoK.SetMinimum(0.);
    h_nrecoK.SetXTitle("CosThetaK");
    h_nrecoK.SetYTitle("#Events/0.2");
    for (int entry = 0; entry < ch->GetEntries(); entry++) {
        ch->GetEntry(entry);
        if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
        if ( fabs(gmupeta) < 2.3 && gmuppt > 2.8 && fabs(gmumeta) < 2.3 && gmumpt > 2.8){
            h2_nacc.Fill(gCosThetaL,gCosThetaK);
            h_naccL.Fill(gCosThetaL);
            h_naccK.Fill(gCosThetaK);
            if (BMass != 0 && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)) ){
                if (isCDFcut == 0 || iBin == 3 || iBin == 5){
                    h2_nreco.Fill(gCosThetaL,gCosThetaK);
                    h_nrecoL.Fill(gCosThetaL);
                    h_nrecoK.Fill(gCosThetaK);
                }else if( (iBin < 3 && fabs(BMass-Mumumass-2.182)<0.16) || (iBin ==4 && fabs(BMass-Mumumass-1.593)<0.06) || (iBin > 5 && fabs(BMass-Mumumass-1.593)<0.06) ){
                    h2_nreco.Fill(gCosThetaL,gCosThetaK);
                    h_nrecoL.Fill(gCosThetaL);
                    h_nrecoK.Fill(gCosThetaK);
                }
            }
        }
    }
    
    // Calculate efficiency
    TH1F h_recL("h_recL","",nLBins,-1,1);
    for (int i = 1; i <= nLBins; i++) {
        h_recL.SetBinContent(i,h_nrecoL.GetBinContent(i)/h_naccL.GetBinContent(i));
        h_recL.SetBinError(i,sqrt(h_recL.GetBinContent(i)*(1-h_recL.GetBinContent(i))/h_naccL.GetBinContent(i)));
    }
    TH1F h_recK("h_recK","",nKBins,-1,1);
    for (int i = 1; i <= nKBins; i++) {
        h_recK.SetBinContent(i,h_nrecoK.GetBinContent(i)/h_naccK.GetBinContent(i));
        h_recK.SetBinError(i,sqrt(h_recK.GetBinContent(i)*(1-h_recK.GetBinContent(i))/h_naccK.GetBinContent(i)));
    }

    TH2F h2_eff("h2_eff","",6,thetaLBins,5,thetaKBins);
    for (int i = 1; i <= 6; i++) {//L
        for (int j = 1; j <= 5; j++) {//K
            // Build from MC samples
            if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
                printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
                h2_eff.SetBinContent(i,j,0.);
                h2_eff.SetBinError(i,j,1.);
            }else{
                h2_eff.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j)*h2_acc->GetBinContent(i,j));
                h2_eff.SetBinError(i,j,h2_eff.GetBinContent(i,j)*sqrt(-1./h2_nacc.GetBinContent(i,j)+1./h2_nreco.GetBinContent(i,j)+pow(h2_acc->GetBinError(i,j)/h2_acc->GetBinContent(i,j),2)));
                printf("INFO: Efficiency(%d,%d)=%f +- %f.\n",i,j,h2_eff.GetBinContent(i,j),h2_eff.GetBinError(i,j));
            }
        }
    }

    TH1F h_effL("h_effL","",nLBins,-1,1);
    for (int i = 1; i <= nLBins; i++) {
        if (h_naccL.GetBinContent(i) == 0 || h_nrecoL.GetBinContent(i) == 0) {
            printf("WARNING: EfficiencyL(%d)=0, set error to be 1.\n",i);
            h_effL.SetBinContent(i,0.);
            h_effL.SetBinError(i,1.);
        }else{
            //h_effL.SetBinContent(i,h_nrecoL.GetBinContent(i)/h_naccL.GetBinContent(i)*h_accL->GetBinContent(i));
            //h_effL.SetBinError(i,h_effL.GetBinContent(i)*sqrt(-1./h_naccL.GetBinContent(i)+1./h_nrecoL.GetBinContent(i)+pow(h_accL->GetBinError(i)/h_accL->GetBinContent(i),2)));
            h_effL.SetBinContent(i,h_recL.GetBinContent(i)*h_accL->GetBinContent(i));
            h_effL.SetBinError(i,h_effL.GetBinContent(i)*h_recL.GetBinError(i)/h_recL.GetBinContent(i));
            //printf("INFO: EfficiencyL(%d)=%f +- %f.\n",i,h_effL.GetBinContent(i),h_effL.GetBinError(i));
        }
    }
    h_effL.SetStats(0);
    h_effL.SetMinimum(0.);
    h_effL.SetXTitle("CosThetaL");
    h_effL.SetYTitle("Efficiency");

    TH1F h_effK("h_effK","",nKBins,-1,1);
    for (int i = 1; i <= nKBins; i++) {
        if (h_naccK.GetBinContent(i) == 0 || h_nrecoK.GetBinContent(i) == 0) {
            printf("WARNING: EfficiencyK(%d)=0, set error to be 1.\n",i);
            h_effK.SetBinContent(i,0.);
            h_effK.SetBinError(i,1.);
        }else{
            //h_effK.SetBinContent(i,h_nrecoK.GetBinContent(i)/h_naccK.GetBinContent(i)*h_accK->GetBinContent(i));
            //h_effK.SetBinError(i,h_effK.GetBinContent(i)*sqrt(-1./h_naccK.GetBinContent(i)+1./h_nrecoK.GetBinContent(i)+pow(h_accK->GetBinError(i)/h_accK->GetBinContent(i),2)));
            h_effK.SetBinContent(i,h_recK.GetBinContent(i)*h_accK->GetBinContent(i));
            h_effK.SetBinError(i,h_effK.GetBinContent(i)*h_recK.GetBinError(i)/h_recK.GetBinContent(i));
            //printf("INFO: EfficiencyK(%d)=%f +- %f.\n",i,h_effK.GetBinContent(i),h_effK.GetBinError(i));
        }
    }
    h_effK.SetStats(0);
    h_effK.SetMinimum(0.);
    h_effK.SetXTitle("CosThetaK");
    h_effK.SetYTitle("Efficiency");

    // Quick check order0 (decoupled)
    TF1 *f_effK_ord0 = new TF1("f_effK_ord0","pol6",-1,1);//y, pol6
    TF1 *f_effL_ord0 = 0;
    if (iBin > 1){
        f_effL_ord0 = new TF1("f_effL_ord0","pol6",-1,1);//x
    }else{
        f_effL_ord0 = new TF1("f_effL_ord0","[2]*exp(-0.5*((x-[0])/[1])**2)+[5]*exp(-0.5*((x-[3])/[4])**2)+[8]*exp(-0.5*((x-[6])/[7])**2)",-1,1);//x
        f_effL_ord0->SetParameter(1,0.5);// width must be non-zero.
        f_effL_ord0->SetParameter(4,0.5);// width must be non-zero.
        f_effL_ord0->SetParameter(7,0.5);// width must be non-zero.
    }
    h_effL.Fit("f_effL_ord0","S");
    h_effK.Fit("f_effK_ord0","S");

    // Using pure TMinuit for order1+
    int nPar = 21;
    TMinuit *gMinuit = new TMinuit(nPar);
    h2_fcn = &h2_eff;
    gMinuit->SetFCN(fcn_binnedChi2_2D);
    
    // 3 Gaussians or 6th order polynomial as 0th order
    // (0~3th order Legendre poly of CosThetaK)*(0~6th order power poly of CosThetaL) as 1st order
    TString f2_model_format_ord0 = TString::Format("(%e*exp(-0.5*((x-(%e))/%e)**2)%+e*exp(-0.5*((x-(%e))/%e)**2)%+e*exp(-0.5*((x-(%e))/%e)**2))*(%e%+e*y%+e*y**2%+e*y**3%+e*y**4%+e*y**5%+e*y**6)",\
            f_effL_ord0->GetParameter(2),\
            f_effL_ord0->GetParameter(0),\
            f_effL_ord0->GetParameter(1),\
            f_effL_ord0->GetParameter(5),\
            f_effL_ord0->GetParameter(3),\
            f_effL_ord0->GetParameter(4),\
            f_effL_ord0->GetParameter(8),\
            f_effL_ord0->GetParameter(6),\
            f_effL_ord0->GetParameter(7),\
            f_effK_ord0->GetParameter(0),\
            f_effK_ord0->GetParameter(1),\
            f_effK_ord0->GetParameter(2),\
            f_effK_ord0->GetParameter(3),\
            f_effK_ord0->GetParameter(4),\
            f_effK_ord0->GetParameter(5),\
            f_effK_ord0->GetParameter(6));
    if (iBin > 1) f2_model_format_ord0 = TString::Format("(%e%+e*x%+e*x**2%+e*x**3%+e*x**4%+e*x**5%+e*x**6)*(%e%+e*y%+e*y**2%+e*y**3%+e*y**4%+e*y**5%+e*y**6)",\
            f_effL_ord0->GetParameter(0),\
            f_effL_ord0->GetParameter(1),\
            f_effL_ord0->GetParameter(2),\
            f_effL_ord0->GetParameter(3),\
            f_effL_ord0->GetParameter(4),\
            f_effL_ord0->GetParameter(5),\
            f_effL_ord0->GetParameter(6),\
            f_effK_ord0->GetParameter(0),\
            f_effK_ord0->GetParameter(1),\
            f_effK_ord0->GetParameter(2),\
            f_effK_ord0->GetParameter(3),\
            f_effK_ord0->GetParameter(4),\
            f_effK_ord0->GetParameter(5),\
            f_effK_ord0->GetParameter(6));
    //printf("%s\n",f2_model_format_ord0.Data());
    TString f2_model_format_ord1 = "([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6";
    TF2 f2_model_ord0("f2_model_ord0",f2_model_format_ord0,-1.,1.,-1.,1.);
    TF2 f2_model_ord1("f2_model_ord1",f2_model_format_ord1,-1.,1.,-1.,1.);
    TF2 f2_model("f2_model",TString::Format("[20]*%s*(1+%s)",f2_model_format_ord0.Data(),f2_model_format_ord1.Data()).Data(),-1.,1.,-1.,1.);
    f2_fcn = &f2_model;
    gMinuit->DefineParameter( 0, "k0l0",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 4, "k0l2",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(10, "k2l3",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(11, "k3l3",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(12, "k0l4",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(13, "k1l4",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(14, "k2l4",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(15, "k3l4",   0.,  1E-4,    -1E+1, 1E+1);
    gMinuit->DefineParameter(16, "k0l6",   0.,  1E-4,    -1E+2, 1E+3);
    gMinuit->DefineParameter(17, "k1l6",   0.,  1E-4,    -1E+2, 1E+3);
    gMinuit->DefineParameter(18, "k2l6",   0.,  1E-4,    -1E+2, 1E+3);
    gMinuit->DefineParameter(19, "k3l6",   0.,  1E-4,    -1E+2, 1E+3);
    gMinuit->DefineParameter(20, "ord0",8000.,  1E-2,       10, 1E+6);
    
    // Calculte normalize factor for 0th order
    for (int i = 1; i < 21; i++) {
        gMinuit->Command(TString::Format("SET PARM %d 0",i));
        gMinuit->Command(TString::Format("FIX %d",i));
    }
    gMinuit->Command("MINI");
    gMinuit->Command("MINI");
    gMinuit->Command("MINOS");
    gMinuit->Command("FIX 21");

    // Start processing 1st order
    for (int i = 1; i < 21; i++) {gMinuit->Command(TString::Format("REL %d",i));}
    if (iBin == 0) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
        
        gMinuit->Command("FIX 13");
        gMinuit->Command("FIX 14");
        gMinuit->Command("FIX 15");
        gMinuit->Command("FIX 16");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else if (iBin == 1) {
        gMinuit->Command("SET PARM 9 0");
        gMinuit->Command("SET PARM 10 0");
        gMinuit->Command("SET PARM 11 0");
        gMinuit->Command("SET PARM 12 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 9");
        gMinuit->Command("FIX 10");
        gMinuit->Command("FIX 11");
        gMinuit->Command("FIX 12");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else if (iBin > 1 && iBin < 6 ) {
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }else{
        gMinuit->Command("SET PARM 13 0");
        gMinuit->Command("SET PARM 14 0");
        gMinuit->Command("SET PARM 15 0");
        gMinuit->Command("SET PARM 16 0");
        gMinuit->Command("SET PARM 17 0");
        gMinuit->Command("SET PARM 18 0");
        gMinuit->Command("SET PARM 19 0");
        gMinuit->Command("SET PARM 20 0");
        gMinuit->Command("FIX 13");
        gMinuit->Command("FIX 14");
        gMinuit->Command("FIX 15");
        gMinuit->Command("FIX 16");
        gMinuit->Command("FIX 17");
        gMinuit->Command("FIX 18");
        gMinuit->Command("FIX 19");
        gMinuit->Command("FIX 20");
    }
    
    gMinuit->Command("MINI");
    gMinuit->Command("MINI");
    gMinuit->Command("IMPROVE");
    gMinuit->Command("MINOS");

    double arrPar[nPar];
    double arrParErr[nPar];
    for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
    for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);
    
    // Draw and write config
    TCanvas canvas("canvas");
    TLatex *latex = new TLatex();
    latex->SetNDC();
    double chi2Val=0;
    fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
    printf("Chi2(Bin center)=%f \n",chi2Val);
        
    h_effL.Draw();
    if (iBin == 0) h_effL.SetMaximum(0.0003);
    canvas.Update();
    canvas.Print(TString::Format("./plots/accXrecoEff2_cosl_order0_bin%d.pdf",iBin));
    h_effK.Draw();
    canvas.Update();
    canvas.Print(TString::Format("./plots/accXrecoEff2_cosk_order0_bin%d.pdf",iBin));
        
    if (true){
        //// Draw 1-D
        h_naccL.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_naccL_bin%d.pdf",iBin));
        h_naccK.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_naccK_bin%d.pdf",iBin));

        h_nrecoL.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_nrecoL_bin%d.pdf",iBin));
        h_nrecoK.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_nrecoK_bin%d.pdf",iBin));
        
        h_recL.SetStats(0);
        h_recL.SetMinimum(0.);
        h_recL.SetXTitle("CosThetaL");
        h_recL.Draw("TEXT");
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_recoEffL_bin%d.pdf",iBin));
        
        h_recK.SetStats(0);
        h_recK.SetMinimum(0.);
        h_recK.SetXTitle("CosThetaK");
        h_recK.Draw("TEXT");
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_recoEffK_bin%d.pdf",iBin));
        
        TH1F h_theoK("h_theoK" ,"h_theoK" ,nKBins,-1,1); 
        h_theoK.SetStats(0);
        h_theoK.SetMinimum(0.);
        h_theoK.SetXTitle("CosThetaK");
        h_theoK.SetYTitle("#Events / 0.2");
        for (int kBin = 1; kBin <= nKBins; kBin++) {
            h_theoK.SetBinContent(kBin,h_ngenK->GetBinContent(kBin)*h_effK.GetBinContent(kBin));
            if (h_effK.GetBinContent(kBin) != 0){
                h_theoK.SetBinError(kBin,h_theoK.GetBinContent(kBin)*h_effK.GetBinError(kBin)/h_effK.GetBinContent(kBin));
            }else{
                h_theoK.SetBinError(kBin,sqrt(h_theoK.GetBinContent(kBin)));
            }
        }
        h_theoK.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_theoK_bin%d.pdf",iBin));
        
        TH1F h_theoL("h_theoL" ,"h_theoL" ,nLBins,-1,1); 
        h_theoL.SetStats(0);
        h_theoL.SetMinimum(0.);
        h_theoL.SetXTitle("CosThetaL");
        h_theoL.SetYTitle("#Events / 0.2");
        for (int lBin = 1; lBin <= nLBins; lBin++) {
            h_theoL.SetBinContent(lBin,h_ngenL->GetBinContent(lBin)*h_effL.GetBinContent(lBin));
            if (h_effL.GetBinContent(lBin) != 0){
                h_theoL.SetBinError(lBin,h_theoL.GetBinContent(lBin)*h_effL.GetBinError(lBin)/h_effL.GetBinContent(lBin));
            }else{
                h_theoL.SetBinError(lBin,sqrt(h_theoL.GetBinContent(lBin)));
            }
        }
        h_theoL.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_theoL_bin%d.pdf",iBin));
        
        //// Draw 2-D
        h2_eff.SetMinimum(0.);
        h2_eff.SetTitleOffset(2,"XY");
        h2_eff.SetXTitle("genCosThetaL");
        h2_eff.SetYTitle("genCosThetaK");
        h2_eff.SetStats(0);
        h2_eff.SetMaximum(effUpperBound);
        h2_eff.Draw("LEGO2");
        latex->DrawLatex(0.35,0.95,TString::Format("#varepsilon in Bin%d",iBin));
        
        // Draw FitResult
        f2_model.SetTitle("");
        f2_model.SetMaximum(effUpperBound);
        f2_model.SetLineWidth(1);
        f2_model.Draw("SURF SAME ");
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_2D_bin%d.pdf",iBin));

        //// Draw compare
        TH2F h2_compFit("h2_compFit","",6,thetaLBins,5,thetaKBins);
        h2_compFit.SetTitleOffset(2,"XY");
        h2_compFit.SetXTitle("genCosThetaL");
        h2_compFit.SetYTitle("genCosThetaK");
        TH2F h2_pullFit("h2_pullFit","",6,thetaLBins,5,thetaKBins);
        h2_pullFit.SetTitleOffset(1,"XY");
        h2_pullFit.SetXTitle("genCosThetaL");
        h2_pullFit.SetYTitle("genCosThetaK");
        for (int i = 1; i <= 6; i++) {//thetaL
            for (int j = 1; j <= 5; j++) {//thetaK
                if (h2_eff.GetBinContent(i,j) != 0){
                    h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_eff.GetXaxis()->GetBinCenter(i),h2_eff.GetYaxis()->GetBinCenter(j))/h2_eff.GetBinContent(i,j));
                    double _xlo = h2_eff.GetXaxis()->GetBinLowEdge(i);
                    double _xhi = h2_eff.GetXaxis()->GetBinUpEdge(i);
                    double _ylo = h2_eff.GetYaxis()->GetBinLowEdge(j);
                    double _yhi = h2_eff.GetYaxis()->GetBinUpEdge(j);
                    h2_pullFit.SetBinContent(i,j,(f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_eff.GetBinContent(i,j))/h2_eff.GetBinError(i,j));
                }else{
                    h2_compFit.SetBinContent(i,j,0.);
                    h2_pullFit.SetBinContent(i,j,0.);
                }
            }
        }
        h2_compFit.SetMinimum(0.);
        h2_compFit.SetStats(0);
        h2_compFit.Draw("LEGO2");
        latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
        latex->DrawLatex(0.3,0.95,TString::Format("#varepsilon_{fit} / #varepsilon_{measured} in Bin%d",iBin));
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_compFit_2D_bin%d.pdf",iBin));
        
        h2_pullFit.SetStats(0);
        h2_pullFit.Draw("COLZ TEXT");
        latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
        latex->DrawLatex(0.3,0.95,TString::Format("(#varepsilon_{fit} - #varepsilon_{measured})/Error in Bin%d",iBin));
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_pullFit_2D_bin%d.pdf",iBin));

        // Draw significance of deviation
        TH1F h_pull("Deviation/Error","",15,-3.,3.);
        h_pull.SetXTitle("Significance of deviation");
        for (int i = 1; i <= 6; i++) {//thetaL
            for (int j = 1; j <= 5; j++) {//thetaK
                double _xlo = h2_eff.GetXaxis()->GetBinLowEdge(i);
                double _xhi = h2_eff.GetXaxis()->GetBinUpEdge(i);
                double _ylo = h2_eff.GetYaxis()->GetBinLowEdge(j);
                double _yhi = h2_eff.GetYaxis()->GetBinUpEdge(j);
                if (h2_eff.GetBinContent(i,j) != 0){
                    h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_eff.GetBinContent(i,j))/h2_eff.GetBinError(i,j));
                }
            }
        }
        h_pull.Draw();
        canvas.Update();
        canvas.Print(TString::Format("./plots/accXrecoEff2_sigma_bin%d.pdf",iBin));

        delete latex;
        
    }
    
    //prepare output
    string output;
    output = TString::Format("%e*(%s)",arrPar[20],f2_model_format_ord0.Data());
    while(output.find("*y") !=std::string::npos ){
        output.replace(output.find("*y"), 2, "*CosThetaK");
    }
    while(output.find("*x") !=std::string::npos ){
        output.replace(output.find("*x"), 2, "*CosThetaL");
    }
    while(output.find("(x") !=std::string::npos ){
        output.replace(output.find("(x"), 2, "(CosThetaL");
    }
    printf("\"%s\",\n",output.c_str());
    writeParam(iBin,"f_accXrecoEff_ord0",output);
    writeParam(iBin,"accXrecoEff2",arrPar,20,true);
    writeParam(iBin,"accXrecoEff2Err",arrParErr,20,true);

    delete gMinuit;// delete before return.
    return output.c_str();
}//}}}

//_________________________________________________________________________________
void angular2D_bin(int iBin, const char outfile[] = "angular2D")
{//{{{
    // Remark: You must use RooFit!! It's better in unbinned fit.
    //         Extended ML fit is adopted by Mauro, just follow!!

    // Read data
    ch->SetBranchStatus("*",0);
    ch->SetBranchStatus("Bmass"         , 1);
    ch->SetBranchStatus("Mumumass"      , 1);
    ch->SetBranchStatus("Mumumasserr"   , 1);
    ch->SetBranchStatus("CosTheta*"     , 1);
    ch->SetBranchStatus("Q2"            , 1);
    RooRealVar Bmass("Bmass","M_{K^{*}#Mu#Mu}",5.,5.56);
    RooRealVar CosThetaK("CosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar CosThetaL("CosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
    RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
    RooRealVar Q2("Q2","q^{2}",0.5,20.);
    RooRealVar fl("fl", "F_{L}", genFl[iBin], 0., 1.);
    RooRealVar afb("afb", "A_{FB}", genAfb[iBin], -1., 1.);
    RooRealVar fs("fs","F_{S}",0.0129254,-0.3,0.3);
    RooRealVar as("as","A_{S}",-0.0975919,-0.3,0.3);

    if (iBin != 3 && iBin != 5){
        // 2011 cross check
        fs.setVal(0.0129254);
        fs.setAsymError(-0.00898344,0.0101371);
        as.setVal(-0.0975919);
        as.setAsymError(-0.00490805,0.0049092);
        
        //fs.setVal(0.);
        //as.setVal(0.);
        //fs.setConstant(kTRUE);
        //as.setConstant(kTRUE);

        // read parameter from datacard
        //fs.setVal(readParam(3,"fs",0));
        //fs.setAsymError(readParam(3,"fs",1),readParam(3,"fs",2));
        //as.setVal(readParam(3,"as",0));
        //fs.setAsymError(readParam(3,"as",1),readParam(3,"as",2));
    }
        // Efficiency
    RooRealVar recK0L0("recK0L0","recK0L0",readParam(iBin,"accXrecoEff2", 0));
    RooRealVar recK1L0("recK1L0","recK1L0",readParam(iBin,"accXrecoEff2", 1));
    RooRealVar recK2L0("recK2L0","recK2L0",readParam(iBin,"accXrecoEff2", 2));
    RooRealVar recK3L0("recK3L0","recK3L0",readParam(iBin,"accXrecoEff2", 3));
    RooRealVar recK0L2("recK0L2","recK0L2",readParam(iBin,"accXrecoEff2", 4));
    RooRealVar recK1L2("recK1L2","recK1L2",readParam(iBin,"accXrecoEff2", 5));
    RooRealVar recK2L2("recK2L2","recK2L2",readParam(iBin,"accXrecoEff2", 6));
    RooRealVar recK3L2("recK3L2","recK3L2",readParam(iBin,"accXrecoEff2", 7));
    RooRealVar recK0L3("recK0L3","recK0L3",readParam(iBin,"accXrecoEff2", 8));
    RooRealVar recK1L3("recK1L3","recK1L3",readParam(iBin,"accXrecoEff2", 9));
    RooRealVar recK2L3("recK2L3","recK2L3",readParam(iBin,"accXrecoEff2",10));
    RooRealVar recK3L3("recK3L3","recK3L3",readParam(iBin,"accXrecoEff2",11));
    RooRealVar recK0L4("recK0L4","recK0L4",readParam(iBin,"accXrecoEff2",12));
    RooRealVar recK1L4("recK1L4","recK1L4",readParam(iBin,"accXrecoEff2",13));
    RooRealVar recK2L4("recK2L4","recK2L4",readParam(iBin,"accXrecoEff2",14));
    RooRealVar recK3L4("recK3L4","recK3L4",readParam(iBin,"accXrecoEff2",15));
    RooRealVar recK0L6("recK0L6","recK0L6",readParam(iBin,"accXrecoEff2",16));
    RooRealVar recK1L6("recK1L6","recK1L6",readParam(iBin,"accXrecoEff2",17));
    RooRealVar recK2L6("recK2L6","recK2L6",readParam(iBin,"accXrecoEff2",18));
    RooRealVar recK3L6("recK3L6","recK3L6",readParam(iBin,"accXrecoEff2",19));
    recK0L0.setError(readParam(iBin,"accXrecoEff2Err", 0));
    recK1L0.setError(readParam(iBin,"accXrecoEff2Err", 1));
    recK2L0.setError(readParam(iBin,"accXrecoEff2Err", 2));
    recK3L0.setError(readParam(iBin,"accXrecoEff2Err", 3));
    recK0L2.setError(readParam(iBin,"accXrecoEff2Err", 4));
    recK1L2.setError(readParam(iBin,"accXrecoEff2Err", 5));
    recK2L2.setError(readParam(iBin,"accXrecoEff2Err", 6));
    recK3L2.setError(readParam(iBin,"accXrecoEff2Err", 7));
    recK0L3.setError(readParam(iBin,"accXrecoEff2Err", 8));
    recK1L3.setError(readParam(iBin,"accXrecoEff2Err", 9));
    recK2L3.setError(readParam(iBin,"accXrecoEff2Err",10));
    recK3L3.setError(readParam(iBin,"accXrecoEff2Err",11));
    recK0L4.setError(readParam(iBin,"accXrecoEff2Err",12));
    recK1L4.setError(readParam(iBin,"accXrecoEff2Err",13));
    recK2L4.setError(readParam(iBin,"accXrecoEff2Err",14));
    recK3L4.setError(readParam(iBin,"accXrecoEff2Err",15));
    recK0L6.setError(readParam(iBin,"accXrecoEff2Err",16));
    recK1L6.setError(readParam(iBin,"accXrecoEff2Err",17));
    recK2L6.setError(readParam(iBin,"accXrecoEff2Err",18));
    recK3L6.setError(readParam(iBin,"accXrecoEff2Err",19));
    RooArgSet f_sigA_argset(CosThetaL,CosThetaK);
    f_sigA_argset.add(RooArgSet(fl,afb,fs,as));
    f_sigA_argset.add(RooArgSet(recK0L0,recK1L0,recK2L0,recK3L0));
    f_sigA_argset.add(RooArgSet(recK0L2,recK1L2,recK2L2,recK3L2));
    TString f_sigA_format;
    TString f_ang_format = "9/16*((2/3*fs+4/3*as*CosThetaK)*(1-CosThetaL**2)+(1-fs)*(2*fl*CosThetaK**2*(1-CosThetaL**2)+1/2*(1-fl)*(1-CosThetaK**2)*(1+CosThetaL**2)+4/3*afb*(1-CosThetaK**2)*CosThetaL))";
    TString f_rec_ord0 = readParam(iBin,"f_accXrecoEff_ord0",f_accXrecoEff_ord0[iBin]);
    TString f_rec_format, f_rec_L0, f_rec_L2, f_rec_L3, f_rec_L4, f_rec_L6;
    f_rec_L0 = "(recK0L0+recK1L0*CosThetaK+recK2L0*(3*CosThetaK**2-1)/2+recK3L0*(5*CosThetaK**3-3*CosThetaK)/2)";
    f_rec_L2 = "(recK0L2+recK1L2*CosThetaK+recK2L2*(3*CosThetaK**2-1)/2+recK3L2*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**2";
    f_rec_L3 = "(recK0L3+recK1L3*CosThetaK+recK2L3*(3*CosThetaK**2-1)/2+recK3L3*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**3";
    f_rec_L4 = "(recK0L4+recK1L4*CosThetaK+recK2L4*(3*CosThetaK**2-1)/2+recK3L4*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**4";
    f_rec_L6 = "(recK0L6+recK1L6*CosThetaK+recK2L6*(3*CosThetaK**2-1)/2+recK3L6*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**6";

    if (iBin == 0) {
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_argset.add(RooArgSet(recK0L6,recK1L6,recK2L6,recK3L6));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L4.Data(),f_rec_L6.Data(),f_ang_format.Data());
    }else if (iBin == 1) {
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L4.Data(),f_ang_format.Data());
    }else if (iBin > 1 && iBin < 6) {
        f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L3.Data(),f_rec_L4.Data(),f_ang_format.Data());
    }else{
        f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L3.Data(),f_ang_format.Data());
    }
        // angular map of signal
    RooGenericPdf f_sigA("f_sigA", f_sigA_format, f_sigA_argset);
    RooRealVar nsig("nsig","nsig",1E4,1E2,1E8);
    RooExtendPdf f_ext("f_ext","f_ext",f_sigA,nsig);
    
    // Get data and apply unbinned fit
    //fs.setConstant(kTRUE); as.setConstant(kTRUE);
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, Bmass, Mumumass, Mumumasserr, CosThetaK,CosThetaL),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[1]),0);
    f_ext.fitTo(*data,Minimizer("Minuit2"));
    RooFitResult *f_fitresult = f_ext.fitTo(*data,Extended(kTRUE),Save(kTRUE),Minimizer("Minuit2"),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    TLatex *t1 = new TLatex();
    t1->SetNDC();
    
    RooPlot* framecosk = CosThetaK.frame(); 
    data->plotOn(framecosk,Binning(20)); 
    f_ext.plotOn(framecosk); 
    if (true) { // 
        double buffFl = fl.getVal();
        double buffAfb = afb.getVal();
        fl.setVal(genFl[iBin]); afb.setVal(genAfb[iBin]);
        f_ext.plotOn(framecosk, LineColor(2),LineWidth(2),LineStyle(2)); 
        fl.setVal(buffFl); afb.setVal(buffAfb);
    }
    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();
    
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    if (iBin > 3) fixNDC = -0.5;
    t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L}=%5.3f#pm%8.6f",fl.getVal(),fl.getError()));
    t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{FB}=%5.3f#pm%8.6f",afb.getVal(),afb.getError()));
    c->Print(TString::Format("./plots/%s_cosk_bin%d.pdf",outfile,iBin));

    // Draw projection to CosThetaL
    RooPlot* framecosl = CosThetaL.frame(); 
    data->plotOn(framecosl,Binning(20)); 
    f_ext.plotOn(framecosl); 
    if (true) {
        double buffFl = fl.getVal();
        double buffAfb = afb.getVal();
        fl.setVal(genFl[iBin]); afb.setVal(genAfb[iBin]);
        f_ext.plotOn(framecosl, LineColor(2),LineWidth(2),LineStyle(2)); 
        fl.setVal(buffFl); afb.setVal(buffAfb);
    }
    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();

    fixNDC = 0.;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    c->Update();
    c->Print(TString::Format("./plots/%s_cosl_bin%d.pdf",outfile,iBin));

    // Make 2-D plot
    TH1 *h1 = data->createHistogram("CosThetaL,CosThetaK", 6, 5);
    h1->SetXTitle("CosThetaL");
    h1->SetYTitle("CosThetaK");
    h1->Draw("LEGO2");
    c->Update();
    c->Print(TString::Format("./plots/%s_bin%d.pdf",outfile,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    //write output
    double output[4] = {0,0};
    output[0] = fl.getVal();
    output[1] = fl.getError();
    writeParam(iBin,"fl",output);
    output[0] = afb.getVal();
    output[1] = afb.getError();
    writeParam(iBin,"afb",output);
    return;
}//}}}

void angular2D(const char outfile[] = "angular2D", bool doFit=false) // 2D check for signal MC at RECO level
{//{{{

    double x[8]={1.5,3.15,6.49,9.385,11.475,13.52,15.09,17.5};
    double xerr[8]={0.5,1.15,2.09,0.705,1.385,0.66,0.91,1.5};
    double yafb[8],yerrafb[8],yfl[8],yerrfl[8];

    if (doFit){
        angular2D_bin(3);
        angular2D_bin(5);
        angular2D_bin(0);
        angular2D_bin(1);
        angular2D_bin(2);
        angular2D_bin(4);
        angular2D_bin(6);
        angular2D_bin(7);
    }

    // Checkout input data
    for(int ibin = 0; ibin < 8; ibin++){
        yfl[ibin]       = readParam(ibin,"fl",0);
        yerrfl[ibin]    = readParam(ibin,"fl",1);
        yafb[ibin]      = readParam(ibin,"afb",0);
        yerrafb[ibin]   = readParam(ibin,"afb",1);
        printf("yafb[%d]=%6.4f +- %6.4f\n",ibin,yafb[ibin],yerrafb[ibin]);
        printf("yfl [%d]=%6.4f +- %6.4f\n",ibin,yfl[ibin],yerrfl[ibin]);
    }
    
    // Draw
    TCanvas *c = new TCanvas("c");
    TH2F *frame = new TH2F("frame","",18,1,19,10,-1,1);
    frame->SetStats(kFALSE);

    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("F_{L}");
    frame->SetAxisRange(0,1,"Y");
    frame->Draw();
    TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(8,x,yfl,xerr,xerr,yerrfl,yerrfl);
    g_fl->SetFillColor(2);
    g_fl->SetFillStyle(3001);
    g_fl->Draw("P2");
    TGraphAsymmErrors *gen_fl  = new TGraphAsymmErrors(8,x,genFl,xerr,xerr,genFlerr,genFlerr);
    gen_fl->SetMarkerStyle(2);
    gen_fl->SetFillColor(4);
    gen_fl->SetFillStyle(3001);
    gen_fl->Draw("P2 same");
    c->Print(TString::Format("./plots/%s_fl.pdf",outfile));
    c->Clear();

    frame->SetTitle("");
    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("A_{FB}");
    frame->SetAxisRange(-1,1,"Y");
    frame->Draw();
    TGraphAsymmErrors *g_afb = new TGraphAsymmErrors(8,x,yafb,xerr,xerr,yerrafb,yerrafb);
    g_afb->SetFillColor(2);
    g_afb->SetFillStyle(3001);
    g_afb->Draw("P2");
    TGraphAsymmErrors *gen_afb = new TGraphAsymmErrors(8,x,genAfb,xerr,xerr,genAfberr,genAfberr);
    gen_afb->SetMarkerStyle(2);
    gen_afb->SetFillColor(4);
    gen_afb->SetFillStyle(3001);
    gen_afb->Draw("P2 same");
    c->Print(TString::Format("./plots/%s_afb.pdf",outfile));
}//}}}
//_________________________________________________________________________________

void angular3D_1a_Sm(int iBin, const char outfile[] = "angular3D_1a_Sm", bool keepParam = false)
{//{{{
    // Fit to signal simulation by YsSm+YcCm to determine Sm
    RooRealVar Bmass("Bmass","M_{K^{*}#Mu#Mu}",5.,5.56);
    RooRealVar Q2("Q2","q^{2}",0.5,20.);
    RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
    RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
    
    // Create parameters and PDFs
        // Signal double gaussian
    RooRealVar sigGauss_mean("sigGauss_mean","M_{K*#Mu#Mu}",5.28,5.25,5.30);
    RooRealVar sigGauss1_sigma("sigGauss1_sigma","#sigma_{1}",readParam(iBin,"sigGauss1_sigma",0,0.02),.01,.05);
    RooRealVar sigGauss2_sigma("sigGauss2_sigma","#sigma_{2}",readParam(iBin,"sigGauss2_sigma",0,0.08),.05,.40);
    RooRealVar sigM_frac("sigM_frac","sigM_frac",readParam(iBin,"sigM_frac",0,0.5),0.,1.);
    
    // Create signal distribution
        // mass distro of signal
    RooGaussian f_sigMGauss1("f_sigMGauss1","f_sigMGauss1", Bmass, sigGauss_mean, sigGauss1_sigma);//double gaussian with shared mean
    RooGaussian f_sigMGauss2("f_sigMGauss2","f_sigMGauss2", Bmass, sigGauss_mean, sigGauss2_sigma);//double gaussian with shared mean
    RooAddPdf f_sigM("f_sigM","f_sigM", f_sigMGauss1, f_sigMGauss2, sigM_frac);
    
    RooRealVar nsig("nsig","nsig",5E4,2000,1E8);
    RooExtendPdf f("f","f",f_sigM,nsig);

    // Get data and apply unbinned fit
    int mumuMassWindowBin = 1+4*isCDFcut;
    if (iBin==3 || iBin==5) mumuMassWindowBin = 2-2*isCDFcut;
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, Bmass, Mumumass, Mumumasserr),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[mumuMassWindowBin]),0);
    RooFitResult *f_fitresult = f.fitTo(*data,Extended(kTRUE),Save(kTRUE),Minimizer("Minuit"),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* frame = Bmass.frame(); 
    data->plotOn(frame,Binning(20)); 
    f.plotOn(frame); 
    f.plotOn(frame,Components(f_sigM),LineColor(2),LineWidth(2));

    frame->SetTitle("");
    frame->SetMinimum(0);
    frame->Draw();

    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    t1->DrawLatex(.13,.86,TString::Format("%.2f/fb",datasetLumi[1]));
    t1->DrawLatex(.13,.78,TString::Format("nsig=%5.1f#pm%5.1f",nsig.getVal(),nsig.getError()));
    c->Print(TString::Format("./plots/%s_bin%d.pdf",outfile,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    // Prepare datacard
    double val[3]={0,0,0};
    writeParam(iBin, "iBin", new double((double)iBin), 1, keepParam);
    if (is7TeVCheck){
        writeParam(iBin, "mode", new double(2011), 1, keepParam);
    }else{
        writeParam(iBin, "mode", new double(2012), 1, keepParam);
    }
    val[0]=sigGauss1_sigma.getVal();val[1]=sigGauss1_sigma.getError();
    writeParam(iBin, "sigGauss1_sigma", val, 2 , keepParam);
    val[0]=sigGauss2_sigma.getVal();val[1]=sigGauss2_sigma.getError();
    writeParam(iBin, "sigGauss2_sigma", val, 2 , keepParam);
    val[0]=sigM_frac.getVal();val[1]=sigM_frac.getError();
    writeParam(iBin, "sigM_frac", val, 2 , keepParam);
}//}}}
void angular3D_1b_YpPm(int iBin, const char outfile[] = "angular3D_1b_YpPm", bool keepParam = false)
{//{{{
    static char decmode[10];
    while(strcmp(decmode,"jpsi")*strcmp(decmode, "psi2s") != 0){
        printf("Please insert background type [ jpsi / psi2s ]:");
        scanf("%19s",decmode);
    }
    printf("Processing Bin#%d, decmode=%s\n",iBin,decmode);

    //if (iBin ==0 || iBin%2 == 1){//0,1,3,5,7
    if (iBin < 2 || iBin == 7){//0,1,7
        double val[3]={1,0,0};
        writeParam(iBin , TString::Format("bkg%sGauss1_mean1" ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss1_mean2" ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss1_sigma1",decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss1_sigma2",decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sM_frac1"      ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss2_mean1" ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss2_mean2" ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss2_sigma1",decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sGauss2_sigma2",decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sM_frac2"      ,decmode) , val , 2 , keepParam);
        writeParam(iBin , TString::Format("bkg%sM_frac12"     ,decmode) , val , 2 , keepParam);
        val[0]=0;val[1]=0.1;
        writeParam(iBin , TString::Format("nbkg%sPeak"        ,decmode) , val , 2 , keepParam);
        return;
    }

    // Fit to control channel simulations by YpPm to determine Yp,Pm.
    RooRealVar Bmass("Bmass","M_{K^{*}#Mu#Mu}",5.,5.56);
    RooRealVar Q2("Q2","q^{2}",0.5,20.);
    RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
    RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
    
    // Create peak background distribution
    RooRealVar bkgGauss1_mean1("bkgGauss1_mean1"   , "M_{K*#Mu#Mu}" , readParam(iBin , TString::Format("bkg%sGauss1_mean1" ,decmode) , 0 , 5.05) , 5.00 , 5.10);
    RooRealVar bkgGauss1_mean2("bkgGauss1_mean2"   , "M_{K*#Mu#Mu}" , readParam(iBin , TString::Format("bkg%sGauss1_mean2" ,decmode) , 0 , 5.15) , 5.10 , 5.30);
    RooRealVar bkgGauss1_mean3("bkgGauss1_mean3"   , "M_{K*#Mu#Mu}" , readParam(iBin , TString::Format("bkg%sGauss1_mean3" ,decmode) , 0 , 5.10) , 4.70 , 5.30);
    RooRealVar bkgGauss1_sigma1("bkgGauss1_sigma1" , "#sigma_{11}"  , readParam(iBin , TString::Format("bkg%sGauss1_sigma1",decmode) , 0 , .03)  , .01  , .80);
    RooRealVar bkgGauss1_sigma2("bkgGauss1_sigma2" , "#sigma_{12}"  , readParam(iBin , TString::Format("bkg%sGauss1_sigma2",decmode) , 0 , .12)  , .02  , 1.2);
    RooRealVar bkgGauss1_sigma3("bkgGauss1_sigma3" , "#sigma_{13}"  , readParam(iBin , TString::Format("bkg%sGauss1_sigma3",decmode) , 0 , .12)  , .02  , 1.2);
    RooRealVar bkgM_frac1("bkgM_frac1"             , "bkgM_frac1"   , readParam(iBin , TString::Format("bkg%sM_frac1"      ,decmode) , 0 , 1.)   , 0.   , 1.);
    RooRealVar bkgM_fracC1("bkgM_fracC1"           , "bkgM_fracC1"  , 0.   , 0.   , 1.);
    RooRealVar bkgGauss2_mean1("bkgGauss2_mean1"   , "M_{K*#Mu#Mu}" , readParam(iBin , TString::Format("bkg%sGauss2_mean1" ,decmode) , 0 , 5.40) , 5.35 , 5.60);
    RooRealVar bkgGauss2_mean2("bkgGauss2_mean2"   , "M_{K*#Mu#Mu}" , readParam(iBin , TString::Format("bkg%sGauss2_mean2" ,decmode) , 0 , 5.40) , 5.35 , 5.60);
    RooRealVar bkgGauss2_sigma1("bkgGauss2_sigma1" , "#sigma_{21}"  , readParam(iBin , TString::Format("bkg%sGauss2_sigma1",decmode) , 0 , .03)  , .01  , .20);
    RooRealVar bkgGauss2_sigma2("bkgGauss2_sigma2" , "#sigma_{22}"  , readParam(iBin , TString::Format("bkg%sGauss2_sigma2",decmode) , 0 , .12)  , .10  , 1.2);
    RooRealVar bkgM_frac2("bkgM_frac2"             , "bkgM_frac2"   , readParam(iBin , TString::Format("bkg%sM_frac2"      ,decmode) , 0 , 1.)   , 0.   , 1.);
    RooRealVar bkgM_frac12("bkgM_frac12"           , "bkgM_frac12"  , readParam(iBin , TString::Format("bkg%sM_frac12"     ,decmode) , 0 , 1.)   , 0.   , 1.);
    RooGaussian f_bkgPeakMGauss11("f_bkgPeakMGauss11","f_bkgPeakMGauss11", Bmass, bkgGauss1_mean1, bkgGauss1_sigma1);
    RooGaussian f_bkgPeakMGauss12("f_bkgPeakMGauss12","f_bkgPeakMGauss12", Bmass, bkgGauss1_mean2, bkgGauss1_sigma2);
    RooGaussian f_bkgPeakMGauss13("f_bkgPeakMGauss13","f_bkgPeakMGauss13", Bmass, bkgGauss1_mean3, bkgGauss1_sigma3);
    RooGaussian f_bkgPeakMGauss21("f_bkgPeakMGauss21","f_bkgPeakMGauss21", Bmass, bkgGauss2_mean1, bkgGauss2_sigma1);
    RooGaussian f_bkgPeakMGauss22("f_bkgPeakMGauss22","f_bkgPeakMGauss22", Bmass, bkgGauss2_mean2, bkgGauss2_sigma2);
    RooAddPdf f_bkgPeakM1("f_bkgPeakM1", "f_bkgPeakM1", RooArgList(f_bkgPeakMGauss11, f_bkgPeakMGauss13, f_bkgPeakMGauss12),RooArgList(bkgM_frac1,bkgM_fracC1));
    RooAddPdf f_bkgPeakM2("f_bkgPeakM2"   , "f_bkgPeakM2"  , RooArgList(f_bkgPeakMGauss21 , f_bkgPeakMGauss22) , bkgM_frac2);
    RooAddPdf f_bkgPeakM12("f_bkgPeakM12" , "f_bkgPeakM12" , RooArgList(f_bkgPeakM1       , f_bkgPeakM2)       , bkgM_frac12);
    
    RooRealVar nbkgPeak("nbkgPeak","nbkgPeak",2E1,1,1E7);
    RooExtendPdf *f = 0;
    switch (iBin) {
        case 2:
            bkgM_frac12.setVal(1.);
            bkgM_frac12.setConstant(kTRUE);
            f = new RooExtendPdf("f","f",f_bkgPeakM1,nbkgPeak); // triple Gaussian at low
            break;
        case 4:
            bkgM_fracC1.setVal(0.);
            bkgM_fracC1.setConstant(kTRUE);
            f = new RooExtendPdf("f","f",f_bkgPeakM12,nbkgPeak); // two double Gaussian
            break;
        case 6:
            bkgM_frac12.setVal(0.);
            bkgM_frac12.setConstant(kTRUE);
            f = new RooExtendPdf("f","f",f_bkgPeakM2,nbkgPeak); // single Gaussian at high
            break;
        case 3:
        case 5:
            bkgM_frac12.setVal(1.);
            bkgM_fracC1.setVal(0.);
            bkgM_fracC1.setConstant(kTRUE);
            bkgGauss1_mean1.setRange(5.23,5.35);
            bkgGauss1_mean2.setRange(5.23,5.35);
            bkgGauss1_mean1.setVal(readParam(iBin , TString::Format("bkg%sGauss1_mean1" ,decmode),0,5.29));
            bkgGauss1_mean2.setVal(readParam(iBin , TString::Format("bkg%sGauss1_mean2" ,decmode),0,5.29));
            bkgGauss1_sigma1.setVal(readParam(iBin , TString::Format("bkg%sGauss1_sigma1",decmode),0,0.03));
            bkgGauss1_sigma2.setVal(readParam(iBin , TString::Format("bkg%sGauss1_sigma2",decmode),0,0.12));
            f = new RooExtendPdf("f","f",f_bkgPeakM1,nbkgPeak); // Should peak around B+
            break;
        default:
            break;
    }

    // Get data and apply unbinned fit
    int mumuMassWindowBin = 1+4*isCDFcut;
    if (iBin==3 || iBin==5) mumuMassWindowBin = 2-2*isCDFcut;// 0:none, 1:sig, 2:bkg
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, Bmass, Mumumass, Mumumasserr),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[mumuMassWindowBin]),0);
    RooFitResult *f_fitresult = f->fitTo(*data,Save(kTRUE),Minimizer("Minuit"),Extended(),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* frame = Bmass.frame(); 
    data->plotOn(frame,Binning(20)); 
    f->plotOn(frame); 

    frame->SetTitle("");
    frame->SetMinimum(0);
    frame->Draw();

    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    //t1->DrawLatex(.13,.78,TString::Format("nsig=%5.2f#pm%5.2f",nbkgPeak.getVal(),nbkgPeak.getError()));
    c->Print(TString::Format("./plots/%s_%s_bin%d.pdf",outfile,decmode,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    double val[3]={0,0,0};
    val[0] = bkgGauss1_mean1.getVal();val[1] = bkgGauss1_mean1.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_mean1"  , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss1_mean2.getVal();val[1] = bkgGauss1_mean2.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_mean2"  , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss1_mean3.getVal();val[1] = bkgGauss1_mean3.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_mean3"  , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss1_sigma1.getVal();val[1] = bkgGauss1_sigma1.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_sigma1" , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss1_sigma2.getVal();val[1] = bkgGauss1_sigma2.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_sigma2" , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss1_sigma3.getVal();val[1] = bkgGauss1_sigma3.getError();
    writeParam(iBin , TString::Format("bkg%sGauss1_sigma3" , decmode) , val , 2 , keepParam);
    val[0] = bkgM_frac1.getVal();val[1] = bkgM_frac1.getError();
    writeParam(iBin , TString::Format("bkg%sM_frac1"       , decmode) , val , 2 , keepParam);
    val[0] = bkgM_fracC1.getVal();val[1] = bkgM_fracC1.getError();
    writeParam(iBin , TString::Format("bkg%sM_fracC1"      , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss2_mean1.getVal();val[1] = bkgGauss2_mean1.getError();
    writeParam(iBin , TString::Format("bkg%sGauss2_mean1"  , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss2_mean2.getVal();val[1] = bkgGauss2_mean2.getError();
    writeParam(iBin , TString::Format("bkg%sGauss2_mean2"  , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss2_sigma1.getVal();val[1] = bkgGauss2_sigma1.getError();
    writeParam(iBin , TString::Format("bkg%sGauss2_sigma1" , decmode) , val , 2 , keepParam);
    val[0] = bkgGauss2_sigma2.getVal();val[1] = bkgGauss2_sigma2.getError();
    writeParam(iBin , TString::Format("bkg%sGauss2_sigma2" , decmode) , val , 2 , keepParam);
    val[0] = bkgM_frac2.getVal();val[1] = bkgM_frac2.getError();
    writeParam(iBin , TString::Format("bkg%sM_frac2"       , decmode) , val , 2 , keepParam);
    val[0] = bkgM_frac12.getVal();val[1] = bkgM_frac12.getError();
    writeParam(iBin , TString::Format("bkg%sM_frac12"      , decmode) , val , 2 , keepParam);
    val[0] = nbkgPeak.getVal();val[1] = nbkgPeak.getError();
    writeParam(iBin, TString::Format("nbkg%sPeak",decmode), val , 2 , keepParam);
}//}}}
void angular3D_2a_PkPl(int iBin, const char outfile[] = "angular3D_2a_PkPl", bool keepParam = false)
{//{{{
    static char decmode[20];
    while(strcmp(decmode,"jpsi")*strcmp(decmode, "psi2s") != 0){
        printf("Please insert background type [ jpsi / psi2s ]:");
        scanf("%19s",decmode);
    }

    // Gaussian constraint on yields and mass is needed.
    //if (iBin ==0 || iBin%2 == 1){//0,1,3,5,7
    if (iBin < 2 || iBin == 7){//0,1,7
        // Pm is flat(and the yield is 0) for bins other than 2,4,6
        double val[3]={0,0,0};
        writeParam(iBin, TString::Format("bkg%sPeak_c1",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c2",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c3",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c4",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c5",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c6",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c7",decmode), val , 2 , keepParam);
        writeParam(iBin, TString::Format("bkg%sPeak_c8",decmode), val , 2 , keepParam);
        return;
    }

    RooRealVar Q2("Q2","q^{2}",0.5,20.);
    RooRealVar CosThetaK("CosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar CosThetaL("CosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,sqrt(20.));
    RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
    
    RooRealVar bkgPeak_c1("bkgPeak_c1","bkgPeak_c1",readParam(iBin,TString::Format("bkg%sPeak_c1",decmode),0),-100,100);
    RooRealVar bkgPeak_c2("bkgPeak_c2","bkgPeak_c2",readParam(iBin,TString::Format("bkg%sPeak_c2",decmode),0),-100,100);
    RooRealVar bkgPeak_c3("bkgPeak_c3","bkgPeak_c3",readParam(iBin,TString::Format("bkg%sPeak_c3",decmode),0),-100,100);
    RooRealVar bkgPeak_c4("bkgPeak_c4","bkgPeak_c4",readParam(iBin,TString::Format("bkg%sPeak_c4",decmode),0),-100,100);
    RooRealVar bkgPeak_c5("bkgPeak_c5","bkgPeak_c5",readParam(iBin,TString::Format("bkg%sPeak_c5",decmode),0),-100,100);
    RooRealVar bkgPeak_c6("bkgPeak_c6","bkgPeak_c6",readParam(iBin,TString::Format("bkg%sPeak_c6",decmode),0),-100,100);
    RooRealVar bkgPeak_c7("bkgPeak_c7","bkgPeak_c7",readParam(iBin,TString::Format("bkg%sPeak_c7",decmode),0),-100,100);
    RooRealVar bkgPeak_c8("bkgPeak_c8","bkgPeak_c8",readParam(iBin,TString::Format("bkg%sPeak_c8",decmode),0),-100,100);
    RooArgSet f_bkgPeakA_argset;
    f_bkgPeakA_argset.add(RooArgSet(CosThetaL));
    f_bkgPeakA_argset.add(RooArgSet(CosThetaK));
    f_bkgPeakA_argset.add(RooArgSet(bkgPeak_c1,bkgPeak_c2,bkgPeak_c3,bkgPeak_c4,bkgPeak_c5,bkgPeak_c6,bkgPeak_c7,bkgPeak_c8));
    TString f_bkgPeakK_format = "1+bkgPeak_c1*CosThetaL+bkgPeak_c2*(3*CosThetaL**2-1)/2+bkgPeak_c3*(5*CosThetaL**3-3*CosThetaL)/2+bkgPeak_c4*(35*CosThetaL**4-30*CosThetaL**2+3)/8";
    TString f_bkgPeakL_format = "1+bkgPeak_c5*CosThetaK+bkgPeak_c6*(3*CosThetaK**2-1)/2+bkgPeak_c7*(5*CosThetaK**3-3*CosThetaK)/2+bkgPeak_c8*(35*CosThetaK**4-30*CosThetaK**2+3)/8";
//    TString f_bkgPeakA_format = TString::Format("(%s)*(%s)",f_bkgPeakK_format.Data(),f_bkgPeakL_format.Data());

//    TString f_bkgPeakA_format = "(1+\
//                                  bkgPeak_c1*CosThetaK+\
//                                  bkgPeak_c2*(3*CosThetaK**2-1)/2+\
//                                  bkgPeak_c3*(5*CosThetaK**3-3*CosThetaK)/2+\
//                                  bkgPeak_c4*(35*CosThetaK**4-30*CosThetaK**2+\3)/8)*(1+\
//                                  bkgPeak_c5*CosThetaL+\
//                                  bkgPeak_c6*(3*CosThetaL**2-1)/2+\
//                                  bkgPeak_c7*(5*CosThetaL**3-3*CosThetaL)/2+\
//                                  bkgPeak_c8*(35*CosThetaL**4-30*CosThetaL**2+\3)/8)";

//    TString f_bkgPeakA_format = "(1+\
//                                  bkgPeak_c1*CosThetaK+\
//                                  bkgPeak_c2*(3*CosThetaK**2-1)/2)*(1+\
//                                  bkgPeak_c3*CosThetaL+\
//                                  bkgPeak_c4*(3*CosThetaL**2-1)/2)+\
//                                  bkgPeak_c5*CosThetaL*CosThetaK+\
//                                  bkgPeak_c6*CosThetaK*(3*CosThetaL**2-1)/2+\
//                                  bkgPeak_c7*CosThetaL*(3*CosThetaK**2-1)/2+\
//                                  bkgPeak_c8*(3*CosThetaK**2-1)/2*(3*CosThetaL**2-1)/2";//NotPos

//    TString f_bkgPeakA_format = "1+\
//                                 bkgPeak_c1*CosThetaK**4+\
//                                 bkgPeak_c2*CosThetaK**2+\
//                                 bkgPeak_c3*CosThetaL+\
//                                 bkgPeak_c4*CosThetaL**2+\
//                                 bkgPeak_c5*CosThetaL*CosThetaK+\
//                                 bkgPeak_c6*CosThetaK**2*CosThetaL+\
//                                 bkgPeak_c7*CosThetaK**4*CosThetaL**2+\
//                                 bkgPeak_c8*CosThetaL**2*CosThetaK**2";

//    TString f_bkgPeakA_format = "1+\
//                                bkgPeak_c1*CosThetaK+\
//                                bkgPeak_c2*CosThetaK**2+\
//                                bkgPeak_c3*CosThetaL**4+\
//                                bkgPeak_c4*CosThetaL**2+\
//                                bkgPeak_c5*CosThetaL**4*CosThetaK+\
//                                bkgPeak_c6*CosThetaK*CosThetaL**4+\
//                                bkgPeak_c7*CosThetaK*CosThetaL**2+\
//                                bkgPeak_c8*CosThetaL**2*CosThetaK**2";
                               
//    TString f_bkgPeakA_format = "1+\
//                                bkgPeak_c1*(1+CosThetaK)+\
//                                bkgPeak_c2*(1-CosThetaK**2)+\
//                                bkgPeak_c3*(1-CosThetaL**2)+\
//                                bkgPeak_c4*(1-CosThetaL**4)+\
//                                bkgPeak_c5*(1-CosThetaL**2)*(1+CosThetaK)+\
//                                bkgPeak_c6*(1-CosThetaL**4)*(1+CosThetaK)+\
//                                bkgPeak_c7*(1-CosThetaL**2)*(1-CosThetaK**2)+\
//                                bkgPeak_c8*(1-CosThetaL**4)*(1-CosThetaK**2)";
    
//    TString f_bkgPeakA_format = "(1+\
//                                bkgPeak_c1*CosThetaK+\
//                                bkgPeak_c2*CosThetaK**2+\
//                                bkgPeak_c3*CosThetaL**2+\
//                                bkgPeak_c4*CosThetaL**4+\
//                                bkgPeak_c5*CosThetaL**2*CosThetaK+\
//                                bkgPeak_c6*CosThetaL**4*CosThetaK+\
//                                bkgPeak_c7*CosThetaL**2*CosThetaK**2+\
//                                bkgPeak_c8*CosThetaL**4*CosThetaK**2)*exp(-0.5*(CosThetaK+CosThetaL)**4-0.8*(CosThetaK-CosThetaL)**4)";

    // Base function+correction
    TString f_bkgPeakA_format = "(1+\
                                bkgPeak_c1*CosThetaK+\
                                bkgPeak_c2*CosThetaK**2+\
                                bkgPeak_c3*CosThetaL**2+\
                                bkgPeak_c4*CosThetaL**4+\
                                bkgPeak_c5*CosThetaL**2*CosThetaK+\
                                bkgPeak_c6*CosThetaL**4*CosThetaK+\
                                bkgPeak_c7*CosThetaL**2*CosThetaK**2+\
                                bkgPeak_c8*CosThetaL**4*CosThetaK**2)*\
                                exp(-(CosThetaK+0.5*CosThetaL)**4-(CosThetaK-0.5*CosThetaL)**4+1.5*(CosThetaK+0.5*CosThetaL)**2+1.5*(CosThetaK-0.5*CosThetaL)**2)";

    switch (iBin) {
        case 2:
            break;
        case 4:
            break;
        case 6:
            bkgPeak_c5.setVal(0.);
            bkgPeak_c6.setVal(0.);
            bkgPeak_c7.setVal(0.);
            bkgPeak_c8.setVal(0.);
            bkgPeak_c5.setConstant(kTRUE);
            bkgPeak_c6.setConstant(kTRUE);
            bkgPeak_c7.setConstant(kTRUE);
            bkgPeak_c8.setConstant(kTRUE);
            break;
        default:
            break;
    }
    RooGenericPdf f_bkgPeakA("f_bkgPeakA", f_bkgPeakA_format, f_bkgPeakA_argset);
    RooRealVar nbkgPeak("nbkgPeak","nbkgPeak",readParam(iBin,TString::Format("nbkg%sPeak",decmode),0),0.1,1E4);
    RooExtendPdf f_bkgPeakA_ext("f_bkgPeakA_ext","f_bkgPeakA_ext",f_bkgPeakA, nbkgPeak);

    // Gaussian Constraint
//    RooGaussian gaus_nbkgPeak("gaus_nbkgPeak","gaus_nbkgPeak",nbkgPeak,RooConst(readParam(iBin,TString::Format("nbkg%sPeak",decmode),0)),RooConst(readParam(iBin, TString::Format("nbkg%sPeak",decmode),1)));
    
    // Get data
    int mumuMassWindowBin = 1+4*isCDFcut;
    if (iBin==3 || iBin==5) mumuMassWindowBin = 2-2*isCDFcut;
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, CosThetaK, CosThetaL ,Mumumass, Mumumasserr),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[mumuMassWindowBin]),0);
    RooFitResult *f_fitresult = f_bkgPeakA_ext.fitTo(*data,Save(kTRUE),Extended(kTRUE),Minimizer("Minuit"),Minos(kTRUE));
    
    // Draw CosThetaK
    TCanvas* c = new TCanvas("c");
    RooPlot* framecosk = CosThetaK.frame();
    data->plotOn(framecosk,Binning(20));
    f_bkgPeakA_ext.plotOn(framecosk);
    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();
    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    c->Print(TString::Format("./plots/%s_%s_cosk_bin%d.pdf",outfile,decmode,iBin));
    
    // Draw CosThetaL
    RooPlot* framecosl = CosThetaL.frame();
    data->plotOn(framecosl,Binning(20));
    f_bkgPeakA_ext.plotOn(framecosl);
    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    c->Update();
    c->Print(TString::Format("./plots/%s_%s_cosl_bin%d.pdf",outfile,decmode,iBin));
    
    // Make 2-D plot
    TH1 *h1 = data->createHistogram("CosThetaK,CosThetaL", 5, 6);
    h1->SetMinimum(0);
    h1->Draw("LEGO2");
    TH1 *h1_fit_fine = f_bkgPeakA_ext.createHistogram("CosThetaK,CosThetaL", 100, 120);
    h1_fit_fine->SetMinimum(0);
    h1_fit_fine->Draw("SURF SAME");
    c->Update();
    c->Print(TString::Format("./plots/%s_%s_bin%d.pdf",outfile,decmode,iBin));

    TH1 *h1_fit = f_bkgPeakA_ext.createHistogram("CosThetaK,CosThetaL", 5, 6);
    TH2F *h2_compFit = new TH2F("h2_compFit","Yields_{MC}/Yields_{Fit}",5,-1.,1.,6,-1.,1.);
    h2_compFit->SetXTitle("CosThetaK");
    h2_compFit->SetYTitle("CosThetaL");
    for (int i = 1; i <= h2_compFit->GetNbinsX(); i++) {
        for (int j = 1; j <= h2_compFit->GetNbinsY(); j++) {
            h2_compFit->SetBinContent(i,j,h1->GetBinContent(i,j)/h1_fit->GetBinContent(i,j)*h2_compFit->GetNbinsX()*h2_compFit->GetNbinsX()/4.);
        }
    }
    h2_compFit->SetStats(kFALSE);
    h2_compFit->SetMinimum(0.);
    h2_compFit->Draw("LEGO2");
    c->Update();
    c->Print(TString::Format("./plots/%s_%s_compFit_2D_bin%d.pdf",outfile,decmode,iBin));
    h2_compFit->Draw("COL TEXT");
    c->Update();
    c->Print(TString::Format("./plots/%s_%s_compFit_2D_TEXT_bin%d.pdf",outfile,decmode,iBin));
    
    
    // clear
    delete t1;
    delete c;
    delete data;
    
    double val[3] = {0,0,0};
    val[0] = bkgPeak_c1.getVal();val[1] = bkgPeak_c1.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c1", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c2.getVal();val[1] = bkgPeak_c2.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c2", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c3.getVal();val[1] = bkgPeak_c3.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c3", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c4.getVal();val[1] = bkgPeak_c4.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c4", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c5.getVal();val[1] = bkgPeak_c5.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c5", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c6.getVal();val[1] = bkgPeak_c6.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c6", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c7.getVal();val[1] = bkgPeak_c7.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c7", decmode), val , 2 , keepParam);
    val[0] = bkgPeak_c8.getVal();val[1] = bkgPeak_c8.getError();
    writeParam(iBin, TString::Format("bkg%sPeak_c8", decmode), val , 2 , keepParam);
}//}}}

void angular3D_prior(int iBin, const char outfile[] = "angular3D_prior", bool keepParam = false) // Under construction. Peakings should be considered.
{//{{{
    // Fit to signal simulation by YsSm+YcCm to determine Sm
    RooRealVar Bmass("Bmass","M_{K^{*}#Mu#Mu}",5.,5.56);
    RooRealVar CosThetaK("CosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar CosThetaL("CosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar Q2("Q2","q^{2}",0.5,20.);
    RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
    RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
    
    // Peaking background [JPsi]
    RooRealVar bkgjpsiPeakL_c1("bkgjpsiPeakL_c1","bkgjpsiPeakL_c1",readParam(iBin,"bkgjpsiPeakL_c1",0));
    RooRealVar bkgjpsiPeakL_c2("bkgjpsiPeakL_c2","bkgjpsiPeakL_c2",readParam(iBin,"bkgjpsiPeakL_c2",0));
    RooRealVar bkgjpsiPeakL_c3("bkgjpsiPeakL_c3","bkgjpsiPeakL_c3",readParam(iBin,"bkgjpsiPeakL_c3",0));
    RooRealVar bkgjpsiPeakL_c4("bkgjpsiPeakL_c4","bkgjpsiPeakL_c4",readParam(iBin,"bkgjpsiPeakL_c4",0));
    RooRealVar bkgjpsiPeakK_c1("bkgjpsiPeakK_c1","bkgjpsiPeakK_c1",readParam(iBin,"bkgjpsiPeakK_c1",0));
    RooRealVar bkgjpsiPeakK_c2("bkgjpsiPeakK_c2","bkgjpsiPeakK_c2",readParam(iBin,"bkgjpsiPeakK_c2",0));
    RooRealVar bkgjpsiPeakK_c3("bkgjpsiPeakK_c3","bkgjpsiPeakK_c3",readParam(iBin,"bkgjpsiPeakK_c3",0));
    RooRealVar bkgjpsiPeakK_c4("bkgjpsiPeakK_c4","bkgjpsiPeakK_c4",readParam(iBin,"bkgjpsiPeakK_c4",0));
    RooArgSet f_bkgjpsiPeakA_argset;
    f_bkgjpsiPeakA_argset.add(RooArgSet(CosThetaL,CosThetaK));
    f_bkgjpsiPeakA_argset.add(RooArgSet(bkgjpsiPeakL_c1,bkgjpsiPeakL_c2,bkgjpsiPeakL_c3,bkgjpsiPeakL_c4));
    f_bkgjpsiPeakA_argset.add(RooArgSet(bkgjpsiPeakK_c1,bkgjpsiPeakK_c2,bkgjpsiPeakK_c3,bkgjpsiPeakK_c4));
    TString f_bkgjpsiPeakA_format = "(1+bkgjpsiPeakK_c1*CosThetaK+bkgjpsiPeakK_c2*(3*CosThetaK**2-1)/2+bkgjpsiPeakK_c3*(5*CosThetaK**3-3*CosThetaK)/2+bkgjpsiPeakK_c4*(35*CosThetaK**4-30*CosThetaK**2+3)/8)*(1+bkgjpsiPeakL_c1*CosThetaL+bkgjpsiPeakL_c2*(3*CosThetaL**2-1)/2+bkgjpsiPeakL_c3*(5*CosThetaL**3-3*CosThetaL)/2+bkgjpsiPeakL_c4*(35*CosThetaL**4-30*CosThetaL**2+3)/8)";
    RooGenericPdf f_bkgjpsiPeakA("f_bkgjpsiPeakA", f_bkgjpsiPeakA_format, f_bkgjpsiPeakA_argset);
    RooRealVar nbkgjpsiPeak("nbkgjpsiPeak","nbkgjpsiPeak",readParam(iBin,"nbkgjpsiPeak",0),1.,1E4);
    nbkgjpsiPeak.setError(readParam(iBin,"nbkgjpsiPeak",1));
    RooExtendPdf f_bkgjpsiPeakA_ext("f_bkgjpsiPeakA_ext","f_bkgjpsiPeakA_ext",f_bkgjpsiPeakA, nbkgjpsiPeak);
    
    // Peaking background [Psi2S]
    RooRealVar bkgpsi2sPeakL_c1("bkgpsi2sPeakL_c1","bkgpsi2sPeakL_c1",readParam(iBin,"bkgpsi2sPeakL_c1",0));
    RooRealVar bkgpsi2sPeakL_c2("bkgpsi2sPeakL_c2","bkgpsi2sPeakL_c2",readParam(iBin,"bkgpsi2sPeakL_c2",0));
    RooRealVar bkgpsi2sPeakL_c3("bkgpsi2sPeakL_c3","bkgpsi2sPeakL_c3",readParam(iBin,"bkgpsi2sPeakL_c3",0));
    RooRealVar bkgpsi2sPeakL_c4("bkgpsi2sPeakL_c4","bkgpsi2sPeakL_c4",readParam(iBin,"bkgpsi2sPeakL_c4",0));
    RooRealVar bkgpsi2sPeakK_c1("bkgpsi2sPeakK_c1","bkgpsi2sPeakK_c1",readParam(iBin,"bkgpsi2sPeakK_c1",0));
    RooRealVar bkgpsi2sPeakK_c2("bkgpsi2sPeakK_c2","bkgpsi2sPeakK_c2",readParam(iBin,"bkgpsi2sPeakK_c2",0));
    RooRealVar bkgpsi2sPeakK_c3("bkgpsi2sPeakK_c3","bkgpsi2sPeakK_c3",readParam(iBin,"bkgpsi2sPeakK_c3",0));
    RooRealVar bkgpsi2sPeakK_c4("bkgpsi2sPeakK_c4","bkgpsi2sPeakK_c4",readParam(iBin,"bkgpsi2sPeakK_c4",0));
    RooArgSet f_bkgpsi2sPeakA_argset;
    f_bkgpsi2sPeakA_argset.add(RooArgSet(CosThetaL,CosThetaK));
    f_bkgpsi2sPeakA_argset.add(RooArgSet(bkgpsi2sPeakL_c1,bkgpsi2sPeakL_c2,bkgpsi2sPeakL_c3,bkgpsi2sPeakL_c4));
    f_bkgpsi2sPeakA_argset.add(RooArgSet(bkgpsi2sPeakK_c1,bkgpsi2sPeakK_c2,bkgpsi2sPeakK_c3,bkgpsi2sPeakK_c4));
    TString f_bkgpsi2sPeakA_format = "(1+bkgpsi2sPeakK_c1*CosThetaK+bkgpsi2sPeakK_c2*(3*CosThetaK**2-1)/2+bkgpsi2sPeakK_c3*(5*CosThetaK**3-3*CosThetaK)/2+bkgpsi2sPeakK_c4*(35*CosThetaK**4-30*CosThetaK**2+3)/8)*(1+bkgpsi2sPeakL_c1*CosThetaL+bkgpsi2sPeakL_c2*(3*CosThetaL**2-1)/2+bkgpsi2sPeakL_c3*(5*CosThetaL**3-3*CosThetaL)/2+bkgpsi2sPeakL_c4*(35*CosThetaL**4-30*CosThetaL**2+3)/8)";
    RooGenericPdf f_bkgpsi2sPeakA("f_bkgpsi2sPeakA", f_bkgpsi2sPeakA_format, f_bkgpsi2sPeakA_argset);
    RooRealVar nbkgpsi2sPeak("nbkgpsi2sPeak","nbkgpsi2sPeak",readParam(iBin,"nbkgpsi2sPeak",0),1.,1E4);
    nbkgpsi2sPeak.setError(readParam(iBin,"nbkgpsi2sPeak",1));
    RooExtendPdf f_bkgpsi2sPeakA_ext("f_bkgpsi2sPeakA_ext","f_bkgpsi2sPeakA_ext",f_bkgpsi2sPeakA, nbkgpsi2sPeak);
    
    // Create combinatorial background distribution
    RooRealVar bkgCombL_c1("bkgCombL_c1","c1",readParam(iBin,"bkgCombL_c1",0),-2.5,2.5);
    RooRealVar bkgCombL_c2("bkgCombL_c2","c2",readParam(iBin,"bkgCombL_c2",0),-2.5,2.5);
    RooRealVar bkgCombL_c3("bkgCombL_c3","c3",readParam(iBin,"bkgCombL_c3",0),-2.5,2.5);
    RooRealVar bkgCombL_c4("bkgCombL_c4","c4",readParam(iBin,"bkgCombL_c4",0),-2.5,2.5);
    RooArgSet f_bkgCombL_argset;
    switch (iBin) {
        case 7:
            f_bkgCombL_argset.add(RooArgSet(bkgCombL_c1));
            bkgCombL_c2.setVal(0.);
            bkgCombL_c3.setVal(0.);
            bkgCombL_c4.setVal(0.);
            bkgCombL_c2.setConstant(kTRUE);
            bkgCombL_c3.setConstant(kTRUE);
            bkgCombL_c4.setConstant(kTRUE);
            break;
        case 0:
        case 1:
        case 4:
            f_bkgCombL_argset.add(RooArgSet(bkgCombL_c1,bkgCombL_c2));
            bkgCombL_c3.setVal(0.);
            bkgCombL_c4.setVal(0.);
            bkgCombL_c3.setConstant(kTRUE);
            bkgCombL_c4.setConstant(kTRUE);
            break;
        case 2:
            f_bkgCombL_argset.add(RooArgSet(bkgCombL_c1,bkgCombL_c2,bkgCombL_c3));
            bkgCombL_c4.setVal(0.);
            bkgCombL_c4.setConstant(kTRUE);
            break;
        case 3:
        case 5:
            f_bkgCombL_argset.add(RooArgSet(bkgCombL_c1,bkgCombL_c2,bkgCombL_c3,bkgCombL_c4));
            break;
        default:
            bkgCombL_c1.setVal(0.);
            bkgCombL_c2.setVal(0.);
            bkgCombL_c3.setVal(0.);
            bkgCombL_c4.setVal(0.);
            bkgCombL_c1.setConstant(kTRUE);
            bkgCombL_c2.setConstant(kTRUE);
            bkgCombL_c3.setConstant(kTRUE);
            bkgCombL_c4.setConstant(kTRUE);
            break;
    }
    RooPolynomial f_bkgCombL("f_bkgCombL","f_bkgCombL",CosThetaL,f_bkgCombL_argset);
    RooRealVar bkgCombK_c1("bkgCombK_c1","c1",readParam(iBin,"bkgCombK_c1",0),-2.5,2.5);
    RooRealVar bkgCombK_c2("bkgCombK_c2","c2",readParam(iBin,"bkgCombK_c2",0),-2.5,2.5);
    RooRealVar bkgCombK_c3("bkgCombK_c3","c3",readParam(iBin,"bkgCombK_c3",0),-5,5);
    RooRealVar bkgCombK_c4("bkgCombK_c4","c4",readParam(iBin,"bkgCombK_c4",0),-5,5);
    RooArgSet f_bkgCombK_argset;
    switch (iBin) {
        case 2:
            f_bkgCombK_argset.add(RooArgSet(bkgCombK_c1));
            bkgCombK_c2.setVal(0.);
            bkgCombK_c3.setVal(0.);
            bkgCombK_c4.setVal(0.);
            bkgCombK_c2.setConstant(kTRUE);
            bkgCombK_c3.setConstant(kTRUE);
            bkgCombK_c4.setConstant(kTRUE);
            break;
        case 0:
        case 1:
            f_bkgCombK_argset.add(RooArgSet(bkgCombK_c1,bkgCombK_c2));
            bkgCombK_c3.setVal(0.);
            bkgCombK_c4.setVal(0.);
            bkgCombK_c3.setConstant(kTRUE);
            bkgCombK_c4.setConstant(kTRUE);
            break;
        case 3:
        case 4:
        case 5:
            f_bkgCombK_argset.add(RooArgSet(bkgCombK_c1,bkgCombK_c2,bkgCombK_c3,bkgCombK_c4));
            break;
        default:
            bkgCombK_c1.setVal(0.);
            bkgCombK_c2.setVal(0.);
            bkgCombK_c3.setVal(0.);
            bkgCombK_c4.setVal(0.);
            bkgCombK_c1.setConstant(kTRUE);
            bkgCombK_c2.setConstant(kTRUE);
            bkgCombK_c3.setConstant(kTRUE);
            bkgCombK_c4.setConstant(kTRUE);
            break;
    }
    RooPolynomial f_bkgCombK("f_bkgCombK","f_bkgCombK",CosThetaK,f_bkgCombK_argset);
    RooProdPdf f_bkgCombA("f_bkgCombA", "f_bckCombA",f_bkgCombK,f_bkgCombL);
    
    // Get data and apply unbinned fit
    int mumuMassWindowBin = 1+4*isCDFcut;
    if (iBin==3 || iBin==5) mumuMassWindowBin = 0;
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, Bmass, CosThetaK, CosThetaL, Mumumass, Mumumasserr),TString::Format("(%s) && (%s) && (Bmass > 5.38 || Bmass < 5.18)",q2range[iBin],mumuMassWindow[mumuMassWindowBin]),0);
    RooFitResult *f_fitresult = f_bkgCombA.fitTo(*data,Save(kTRUE),Minimizer("Minuit"),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* framecosk = CosThetaK.frame(); 
    data->plotOn(framecosk,Binning(20)); 
    f_bkgCombA.plotOn(framecosk); 

    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();

    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    c->Print(TString::Format("./plots/%s_cosk_bin%d.pdf",outfile,iBin));
    
    // 
    RooPlot* framecosl = CosThetaL.frame(); 
    data->plotOn(framecosl,Binning(20)); 
    f_bkgCombA.plotOn(framecosl); 

    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();

    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    c->Update();
    c->Print(TString::Format("./plots/%s_cosl_bin%d.pdf",outfile,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    // Prepare datacard
    double val[3] = {0,0,0};
    val[0] = bkgCombL_c1.getVal();val[1] = bkgCombL_c1.getError();
    writeParam(iBin, "bkgCombL_c1", val , 2 , keepParam);
    val[0] = bkgCombL_c2.getVal();val[1] = bkgCombL_c2.getError();
    writeParam(iBin, "bkgCombL_c2", val , 2 , keepParam);
    val[0] = bkgCombL_c3.getVal();val[1] = bkgCombL_c3.getError();
    writeParam(iBin, "bkgCombL_c3", val , 2 , keepParam);
    val[0] = bkgCombL_c4.getVal();val[1] = bkgCombL_c4.getError();
    writeParam(iBin, "bkgCombL_c4", val , 2 , keepParam);
    val[0] = bkgCombK_c1.getVal();val[1] = bkgCombK_c1.getError();
    writeParam(iBin, "bkgCombK_c1", val , 2 , keepParam);
    val[0] = bkgCombK_c2.getVal();val[1] = bkgCombK_c2.getError();
    writeParam(iBin, "bkgCombK_c2", val , 2 , keepParam);
    val[0] = bkgCombK_c3.getVal();val[1] = bkgCombK_c3.getError();
    writeParam(iBin, "bkgCombK_c3", val , 2 , keepParam);
    val[0] = bkgCombK_c4.getVal();val[1] = bkgCombK_c4.getError();
    writeParam(iBin, "bkgCombK_c4", val , 2 , keepParam);
}//}}}

void angular3D_bin(int iBin, const char outfile[] = "angular3D")
{//{{{
    // Remark: You must use RooFit!! It's better in unbinned fit.
    //         Extended ML fit is adopted by Mauro, just follow!!
    //         This function is NOT designed to determine parameters in resonance region.
    if (iBin==3 || iBin==5) return;
    
    // Read data
    RooRealVar CosThetaK("CosThetaK"     , "cos#theta_{K}"       , -1. , 1.   ) ;
    RooRealVar CosThetaL("CosThetaL"     , "cos#theta_{L}"       , -1. , 1.   ) ;
    RooRealVar Bmass("Bmass"             , "M_{K^{*}#mu#mu}"     , 5.  , 5.56 ) ;
    RooRealVar Mumumass("Mumumass"       , "M^{#mu#mu}"          , 0.  , 10.  ) ;
    RooRealVar Mumumasserr("Mumumasserr" , "Error of M^{#mu#mu}" , 0.  , 10.  ) ;
    RooRealVar Q2("Q2"                   , "q^{2}"               , 0.5 , 20.  ) ;
    RooAddition Bmass_offset("Bmass_offset","Bmass_offset",RooArgSet(Bmass,RooConst(-5)));
    RooProduct Bmass_norm("Bmass_norm","Bmass_norm",RooArgSet(Bmass_offset,RooConst(1./0.56)));
        
    // Angular parameters
    RooRealVar afb("afb" , "A_{FB}" , genAfb[iBin] , -1. , 1.);
    RooRealVar fl("fl"   , "F_{L}"  , genFl[iBin]  , 0.  , 1.);
    RooRealVar fs("fs"   , "F_{S}"  , 0 , 0.  , 1.);//Derive from B0ToKstarJpsi , Bin3
    RooRealVar as("as"   , "A_{S}"  , 0 , -1. , 1.);//Derive from B0ToKstarJpsi , Bin3
    if (iBin != 3 && iBin != 5){
        // 2011 cross check
        //fs.setVal(0.0129254);
        //fs.setAsymError(-0.00898344,0.0101371);
        //as.setVal(-0.0975919);
        //as.setAsymError(-0.00490805,0.0049092);

        // read parameter from datacard
        fs.setVal(readParam(3,"fs",0, 0.0129254));
        fs.setAsymError(readParam(3,"fs",1, -0.00898344),readParam(3,"fs",2, 0.0101371));
        as.setVal(readParam(3,"as",0, -0.0975919));
        as.setAsymError(readParam(3,"as",1, -0.00490805),readParam(3,"as",2, 0.0049092));
    }

    // Create parameters and PDFs
        // Signal double gaussian
    RooRealVar sigGauss_mean("sigGauss_mean","M_{K*#Mu#Mu}",5.28,5.25,5.30);
    RooRealVar sigGauss1_sigma("sigGauss1_sigma","#sigma_{1}",readParam(iBin,"sigGauss1_sigma",0,0.02),.01,.05);
    RooRealVar sigGauss2_sigma("sigGauss2_sigma","#sigma_{2}",readParam(iBin,"sigGauss2_sigma",0,0.08),.05,.40);
    sigGauss1_sigma.setError(readParam(iBin,"sigGauss1_sigma",1));
    sigGauss2_sigma.setError(readParam(iBin,"sigGauss2_sigma",1));
    RooGaussian f_sigMGauss1("f_sigMGauss1","f_sigMGauss1", Bmass, sigGauss_mean, sigGauss1_sigma);//double gaussian with shared mean
    RooGaussian f_sigMGauss2("f_sigMGauss2","f_sigMGauss2", Bmass, sigGauss_mean, sigGauss2_sigma);//double gaussian with shared mean
    RooRealVar sigM_frac("sigM_frac","sigM_frac",readParam(iBin,"sigM_frac",0,0.5),0.,1.);
    sigM_frac.setError(readParam(iBin,"sigM_frac",1));
    RooAddPdf f_sigM("f_sigM","f_sigM", RooArgList(f_sigMGauss1, f_sigMGauss2), sigM_frac);

    // Efficiency, acceptance and angular distribution of signal (checked)
    RooRealVar recK0L0("recK0L0","recK0L0",readParam(iBin,"accXrecoEff2", 0));
    RooRealVar recK1L0("recK1L0","recK1L0",readParam(iBin,"accXrecoEff2", 1));
    RooRealVar recK2L0("recK2L0","recK2L0",readParam(iBin,"accXrecoEff2", 2));
    RooRealVar recK3L0("recK3L0","recK3L0",readParam(iBin,"accXrecoEff2", 3));
    RooRealVar recK0L2("recK0L2","recK0L2",readParam(iBin,"accXrecoEff2", 4));
    RooRealVar recK1L2("recK1L2","recK1L2",readParam(iBin,"accXrecoEff2", 5));
    RooRealVar recK2L2("recK2L2","recK2L2",readParam(iBin,"accXrecoEff2", 6));
    RooRealVar recK3L2("recK3L2","recK3L2",readParam(iBin,"accXrecoEff2", 7));
    RooRealVar recK0L3("recK0L3","recK0L3",readParam(iBin,"accXrecoEff2", 8));
    RooRealVar recK1L3("recK1L3","recK1L3",readParam(iBin,"accXrecoEff2", 9));
    RooRealVar recK2L3("recK2L3","recK2L3",readParam(iBin,"accXrecoEff2",10));
    RooRealVar recK3L3("recK3L3","recK3L3",readParam(iBin,"accXrecoEff2",11));
    RooRealVar recK0L4("recK0L4","recK0L4",readParam(iBin,"accXrecoEff2",12));
    RooRealVar recK1L4("recK1L4","recK1L4",readParam(iBin,"accXrecoEff2",13));
    RooRealVar recK2L4("recK2L4","recK2L4",readParam(iBin,"accXrecoEff2",14));
    RooRealVar recK3L4("recK3L4","recK3L4",readParam(iBin,"accXrecoEff2",15));
    RooRealVar recK0L6("recK0L6","recK0L6",readParam(iBin,"accXrecoEff2",16));
    RooRealVar recK1L6("recK1L6","recK1L6",readParam(iBin,"accXrecoEff2",17));
    RooRealVar recK2L6("recK2L6","recK2L6",readParam(iBin,"accXrecoEff2",18));
    RooRealVar recK3L6("recK3L6","recK3L6",readParam(iBin,"accXrecoEff2",19));
    recK0L0.setError(readParam(iBin,"accXrecoEff2Err", 0));
    recK1L0.setError(readParam(iBin,"accXrecoEff2Err", 1));
    recK2L0.setError(readParam(iBin,"accXrecoEff2Err", 2));
    recK3L0.setError(readParam(iBin,"accXrecoEff2Err", 3));
    recK0L2.setError(readParam(iBin,"accXrecoEff2Err", 4));
    recK1L2.setError(readParam(iBin,"accXrecoEff2Err", 5));
    recK2L2.setError(readParam(iBin,"accXrecoEff2Err", 6));
    recK3L2.setError(readParam(iBin,"accXrecoEff2Err", 7));
    recK0L3.setError(readParam(iBin,"accXrecoEff2Err", 8));
    recK1L3.setError(readParam(iBin,"accXrecoEff2Err", 9));
    recK2L3.setError(readParam(iBin,"accXrecoEff2Err",10));
    recK3L3.setError(readParam(iBin,"accXrecoEff2Err",11));
    recK0L4.setError(readParam(iBin,"accXrecoEff2Err",12));
    recK1L4.setError(readParam(iBin,"accXrecoEff2Err",13));
    recK2L4.setError(readParam(iBin,"accXrecoEff2Err",14));
    recK3L4.setError(readParam(iBin,"accXrecoEff2Err",15));
    recK0L6.setError(readParam(iBin,"accXrecoEff2Err",16));
    recK1L6.setError(readParam(iBin,"accXrecoEff2Err",17));
    recK2L6.setError(readParam(iBin,"accXrecoEff2Err",18));
    recK3L6.setError(readParam(iBin,"accXrecoEff2Err",19));
    RooArgSet f_sigA_argset(CosThetaL,CosThetaK);
    f_sigA_argset.add(RooArgSet(fl,afb,fs,as));
    f_sigA_argset.add(RooArgSet(recK0L0,recK1L0,recK2L0,recK3L0));
    f_sigA_argset.add(RooArgSet(recK0L2,recK1L2,recK2L2,recK3L2));
    TString f_sigA_format;
    TString f_ang_format = "9/16*((2/3*fs+4/3*as*CosThetaK)*(1-CosThetaL**2)+(1-fs)*(2*fl*CosThetaK**2*(1-CosThetaL**2)+1/2*(1-fl)*(1-CosThetaK**2)*(1+CosThetaL**2)+4/3*afb*(1-CosThetaK**2)*CosThetaL))";
    TString f_rec_ord0 = readParam(iBin,"f_accXrecoEff_ord0",f_accXrecoEff_ord0[iBin]);
    TString f_rec_format, f_rec_L0, f_rec_L2, f_rec_L3, f_rec_L4, f_rec_L6;
    f_rec_L0 = "(recK0L0+recK1L0*CosThetaK+recK2L0*(3*CosThetaK**2-1)/2+recK3L0*(5*CosThetaK**3-3*CosThetaK)/2)";
    f_rec_L2 = "(recK0L2+recK1L2*CosThetaK+recK2L2*(3*CosThetaK**2-1)/2+recK3L2*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**2";
    f_rec_L3 = "(recK0L3+recK1L3*CosThetaK+recK2L3*(3*CosThetaK**2-1)/2+recK3L3*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**3";
    f_rec_L4 = "(recK0L4+recK1L4*CosThetaK+recK2L4*(3*CosThetaK**2-1)/2+recK3L4*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**4";
    f_rec_L6 = "(recK0L6+recK1L6*CosThetaK+recK2L6*(3*CosThetaK**2-1)/2+recK3L6*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**6";

    if (iBin == 0) {
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_argset.add(RooArgSet(recK0L6,recK1L6,recK2L6,recK3L6));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L4.Data(),f_rec_L6.Data(),f_ang_format.Data());
    }else if (iBin == 1) {
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L4.Data(),f_ang_format.Data());
    }else if (iBin > 1 && iBin < 6) {
        f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
        f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L3.Data(),f_rec_L4.Data(),f_ang_format.Data());
    }else{
        f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
        f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_rec_ord0.Data(),f_rec_L0.Data(),f_rec_L2.Data(),f_rec_L3.Data(),f_ang_format.Data());
    }
    RooGenericPdf f_sigA("f_sigA", f_sigA_format, f_sigA_argset);

        // merge mass and angular distro of signal
    RooProdPdf f_sig("f_sig","f_sig",f_sigM,f_sigA);
    printf("INFO: f_sig prepared.\n");
    
    // Create combinatorial background distribution (to be checked)
    RooRealVar bkgCombM_c1("bkgCombM_c1","c1",readParam(iBin,"bkgCombM_c1",0),-100,100);
    RooRealVar bkgCombM_c2("bkgCombM_c2","c2",readParam(iBin,"bkgCombM_c2",0),-100,100);
    RooRealVar bkgCombM_c3("bkgCombM_c3","c3",readParam(iBin,"bkgCombM_c3",0),-100,100);
    RooRealVar bkgCombM_c4("bkgCombM_c4","c4",readParam(iBin,"bkgCombM_c4",0),-100,100);
    RooRealVar bkgCombM_c5("bkgCombM_c5","c5",readParam(iBin,"bkgCombM_c5",0),-100,100);
    RooRealVar bkgCombM_c6("bkgCombM_c6","c6",readParam(iBin,"bkgCombM_c6",0),-100,100);
    RooRealVar bkgCombM_c7("bkgCombM_c7","c7",readParam(iBin,"bkgCombM_c7",0),-100,100);
    RooArgSet f_bkgCombM_argset;
    switch (iBin){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            f_bkgCombM_argset.add(RooArgSet(bkgCombM_c1,bkgCombM_c2,bkgCombM_c3,bkgCombM_c4,bkgCombM_c5,bkgCombM_c6,bkgCombM_c7));
            break;
        default:
            f_bkgCombM_argset.add(RooArgSet(bkgCombM_c1,bkgCombM_c2,bkgCombM_c3,bkgCombM_c4));
            break;
    }
    RooPolynomial f_bkgCombM("f_bkgCombM","f_bkgCombM",Bmass_norm,f_bkgCombM_argset);
    RooRealVar bkgCombL_c1("bkgCombL_c1","c1",readParam(iBin,"bkgCombL_c1",0),-100,100);
    RooRealVar bkgCombL_c2("bkgCombL_c2","c2",readParam(iBin,"bkgCombL_c2",0.01),-100,100);
    RooRealVar bkgCombL_c3("bkgCombL_c3","c3",readParam(iBin,"bkgCombL_c3",0),-100,100);
    RooRealVar bkgCombL_c4("bkgCombL_c4","c4",readParam(iBin,"bkgCombL_c4",0.01),-100,100);
    RooRealVar bkgCombL_c5("bkgCombL_c5","c5",readParam(iBin,"bkgCombL_c5",0),-100,100);
    RooRealVar bkgCombL_c6("bkgCombL_c6","c6",readParam(iBin,"bkgCombL_c6",0.01),-100,100);
    RooRealVar bkgCombL_c7("bkgCombL_c7","c7",readParam(iBin,"bkgCombL_c7",0),-100,100);
    RooRealVar bkgCombL_c8("bkgCombL_c8","c8",readParam(iBin,"bkgCombL_c8",0),-100,100);
    RooArgSet f_bkgCombL_argset;
    switch (iBin) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            f_bkgCombL_argset.add(RooArgSet(bkgCombL_c1,bkgCombL_c2,bkgCombL_c3,bkgCombL_c4,bkgCombL_c5,bkgCombL_c6,bkgCombL_c7));
            break;
        default:
            bkgCombL_c1.setVal(0.);
            bkgCombL_c2.setVal(0.);
            bkgCombL_c3.setVal(0.);
            bkgCombL_c4.setVal(0.);
            bkgCombL_c1.setConstant(kTRUE);
            bkgCombL_c2.setConstant(kTRUE);
            bkgCombL_c3.setConstant(kTRUE);
            bkgCombL_c4.setConstant(kTRUE);
            break;
    }
    RooPolynomial f_bkgCombL("f_bkgCombL","f_bkgCombL",CosThetaL,f_bkgCombL_argset); // RooPolynomial
//    RooGaussian f_bkgCombL_gaus1("f_bkgCombL_gaus1","f_bkgCombL_gaus1",CosThetaL,bkgCombL_c1,bkgCombL_c2);
//    RooGaussian f_bkgCombL_gaus2("f_bkgCombL_gaus2","f_bkgCombL_gaus2",CosThetaL,bkgCombL_c3,bkgCombL_c4);
//    RooGaussian f_bkgCombL_gaus3("f_bkgCombL_gaus3","f_bkgCombL_gaus3",CosThetaL,bkgCombL_c5,bkgCombL_c6);
//    RooAddPdf f_bkgCombL("f_bkgCombL","f_bkgCombL",RooArgList(f_bkgCombL_gaus1,f_bkgCombL_gaus2,f_bkgCombL_gaus3),RooArgList(bkgCombL_c7,bkgCombL_c8));//3 Gaussians
    RooRealVar bkgCombK_c1("bkgCombK_c1","c1",readParam(iBin,"bkgCombK_c1",0),-100,100);
    RooRealVar bkgCombK_c2("bkgCombK_c2","c2",readParam(iBin,"bkgCombK_c2",0),-100,100);
    RooRealVar bkgCombK_c3("bkgCombK_c3","c3",readParam(iBin,"bkgCombK_c3",0),-100,100);
    RooRealVar bkgCombK_c4("bkgCombK_c4","c4",readParam(iBin,"bkgCombK_c4",0),-100,100);
    RooRealVar bkgCombK_c5("bkgCombK_c5","c5",readParam(iBin,"bkgCombK_c5",0),-100,100);
    RooRealVar bkgCombK_c6("bkgCombK_c6","c6",readParam(iBin,"bkgCombK_c6",0),-100,100);
    RooRealVar bkgCombK_c7("bkgCombK_c7","c7",readParam(iBin,"bkgCombK_c7",0),-100,100);
    RooArgSet f_bkgCombK_argset;
    switch (iBin) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            f_bkgCombK_argset.add(RooArgSet(bkgCombK_c1,bkgCombK_c2,bkgCombK_c3,bkgCombK_c4,bkgCombK_c5,bkgCombK_c6,bkgCombK_c7));
            break;
        default:
            bkgCombK_c1.setVal(0.);
            bkgCombK_c2.setVal(0.);
            bkgCombK_c3.setVal(0.);
            bkgCombK_c4.setVal(0.);
            bkgCombK_c1.setConstant(kTRUE);
            bkgCombK_c2.setConstant(kTRUE);
            bkgCombK_c3.setConstant(kTRUE);
            bkgCombK_c4.setConstant(kTRUE);
            break;
    }
    RooPolynomial f_bkgCombK("f_bkgCombK","f_bkgCombK",CosThetaK,f_bkgCombK_argset);
    RooProdPdf f_bkgCombA("f_bkgCombA", "f_bckCombA",f_bkgCombK,f_bkgCombL);
    RooProdPdf f_bkgComb("f_bkgComb", "f_bckComb",f_bkgCombA,f_bkgCombM);
    
    //TString f_bkgComb_format = "";
    //RooArgSet f_bkgComb_argset;
    //f_bkgComb_argset.add(RooArgSet(bkgCombK_c1,bkgCombK_c2,bkgCombK_c3,bkgCombK_c4,CosThetaK));
    //f_bkgComb_argset.add(RooArgSet(bkgCombL_c1,bkgCombL_c2,bkgCombL_c3,bkgCombL_c4,CosThetaL));
    //f_bkgComb_argset.add(RooArgSet(bkgCombM_c1,bkgCombM_c2,bkgCombM_c3,bkgCombM_c4,Bmass_norm));
    //RooGenericPdf f_bkgComb("f_bkgComb", f_bkgComb_format,f_bkgComb_argset);
    printf("INFO: f_bkgComb prepared.\n");
    
    // Create peak background distribution(jpsi/psi2s)
    RooRealVar bkgjpsiGauss1_mean1("bkgjpsiGauss1_mean1","M_{K*#Mu#Mu}",readParam(iBin,"bkgjpsiGauss1_mean1",0));
    RooRealVar bkgjpsiGauss1_mean2("bkgjpsiGauss1_mean2","M_{K*#Mu#Mu}",readParam(iBin,"bkgjpsiGauss1_mean2",0));
    RooRealVar bkgjpsiGauss1_sigma1("bkgjpsiGauss1_sigma1","#sigma_{11}",readParam(iBin,"bkgjpsiGauss1_sigma1",0));
    RooRealVar bkgjpsiGauss1_sigma2("bkgjpsiGauss1_sigma2","#sigma_{12}",readParam(iBin,"bkgjpsiGauss1_sigma2",0));
    RooRealVar bkgjpsiM_frac1("bkgjpsiM_frac1","bkgjpsiM_frac1",readParam(iBin,"bkgjpsiM_frac1",0));
    RooRealVar bkgjpsiM_fracC1("bkgjpsiM_fracC1"           , "bkgjpsiM_fracC1"  , readParam(iBin,"bkgjpsiM_fracC1",0));
    RooRealVar bkgjpsiGauss2_mean1("bkgjpsiGauss2_mean1","M_{K*#Mu#Mu}",readParam(iBin,"bkgjpsiGauss2_mean1",0));
    RooRealVar bkgjpsiGauss2_mean2("bkgjpsiGauss2_mean2","M_{K*#Mu#Mu}",readParam(iBin,"bkgjpsiGauss2_mean2",0));
    RooRealVar bkgjpsiGauss2_sigma1("bkgjpsiGauss2_sigma1","#sigma_{21}",readParam(iBin,"bkgjpsiGauss2_sigma1",0));
    RooRealVar bkgjpsiGauss2_sigma2("bkgjpsiGauss2_sigma2","#sigma_{22}",readParam(iBin,"bkgjpsiGauss2_sigma2",0));
    RooRealVar bkgjpsiM_frac2("bkgjpsiM_frac2","bkgjpsiM_frac2",readParam(iBin,"bkgjpsiM_frac2",0));
    RooRealVar bkgjpsiM_frac12("bkgjpsiM_frac12","bkgjpsiM_frac12",readParam(iBin,"bkgjpsiM_frac12",0));
    bkgjpsiGauss1_mean1.setError(readParam(iBin,"bkgjpsiGauss1_mean1",1));
    bkgjpsiGauss1_mean2.setError(readParam(iBin,"bkgjpsiGauss1_mean2",1));
    bkgjpsiGauss1_sigma1.setError(readParam(iBin,"bkgjpsiGauss1_sigma1",1));
    bkgjpsiGauss1_sigma2.setError(readParam(iBin,"bkgjpsiGauss1_sigma2",1));
    bkgjpsiM_frac1.setError(readParam(iBin,"bkgjpsiM_frac1",1));
    bkgjpsiM_fracC1.setError(readParam(iBin,"bkgjpsiM_fracC1",1));
    bkgjpsiGauss2_mean1.setError(readParam(iBin,"bkgjpsiGauss2_mean1",1));
    bkgjpsiGauss2_mean2.setError(readParam(iBin,"bkgjpsiGauss2_mean2",1));
    bkgjpsiGauss2_sigma1.setError(readParam(iBin,"bkgjpsiGauss2_sigma1",1));
    bkgjpsiGauss2_sigma2.setError(readParam(iBin,"bkgjpsiGauss2_sigma2",1));
    bkgjpsiM_frac2.setError(readParam(iBin,"bkgjpsiM_frac2",1));
    bkgjpsiM_frac12.setError(readParam(iBin,"bkgjpsiM_frac12",1));
    RooGaussian f_bkgjpsiPeakMGauss11("f_bkgjpsiPeakMGauss11","f_bkgjpsiPeakMGauss11", Bmass, bkgjpsiGauss1_mean1, bkgjpsiGauss1_sigma1);
    RooGaussian f_bkgjpsiPeakMGauss12("f_bkgjpsiPeakMGauss12","f_bkgjpsiPeakMGauss12", Bmass, bkgjpsiGauss1_mean2, bkgjpsiGauss1_sigma2);
    RooGaussian f_bkgjpsiPeakMGauss21("f_bkgjpsiPeakMGauss21","f_bkgjpsiPeakMGauss21", Bmass, bkgjpsiGauss2_mean1, bkgjpsiGauss2_sigma1);
    RooGaussian f_bkgjpsiPeakMGauss22("f_bkgjpsiPeakMGauss22","f_bkgjpsiPeakMGauss22", Bmass, bkgjpsiGauss2_mean2, bkgjpsiGauss2_sigma2);
    RooAddPdf f_bkgjpsiPeakM1("f_bkgjpsiPeakM1","f_bkgjpsiPeakM1", RooArgList(f_bkgjpsiPeakMGauss11, f_bkgjpsiPeakMGauss12), bkgjpsiM_frac1);
    RooAddPdf f_bkgjpsiPeakM2("f_bkgjpsiPeakM2","f_bkgjpsiPeakM2", RooArgList(f_bkgjpsiPeakMGauss21, f_bkgjpsiPeakMGauss22), bkgjpsiM_frac2);
    RooAddPdf f_bkgjpsiPeakM12("f_bkgjpsiPeakM12","f_bkgjpsiPeakM12", RooArgList(f_bkgjpsiPeakM1, f_bkgjpsiPeakM2), bkgjpsiM_frac12);
    RooRealVar bkgpsi2sGauss1_mean1("bkgpsi2sGauss1_mean1","M_{K*#Mu#Mu}",readParam(iBin,"bkgpsi2sGauss1_mean1",0));
    RooRealVar bkgpsi2sGauss1_mean2("bkgpsi2sGauss1_mean2","M_{K*#Mu#Mu}",readParam(iBin,"bkgpsi2sGauss1_mean2",0));
    RooRealVar bkgpsi2sGauss1_sigma1("bkgpsi2sGauss1_sigma1","#sigma_{11}",readParam(iBin,"bkgpsi2sGauss1_sigma1",0));
    RooRealVar bkgpsi2sGauss1_sigma2("bkgpsi2sGauss1_sigma2","#sigma_{12}",readParam(iBin,"bkgpsi2sGauss1_sigma2",0));
    RooRealVar bkgpsi2sM_frac1("bkgpsi2sM_frac1","bkgpsi2sM_frac1",readParam(iBin,"bkgpsi2sM_frac1",0));
    RooRealVar bkgpsi2sM_fracC1("bkgpsi2sM_fracC1"           , "bkgpsi2sM_fracC1"  , readParam(iBin,"bkgpsi2sM_fracC1",0));
    RooRealVar bkgpsi2sGauss2_mean1("bkgpsi2sGauss2_mean1","M_{K*#Mu#Mu}",readParam(iBin,"bkgpsi2sGauss2_mean1",0));
    RooRealVar bkgpsi2sGauss2_mean2("bkgpsi2sGauss2_mean2","M_{K*#Mu#Mu}",readParam(iBin,"bkgpsi2sGauss2_mean2",0));
    RooRealVar bkgpsi2sGauss2_sigma1("bkgpsi2sGauss2_sigma1","#sigma_{21}",readParam(iBin,"bkgpsi2sGauss2_sigma1",0));
    RooRealVar bkgpsi2sGauss2_sigma2("bkgpsi2sGauss2_sigma2","#sigma_{22}",readParam(iBin,"bkgpsi2sGauss2_sigma2",0));
    RooRealVar bkgpsi2sM_frac2("bkgpsi2sM_frac2","bkgpsi2sM_frac2",readParam(iBin,"bkgpsi2sM_frac2",0));
    RooRealVar bkgpsi2sM_frac12("bkgpsi2sM_frac12","bkgpsi2sM_frac12",readParam(iBin,"bkgpsi2sM_frac12",0));
    bkgpsi2sGauss1_mean1.setError(readParam(iBin,"bkgpsi2sGauss1_mean1",1));
    bkgpsi2sGauss1_mean2.setError(readParam(iBin,"bkgpsi2sGauss1_mean2",1));
    bkgpsi2sGauss1_sigma1.setError(readParam(iBin,"bkgpsi2sGauss1_sigma1",1));
    bkgpsi2sGauss1_sigma2.setError(readParam(iBin,"bkgpsi2sGauss1_sigma2",1));
    bkgpsi2sM_frac1.setError(readParam(iBin,"bkgpsi2sM_frac1",1));
    bkgpsi2sM_fracC1.setError(readParam(iBin,"bkgpsi2sM_fracC1",1));
    bkgpsi2sGauss2_mean1.setError(readParam(iBin,"bkgpsi2sGauss2_mean1",1));
    bkgpsi2sGauss2_mean2.setError(readParam(iBin,"bkgpsi2sGauss2_mean2",1));
    bkgpsi2sGauss2_sigma1.setError(readParam(iBin,"bkgpsi2sGauss2_sigma1",1));
    bkgpsi2sGauss2_sigma2.setError(readParam(iBin,"bkgpsi2sGauss2_sigma2",1));
    bkgpsi2sM_frac2.setError(readParam(iBin,"bkgpsi2sM_frac2",1));
    bkgpsi2sM_frac12.setError(readParam(iBin,"bkgpsi2sM_frac12",1));
    RooGaussian f_bkgpsi2sPeakMGauss11("f_bkgpsi2sPeakMGauss11","f_bkgpsi2sPeakMGauss11", Bmass, bkgpsi2sGauss1_mean1, bkgpsi2sGauss1_sigma1);
    RooGaussian f_bkgpsi2sPeakMGauss12("f_bkgpsi2sPeakMGauss12","f_bkgpsi2sPeakMGauss12", Bmass, bkgpsi2sGauss1_mean2, bkgpsi2sGauss1_sigma2);
    RooGaussian f_bkgpsi2sPeakMGauss21("f_bkgpsi2sPeakMGauss21","f_bkgpsi2sPeakMGauss21", Bmass, bkgpsi2sGauss2_mean1, bkgpsi2sGauss2_sigma1);
    RooGaussian f_bkgpsi2sPeakMGauss22("f_bkgpsi2sPeakMGauss22","f_bkgpsi2sPeakMGauss22", Bmass, bkgpsi2sGauss2_mean2, bkgpsi2sGauss2_sigma2);
    RooAddPdf f_bkgpsi2sPeakM1("f_bkgpsi2sPeakM1","f_bkgpsi2sPeakM1", RooArgList(f_bkgpsi2sPeakMGauss11, f_bkgpsi2sPeakMGauss12), bkgpsi2sM_frac1);
    RooAddPdf f_bkgpsi2sPeakM2("f_bkgpsi2sPeakM2","f_bkgpsi2sPeakM2", RooArgList(f_bkgpsi2sPeakMGauss21, f_bkgpsi2sPeakMGauss22), bkgpsi2sM_frac2);
    RooAddPdf f_bkgpsi2sPeakM12("f_bkgpsi2sPeakM12","f_bkgpsi2sPeakM12", RooArgList(f_bkgpsi2sPeakM1, f_bkgpsi2sPeakM2), bkgpsi2sM_frac12);
    switch (iBin) {// Should be constants.
        case 2:
            // double Gaussian
            bkgjpsiM_frac12.setVal(1.);
            bkgjpsiM_frac12.setConstant(kTRUE);
            bkgpsi2sM_frac12.setVal(1.);
            bkgpsi2sM_frac12.setConstant(kTRUE);
//            f = new RooExtendPdf("f","f",f_bkgPeakM1,nbkgPeak); // triple Gaussian at low
            break;
        case 4:
            // two double Gaussians
            bkgjpsiM_fracC1.setVal(0.);
            bkgjpsiM_fracC1.setConstant(kTRUE);
            bkgjpsiM_frac12.setVal(0.);
            bkgpsi2sM_fracC1.setVal(0.);
            bkgpsi2sM_fracC1.setConstant(kTRUE);
            bkgpsi2sM_frac12.setVal(1.);
            bkgpsi2sM_frac12.setConstant(kTRUE);
//            f = new RooExtendPdf("f","f",f_bkgPeakM12,nbkgPeak); // two double Gaussian
            break;
        case 6:
            // single Gaussian
            bkgjpsiM_frac12.setVal(0.);
            bkgjpsiM_frac12.setConstant(kTRUE);
            bkgpsi2sM_frac12.setVal(0.);
            bkgpsi2sM_frac12.setConstant(kTRUE);
//            f = new RooExtendPdf("f","f",f_bkgPeakM2,nbkgPeak); // single Gaussian at high
            break;
        case 3:
        case 5:
            bkgjpsiM_frac12.setVal(1.);
            bkgjpsiM_frac12.setConstant(kTRUE);
            bkgjpsiM_fracC1.setVal(0.);
            bkgjpsiM_fracC1.setConstant(kTRUE);
            bkgpsi2sM_frac12.setVal(1.);
            bkgpsi2sM_frac12.setConstant(kTRUE);
            bkgpsi2sM_fracC1.setVal(0.); 
            bkgpsi2sM_fracC1.setConstant(kTRUE);
//            f = new RooExtendPdf("f","f",f_bkgPeakM1,nbkgPeak); // double Gaussian peak around B+
            break;
        default:
            break;
    }
        // Angular distribution of peaking background
    RooRealVar bkgjpsiPeak_c1("bkgjpsiPeak_c1","bkgjpsiPeak_c1",readParam(3,"bkgjpsiPeak_c1",0));
    RooRealVar bkgjpsiPeak_c2("bkgjpsiPeak_c2","bkgjpsiPeak_c2",readParam(3,"bkgjpsiPeak_c2",0));
    RooRealVar bkgjpsiPeak_c3("bkgjpsiPeak_c3","bkgjpsiPeak_c3",readParam(3,"bkgjpsiPeak_c3",0));
    RooRealVar bkgjpsiPeak_c4("bkgjpsiPeak_c4","bkgjpsiPeak_c4",readParam(3,"bkgjpsiPeak_c4",0));
    RooRealVar bkgjpsiPeak_c5("bkgjpsiPeak_c5","bkgjpsiPeak_c5",readParam(3,"bkgjpsiPeak_c5",0));
    RooRealVar bkgjpsiPeak_c6("bkgjpsiPeak_c6","bkgjpsiPeak_c6",readParam(3,"bkgjpsiPeak_c6",0));
    RooRealVar bkgjpsiPeak_c7("bkgjpsiPeak_c7","bkgjpsiPeak_c7",readParam(3,"bkgjpsiPeak_c7",0));
    RooRealVar bkgjpsiPeak_c8("bkgjpsiPeak_c8","bkgjpsiPeak_c8",readParam(3,"bkgjpsiPeak_c8",0));
    bkgjpsiPeak_c1.setError(readParam(3,"bkgjpsiPeak_c1",1));
    bkgjpsiPeak_c2.setError(readParam(3,"bkgjpsiPeak_c2",1));
    bkgjpsiPeak_c3.setError(readParam(3,"bkgjpsiPeak_c3",1));
    bkgjpsiPeak_c4.setError(readParam(3,"bkgjpsiPeak_c4",1));
    bkgjpsiPeak_c5.setError(readParam(3,"bkgjpsiPeak_c5",1));
    bkgjpsiPeak_c6.setError(readParam(3,"bkgjpsiPeak_c6",1));
    bkgjpsiPeak_c7.setError(readParam(3,"bkgjpsiPeak_c7",1));
    bkgjpsiPeak_c8.setError(readParam(3,"bkgjpsiPeak_c8",1));
    RooRealVar bkgpsi2sPeak_c1("bkgpsi2sPeak_c1","bkgpsi2sPeak_c1",readParam(5,"bkgpsi2sPeak_c1",0));
    RooRealVar bkgpsi2sPeak_c2("bkgpsi2sPeak_c2","bkgpsi2sPeak_c2",readParam(5,"bkgpsi2sPeak_c2",0));
    RooRealVar bkgpsi2sPeak_c3("bkgpsi2sPeak_c3","bkgpsi2sPeak_c3",readParam(5,"bkgpsi2sPeak_c3",0));
    RooRealVar bkgpsi2sPeak_c4("bkgpsi2sPeak_c4","bkgpsi2sPeak_c4",readParam(5,"bkgpsi2sPeak_c4",0));
    RooRealVar bkgpsi2sPeak_c5("bkgpsi2sPeak_c5","bkgpsi2sPeak_c5",readParam(5,"bkgpsi2sPeak_c5",0));
    RooRealVar bkgpsi2sPeak_c6("bkgpsi2sPeak_c6","bkgpsi2sPeak_c6",readParam(5,"bkgpsi2sPeak_c6",0));
    RooRealVar bkgpsi2sPeak_c7("bkgpsi2sPeak_c7","bkgpsi2sPeak_c7",readParam(5,"bkgpsi2sPeak_c7",0));
    RooRealVar bkgpsi2sPeak_c8("bkgpsi2sPeak_c8","bkgpsi2sPeak_c8",readParam(5,"bkgpsi2sPeak_c8",0));
    bkgpsi2sPeak_c1.setError(readParam(5,"bkgjpsiPeak_c1",1));
    bkgpsi2sPeak_c2.setError(readParam(5,"bkgjpsiPeak_c2",1));
    bkgpsi2sPeak_c3.setError(readParam(5,"bkgjpsiPeak_c3",1));
    bkgpsi2sPeak_c4.setError(readParam(5,"bkgjpsiPeak_c4",1));
    bkgpsi2sPeak_c5.setError(readParam(5,"bkgjpsiPeak_c5",1));
    bkgpsi2sPeak_c6.setError(readParam(5,"bkgjpsiPeak_c6",1));
    bkgpsi2sPeak_c7.setError(readParam(5,"bkgjpsiPeak_c7",1));
    bkgpsi2sPeak_c8.setError(readParam(5,"bkgjpsiPeak_c8",1));
    switch (iBin) {// Should be fixed constants already.
        case 3:
        case 5:
            bkgjpsiPeak_c1.setConstant(kTRUE);
            bkgjpsiPeak_c2.setConstant(kTRUE);
            bkgjpsiPeak_c3.setConstant(kTRUE);
            bkgjpsiPeak_c4.setConstant(kTRUE);
            bkgjpsiPeak_c5.setConstant(kTRUE);
            bkgjpsiPeak_c6.setConstant(kTRUE);
            bkgjpsiPeak_c7.setConstant(kTRUE);
            bkgjpsiPeak_c8.setConstant(kTRUE);
            bkgpsi2sPeak_c1.setConstant(kTRUE);
            bkgpsi2sPeak_c2.setConstant(kTRUE);
            bkgpsi2sPeak_c3.setConstant(kTRUE);
            bkgpsi2sPeak_c4.setConstant(kTRUE);
            bkgpsi2sPeak_c5.setConstant(kTRUE);
            bkgpsi2sPeak_c6.setConstant(kTRUE);
            bkgpsi2sPeak_c7.setConstant(kTRUE);
            bkgpsi2sPeak_c8.setConstant(kTRUE);
            break;
        default:
            bkgjpsiPeak_c1.setConstant(kTRUE);
            bkgjpsiPeak_c2.setConstant(kTRUE);
            bkgjpsiPeak_c3.setConstant(kTRUE);
            bkgjpsiPeak_c4.setConstant(kTRUE);
            bkgjpsiPeak_c5.setConstant(kTRUE);
            bkgjpsiPeak_c6.setConstant(kTRUE);
            bkgjpsiPeak_c7.setConstant(kTRUE);
            bkgjpsiPeak_c8.setConstant(kTRUE);
            bkgpsi2sPeak_c1.setConstant(kTRUE);
            bkgpsi2sPeak_c2.setConstant(kTRUE);
            bkgpsi2sPeak_c3.setConstant(kTRUE);
            bkgpsi2sPeak_c4.setConstant(kTRUE);
            bkgpsi2sPeak_c5.setConstant(kTRUE);
            bkgpsi2sPeak_c6.setConstant(kTRUE);
            bkgpsi2sPeak_c7.setConstant(kTRUE);
            bkgpsi2sPeak_c8.setConstant(kTRUE);
            break;
    }
    TString f_bkgjpsiPeakA_format = "(1+\
                                bkgjpsiPeak_c1*CosThetaK+\
                                bkgjpsiPeak_c2*CosThetaK**2+\
                                bkgjpsiPeak_c3*CosThetaL**2+\
                                bkgjpsiPeak_c4*CosThetaL**4+\
                                bkgjpsiPeak_c5*CosThetaL**2*CosThetaK+\
                                bkgjpsiPeak_c6*CosThetaL**4*CosThetaK+\
                                bkgjpsiPeak_c7*CosThetaL**2*CosThetaK**2+\
                                bkgjpsiPeak_c8*CosThetaL**4*CosThetaK**2)*\
                                exp(-(CosThetaK+0.5*CosThetaL)**4-(CosThetaK-0.5*CosThetaL)**4+1.5*(CosThetaK+0.5*CosThetaL)**2+1.5*(CosThetaK-0.5*CosThetaL)**2)";
    TString f_bkgpsi2sPeakA_format = "(1+\
                                bkgpsi2sPeak_c1*CosThetaK+\
                                bkgpsi2sPeak_c2*CosThetaK**2+\
                                bkgpsi2sPeak_c3*CosThetaL**2+\
                                bkgpsi2sPeak_c4*CosThetaL**4+\
                                bkgpsi2sPeak_c5*CosThetaL**2*CosThetaK+\
                                bkgpsi2sPeak_c6*CosThetaL**4*CosThetaK+\
                                bkgpsi2sPeak_c7*CosThetaL**2*CosThetaK**2+\
                                bkgpsi2sPeak_c8*CosThetaL**4*CosThetaK**2)*\
                                exp(-(CosThetaK+0.5*CosThetaL)**4-(CosThetaK-0.5*CosThetaL)**4+1.5*(CosThetaK+0.5*CosThetaL)**2+1.5*(CosThetaK-0.5*CosThetaL)**2)";

    RooArgSet f_bkgjpsiPeakA_argset;
    f_bkgjpsiPeakA_argset.add(RooArgSet(CosThetaL,CosThetaK));
    f_bkgjpsiPeakA_argset.add(RooArgSet(bkgjpsiPeak_c1,bkgjpsiPeak_c2,bkgjpsiPeak_c3,bkgjpsiPeak_c4,bkgjpsiPeak_c5,bkgjpsiPeak_c6,bkgjpsiPeak_c7,bkgjpsiPeak_c8));
    RooGenericPdf f_bkgjpsiPeakA("f_bkgjpsiPeakA", "f_bkgjpsiPeakA",f_bkgjpsiPeakA_format,f_bkgjpsiPeakA_argset);

    RooArgSet f_bkgpsi2sPeakA_argset;
    f_bkgpsi2sPeakA_argset.add(RooArgSet(CosThetaL,CosThetaK));
    f_bkgpsi2sPeakA_argset.add(RooArgSet(bkgpsi2sPeak_c1,bkgpsi2sPeak_c2,bkgpsi2sPeak_c3,bkgpsi2sPeak_c4,bkgpsi2sPeak_c5,bkgpsi2sPeak_c6,bkgpsi2sPeak_c7,bkgpsi2sPeak_c8));
    RooGenericPdf f_bkgpsi2sPeakA("f_bkgpsi2sPeakA", "f_bkgpsi2sPeakA",f_bkgpsi2sPeakA_format,f_bkgpsi2sPeakA_argset);

        // merge mass with angular term
    RooProdPdf f_bkgjpsiPeak("f_bkgjpsiPeak", "f_bkgjpsiPeak",f_bkgjpsiPeakA,f_bkgjpsiPeakM12);
    RooProdPdf f_bkgpsi2sPeak("f_bkgpsi2sPeak", "f_bkgpsi2sPeak",f_bkgpsi2sPeakA,f_bkgpsi2sPeakM12);
    printf("INFO: f_bkgPeak prepared.\n");

    // Observed spectrum = model*fullEfficiency
    RooRealVar nsig("nsig","nsig",50,0,5E3);
    RooRealVar nbkgComb("nbkgComb","nbkgComb",100,0,1E8);
    RooRealVar nbkgjpsiPeak("nbkgjpsiPeak","nbkgjpsiPeak",readParam(iBin,"nbkgjpsiPeak",0)*datasetLumi[0]/datasetLumi[2],0,1E8);
    RooRealVar nbkgpsi2sPeak("nbkgpsi2sPeak","nbkgpsi2sPeak",readParam(iBin,"nbkgpsi2sPeak",0)*datasetLumi[0]/datasetLumi[3],0,1E8);
    switch (iBin){
        case 2:
        case 3:
            nbkgpsi2sPeak.setVal(0.);
            nbkgpsi2sPeak.setConstant(kTRUE);
            break;
        case 4:
            break;
        case 5:
        case 6:
            nbkgjpsiPeak.setVal(0.);
            nbkgjpsiPeak.setConstant(kTRUE);
            break;
        case 0:
        case 1:
        case 7:
        default:
            nbkgjpsiPeak.setVal(0.);
            nbkgpsi2sPeak.setVal(0.);
            nbkgjpsiPeak.setConstant(kTRUE);
            nbkgpsi2sPeak.setConstant(kTRUE);
            break;
    }
    RooAddPdf f("kernel","kernel",RooArgList(f_bkgComb,f_bkgjpsiPeak,f_bkgpsi2sPeak,f_sig),RooArgList(nbkgComb,nbkgjpsiPeak,nbkgpsi2sPeak,nsig));// no penalty term

    // Extra penalty term to confine As, Fs, Fl, Afb.
    //RooRealVar t_penalty("t_penalty","t",0.01);
    //RooGenericPdf f_penaltyAfb("f_penaltyAfb","(1-TMath::Erf((afb-0.75*(1-fl))/(1.5*t_penalty*(1-fl))))*(1-TMath::Erf((-afb-0.75*(1-fl))/(1.5*t_penalty*(1-fl))))",RooArgSet(afb,fl,t_penalty));
    //RooGenericPdf f_penaltyAs("f_penaltyAfb","(1-TMath::Erf((afb-2*(1-fl)/3)/(1.5*t_penalty*(1-fl))))*(1-TMath::Erf((-afb-0.75*(1-fl))/(1.5*t_penalty*(1-fl))))",RooArgSet(afb,fl,t_penalty));
    //RooProdPdf f_penalty("f_penalty","f_penalty",f_penaltyAfb,f_penaltyAs);
    //RooProdPdf f("f","f",f_model,f_penalty);
    printf("INFO: f_penalty NOT applied.\n");

    // Gaussian constraints
    RooGaussian gaus_sigGauss1_sigma("gaus_sigGauss1_sigma","gaus_sigGauss1_sigma",sigGauss1_sigma,RooConst(readParam(iBin,"sigGauss1_sigma",0)),RooConst(readParam(iBin,"sigGauss1_sigma",1)));
    RooGaussian gaus_sigGauss2_sigma("gaus_sigGauss2_sigma","gaus_sigGauss2_sigma",sigGauss2_sigma,RooConst(readParam(iBin,"sigGauss2_sigma",0)),RooConst(readParam(iBin,"sigGauss2_sigma",1)));
    RooGaussian gaus_sigM_frac("gaus_sigM_frac","gaus_sigM_frac",sigM_frac,RooConst(readParam(iBin,"sigM_frac",0)),RooConst(readParam(iBin,"sigM_frac",1)));
    RooGaussian gaus_nbkgjpsiPeak("gaus_nbkgjpsiPeak","gaus_nbkgjpsiPeak",nbkgjpsiPeak,RooConst(readParam(iBin,"nbkgjpsiPeak",0)*datasetLumi[0]/datasetLumi[2]),RooConst(readParam(iBin,"nbkgjpsiPeak",1)*datasetLumi[0]/datasetLumi[2]));
    RooGaussian gaus_nbkgpsi2sPeak("gaus_nbkgpsi2sPeak","gaus_nbkgpsi2sPeak",nbkgpsi2sPeak,RooConst(readParam(iBin,"nbkgpsi2sPeak",0)*datasetLumi[0]/datasetLumi[3]),RooConst(readParam(iBin,"nbkgpsi2sPeak",1)*datasetLumi[0]/datasetLumi[3]));
    RooBifurGauss gaus_fs("gaus_fs","gaus_fs",fs,RooConst(readParam(iBin,"fs",0,0.00129254)),RooConst(readParam(iBin,"fs",1)),RooConst(readParam(iBin,"fs",1,0.0101371)));
    RooBifurGauss gaus_as("gaus_as","gaus_as",as,RooConst(readParam(iBin,"as",0,-0.0975917)),RooConst(readParam(iBin,"as",1)),RooConst(readParam(iBin,"as",1,0.0049092)));
//    RooBifurGauss gaus_fs("gaus_fs","gaus_fs",fs,RooConst(0.0129254),RooConst(0.00898344),RooConst(0.0101371));// 2011 result
//    RooBifurGauss gaus_as("gaus_as","gaus_as",as,RooConst(-0.0975919),RooConst(0.00490805),RooConst(0.0049092));
    
    RooArgSet gausConstraints(gaus_sigGauss1_sigma,gaus_sigGauss2_sigma,gaus_sigM_frac);
    if (iBin<2 || iBin > 6) gausConstraints.add(RooArgSet(gaus_nbkgjpsiPeak,gaus_nbkgpsi2sPeak));
    if (iBin == 3 || iBin == 5) gausConstraints.add(RooArgSet(gaus_fs,gaus_as));
    printf("INFO: gausConstraints are settled.\n");
    
    // Get data and apply unbinned fit
    int mumuMassWindowBin = 1+4*isCDFcut;
    if (iBin==3 || iBin==5) mumuMassWindowBin = 0; // no cut
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(Q2, CosThetaK, CosThetaL, Bmass, Mumumass, Mumumasserr),TString::Format("(%s) && (%s)",q2range[iBin],mumuMassWindow[mumuMassWindowBin]),0);
    f.fitTo(*data,Extended(kTRUE),ExternalConstraints(gausConstraints),Minimizer("Minuit"),Minos(kTRUE));

    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* framemass = Bmass.frame();
    data->plotOn(framemass,Binning(20));// TGraphPainter options are allowed by DrawOption()
    f.plotOn(framemass,LineColor(1));
    f.plotOn(framemass,Components(f_sig),LineColor(4),LineWidth(2));
    f.plotOn(framemass,Components(f_bkgComb),LineColor(2),LineWidth(2),LineStyle(2));
    f.plotOn(framemass,Components(f_bkgjpsiPeak),LineColor(6),LineWidth(2),LineStyle(2));
    f.plotOn(framemass,Components(f_bkgpsi2sPeak),LineColor(8),LineWidth(2),LineStyle(2));

    framemass->SetTitle("");
    framemass->SetMinimum(0);
    framemass->Draw();
    
    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = 0;
    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    t1->DrawLatex(.65,.79+fixNDC,TString::Format("Y_{Signal}=%.3f",nsig.getVal()));
    t1->DrawLatex(.65,.72+fixNDC,TString::Format("Y_{J/#Psi}=%.3f",nbkgjpsiPeak.getVal()));
    t1->DrawLatex(.65,.65+fixNDC,TString::Format("Y_{#Psi'}=%.3f",nbkgpsi2sPeak.getVal()));
    t1->DrawLatex(.65,.58+fixNDC,TString::Format("Y_{Comb}=%.3f",nbkgComb.getVal()));
    c->Print(TString::Format("./plots/%s_bin%d.pdf",outfile,iBin));

    // Draw projection to CosThetaK
    RooPlot* framecosk = CosThetaK.frame(); 
    data->plotOn(framecosk,Binning(20)); 
    f.plotOn(framecosk,LineColor(1)); 
    f.plotOn(framecosk,Components(f_sig),LineColor(4),LineWidth(2));
    f.plotOn(framecosk,Components(f_bkgComb),LineColor(2),LineWidth(2),LineStyle(2));
    f.plotOn(framecosk,Components(f_bkgjpsiPeak),LineColor(6),LineWidth(2),LineStyle(2));
    f.plotOn(framecosk,Components(f_bkgpsi2sPeak),LineColor(8),LineWidth(2),LineStyle(2));

    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();

    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    t1->DrawLatex(.35,.79+fixNDC,TString::Format("F_{L}=%.3f#pm%.3f",fl.getVal(),fl.getError()));
    c->Update();
    c->Print(TString::Format("./plots/%s_cosk_bin%d.pdf",outfile,iBin));

    // Draw projection to CosThetaL
    RooPlot* framecosl = CosThetaL.frame(); 
    data->plotOn(framecosl,Binning(20)); 
    f.plotOn(framecosl,LineColor(1)); 
    f.plotOn(framecosl,Components(f_sig),LineColor(4),LineWidth(2));
    f.plotOn(framecosl,Components(f_bkgComb),LineColor(2),LineWidth(2),LineStyle(2));
    f.plotOn(framecosl,Components(f_bkgjpsiPeak),LineColor(6),LineWidth(2),LineStyle(2));
    f.plotOn(framecosl,Components(f_bkgpsi2sPeak),LineColor(8),LineWidth(2),LineStyle(2));

    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();

    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
    t1->DrawLatex(.35,.79+fixNDC,TString::Format("A_{FB}=%.3f#pm%.3f",afb.getVal(),afb.getError()));
    c->Update();
    c->Print(TString::Format("./plots/%s_cosl_bin%d.pdf",outfile,iBin));

    // clear
    delete t1;
    delete c;
    delete data;

    // write output
    double val[3]={0,0,0};
    val[0] = fl.getVal();val[1] = fl.getError();
    writeParam(iBin, "fl", val);
    val[0] = afb.getVal();val[1] = afb.getError();
    writeParam(iBin, "afb",val);
    val[0] = fs.getVal();val[1] = fs.getErrorHi();val[2] = fs.getErrorLo();
    writeParam(iBin, "fs", val, 3);
    val[0] = as.getVal();val[1] = as.getErrorHi();val[2] = fs.getErrorLo();
    writeParam(iBin, "as", val, 3);
}//}}}

void angular3D(const char outfile[] = "angular", bool doFit = true)
{//{{{

    double x[8]={1.5,3.15,6.49,9.385,11.475,13.52,15.09,17.5};
    double xerr[8]={0.5,1.15,2.09,0.705,1.385,0.66,0.91,1.5};
    double yafb[8],yerrafb[8],yfl[8],yerrfl[8];

    if (doFit){
        angular3D_bin(0);
        angular3D_bin(1);
        angular3D_bin(2);
        angular3D_bin(4);
        angular3D_bin(6);
        angular3D_bin(7);
    }

    // Checkout input data
    for(int ibin = 0; ibin < 8; ibin++){
        yfl[ibin]       = readParam(ibin,"fl",0);
        yerrfl[ibin]    = readParam(ibin,"fl",1);
        yafb[ibin]      = readParam(ibin,"afb",0);
        yerrafb[ibin]   = readParam(ibin,"afb",1);
        printf("yafb[%d]=%6.4f +- %6.4f\n",ibin,yafb[ibin],yerrafb[ibin]);
        printf("yfl [%d]=%6.4f +- %6.4f\n",ibin,yfl[ibin],yerrfl[ibin]);
    }
    
    // Draw
    TCanvas *c = new TCanvas("c");
    TH2F *frame = new TH2F("frame","",18,1,19,10,-1,1);
    frame->SetStats(kFALSE);

    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("F_{L}");
    frame->SetAxisRange(0,1,"Y");
    frame->Draw();
    TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(8,x,yfl,xerr,xerr,yerrfl,yerrfl);
    g_fl->SetFillColor(2);
    g_fl->SetFillStyle(3001);
    g_fl->Draw("P2");
    c->Print(TString::Format("./plots/%s_fl.pdf",outfile));
    c->Clear();

    frame->SetTitle("");
    frame->SetXTitle("q^{2} [(GeV)^{2}]");
    frame->SetYTitle("A_{FB}");
    frame->SetAxisRange(-1,1,"Y");
    frame->Draw();
    TGraphAsymmErrors *g_afb = new TGraphAsymmErrors(8,x,yafb,xerr,xerr,yerrafb,yerrafb);
    g_afb->SetFillColor(2);
    g_afb->SetFillStyle(3001);
    g_afb->Draw("P2");
    c->Print(TString::Format("./plots/%s_afb.pdf",outfile));
}//}}}

//_________________________________________________________________________________
// Validation tools
void getToyFromUnfilterGen(int iBin)
{//{{{
    bool flatModel = false;

    // Generate random toy using unfiltered input and efficiency function
    double gQ2 = 0;
    double gCosThetaK = 0;
    double gCosThetaL = 0;
    double Q2 = 0;
    double CosThetaK = 0;
    double CosThetaL = 0;
    double Mumumass = 0;
    double Mumumasserr = 0;

    //TChain *treein=new TChain("tree");
    //treein->Add("./data/2012/sel_BuToKstarMuMu_NoGenFilter_8TeV_part*_mc.lite.root");//Contact po-hsun.chen@cern.ch to get these files.
    TChain *treein=ch;
    if (treein == NULL) {
        printf("Unfiltered MC sample is missing. Please contact pchen@cern.ch to get it.");
        return;
    }
    treein->SetBranchAddress("genQ2"        , &gQ2);
    treein->SetBranchAddress("genCosThetaK" , &gCosThetaK);
    treein->SetBranchAddress("genCosThetaL" , &gCosThetaL);
    treein->SetBranchAddress("Q2"           , &Q2);
    treein->SetBranchAddress("CosThetaK"    , &CosThetaK);
    treein->SetBranchAddress("CosThetaL"    , &CosThetaL);
    treein->SetBranchAddress("Mumumass"     , &Mumumass);
    treein->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
    
    // Get efficiency map, y=cosThetaK, x=cosThetaL
    std::string f2_model_format;
    if (flatModel){
        f2_model_format = TString::Format("0.01+0.*x+0.*y");
    }else{
        f2_model_format = TString::Format("%s*(1+([0]+[1]*CosThetaK+[2]*(3*CosThetaK**2-1)/2+[3]*(5*CosThetaK**3-3*CosThetaK)/2)+([4]+[5]*CosThetaK+[6]*(3*CosThetaK**2-1)/2+[7]*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**2+([8]+[9]*CosThetaK+[10]*(3*CosThetaK**2-1)/2+[11]*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**3+([12]+[13]*CosThetaK+[14]*(3*CosThetaK**2-1)/2+[15]*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**4+([16]+[17]*CosThetaK+[18]*(3*CosThetaK**2-1)/2+[19]*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**6)",readParam(iBin,"f_accXrecoEff_ord0",f_accXrecoEff_ord0[iBin]).c_str());
    }
    //f2_model_format = regex_replace(f2_model_format,regex("CosThetaK"),"y");//use string::replace() for instead
    //f2_model_format = regex_replace(f2_model_format,regex("CosThetaL"),"x");
    while(f2_model_format.find("CosThetaK") !=std::string::npos ){
        f2_model_format.replace(f2_model_format.find("CosThetaK"), 9, "y");
    }
    while(f2_model_format.find("CosThetaL") !=std::string::npos ){
        f2_model_format.replace(f2_model_format.find("CosThetaL"), 9, "x");
    }
    printf("%s\n",f2_model_format.c_str());
    TF2 f2_model("f2_model",f2_model_format.c_str(),-1.,1.,-1.,1.);
    if (flatModel){
        //Set parameters here.
    }else{
        for (int i = 0; i < 20; i++) {
            f2_model.SetParameter(i,readParam(iBin,"accXrecoEff2",i));
            f2_model.SetParError(i,readParam(iBin,"accXrecoEff2Err",i));
        }
    }

    // 
    TFile fout(TString::Format("./rndToy_Bin%d.root",iBin), "RECREATE") ;
    TTree *treeout = treein->CloneTree(0);
    int _count = 0;//number of accepted events
    int _entry = 0;
    TRandom3 *rndGenerator = new TRandom3();
    do {
        treein->GetEntry(_entry); _entry++;
        if (gQ2 > q2rangedn[iBin] && gQ2 < q2rangeup[iBin]) {
            if (rndGenerator->Rndm() < f2_model.Eval(gCosThetaL,gCosThetaK)) {
                Q2 = gQ2;
                CosThetaL   = gCosThetaL;
                CosThetaK   = gCosThetaK;
                Mumumass    = sqrt(gQ2);
                Mumumasserr = 0.001;
                treeout->Fill();
                _count++;
            }
        }
    } while ( _entry < treein->GetEntries() );
    fout.Write();
    fout.Close();
}//}}}

void genToyCombBkg(int iBin, int nEvents) // For pull test
{//{{{
    // Generate in pure ROOT.
    double bkgCombM_c(readParam(iBin,"bkgCombM_c",0,-1));

    string f_bkgCombM_formula = "exp([0]*(x-5))";
    TF1 *f_bkgCombM = new TF1("f_bkgCombM",f_bkgCombM_formula.c_str(),5,5.56);
    f_bkgCombM->SetParameter(0,bkgCombM_c);

    double bkgCombL_c1(readParam(iBin,"bkgCombL_c1",0,0));
    double bkgCombL_c2(readParam(iBin,"bkgCombL_c2",0,0));
    double bkgCombL_c3(readParam(iBin,"bkgCombL_c3",0,0));
    double bkgCombL_c4(readParam(iBin,"bkgCombL_c4",0,0));
    double bkgCombK_c1(readParam(iBin,"bkgCombK_c1",0,0));
    double bkgCombK_c2(readParam(iBin,"bkgCombK_c2",0,0));
    double bkgCombK_c3(readParam(iBin,"bkgCombK_c3",0,0));
    double bkgCombK_c4(readParam(iBin,"bkgCombK_c4",0,0));

    string f2_bkgCombA_formula = "(1+[0]*x+[1]*x**2+[2]*x**3+[3]*x**4)*(1+[4]*y+[5]*y**2+[6]*y**3+[7]*y**4)";
    TF2 *f2_bkgCombA = new TF2("f2_bkgCombA",f2_bkgCombA_formula.c_str(), -1, 1, -1, 1);
    f2_bkgCombA->SetParameters(bkgCombL_c1,bkgCombL_c2,bkgCombL_c3,bkgCombL_c4,bkgCombK_c1,bkgCombK_c2,bkgCombK_c3,bkgCombK_c4);
    
    double rndBmass(0);
    double rndCosL(0);
    double rndCosK(0);
    double Q2((q2rangeup[iBin]+q2rangedn[iBin])/2);
    double mumuMass(sqrt(Q2));
    double mumuMasserr(0.01);
    
    TFile fout(TString::Format("./rndBkgToy_Bin%d.root",iBin), "RECREATE");
    TTree *tree = new TTree("tree","tree");
    tree->Branch("CosThetaK",&rndCosK,"CosThetaK/D");
    tree->Branch("CosThetaL",&rndCosL,"CosThetaL/D");
    tree->Branch("Bmass",&rndBmass,"Bmass/D");
    tree->Branch("Mumumass",&mumuMass,"Mumumass/D");
    tree->Branch("Mumumasserr",&mumuMasserr,"Mumumasserr/D");
    tree->Branch("Q2",&Q2,"Q2/D");

    for (int iEvt = 0; iEvt < nEvents; iEvt++) {
        rndBmass = f_bkgCombM ->GetRandom();
        f2_bkgCombA->GetRandom2(rndCosL,rndCosK);
        tree->Fill();
    }
    fout.Write();
    fout.Close();
}//}}}

void genToyPeakBkg(int iBin, int nEvents) // For pull test. Under construction.
{//{{{
    static char decmode[20];
    while(strcmp(decmode,"jpsi")*strcmp(decmode, "psi2s") != 0){
        printf("Please insert background type [ jpsi / psi2s ]:");
        scanf("%19s",decmode);
    }

    // Generate in pure ROOT.
    double bkgPeakM_c(readParam(iBin,TString::Format("bkg%sPeakM_c",decmode),0));

    string f_bkgPeakM_formula = "exp([0]*(x-5))";
    TF1 *f_bkgPeakM = new TF1("f_bkgPeakM",f_bkgPeakM_formula.c_str(),5,5.56);
    f_bkgPeakM->SetParameter(0,bkgPeakM_c);

    double bkgPeakL_c1(readParam(iBin,TString::Format("bkg%sPeakL_c1",decmode),0));
    double bkgPeakL_c2(readParam(iBin,TString::Format("bkg%sPeakL_c2",decmode),0));
    double bkgPeakL_c3(readParam(iBin,TString::Format("bkg%sPeakL_c3",decmode),0));
    double bkgPeakL_c4(readParam(iBin,TString::Format("bkg%sPeakL_c4",decmode),0));
    double bkgPeakK_c1(readParam(iBin,TString::Format("bkg%sPeakK_c1",decmode),0));
    double bkgPeakK_c2(readParam(iBin,TString::Format("bkg%sPeakK_c2",decmode),0));
    double bkgPeakK_c3(readParam(iBin,TString::Format("bkg%sPeakK_c3",decmode),0));
    double bkgPeakK_c4(readParam(iBin,TString::Format("bkg%sPeakK_c4",decmode),0));

    string f2_bkgPeakA_formula = "(1+[0]*x+[1]*x**2+[2]*x**3+[3]*x**4)*(1+[4]*y+[5]*y**2+[6]*y**3+[7]*y**4)";
    TF2 *f2_bkgPeakA = new TF2("f2_bkgPeakA",f2_bkgPeakA_formula.c_str(), -1, 1, -1, 1);
    f2_bkgPeakA->SetParameters(bkgPeakL_c1,bkgPeakL_c2,bkgPeakL_c3,bkgPeakL_c4,bkgPeakK_c1,bkgPeakK_c2,bkgPeakK_c3,bkgPeakK_c4);
    
    double rndBmass(0);
    double rndCosL(0);
    double rndCosK(0);
    double Q2((q2rangeup[iBin]+q2rangedn[iBin])/2);
    double mumuMass(sqrt(Q2));
    double mumuMasserr(0.01);
    
    TFile fout(TString::Format("./rndBkgToy_Bin%d.root",iBin), "RECREATE");
    TTree *tree = new TTree("tree","tree");
    tree->Branch("CosThetaK",&rndCosK,"CosThetaK/D");
    tree->Branch("CosThetaL",&rndCosL,"CosThetaL/D");
    tree->Branch("Bmass",&rndBmass,"Bmass/D");
    tree->Branch("Mumumass",&mumuMass,"Mumumass/D");
    tree->Branch("Mumumasserr",&mumuMasserr,"Mumumasserr/D");
    tree->Branch("Q2",&Q2,"Q2/D");

    for (int iEvt = 0; iEvt < nEvents; iEvt++) {
        rndBmass = f_bkgPeakM ->GetRandom();
        f2_bkgPeakA->GetRandom2(rndCosL,rndCosK);
        tree->Fill();
    }
    fout.Write();
    fout.Close();
}//}}}

void printListOfTChainElements(TChain *chain){
    TObjArray *fileElements=chain->GetListOfFiles();
    int nFiles = fileElements->GetEntries();
    TIter next(fileElements);
    TChainElement *chEl=0;
    for( int entry=0; entry < nFiles; entry++ ) {
        chEl=(TChainElement*)next();
        printf("%s\n",chEl->GetTitle());
    }
}

//_________________________________________________________________________________
int main(int argc, char** argv) {
    // Tags
    is7TeVCheck = false;   
    isCDFcut = 0;// 1 for true, 0 for false.
    
    // Help message
    if (argc <= 2) {
        printf("Usage       : ./fit Function infile\n");
        printf("Functions   :\n");
        printf("    bmass               Fit to mass spectrum using a double Gaussian signal and Chebyshev bkg.\n");
        printf("    fl_gen              Derive F_{L} from cosThetaK distribution at GEN level.\n");
        printf("    angular_gen         Derive F_{L} and A_{FB} from angular distribution at GEN level.\n");
        printf("    accXrecoEff         Get 2D efficiency map from signal simulation.\n");
        printf("    angular2D           Same as angular_gen, but fit to data with efficiency correction, bkg component is NOT considered.\n");
        printf("    angular3D_1a_Sm     Leading step1 to angular3D, determine signal shape from simulation.\n");
        printf("    angular3D_1b_YpPm   Leading step2 to angular3D, determine mass spectrum of peaking bkg from simulation.\n");
        printf("    angular3D_2a_PkPl   Leading step3 to angular3D, determine angular dist. of peaking bkg from simulation.\n");
        printf("    angular3D_prior     Leading step4 to angular3D, fit to data sideband to get initial values of combinatorial bkg.\n");
        printf("    angular3D           Derive F_{L} and A_{FB} by fitting to mass and angular distribution.\n");
        printf("Remark      :\n");
        printf("    1. Outputs will be stored in ./plots, please keep the directory.\n");
        printf("    2. Wildcard is allowed for infile. But you must quote infile like \"inputData_Run*.root\"!\n");
        return 0;
    }

    // main
    TString func    = argv[1];
    TString infile  = argv[2];
  
    if (func == "bmass") {
        if (argc != 4){
            printf("./fit bmass infile binID\n");
            for (int i = 0; i < 10; i++) {
                printf("    Bin %d : %s\n",i,q2range[i]);
            }
            return 0;
        }
        int iBin = atoi(argv[3]);
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        const char outfile[]="bmass";
        bmass(iBin, outfile); 
    }else if (func == "fl_gen"){
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        const char outfile[]="fl_gen";
        fl_gen(outfile);
    }else if (func == "angular_gen"){
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        const char outfile[]="angular_gen";
        angular_gen(outfile);
    }else if (func == "accXrecoEff") {
        if (false) createAccptanceHist(); // For experts only. Set to true if no given acceptance_8TeV.root
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        for (int iBin = 0; iBin < 8; iBin++) {
            accXrecoEff2(iBin);
        }
    }else if (func == "angular2D"){
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        const char outfile[]="angular2D";
        for (int iBin = 0; iBin < 8; iBin++) {
            if (iBin == 3 || iBin == 5) continue;
            angular2D_bin(iBin);
        }
    }else if (func == "angular3D_1a_Sm" || func == "angular3D_1b_YpPm" || func == "angular3D_2a_PkPl" || func == "angular3D_prior"){
        void (*fx)(int, const char*, bool);
        if ( func == "angular3D_1a_Sm" ){
            fx = angular3D_1a_Sm;
        }else if (func == "angular3D_1b_YpPm"){
            fx = angular3D_1b_YpPm;
        }else if (func == "angular3D_2a_PkPl"){
            fx = angular3D_2a_PkPl;
        }else{
            fx = angular3D_prior;
        }
        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        for (int iBin = 0; iBin < 8; iBin++) {
            // By default overwrite exist parameters.
            fx(iBin,func,true);
        }
    }else if (func == "angular3D"){
        if (argc != 4){
            printf("./fit angular3D infile doFit\n");
            printf("    If dofit is non-zero, perform fit and write results first.\n");
            printf("    Else, directly read fitParameter?.txt and make plots.\n");
            return 0;
        }
        bool doFit = false;
        if ( atoi(argv[3]) != 0 ) doFit = true;

        ch->Add(infile.Data());
        if (ch == NULL) return 1;
        const char outfile[]="angular3D";
        angular3D(outfile, doFit);
    }else if (func == "test"){
        ch->Add(infile.Data());
        printListOfTChainElements(ch);
        if (ch == NULL) return 1;
        const char outfile[]="test";
        for (int iBin = 0; iBin < 8; iBin++) {
//            if (iBin != 3 && iBin != 5) continue;
            if (iBin == 3 || iBin == 5) continue;
            //getToyFromUnfilterGen(iBin);
            //createRecoEffHist(iBin);
            //accXrecoEff2(iBin);
            //angular2D_bin(iBin);
            //genToyCombBkg(iBin,10000);
            angular3D_bin(iBin);
        }
        //angular3D_bin(4);
        //createAccptanceHist();
        //angular2D();
    }else{ 
        cerr << "No function available for: " << func.Data() << endl; 
    }
    printf("%lld entries processed.\n",ch->GetEntries());

    return 0 ;
}
