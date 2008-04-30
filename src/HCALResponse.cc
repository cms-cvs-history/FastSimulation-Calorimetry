//FastSimulation headers
#include "FastSimulation/Calorimetry/interface/HCALResponse.h"
#include "FastSimulation/Utilities/interface/RandomEngine.h"

// CMSSW Headers
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include<iostream>
#include <math.h>

#define debug 0

using namespace edm;

HCALResponse::HCALResponse(const edm::ParameterSet& pset,
			   const RandomEngine* engine) :
  random(engine)
{ 
  
  RespPar[HCAL][0][0] =  
    pset.getParameter<double>("HadronBarrelResolution_Stochastic");
  RespPar[HCAL][0][1] =  
    pset.getParameter<double>("HadronBarrelResolution_Constant");
  RespPar[HCAL][0][2] =  
    pset.getParameter<double>("HadronBarrelResolution_Noise");

  RespPar[HCAL][1][0] =  
    pset.getParameter<double>("HadronEndcapResolution_Stochastic");
  RespPar[HCAL][1][1] =  
    pset.getParameter<double>("HadronEndcapResolution_Constant");
  RespPar[HCAL][1][2] =  
    pset.getParameter<double>("HadronEndcapResolution_Noise");

  RespPar[VFCAL][0][0] =  
    pset.getParameter<double>("HadronForwardResolution_Stochastic");
  RespPar[VFCAL][0][1] =  
    pset.getParameter<double>("HadronForwardResolution_Constant");
  RespPar[VFCAL][0][2] =  
    pset.getParameter<double>("HadronForwardResolution_Noise");

  RespPar[VFCAL][1][0] =  
    pset.getParameter<double>("ElectronForwardResolution_Stochastic");
  RespPar[VFCAL][1][1] =  
    pset.getParameter<double>("ElectronForwardResolution_Constant");
  RespPar[VFCAL][1][2] =  
    pset.getParameter<double>("ElectronForwardResolution_Noise");

  eResponseScale[0] = 
    pset.getParameter<double>("eResponseScaleHB");  
  eResponseScale[1] = 
    pset.getParameter<double>("eResponseScaleHE");
  eResponseScale[2] = 
    pset.getParameter<double>("eResponseScaleHF");

  eResponsePlateau[0] = 
    pset.getParameter<double>("eResponsePlateauHB");
  eResponsePlateau[1] = 
    pset.getParameter<double>("eResponsePlateauHE");
  eResponsePlateau[2] = 
    pset.getParameter<double>("eResponsePlateauHF");

  eResponseExponent = 
    pset.getParameter<double>("eResponseExponent");

  eResponseCoefficient = 
    pset.getParameter<double>("eResponseCoefficient");

  eResponseCorrection = 
    pset.getParameter<double>("eResponseCorrection");

  // If need - add a small energy to each hadron ...
  eBias = 
    pset.getParameter<double>("energyBias");
  
  
  etaStep = 0.1;
  muStep  = 0.25;

  double _eGridHDB[maxHDeB]   = {1., 2., 3., 5., 9., 15., 20., 30., 50., 150.,
                                 225., 300. };
  double _eGridHDF[maxHDeF]   = {10., 30., 100., 300., 700., 1000., 3000.};
  double _eGridEM[maxEMe]     = {10., 30., 100., 300., 700., 1000., 3000.};
  double _eGridMU[maxMUe]     = {10., 30., 100., 300.};
  double _etaGridMU[maxMUeta] = {0.3, 0.6, 0.8, 1.25, 1.4, 3.0};


  // new HB parameters and eta-energy 

  double _meanHDB[maxHDeB][maxHDetaB] = {

    {0.476624 , 0.493062 , 0.459943 , 0.457669 , 0.429081 , 0.410654 , 0.336268 , 0.458954 , 0.439792 , 0.357874 , 0.360439 , 0.345815 , 0.349712 , 0.371513 , 0.362809 , 0.373754 , 0.360523 , 0.369565 , 0.395226 , 0.414336 , 0.461451 , 0.449395 , 0.441257 , 0.453467 , 0.451025 , 0.472335 , 0.496796 , 0.416431 , 0.464634 , 0.454192},

    {1.07501 , 1.08899 , 1.07219 , 1.03339 , 1.05252 , 1.02754 , 1.03136 , 0.993116 , 0.991691 , 0.934741 , 0.949417 , 0.921167 , 0.984902 , 1.04644 , 1.0273 , 1.03274 , 1.04837 , 1.03981 , 1.05575 , 1.04064 , 0.994231 , 1.04372 , 1.11736 , 1.05025 , 1.02541 , 1.10585 , 1.10235 , 1.1149 , 1.06456 , 0.951763 },

    {1.75267 , 1.76085 , 1.71224 , 1.67669 , 1.69225 , 1.66675 , 1.60665 , 1.62723 , 1.63054 , 1.5911 , 1.59027 , 1.59523 , 1.5011 , 1.60918 , 1.77619 , 1.7481 , 1.70181 , 1.65281 , 1.70343 , 1.78156 , 1.70135 , 1.79503 , 1.67103 , 1.71423 , 1.68329 , 1.81732 , 1.67877 , 1.80135 , 1.78928 , 1.56443},

    {3.18918 , 3.17163 , 3.22115 , 3.16199 , 3.119 , 3.12614 , 3.12964 , 3.08764 , 3.11962 , 3.02271 , 2.95358 , 2.93961 , 2.91184 , 2.87895 , 3.36679 , 3.46809 , 3.37874 , 3.24858 , 3.47626 , 3.44517 , 3.34893 , 3.40316 , 3.21936 , 3.20451 , 3.18082 , 3.3736 , 3.24415 , 3.23962 , 3.23106 , 2.8612  },


    {6.28886 , 6.32502 , 6.153 , 6.25144 , 6.04514 , 6.24791 , 6.03883 , 5.86917 , 6.0003 , 5.99398 , 5.7673 , 5.63033 , 5.77187 , 5.45172 , 5.72669 , 5.90017 , 6.64505 , 6.54108 , 6.48325 , 6.6204 , 6.73316 , 6.5678 , 6.51963 , 6.58446 , 6.54222 , 6.35741 , 6.36073 , 6.44069 , 6.18653 , 5.71932},

    {11.0468 , 11.0989 , 11.4924 , 11.1676 , 11.0245 , 11.0413 , 10.7327 , 10.5544 , 10.7685 , 10.6794 , 10.4369 , 10.3149 , 10.3694 , 9.75377 , 10.5567 , 10.7242 , 11.7882 , 11.9009 , 11.9322 , 11.8657 , 11.9197 , 12.0314 , 11.6992 , 11.6422 , 11.6797 , 11.3177 , 11.323 , 11.0776 , 10.6015 , 9.95773},

    {15.6749 , 15.6849 , 15.7885 , 15.6985 , 15.3807 , 15.2899 , 15.4558 , 15.2967 , 15.0239 , 14.962 , 14.6649 , 14.6423 , 14.2909 , 13.7475 , 14.7331 , 15.6338 , 16.653 , 16.4962 , 16.3308 , 16.595 , 16.4501 , 16.3102 , 16.387 , 16.4611 , 16.1331 , 15.7414 , 15.7312 , 15.5432 , 15.2701 , 13.8841},

    { 24.9762 , 24.9213 , 24.6044 , 24.5267 , 24.4909 , 24.5489 , 24.2173 , 23.7148 , 23.9466 , 23.6425 , 23.326 , 22.8739 , 23.0673 , 21.8652 , 23.6346 , 24.9795 , 25.7686 , 25.8465 , 26.0016 , 26.107 , 26.0871 , 25.7757 , 25.8498 , 25.9779 , 25.4736 , 25.1349 , 25.0872 , 24.6797 , 23.9769 , 22.3049 },
    // mean 50  ~44.7  ieta = 24
    {43.289 , 43.3581 , 42.9832 , 43.0558 , 42.8329 , 42.9838 , 42.1173 , 41.5308 , 41.4594 , 41.2223 , 40.767 , 40.761 , 39.9577 , 38.9879 , 41.8352 , 45.8239 , 45.3216 , 45.1074 , 45.2885 , 45.6472 , 44.7215 , 45.2696 , 44.7658 , 44.74 , 44.2714 , 43.578 , 43.4958 , 42.9913 , 41.7443 , 39.5586 },
    // mean 150 ~143.5 ieta = 24
    { 137.217 , 136.927 , 137.15 , 135.716 , 136.902 , 136.973 , 136.071 , 135.128 , 135.227 , 134.228 , 132.294 , 132.868 , 132.098 , 127.226 , 135.754 , 146.717 , 145.656 , 145.557 , 145.857 , 146.9 , 145.292 , 144.852 , 144.525 , 143.523 , 143.216 , 141.373 , 141.165 , 140.378 , 137.178 , 129.413 },

    {208.103 , 209.376 , 209.606 , 208.472 , 205.868 , 208.111 , 206.715 , 205.856 , 206.152 , 206.516 , 202.321 , 202.956 , 200.515 , 191.649 , 207.13 , 225.289 , 222.813 , 223.076 , 222.782 , 221.931 , 220.613 , 220.677 , 219.891 , 218.301 , 218.256 , 216.493 , 216.29 , 213.763 , 209.39 , 197.281},

    {281.567 , 280.388 , 282.927 , 280.474 , 279.958 , 278.27 , 279.849 , 276.586 , 278.721 , 277.068 , 271.767 , 274.943 , 272.199 , 262.063 , 278.626 , 303.726 , 299.117 , 299.115 , 297.693 , 299.998 , 297.985 , 298.479 , 296.6 , 295.734 , 294.307 , 292.796 , 294.147 , 289.703 , 284.105 , 270.241 }


  };

  double _sigmaHDB[maxHDeB][maxHDetaB] = {

    { 0.286294 , 0.314112 , 0.279911 , 0.292271 , 0.248129 , 0.27642 , 0.313233 , 0.796113 , 0.942607 , 0.608183 , 0.528481 , 0.445244 , 0.481548 , 0.481018 , 0.535551 , 0.564222 , 0.541623 , 0.514204 , 0.550931 , 0.705837 , 0.822402 , 0.857903 , 0.958402 , 1.01725 , 0.98673 , 1.01132 , 1.06599 , 0.917358 , 0.98859 , 1.00675 },
 
    { 0.787044 , 0.970294 , 0.78483 , 0.694544 , 0.934925 , 0.708963 , 0.784491 , 0.730501 , 0.775871 , 0.620378 , 0.703707 , 0.59877 , 0.950485 , 1.12665 , 1.07585 , 1.04795 , 1.26084 , 1.08748 , 1.17623 , 1.12611 , 1.00577 , 1.04906 , 1.20905 , 1.13386 , 1.01885 , 1.19903 , 1.17179 , 1.39038 , 1.62371 , 1.56872  },

    { 1.26803 , 1.26139 , 1.16506 , 1.10316 , 1.18992 , 1.14155 , 1.0352 , 1.24238 , 1.11302 , 1.09693 , 1.0742 , 1.18951 , 1.00337 , 1.43047 , 1.63705 , 1.58703 , 1.46269 , 1.48261 , 1.50411 , 1.48352 , 1.60814 , 1.66115 , 1.40656 , 1.46287 , 1.41863 , 1.65498 , 1.3766 , 1.72453 , 1.96812 , 1.99329 },

    {1.74196 , 1.65704 , 1.73762 , 1.59157 , 1.63089 , 1.62314 , 1.74948 , 1.90715 , 1.8201 , 1.6331 , 1.65894 , 1.92087 , 1.77277 , 1.75834 , 2.41532 , 2.37577 , 2.19557 , 2.13748 , 2.8007 , 2.27877 , 2.24879 , 2.35961 , 1.92939 , 1.91239 , 2.18873 , 2.14735 , 1.9647 , 2.07953 , 2.26013 , 2.55134 },

    {2.76842 , 2.51295 , 2.60748 , 2.71764 , 2.59004 , 2.73338 , 2.62758 , 2.44544 , 2.54971 , 2.68488 , 2.56979 , 2.54398 , 2.55975 , 2.49233 , 2.99266 , 3.263 , 3.16696 , 2.98336 , 2.81342 , 3.10926 , 3.2556 , 3.12001 , 2.89892 , 3.16743 , 3.14679 , 3.05551 , 2.91361 , 3.32305 , 3.02677 , 3.22967 },

    { 3.53359 , 3.54399 , 4.00711 , 3.57557 , 3.79477 , 3.68451 , 3.79727 , 3.66063 , 3.72521 , 3.85844 , 3.81058 , 3.72625 , 3.84935 , 3.60238 , 4.5362 , 5.16396 , 4.1082 , 4.36567 , 4.3332 , 4.0873 , 4.24325 , 4.30435 , 4.14673 , 4.29711 , 4.36524 , 4.07471 , 4.10639 , 3.71646 , 3.90567 , 4.32878 },

    { 4.52494 , 4.33244 , 4.58711 , 4.48315 , 4.379 , 4.47354 , 4.5651 , 4.77448 , 4.65679 , 4.59795 , 4.76155 , 4.71759 , 4.57076 , 4.62724 , 5.47952 , 6.19001 , 4.89337 , 4.89576 , 4.88666 , 5.02671 , 5.26075 , 4.69786 , 4.8569 , 5.13536 , 4.90902 , 4.71894 , 4.78662 , 4.64671 , 5.0563 , 4.82317 },

    { 5.85424 , 5.66142 , 5.77927 , 5.81356 , 5.98731 , 5.62031 , 6.0177 , 5.86444 , 6.02983 , 6.0795 , 6.28575 , 6.26118 , 6.1812 , 6.01756 , 7.0064 , 7.27844 , 6.10678 , 6.12749 , 6.25753 , 5.96585 , 6.04877 , 6.18682 , 6.01746 , 6.33158 , 6.00752 , 5.90898 , 6.07648 , 6.0165 , 6.22375 , 5.89738 },
    // sigma 50  ~8.6  ieta = 24
    { 8.0741 , 8.00088 , 8.08148 , 8.2901 , 8.11926 , 8.37254 , 8.171 , 8.48677 , 8.75446 , 8.64733 , 8.92317 , 9.27092 , 8.89571 , 8.77164 , 9.86803 , 9.86663 , 8.37179 , 8.2143 , 8.39946 , 8.9227 , 8.34289 , 8.556 , 8.50074 , 8.56543 , 8.20102 , 8.16168 , 8.23613 , 8.2901 , 8.47213 , 8.75543 },
    // sigma 150  ~15.6  ieta = 24
    { 16.6963 , 16.6611 , 16.0767 , 16.9969 , 18.2622 , 17.6547 , 18.4697 , 19.2527 , 18.9362 , 19.8006 , 20.5657 , 21.0375 , 20.346 , 18.7725 , 19.9886 , 17.7821 , 16.3089 , 16.1056 , 16.7162 , 16.3537 , 16.511 , 16.3392 , 15.6164 , 15.5667 , 15.9068 , 15.8766 , 16.3442 , 16.5438 , 17.0363 , 17.3889 },

    { 22.4873 , 22.1868 , 22.2549 , 23.511 , 23.9024 , 24.1333 , 25.1788 , 26.7251 , 25.2885 , 26.5808 , 27.7687 , 28.3316 , 28.0709 , 24.6996 , 27.164 , 22.9814 , 20.2851 , 21.0963 , 21.1259 , 21.3166 , 19.9723 , 21.8291 , 19.8662 , 20.7755 , 21.1349 , 20.1872 , 20.9016 , 20.8376 , 22.6607 , 26.0368 },

    { 28.9587 , 26.3684 , 28.5207 , 29.6606 , 29.7385 , 29.2363 , 32.054 , 34.0372 , 32.7868 , 33.8559 , 35.2544 , 36.9558 , 35.1701 , 31.9169 , 34.4549 , 26.7708 , 24.2234 , 24.8681 , 24.7431 , 26.3299 , 24.3928 , 25.5786 , 25.6245 , 24.6741 , 25.1689 , 24.8101 , 24.5289 , 25.2526 , 29.3835 , 33.338 }

  };

  // new HF parameters and eta-et grid 
  // sometimes - too precise, just like it came from printout ...  

  double _meanHDF[maxHDeF][maxHDetaF] = {

    { 7.57591 , 8.33004 , 7.83697 , 7.12823 , 7.96476 , 7.40117 , 7.79405 , 7.55665 , 7.70435 , 7.14182 , 7.67556 , 7.84583 , 7.55453 , 7.62121 , 7.04319 , 6.80797 , 7.48835 , 6.29018 , 5.53254 , 5.8641 },
   
    {  27.6418 , 27.8125 , 26.9753 , 27.0838 , 26.2757 , 27.5768 , 26.6758 , 26.339 , 25.4984 , 26.5209 , 25.4216 , 25.0822 , 26.4129 , 25.7823 , 23.3293 , 25.0636 , 23.0646 , 23.4203 , 20.3477 , 22.1853 },

    { 102.893 , 103.051 , 104.753 , 103.567 , 103.759 , 102.4 , 103.311 , 103.706 , 102.475 , 101.715 , 101.218 , 102.852 , 97.3419 , 94.9526 , 99.2764 , 94.1891 , 93.95 , 95.5811 , 86.8717 , 88.5269 },

    { 328.572 , 321.977 , 327.175 , 325.702 , 322.903 , 328.56 , 319.991 , 321.768 , 320.221 , 316.707 , 313.441 , 312.386 , 310.32 , 312.264 , 304.283 , 296.175 , 292.838 , 296.155 , 280.258 , 280.792 },

    { 787.509 , 785.007 , 781.866 , 775.069 , 774.054 , 782.198 , 774.966 , 767.002 , 766.461 , 756.732 , 763.592 , 756.682 , 748.805 , 743.501 , 732.646 , 725.772 , 721.646 , 710.744 , 684.092 , 671.511 },

    { 1130.51 , 1132.68 , 1122.05 , 1122.26 , 1121.82 , 1134.7 , 1111.66 , 1129.58 , 1112.18 , 1104.95 , 1095.94 , 1075.3 , 1076.28 , 1084.9 , 1070.55 , 1050.28 , 1040.29 , 1008.48 , 989.144 , 962.206 },

    { 3397.39 , 3407.38 , 3406.2 , 3370.96 , 3372.23 , 3360.26 , 3333.91 , 3347.55 , 3315.63 , 3303.05 , 3280.36 , 3256.76 , 3269.86 , 3233.37 , 3177.61 , 3156.6 , 3142.69 , 3048.8 , 3033.91 , 2901.93 }
  
  };


  double _sigmaHDF[maxHDeF][maxHDetaF] = {

    { 7.21428 , 7.2655 , 7.10897 , 6.44593 , 7.4628 , 7.09214 , 7.72143 , 7.04127 , 6.80605 , 6.71848 , 7.10968 , 7.50737 , 6.81337 , 7.03095 , 6.49406 , 6.5828 , 7.39794 , 6.59783 , 5.85046 , 6.42454 },

    { 13.4795 , 13.6844 , 13.1064 , 13.0285 , 11.9962 , 12.9159 , 12.3113 , 12.4975 , 12.3698 , 12.9622 , 12.5086 , 12.8897 , 12.9483 , 13.2713 , 11.9734 , 12.6523 , 11.7811 , 12.2943 , 14.0499 , 12.2862 },

    { 29.7048 , 30.6147 , 31.3521 , 30.8227 , 30.6437 , 30.7686 , 32.0622 , 30.487 , 31.604 , 32.7244 , 29.7642 , 31.98 , 29.7257 , 30.2833 , 30.9487 , 30.0073 , 29.1148 , 28.5399 , 28.8705 , 28.3978 },

    { 53.2783 , 52.044 , 53.5698 , 58.3353 , 53.4832 , 54.8112 , 58.9478 , 58.2511 , 56.576 , 56.0014 , 56.5491 , 57.2649 , 54.2963 , 57.4275 , 61.7037 , 53.4861 , 54.4125 , 54.4004 , 52.5522 , 55.4795 },

    { 99.8769 , 107.17 , 101.501 , 98.6464 , 105.424 , 98.7739 , 100.753 , 105.924 , 98.5924 , 108.963 , 102.736 , 107.325 , 109.969 , 105.851 , 101.558 , 112.298 , 108.995 , 125.931 , 105.68 },

    { 163.667 , 174.034 , 171.254 , 165.645 , 177.574 , 184.851 , 179.787 , 179.29 , 188.247 , 168.282 , 180.037 , 174.934 , 172.627 , 181.443 , 181.828 , 180.122 , 183.794 , 180.219 , 202.188 , 181.48 },

    { 288.224 , 318.123 , 335.645 , 347.205 , 317.727 , 331.226 , 319.015 , 335.482 , 335.65 , 333.37 , 343.217 , 376.658 , 363.492 , 346.877 , 344.02 , 360.171 , 346.01 , 322.359 , 334.337 , 329.984 }

  };


  // e-gamma mean response and sigma in HF 
  double _meanEM[maxEMe][maxEMeta] = 
    { 

      { 8.45992 , 8.63583 , 8.02996 , 8.97919 , 8.82544 , 8.25 , 8.07106 , 8.34085 , 7.94789 , 8.64608 , 8.54919 , 7.93515 , 8.24598 , 8.27851 , 6.99648 , 6.38774 , 1.96863 , 1.99444 , 0.463971 , 1.85597 },

      { 27.0178 , 27.5436 , 27.2402 , 27.033 , 27.3868 , 25.6628 , 27.281 , 28.5243 , 26.2394 , 26.6339 , 25.1889 , 26.7813 , 25.7826 , 25.489 , 25.5799 , 20.935 , 8.29258 , 7.65 , 1.63656 , 6.04511 },

      { 96.8961 , 94.332 , 94.5504 , 98.344 , 96.5179 , 93.4123 , 94.3248 , 93.201 , 93.8986 , 94.2246 , 94.5596 , 91.9108 , 91.6629 , 93.0828 , 90.2524 , 82.5382 , 38.8545 , 52.6627 , 4.11778 , 23.5803 },

      { 314.837 , 306.694 , 314.115 , 305.044 , 305.018 , 308.364 , 306.552 , 302.859 , 299.284 , 300.775 , 303.902 , 297.587 , 298.982 , 297.452 , 288.804 , 277.016 , 221.93 , 230.533 , 14.5807 , 76.9597 },

      { 780.491 , 779.594 , 764.682 , 777.392 , 772.422 , 773.774 , 765.144 , 758.003 , 756.913 , 753.151 , 761.062 , 742.631 , 736.608 , 745.717 , 718.31 , 704.612 , 676.861 , 676.872 , 30.3429 , 231.996 },

      { 1123.48 , 1138.38 , 1139.27 , 1133.34 , 1142.2 , 1120.54 , 1100.99 , 1113.59 , 1092.92 , 1105.28 , 1110.92 , 1079.62 , 1079.63 , 1073.68 , 1066.91 , 1029.93 , 936.019 , 975.08 , 55.2746 , 322.708 },

      { 3418.28 , 3448.06 , 3415.97 , 3346.42 , 3404.7 , 3374.67 , 3372.47 , 3351.11 , 3316 , 3337.46 , 3309.82 , 3247.97 , 3241.04 , 3271.23 , 3161.16 , 3121.28 , 3062.65 , 3175.16 , 126.643 , 982.95 }


    }; 

  double _sigmaEM[maxEMe][maxEMeta] =
    { 
      
      { 7.34058 , 7.08079 , 7.23214 , 8.06263 , 7.30624 , 7.08314 , 7.12469 , 7.18147 , 7.17955 , 7.28179 , 7.56118 , 7.0407 , 7.59106 , 7.49638 , 6.75414 , 7.17367 , 4.70368 , 4.42008 , 2.05961 , 4.8895 },

      {  12.9084 , 12.6178 , 12.4915 , 12.3874 , 13.2901 , 11.8766 , 11.9469 , 14.1489 , 12.9496 , 12.2286 , 12.3921 , 14.045 , 13.4044 , 13.6155 , 14.0631 , 14.6692 , 13.6443 , 13.6275 , 6.82313 , 11.9411 },

      { 33.2929 , 32.7502 , 31.5466 , 34.8465 , 34.189 , 30.3185 , 32.5117 , 35.5203 , 36.7793 , 32.5331 , 32.9382 , 33.3213 , 33.8772 , 34.2328 , 34.0775 , 40.2445 , 47.9594 , 48.9547 , 19.6232 , 43.8259 },

      { 65.6405 , 64.1397 , 62.6258 , 64.9663 , 63.5784 , 68.6845 , 68.383 , 63.4324 , 61.6744 , 61.3449 , 66.4723 , 66.5365 , 71.3094 , 69.2828 , 68.652 , 79.5868 , 123.485 , 123.836 , 59.2157 , 130.709 },

      { 126.574 , 131.893 , 129.359 , 138.861 , 127.105 , 143.23 , 141.569 , 136.978 , 141.678 , 136.704 , 151.253 , 149.417 , 150.685 , 140.786 , 148.325 , 161.185 , 262.263 , 167.322 , 146.05 , 337.333 },

      { 205.354 , 218.242 , 224.552 , 237.143 , 238.286 , 244.237 , 230.449 , 243.406 , 250.26 , 260.09 , 248.435 , 247.354 , 272.659 , 246.76 , 286.364 , 259.995 , 283.664 , 363.005 , 249.778 , 476.116 },

      { 399.332 , 426.213 , 439.176 , 461.66 , 480.137 , 438.204 , 437.622 , 451.579 , 509.787 , 487.307 , 482.618 , 514.53 , 554.844 , 506.243 , 525.057 , 584.281 , 613.339 , 537.544 , 620.743 , 1434. }


    }; 


  // MUON probability histos for bin size = 0.25 GeV (0-10 GeV, 40 bins)  

  double _responseMU[maxMUe][maxMUeta][maxMUbin] = {
    { 
      // 10 GeV
      {1, 0.999009, 0.999009, 0.999009, 0.997027, 0.981169, 0.949455, 0.922696, 0.863231, 0.782953, 0.665015, 0.555996, 0.443013, 0.322101, 0.230922, 0.162537, 0.114965, 0.0812686, 0.0604559, 0.0475719, 0.0376611, 0.0307235, 0.0178394, 0.0138751, 0.0099108, 0.00792864, 0.00792864, 0.00693756, 0.00396432, 0.00297324, 0.00297324, 0.00198216, 0.00198216, 0.00198216, 0.00198216, 0.00198216, 0.00198216, 0.00099108, 0.00099108, 0}, 
      {1, 1, 1, 0.998037, 0.993131, 0.976447, 0.908734, 0.843965, 0.698724, 0.546614, 0.437684, 0.336605, 0.245339, 0.177625, 0.123651, 0.077527, 0.063788, 0.0451423, 0.0353288, 0.0284593, 0.0245339, 0.0215898, 0.0186457, 0.0147203, 0.013739, 0.00981354, 0.00490677, 0.00490677, 0.00392542, 0.00294406, 0.00294406, 0.00196271, 0.00196271, 0.00196271, 0.00196271, 0.000981354, 0.000981354, 0.000981354, 0.000981354, 0}, 
      {1, 1, 1, 1, 0.99854, 0.986861, 0.964964, 0.941606, 0.870073, 0.747445, 0.640876, 0.562044, 0.426277, 0.344526, 0.255474, 0.191241, 0.135766, 0.109489, 0.0729927, 0.0525547, 0.0423358, 0.0291971, 0.0233577, 0.0175182, 0.0131387, 0.010219, 0.00437956, 0.00437956, 0.00437956, 0.00291971, 0.00145985, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
      {1, 1, 1, 1, 1, 1, 0.987603, 0.982782, 0.9573, 0.909091, 0.85124, 0.800275, 0.727961, 0.626722, 0.536501, 0.447658, 0.341598, 0.274793, 0.208678, 0.153581, 0.125344, 0.0971074, 0.0764463, 0.0633609, 0.0495868, 0.0371901, 0.0316804, 0.0213499, 0.0172176, 0.0144628, 0.0110193, 0.00964187, 0.00826446, 0.00757576, 0.00688705, 0.00413223, 0.00344353, 0.00206612, 0.00206612, 0.00206612}, 
      {1, 0.995918, 0.995918, 0.995918, 0.953061, 0.846939, 0.75102, 0.634694, 0.512245, 0.355102, 0.263265, 0.218367, 0.136735, 0.0836735, 0.0612245, 0.0469388, 0.0326531, 0.0265306, 0.0142857, 0.0102041, 0.00408163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0},
      {1, 0.999541, 0.999541, 0.999541, 0.996784, 0.977257, 0.977257, 0.909028, 0.909028, 0.769354, 0.698369, 0.698369, 0.455088, 0.244659, 0.244429, 0.129566, 0.129336, 0.0884448, 0.0443372, 0.0438778, 0.0238916, 0.0238916, 0.0130944, 0.00987824, 0.00735125, 0.00574317, 0.00436481, 0.00229727, 0.00229727, 0.00160809, 0.00160809, 0.00160809, 0.00114863, 0.00114863, 0.000459453, 0.000229727, 0.000229727, 0.000229727, 0.000229727, 0}
    },
    //30 GeV
    {
      {1, 1, 1, 1, 1, 0.997006, 0.981038, 0.963074, 0.928144, 0.852295, 0.761477, 0.662675, 0.538922, 0.399202, 0.299401, 0.234531, 0.190619, 0.154691, 0.125749, 0.0918164, 0.0718563, 0.0638723, 0.0568862, 0.0399202, 0.0319361, 0.0259481, 0.0199601, 0.0159681, 0.0149701, 0.011976, 0.00898204, 0.00798403, 0.00299401, 0.00299401, 0.00299401, 0.00299401, 0.00199601, 0.00199601, 0.000998004, 0.000998004}, 
      {1, 0.998024, 0.998024, 0.998024, 0.996047, 0.982213, 0.927866, 0.885375, 0.791502, 0.601779, 0.477273, 0.383399, 0.29249, 0.205534, 0.177866, 0.126482, 0.100791, 0.0790514, 0.0494071, 0.0375494, 0.0286561, 0.0217391, 0.0217391, 0.0158103, 0.0128458, 0.0118577, 0.0108696, 0.00988142, 0.00889328, 0.00889328, 0.00889328, 0.00790514, 0.00592885, 0.00592885, 0.00395257, 0.00197628, 0.000988142, 0.000988142, 0, 0}, 
      {1, 1, 1, 1, 1, 0.997041, 0.97929, 0.964497, 0.903846, 0.803254, 0.681953, 0.594675, 0.5, 0.360947, 0.298817, 0.214497, 0.150888, 0.130178, 0.0872781, 0.0695266, 0.0591716, 0.0384615, 0.0340237, 0.0251479, 0.0192308, 0.0177515, 0.0177515, 0.0162722, 0.0162722, 0.0133136, 0.010355, 0.00739645, 0.00443787, 0.00443787, 0.00443787, 0.00443787, 0.00295858, 0.00295858, 0, 0}, 
      {1, 1, 1, 1, 1, 0.999306, 0.997917, 0.997222, 0.986111, 0.953472, 0.901389, 0.865972, 0.775, 0.6625, 0.603472, 0.501389, 0.402083, 0.349306, 0.264583, 0.220833, 0.194444, 0.145139, 0.119444, 0.09375, 0.0763889, 0.0625, 0.0472222, 0.0368056, 0.0326389, 0.0291667, 0.0201389, 0.0145833, 0.0145833, 0.0111111, 0.00833333, 0.00694444, 0.00486111, 0.00416667, 0.00208333, 0.000694444}, 
      {1, 0.991803, 0.991803, 0.991803, 0.969262, 0.881148, 0.762295, 0.67623, 0.532787, 0.409836, 0.295082, 0.243852, 0.188525, 0.139344, 0.122951, 0.0778689, 0.0532787, 0.0471311, 0.0368852, 0.0286885, 0.0286885, 0.022541, 0.0184426, 0.0163934, 0.0163934, 0.0122951, 0.0122951, 0.00819672, 0.00819672, 0.00819672, 0.00819672, 0.00614754, 0.00204918, 0.00204918, 0.00204918, 0.00204918, 0, 0, 0, 0}, 
      {1, 0.999304, 0.999304, 0.999304, 0.998609, 0.990958, 0.990958, 0.952469, 0.952237, 0.819615, 0.777417, 0.777417, 0.563877, 0.332251, 0.332251, 0.190587, 0.190587, 0.157663, 0.0987712, 0.0987712, 0.061674, 0.061674, 0.0377927, 0.0352423, 0.0268954, 0.0257361, 0.0234176, 0.0132159, 0.0127521, 0.0106654, 0.0106654, 0.00881057, 0.00394157, 0.00394157, 0.00370971, 0.001623, 0.001623, 0.000927429, 0.000927429, 0.000695572}
    },
    // 100 GeV
    {
      {1, 1, 1, 1, 1, 0.998976, 0.9826, 0.971341, 0.947799, 0.895599, 0.804504, 0.73695, 0.650972, 0.520983, 0.417605, 0.350051, 0.286592, 0.239509, 0.192426, 0.146366, 0.125896, 0.106448, 0.0921187, 0.073695, 0.0593654, 0.0470829, 0.0388946, 0.0286592, 0.0225179, 0.0214944, 0.0184237, 0.0153531, 0.0143296, 0.011259, 0.00921187, 0.00716479, 0.00716479, 0.00511771, 0.00409417, 0.00307062}, 
      {1, 1, 1, 1, 0.999004, 0.989044, 0.949203, 0.922311, 0.850598, 0.724104, 0.600598, 0.510956, 0.432271, 0.334661, 0.293825, 0.217131, 0.184263, 0.164343, 0.123506, 0.10757, 0.0916335, 0.0717131, 0.062749, 0.0507968, 0.0418327, 0.0408367, 0.0328685, 0.0278884, 0.0268924, 0.0229084, 0.0199203, 0.0179283, 0.0149402, 0.0119522, 0.0109562, 0.00996016, 0.00896414, 0.00697211, 0.00498008, 0.00199203},
      {1, 1, 1, 1, 1, 0.994048, 0.985119, 0.974702, 0.934524, 0.827381, 0.708333, 0.639881, 0.5625, 0.450893, 0.392857, 0.316964, 0.258929, 0.235119, 0.162202, 0.130952, 0.120536, 0.0907738, 0.078869, 0.0669643, 0.0625, 0.0565476, 0.047619, 0.0357143, 0.0342262, 0.0282738, 0.0208333, 0.0208333, 0.016369, 0.0133929, 0.0119048, 0.0104167, 0.00446429, 0.00446429, 0.0014881, 0.0014881}, 
      {1, 0.999281, 0.999281, 0.999281, 0.999281, 0.999281, 0.997124, 0.996405, 0.991373, 0.966211, 0.925953, 0.893602, 0.839684, 0.742631, 0.695902, 0.597412, 0.507549, 0.470884, 0.38821, 0.327822, 0.295471, 0.230769, 0.199856, 0.166068, 0.136592, 0.119339, 0.0941769, 0.0754853, 0.069734, 0.0603882, 0.0510424, 0.0409777, 0.0359454, 0.0287563, 0.0201294, 0.0143781, 0.0107836, 0.00790798, 0.00647017, 0.00359454}, 
      {1, 0.997934, 0.997934, 0.997934, 0.987603, 0.92562, 0.834711, 0.77686, 0.654959, 0.518595, 0.409091, 0.353306, 0.299587, 0.216942, 0.206612, 0.161157, 0.130165, 0.123967, 0.0909091, 0.0867769, 0.0681818, 0.053719, 0.0433884, 0.035124, 0.0330579, 0.0330579, 0.0289256, 0.0268595, 0.0227273, 0.0227273, 0.0206612, 0.018595, 0.0123967, 0.0103306, 0.00619835, 0.00619835, 0.00413223, 0.00413223, 0.00206612, 0.00206612},
      {1, 1, 1, 1, 0.999765, 0.995535, 0.9953, 0.970153, 0.969918, 0.881316, 0.853584, 0.853584, 0.670035, 0.441833, 0.441833, 0.286251, 0.286016, 0.251234, 0.171798, 0.171798, 0.114454, 0.114454, 0.0777908, 0.0747356, 0.0571093, 0.0552291, 0.053349, 0.0293772, 0.0293772, 0.026557, 0.0249119, 0.0211516, 0.012691, 0.0115159, 0.0110458, 0.00658049, 0.00634548, 0.00305523, 0.00305523, 0.00188014}
    },
    // 300 GeV
    {
      {1, 1, 1, 1, 1, 0.997817, 0.99345, 0.983624, 0.966157, 0.933406, 0.873362, 0.815502, 0.74345, 0.64083, 0.558952, 0.479258, 0.419214, 0.379913, 0.325328, 0.270742, 0.239083, 0.19869, 0.167031, 0.141921, 0.113537, 0.0971616, 0.0840611, 0.0687773, 0.058952, 0.0436681, 0.0393013, 0.0327511, 0.0305677, 0.0262009, 0.0196507, 0.0163755, 0.0120087, 0.00982533, 0.00873362, 0.00327511}, 
      {1, 1, 1, 1, 1, 0.996865, 0.972832, 0.948798, 0.904911, 0.802508, 0.719958, 0.649948, 0.584117, 0.491118, 0.46604, 0.364681, 0.331243, 0.308255, 0.244514, 0.221526, 0.202717, 0.169279, 0.15256, 0.126437, 0.111808, 0.106583, 0.0909091, 0.0783699, 0.0710554, 0.0574713, 0.0501567, 0.045977, 0.0376176, 0.0344828, 0.030303, 0.0261233, 0.0188088, 0.0135841, 0.0104493, 0.00522466}, 
      {1, 1, 1, 1, 1, 0.998415, 0.993661, 0.988906, 0.977813, 0.935024, 0.858954, 0.812995, 0.762282, 0.662441, 0.622821, 0.505547, 0.445325, 0.426307, 0.329635, 0.29794, 0.275753, 0.242472, 0.22187, 0.191759, 0.171157, 0.158479, 0.141046, 0.125198, 0.120444, 0.110935, 0.0982567, 0.0792393, 0.0586371, 0.0570523, 0.0538827, 0.0459588, 0.0332805, 0.0269414, 0.0174326, 0.0110935}, 
      {1, 1, 1, 1, 1, 1, 0.998436, 0.997654, 0.992963, 0.982017, 0.960125, 0.94527, 0.906177, 0.845192, 0.820954, 0.744332, 0.671618, 0.624707, 0.538702, 0.475371, 0.441751, 0.36982, 0.333855, 0.291634, 0.250977, 0.22674, 0.199375, 0.1681, 0.150899, 0.137608, 0.110242, 0.0992963, 0.0820954, 0.0664582, 0.0578577, 0.0453479, 0.0359656, 0.0289289, 0.0187647, 0.00938233},
      {1, 1, 1, 1, 0.989177, 0.941558, 0.887446, 0.839827, 0.75974, 0.612554, 0.525974, 0.478355, 0.426407, 0.361472, 0.350649, 0.279221, 0.253247, 0.24026, 0.168831, 0.145022, 0.134199, 0.108225, 0.101732, 0.0779221, 0.0606061, 0.0562771, 0.0497835, 0.04329, 0.0411255, 0.034632, 0.034632, 0.0238095, 0.0194805, 0.017316, 0.017316, 0.0108225, 0.0108225, 0.0108225, 0.00649351, 0.00649351},
      {1, 0.99926, 0.99926, 0.99926, 0.99852, 0.995312, 0.995312, 0.984949, 0.984703, 0.925241, 0.915865, 0.915865, 0.798668, 0.630891, 0.630891, 0.484826, 0.484579, 0.455712, 0.356279, 0.356279, 0.268937, 0.268937, 0.196891, 0.191463, 0.154453, 0.149519, 0.146558, 0.0957316, 0.0949914, 0.0885764, 0.0880829, 0.0745127, 0.0478658, 0.0471256, 0.0446583, 0.0264002, 0.0261535, 0.0157908, 0.0152973, 0.00986923}
    }
  };

  // now assign
  for (int i1 = 0; i1 < maxMUe; i1++) { 
    eGridMU[i1] = _eGridMU[i1];
    for (int i2 = 0; i2 < maxMUeta; i2++) {
      for (int i3 = 0; i3 < maxMUbin; i3++) {
	
	responseMU[i1][i2][i3] = _responseMU[i1][i2][i3];

	if(debug) {
	  //cout.width(6);
	  LogDebug("FastCalorimetry") << " responseMU " << i1 << " " << i2 << " " << i3  << " = " 
				      << responseMU[i1][i2][i3] << std::endl;
	}
	
      }
    }
  }
  for (int i2 = 0; i2 < maxMUeta; i2++) {
    etaGridMU[i2] = _etaGridMU[i2];
  }


  // Normalize the response and sigmas to the corresponding energies
  for(int i = 0; i<maxHDeB;  i++) {
    eGridHDB[i] = _eGridHDB[i];
    for(int j = 0; j<maxHDetaB; j++) {
      meanHDB[i][j] =  _meanHDB[i][j] / eGridHDB[i];
      sigmaHDB[i][j] =  _sigmaHDB[i][j] / eGridHDB[i];
    }
  }

  for(int i = 0; i<maxHDeF;  i++) {
    eGridHDF[i] = _eGridHDF[i];
    for(int j = 0; j<maxHDetaF; j++) {
      meanHDF[i][j] =  _meanHDF[i][j] / eGridHDF[i];
      sigmaHDF[i][j] =  _sigmaHDF[i][j] / eGridHDF[i];
    }
  }



  for(int i = 0; i<maxEMe;  i++) {
    eGridEM[i] = _eGridEM[i];
    for(int j = 0; j<maxEMeta; j++) {
      meanEM[i][j] = _meanEM[i][j] / eGridEM[i];
      sigmaEM[i][j] = _sigmaEM[i][j] / eGridEM[i];
    }
  }


}

 
std::pair<double,double> 
HCALResponse::responseHCAL(double energy, double eta, int partype)
{

  int ieta = abs((int)(eta / etaStep)) ;
  int ie = -1;

  mean  = 0.;
  sigma = 0.;

  // e/gamma
  if(partype == 0) {
    ieta -=  maxHDetaB;  // HF starts at eta=3, but resp.vector from index = 0
    if(ieta >= maxEMeta ) ieta = maxEMeta;
    else if(ieta < 0) ieta = 0;
 
    for (int i = 0; i < maxEMe; i++) {
      if(energy < eGridEM[i])  {
	if(i == 0) ie = 0;       
        else  ie = i-1;
        break;
      }
    }
    if(ie == -1) ie = maxEMe - 2;  
    interEM(energy, ie, ieta);
  }  
     
  else { 
    // hadrons
    if(partype == 1) {  
      
      if(ieta > maxHDetaB)  {  // HF
	ieta -= maxHDetaB; 

	if(ieta >= maxHDetaF) ieta = maxHDetaF;
	
	for (int i = 0; i < maxHDeF; i++) {
	  if(energy < eGridHDF[i])  {
	    if(i == 0) ie = 0;     // less than minimal -
	    else  ie = i-1;        // back extrapolation with the 1st interval
	    break;
	  }
	}
	if(ie == -1) ie = maxHDeF - 2;     // more than maximum - 
	interHDF(energy, ie, ieta);         // extrapolation with last interv.
      }
      else { // HB-HE
	if(ieta <0 ) ieta = 0;
	for (int i = 0; i < maxHDeB; i++) {
	  if(energy < eGridHDB[i])  {
	    if(i == 0) ie = 0;     // less than minimal -
	    else  ie = i-1;        // back extrapolation with the 1st interval
	    break;
	  }	
	}
	if(ie == -1) ie = maxHDeB - 2;     // more than maximum - 
	interHDB(energy, ie, ieta);        // extrapolation with last interv.
   
	  // HE correction 
	if(ieta > 14) {
          mean  *= 1.05;
	  //  sigma *= 1.00;
        }
	

      }

      // finally apply energy scale correction
      mean  *= eResponseCorrection;
      mean  += eBias;
      sigma *= eResponseCorrection;      
      
    }
    
  }
  
  // muons
  if(partype == 2) { 
    
    ieta = maxMUeta;
    for(int i = 0; i < maxMUeta; i++) {
      if(fabs(eta) < etaGridMU[i]) {
	ieta = i;  
	break;
      }       
    }     
    
    if(ieta < 0) ieta = 0;
    if(ieta < maxMUeta) {  // HB-HE
      
      for (int i = 0; i < maxMUe; i++) {
	if(energy < eGridMU[i])  {
	  if(i == 0) ie = 0;     // less than minimal -
	  else  ie = i-1;        // back extrapolation with the first interval
	  break;
	}
	
	if(ie == -1) ie = maxMUe - 2;     // more than maximum - 
	interMU(energy, ie, ieta);         // extrapolation using the last interv
	if(mean > energy) mean = energy;  
	
      }
    }
  }
  
  // debugging
  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::responseHCAL, partype = " <<  partype 
				<< " E, eta = " << energy << " " << eta  
				<< "  mean & sigma = " << mean   << " " << sigma << std::endl;

  }

  return std::pair<double,double>(mean,sigma);

}

