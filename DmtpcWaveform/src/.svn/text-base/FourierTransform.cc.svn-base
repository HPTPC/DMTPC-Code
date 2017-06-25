/** \file FourierTransform.cc
 \brief Implementation of dmtpc::waveform::FourierTransform class
 \author Jeremy Lopez
 */
#include "FourierTransform.hh"
#include "fftw3.h"

dmtpc::waveform::FourierTransform::FourierTransform(int n,unsigned flag) : fN(n), fFlag(flag), fFFTisDone(false), fUseFilter(false)
{
  fData = (double*) fftw_malloc(n * sizeof(double));
  fIfft = (double*) fftw_malloc(n * sizeof(double));
  fCxOut = (fftw_complex*) fftw_malloc((n/2+1) * sizeof(fftw_complex));
  fCxFil = (fftw_complex*) fftw_malloc((n/2+1) * sizeof(fftw_complex));
  fCxIn = (fftw_complex*) fftw_malloc((n/2+1) * sizeof(fftw_complex));
  fPlan = fftw_plan_dft_r2c_1d(fN,fData,fCxOut,flag);
  fIPlan = fftw_plan_dft_c2r_1d(fN,fCxIn,fIfft,flag);
  fOutputType = 0;
}

dmtpc::waveform::FourierTransform::~FourierTransform()
{
  fftw_destroy_plan(fPlan);
  fftw_destroy_plan(fIPlan);
  fftw_free(fData);
  fftw_free(fCxOut);
  fftw_free(fCxFil);
  fftw_free(fCxIn);
  fftw_free(fIfft);
}

void
dmtpc::waveform::FourierTransform::setWaveform(const double* wf)
{
  fFFTisDone=false;
  for (int i = 0; i<fN; i++)	
    fData[i] = wf[i];
}

void
dmtpc::waveform::FourierTransform::setWaveform(const float* wf)
{
  fFFTisDone=false;
  for (int i = 0; i<fN; i++)	
    fData[i] = wf[i];
}

void
dmtpc::waveform::FourierTransform::setFFT(const fftw_complex* cx)
{
  for (int i = 0; i < (fN/2+1);i++)
  {	
    fCxOut[i][0] = cx[i][0];
    fCxOut[i][1] = cx[i][1];	
    fCxFil[i][0] = cx[i][0];
    fCxFil[i][1] = cx[i][1];	
  }
  fFFTisDone = true;
}

void
dmtpc::waveform::FourierTransform::setFFT(const double* r,const double* im)
{
  for (int i = 0; i < (fN/2+1);i++)	
  {
    fCxOut[i][0] = r[i];
    fCxOut[i][1] = im[i];
    fCxFil[i][0] = r[i];
    fCxFil[i][1] = im[i];
  }
  fFFTisDone = true;
}

void 
dmtpc::waveform::FourierTransform::fft()
{
  if (!fFFTisDone)
    fftw_execute(fPlan);
  fFFTisDone = true;
  applyFilter();
}

void
dmtpc::waveform::FourierTransform::fft(const double* wf)
{setWaveform(wf); fft();}


void 
dmtpc::waveform::FourierTransform::fftInverse(){
  for (int i = 0; i < (fN/2+1); i++)
  {
    fCxIn[i][0] = fCxFil[i][0];
    fCxIn[i][1] = fCxFil[i][1];
  }
  fftw_execute(fIPlan);
  fOutputType = 0;
  for (int i = 0; i < fN; i++)
    fIfft[i] /= fN;
}

void
dmtpc::waveform::FourierTransform::fftInverse(const fftw_complex* cx)
{
  setFFT(cx);
  fftInverse();
}

void
dmtpc::waveform::FourierTransform::fftInverse(const double* r,const double* i)
{
  setFFT(r,i);
  fftInverse();
}

const double*
dmtpc::waveform::FourierTransform::autocorrelation()
{
  fft();
  for (int i = 0; i < (fN/2+1);i++)
  {
    double re = fCxFil[i][0];
    double im = fCxFil[i][1];
    fCxIn[i][0] = re*re+im*im;
    fCxIn[i][1] = 0;
  }
  fOutputType = 1;
  fftw_execute(fIPlan);
  for (int i = 0; i < fN; i++)
    fIfft[i] /= fN;
  return fIfft;
}

const double*
dmtpc::waveform::FourierTransform::autocorrelation(const double* wf)
{
  setWaveform(wf); 
  return autocorrelation();
}

const double*
dmtpc::waveform::FourierTransform::spectralDensity()
{
  fft();
  for (int i = 0; i < (fN/2+1);i++)
  {
    double re = fCxFil[i][0];
    double im = fCxFil[i][1];
    fIfft[i] = re*re+im*im;
  }
  fOutputType = 2;
  return fIfft;
}

const double*
dmtpc::waveform::FourierTransform::spectralDensity(const double* wf)
{
  setWaveform(wf); 
  return spectralDensity();
}
    
