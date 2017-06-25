#include "ImagePalettes.hh" 
#include "TStyle.h"
#include "TRandom3.h"

#include "TColor.h"
#include <cmath>

void 
dmtpc::image::palette::setGrayscale(double p)
{
  double stops[5] = {0,0.25,0.5,0.75,1.0};
  double red[5], green[5], blue[5];
  for (int i = 0; i<5; i++)
  {
    double pc = 1 - pow(stops[i],p);
    red[i] = pc;
    green[i] = pc;
    blue[i] = pc;
  }

  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setGreen(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = 0;
    green[i] = pc;
    blue[i] = 0;
  }
  red[5] = 1, green[5]=1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);


}

void 
dmtpc::image::palette::setBlue(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = 0;
    green[i] = 0;
    blue[i] = pc;
  }
  red[5] = 1, green[5] = 1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setRed(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = pc;
    green[i] = 0;
    blue[i] = 0;
  }
  red[5] = 1, green[5] = 1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setCyan(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = 0;
    green[i] = pc;
    blue[i] = pc;
  }
  green[5] = 1, red[5] =1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setMagenta(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = pc;
    green[i] = 0;
    blue[i] = pc;
  }
  red[5] = 1, green[5] = 1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setYellow(double p)
{
  double stops[6] = {0,0.25,0.5,0.75,0.95,1.0};
  double red[6], green[6], blue[6];
  for (int i = 0; i<5; i++)
  {
    double pc = pow(stops[i],p);
    red[i] = pc;
    green[i] = pc;
    blue[i] = 0;
  }
  red[5] = 1, green[5] = 1, blue[5] = 1;
  TColor::CreateGradientColorTable(6,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setStandard1()
{
  double stops[5] = {0,0.34,0.61,0.84,1.0};
  double red[5] = {0.0,0.0,0.87,1.0,0.51};
  double green[5] = {0.0,0.81,1.00,0.2,0.0};
  double blue[5] = {0.51,1.0,0.12,0.0,0.0};
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setPalette1()
{
  double stops[5] = {0,0.34,0.61,0.84,1.0};
  double red[5] = {0.0,0.09,0.18,0.09,0.00};
  double green[5] = {0.01,0.02,0.39,0.68,0.97};
  double blue[5] = {0.17,0.39,0.62,0.79,0.97};
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setPalette2()
{
  double stops[3] = {0,0.5,1.0};
  double red[3] = {1,0.5,0};
  double green[3] = {0.5,0,1};
  double blue[3] = {1,0,0.5};
  TColor::CreateGradientColorTable(3,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setPalette3()
{
  double stops[3] = {0,0.5,1.0};
  double red[3] = {0,0.5,1};
  double green[3] = {0.5,1,0};
  double blue[3] = {0,1,.5};
  TColor::CreateGradientColorTable(3,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setHot()
{
  int NCOL=26;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    red[i] = i>8?1:i/9.;
    green[i] = i<10?0:1;
    if (i>9&&i<18) green[i] = (i-9.)/9.;
    blue[i] = i<19?0:(i-18.)/7.;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setCool(double p)
{
  int NCOL=10;
  double r1 = 1.0;
  double g1 = 0.0;
  double b1 = 1.0;

  double r2 = 0;
  double g2 = 1;
  double b2 = 1;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}


void 
dmtpc::image::palette::setJet()
{

  const int NCOL=100;
  double stops[NCOL],red[NCOL],green[NCOL],blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    //Red
    if (i<38) red[i] = 0;
    else if (i<62) red[i] = 0.04*(i-37);
    else if (i<88) red[i] = 1;
    else red[i] = 1 - 0.04*(i-87);
    //Green
    if (i<13||i>86) green[i] = 0;
    else if (i<63&&i>36) green[i] = 1;
    else if (i<63) green[i] = 0.04*(i-12);
    else green[i] = 1. - 0.04*(i-62);
    //Blue
    if (i<38) blue[99-i] = 0;
    else if (i<62) blue[99-i] = 0.04*(i-37);
    else if (i<88) blue[99-i] = 1;
    else blue[99-i] = 1 - 0.04*(i-87);
  }
  //for (int i = 0 ; i < NCOL; i++)
  //{
  //  cout <<i<<"\t"<<red[i] <<"\t"<<green[i]<<"\t"<<blue[i]<<endl;
  //}
  const int NCOL2=26;
  double st[NCOL2],b[NCOL2],r[NCOL2],g[NCOL2];
  for (int i = 0 ; i<NCOL2-1; i++)
  {
    st[i] = stops[4*i];
    r[i] = red[4*i];
    g[i] = green[4*i];
    b[i] = blue[4*i];
  }
  st[NCOL2-1] = stops[NCOL-1];
  r[NCOL2-1] = red[NCOL-1];
  g[NCOL2-1] = green[NCOL-1];
  b[NCOL2-1] = blue[NCOL-1];
  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setCopper()
{

  const int NCOL=25;
  double stops[NCOL];
  double red[NCOL] = {0,0.0521,0.1042,0.1562,0.2083,0.2604,
    0.3125,0.3646,0.4167,0.4688,0.5208,0.5729,0.6250,
    0.6771,0.7292,0.7812,0.8333,0.8854,0.9375,0.9896,
    1.0000,1.0000,1.0000,1.0000,1.0000};
  double green[NCOL] ={0,0.0325,0.0651,0.0977,0.1302,0.1628,
    0.1953,0.2279,0.2604,0.2929,0.3255,0.3580,
    0.3906,0.4231,0.4557,0.4883,0.5208,0.5534,
    0.5859,0.6184,0.6510,0.6835,0.7161,0.7487,0.7812};
  double blue[NCOL] ={0,0.0207,0.0415,0.0622,0.0829,0.1036,
    0.1244,0.1451,0.1658,0.1866,0.2073,0.2280,0.2487,
    0.2695,0.2902,0.3109,0.3317,0.3524,0.3731,0.3939,
    0.4146,0.4353,0.4560,0.4768,0.4975};
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setBone()
{
  const int NCOL=25;

  double stops[NCOL];
  double red[NCOL] = {0,0.0365,0.0729,0.1094,0.1458,0.1823,
    0.2188,0.2552,0.2917,0.3281,0.3646,0.4010,0.4375,0.4740,
    0.5104,0.5469,0.5833,0.6198,0.6741,0.7284,0.7827,0.8371,
    0.8914,0.9457,1.000};
  double green[NCOL] = {0,0.0365,0.0729,0.1094,0.1458,0.1823,
    0.2188,0.2552,0.2917,0.3420,0.3924,0.4427,0.4931,0.5434,
    0.5938,0.6441,0.6944,0.7448,0.7812,0.8177,0.8542,
    0.8906,0.9271,0.9635,1.0000};
  double blue[NCOL] = {0.0139,0.0642,0.1146,0.1649,0.2153,0.2656,
    0.3160,0.3663,0.4167,0.4531,0.4896,0.5260,0.5625,0.5990,
    0.6354,0.6719,0.7083,0.7448,0.7812,0.8177,0.8542,0.8906,
    0.9271,0.9635,1.0000};
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setWinter(double p)
{
  int NCOL=10;
  double r1 = 0.0;
  double g1 = 1.0;
  double b1 = 0.5;

  double r2 = 0;
  double g2 = 0;
  double b2 = 1;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setSpring(double p)
{
  int NCOL = 10;
  double r1 = 1;
  double g1 = 1;
  double b1 = 0;

  double r2 = 1;
  double g2 = 0;
  double b2 = 1;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setSummer(double p)
{
  int NCOL = 10;
  double r1 = 1;
  double g1 = 1;
  double b1 = 0.5;

  double r2 = 0;
  double g2 = 0.5;
  double b2 = 0.4;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setAutumn(double p)
{
  int NCOL = 10;
  double r1 = 1;
  double g1 = 1;
  double b1 = 0;

  double r2 = 1;
  double g2 = 0;
  double b2 = 0;

  double stops[NCOL];
  double red[NCOL], green[NCOL], blue[NCOL];
  for (int i = 0; i<NCOL; i++){
    stops[i] = i/(NCOL-1.);
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(NCOL,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setMIT(double p)
{
  //Maroon
  double r1 = 163./255;
  double g1 = 38./255;
  double b1 = 56./255;

  //Gray
  double r2 = 130./255;
  double g2 = 127./255;
  double b2 = 119./255;

  double stops[5] = {0,0.25,0.50,0.75,1.0};
  double red[5], green[5], blue[5];
  for (int i = 0; i<5; i++){
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = r1*p1+r2*p2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }

  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setColumbia(double p)
{
  double stops[5] = {0,0.25,0.5,0.75,1.0};

  //White
  double r2 = 1, g2 = 1, b2 = 1;

  //Pantone 292
  double r1 = 117./255;
  double g1 = 178./255;
  double b1 = 221./255;
/*
  //Pantone 290
  double rmax = 196./255;
  double gmax = 216./255;
  double bmax = 226./255;
  

  //Pantone 279
  double rmax = 102./255;
  double gmax = 137./255;
  double bmax = 204./255;
*/

  double red[5], green[5], blue[5];
  for (int i = 0; i<5; i++){
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = p1*r1+p2*r2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setBU(double p)
{
  double stops[5] = {0,0.25,0.5,0.75,1.0};

  //White
  double r2 = 1, g2 = 1, b2 = 1;

  //Red
  double r1 = 204./255;
  double g1 = 0;
  double b1 = 0;

  double red[5], green[5], blue[5];
  for (int i = 0; i<5; i++){
    double p1 = pow(stops[i],p);
    double p2 = 1-p1;

    red[i] = p1*r1+p2*r2;
    green[i] = g1*p1+g2*p2;
    blue[i] = b1*p1+b2*p2;
  }
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}

void 
dmtpc::image::palette::setRandom(int n)
{
  TRandom3 rnd(0);
  double stops[n], green[n], blue[n], red[n];
  for (int i = 0; i < n; i++)
  {
    stops[i] = i/(n-1.);
    green[i] = rnd.Rndm();
    blue[i] = rnd.Rndm();
    red[i] = rnd.Rndm();
  }
  blue[99] = 1, red[99]= 1;
  TColor::CreateGradientColorTable(n,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);

}