void HCALResponse::interMU(double e, int ie, int ieta)
{

  double x = random->flatShoot();

  int bin1 = maxMUbin;
  for(int i = 0; i < maxMUbin; i++) {
    if(x > responseMU[ie][ieta][i]) {
      bin1 = i-1;
      break;
    }
  }
  int bin2 = maxMUbin;
  for(int i = 0; i < maxMUbin; i++) {
    if(x > responseMU[ie+1][ieta][i]) {
      bin2 = i-1;
      break;
    }
  }
   
  double x1 = eGridMU[ie];
  double x2 = eGridMU[ie+1];
  double y1 = (bin1 + random->flatShoot()) * muStep;   
  double y2 = (bin2 + random->flatShoot()) * muStep;   

  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interMU  " << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }


  mean  = y1 + (y2-y1) * (e - x1)/(x2 - x1);
  sigma = 0.;

  if(debug) {
    //cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interMU " << std::endl
				<< " e, ie, ieta = " << e << " " << ie << " " << ieta << std::endl
				<< " response  = " << mean << std::endl; 
  }

}

void HCALResponse::interHDB(double e, int ie, int ieta)
{
  double y1 = meanHDB[ie][ieta]; 
  double y2 = meanHDB[ie+1][ieta]; 
  double x1 = eGridHDB[ie];
  double x2 = eGridHDB[ie+1];

  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDB mean " << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }
  
  mean =  e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      
  
  y1 = sigmaHDB[ie][ieta]; 
  y2 = sigmaHDB[ie+1][ieta]; 
  
  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDB sigma" << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }
 
  sigma = e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      


  if(debug) {
    //cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDB " << std::endl
				<< " e, ie, ieta = " << e << " " << ie << " " << ieta << std::endl
				<< " mean, sigma  = " << mean << " " << sigma << std::endl ;
  }

}