void 
dmtpc::waveform::FourierTransform::setFilter(const double* x)
{
  fUseFilter = true;
  fFilterReal.resize(fN/2+1);
  fFilterIm.resize(fN/2+1);
  for (int i = 0; i < (fN/2+1); i++)
  {
    fFilterReal[i] = x[i];
    fFilterIm[i] = 0;
  }
}


void 
dmtpc::waveform::FourierTransform::setFilter(const double* x, const double* y)
{
  fUseFilter = true;
  fFilterReal.resize(fN/2+1);
  fFilterIm.resize(fN/2+1);
  for (int i = 0; i < (fN/2+1); i++)
  {
    fFilterReal[i] = x[i];
    fFilterIm[i] = y[i];
  }
}

void 
dmtpc::waveform::FourierTransform::setFilterPolar(const double* r, const double* phi)
{
  fUseFilter = true;
  fFilterReal.resize(fN/2+1);
  fFilterIm.resize(fN/2+1);
  for (int i = 0; i < (fN/2+1); i++)
  {
    fFilterReal[i] = r[i]*cos(phi[i]);
    fFilterIm[i] = r[i]*sin(phi[i]);
  }
}




void 
dmtpc::waveform::FourierTransform::setFilter(const fftw_complex* x)
{
  fUseFilter = true;
  fFilterReal.resize(fN/2+1);
  fFilterIm.resize(fN/2+1);
  for (int i = 0; i < (fN/2+1); i++)
  {
    fFilterReal[i] = x[i][0];
    fFilterIm[i] = x[i][1];
  }
}

void
dmtpc::waveform::FourierTransform::multiply(const double* x)
{
  for (int i = 0; i < (fN/2+1); i++)
  {
    fCxFil[i][0] *= x[i];
    fCxFil[i][1] *= x[i];
  }
}

void
dmtpc::waveform::FourierTransform::multiply(const double* x, const double* y)
{
  for (int i = 0; i < (fN/2+1); i++)
  {
    double re = fCxFil[i][0], im = fCxFil[i][1];
    fCxFil[i][0] *= re*x[i] - im*y[i] ;
    fCxFil[i][1] *= x[i] * im + y[i] * re;
  }
}

void
dmtpc::waveform::FourierTransform::multiply(const fftw_complex* x)
{
  for (int i = 0; i < (fN/2+1); i++)
  {
    double re = fCxFil[i][0], im = fCxFil[i][1];
    fCxFil[i][0] *= re*x[i][0] - im*x[i][1] ;
    fCxFil[i][1] *= x[i][0] * im + x[i][1] * re;
  }
}

void
dmtpc::waveform::FourierTransform::convolution(const double* x)
{
  fft();
  multiply(x);
  fftInverse();
}

void
dmtpc::waveform::FourierTransform::convolution(const double* x,const double* y)
{
  fft();
  multiply(x,y);
  fftInverse();
}

void
dmtpc::waveform::FourierTransform::convolution(const fftw_complex* x)
{
  fft();
  multiply(x);
  fftInverse();
}

void
dmtpc::waveform::FourierTransform::applyFilter()
{
  if (fUseFilter&&(int)fFilterReal.size()<fN/2+1){
    for (int i = 0 ; i < fN/2 + 1; i++)
    {
      fCxFil[i][0] = fFilterReal[i] * fCxOut[i][0] - fFilterIm[i] * fCxOut[i][1];
      fCxFil[i][1] = fFilterIm[i] * fCxOut[i][1] + fFilterReal[i] * fCxOut[i][0];
    }
  } else {
    for (int i = 0 ; i < fN/2 + 1; i++)
    {
      fCxFil[i][0] = fCxOut[i][0];
      fCxFil[i][1] = fCxOut[i][1];
    }
  }
}

void
dmtpc::waveform::FourierTransform::clearFilter()
{
  fFilterReal.clear();
  fFilterIm.clear();
}

void
dmtpc::waveform::FourierTransform::setWaveform(int n,double x)
{
  fData[n] = x;
  fFFTisDone = false;
}

void
dmtpc::waveform::FourierTransform::setFFT(int n, double x,double y)
{
  fCxOut[n][0] = x;
  fCxOut[n][1] = y;
  fFFTisDone = true;
}

void
dmtpc::waveform::FourierTransform::setFFT(int n, fftw_complex x)
{
  fCxOut[n][0] = x[0];
  fCxOut[n][1] = x[1];
  fFFTisDone = true;
}

void
dmtpc::waveform::FourierTransform::setFilteredFFT(int n, double x,double y)
{
  fCxFil[n][0] = x;
  fCxFil[n][1] = y;
  fFFTisDone = true;
}

void
dmtpc::waveform::FourierTransform::setFilteredFFT(int n, fftw_complex x)
{
  fCxFil[n][0] = x[0];
  fCxFil[n][1] = x[1];
  fFFTisDone = true;
}