void HCALResponse::interHDF(double e, int ie, int ieta)
{
  double y1 = meanHDF[ie][ieta]; 
  double y2 = meanHDF[ie+1][ieta]; 
  double x1 = eGridHDF[ie];
  double x2 = eGridHDF[ie+1];

  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDF mean " << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl;
  
  }
  
  mean =  e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      
  
  y1 = sigmaHDF[ie][ieta]; 
  y2 = sigmaHDF[ie+1][ieta]; 
  
  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDF sigma" << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }
 
  sigma = e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      


  if(debug) {
    //cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interHDF " << std::endl
				<< " et, ie, ieta = " << e << " " << ie << " " << ieta << std::endl
				<< " mean, sigma  = " << mean << " " << sigma << std::endl; 
  }

}

void HCALResponse::interEM(double e, int ie, int ieta)
{ 
  double y1 = meanEM[ie][ieta]; 
  double y2 = meanEM[ie+1][ieta]; 
  double x1 = eGridEM[ie];
  double x2 = eGridEM[ie+1];
  
  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interEM mean " << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }

  mean =  e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      
  
  y1 = sigmaEM[ie][ieta]; 
  y2 = sigmaEM[ie+1][ieta]; 
  
  if(debug) {
    //  cout.width(6);
    LogDebug("FastCalorimetry") << std::endl
				<< " HCALResponse::interEM sigma" << std::endl
				<< " x, x1-x2, y1-y2 = " 
				<< e << ", " << x1 <<"-" << x2 << " " << y1 <<"-" << y2 << std::endl; 
  
  }

  sigma = e * (y1 + (y2 - y1) * (e - x1)/(x2 - x1));      
}



// Old parametrization for hadrons
double HCALResponse::getHCALEnergyResolution(double e, int hit){
  
  if(hit==hcforward) 
    return e *sqrt( RespPar[VFCAL][1][0]*RespPar[VFCAL][1][0] / e + 
		    RespPar[VFCAL][1][1]*RespPar[VFCAL][1][1] );
  else
    return  e * sqrt( RespPar[HCAL][hit][0]*RespPar[HCAL][hit][0]/(e)
		      + RespPar[HCAL][hit][1]*RespPar[HCAL][hit][1]);   

}

// Old parameterization of the calo response to hadrons
double HCALResponse::getHCALEnergyResponse(double e, int hit){

  double s = eResponseScale[hit];
  double n = eResponseExponent;
  double p = eResponsePlateau[hit];
  double c = eResponseCoefficient;

  double response = e * p / (1+c*exp(n * log(s/e)));

  if(response<0.) response = 0.;

  return response;
}

// old parameterization of the HF response to electrons
double HCALResponse::getHFEnergyResolution(double EGen)
{
  return EGen *sqrt( RespPar[VFCAL][0][0]*RespPar[VFCAL][0][0] / EGen + 
		     RespPar[VFCAL][0][1]*RespPar[VFCAL][0][1] );
}  


#ifdef IJKLMMNOP // This stuff was moved to Calorimetry.cfi
// default values for old prameterizations
const double HCALResponse::RespByDefault[3][2][3] = {
  //Numbers taken from Calorimetry TDR's.
  //ECAL - dummy, the numbers are in  FASTEnergyReconstructor.cc
  //Stochastic Constant Noise 
  {{3*0.},   //barrel
   {3*0.}}, //endcap
  //HCAL
  {{1.22,       0.05,       0},    //barrel
   {1.30,       0.05,       0}},   //endcap (R. Harris - 17-oct-2004)
  //VFCAL
  {{1.82,       0.09,    0},  //hadrons
   {1.38,       0.05,    0}}  //electrons & photons
};
#endif
