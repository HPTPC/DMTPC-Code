#include "ParametricVonMisesHTTest.hh" 
#include <alloca.h>
#include "Angular.hh"
#include "TMath.h"
#include "TDecompChol.h"
#include <assert.h>
#include "TF1.h" 


static bool hasLimits(const TF1* f, int i)
{

  double up,down; 
  f->GetParLimits(i,down,up); 

  return ((up || down) && (up != down)); 

}



static bool isFixed(const TF1* f, int i)
{

  double up,down; 
  f->GetParLimits(i,down,up); 

  return (up == down) && up;  

}

dmtpc::math::ParametricVonMisesHTTest::~ParametricVonMisesHTTest()
{

  delete params; 
  delete param_errs; 
  delete param_names; 
  delete minfn; 
  delete min; 
  delete epsilons; 

}

void dmtpc::math::ParametricVonMisesHTTest::setEpsilons(const double * eps) 
{
  memcpy(epsilons,eps,sizeof(*epsilons)*ndim);
}

dmtpc::math::ParametricVonMisesHTTest::ParametricVonMisesHTTest(unsigned ndim, 
                                                              const TF1 * width_fn,
                                                              const TF1 * ht_fn, 
                                                              const TF1 * normalization_fn,
                                                              const TF1 * mean_fn)
    : ndim(ndim), width_fn(width_fn),
      ht_fn(ht_fn), normalization_fn(normalization_fn), 
      mean_fn(mean_fn)  , rand(0) , X_upper_limits(ndim-1), 
      X_lower_limits(ndim-1,0), X_upper_limits_enable(ndim-1,false),
      X_lower_limits_enable(ndim-1,true)
{

  ntrials = (int) (2*pow(10.,ndim)); 
  max_attempts = 100; 
  min = new ROOT::Minuit2::Minuit2Minimizer;
  assert(ndim > 1); 
  assert(width_fn); 
  assert(ht_fn); 
  assert(normalization_fn); 
  epsilons = new double[ndim]; 
  for (unsigned i = 0; i < ndim; i++) epsilons[i] = 1.e-2; 


  nparams  = width_fn->GetNpar() + ht_fn->GetNpar() + normalization_fn->GetNpar() + (mean_fn ? mean_fn->GetNpar() : 0); 

  params = new double[nparams]; 
  param_errs = new double[nparams]; 
  param_names = new const char * [nparams]; 
  minfn = new ROOT::Math::Functor(this, &dmtpc::math::ParametricVonMisesHTTest::minFn, nparams); 
  min->SetFunction(*minfn); 

  min->SetPrintLevel(10); 


  int parctr = 0; 
  for (int f = 0; f < 4; f++)
  {

    const TF1 * fn  = f == 0 ? width_fn : 
                      f == 1 ? ht_fn : 
                      f == 2 ? normalization_fn : 
                      mean_fn; 

    if (!fn) continue; 

    for (int i = 0; i < fn->GetNpar(); i++)
    {

      param_names[parctr] = fn->GetParName(i); 
      params[parctr] = fn->GetParameter(i); 
      param_errs[parctr] = fn->GetParError(i); 

      if (hasLimits(fn,i))
      {
        double up,down; 
        fn->GetParLimits(i, down,up); 
        min->SetLimitedVariable(parctr++,fn->GetParName(i), fn->GetParameter(i), fn->GetParError(i), down, up); 
        printf("Adding limited variable %s with value %f, step size %f, limits [%f,%f]\n", fn->GetParName(i), fn->GetParameter(i), fn->GetParError(i),down,up); 
      }
      else if (isFixed(fn,i))
      {
        min->SetFixedVariable(parctr++, fn->GetParName(i), fn->GetParameter(i)); 
        printf("Adding fixed variable %s with value %f, \n", fn->GetParName(i), fn->GetParameter(i)); 

      }
      else
      {
        min->SetVariable(parctr++,fn->GetParName(i), fn->GetParameter(i), fn->GetParError(i)); 
        printf("Adding normal variable %s with value %f, step size %f \n", fn->GetParName(i), fn->GetParameter(i), fn->GetParError(i)); 

      }
    }
  }

}

double dmtpc::math::ParametricVonMisesHTTest::likelihood(int n, const Entry * entries,
                                                         const double * par,
                                                         double * fs, double * d2fs) const 
{

  return exp(loglikelihood(n,entries,par, fs,d2fs));
}

double dmtpc::math::ParametricVonMisesHTTest::loglikelihood(int n, const Entry * entries,
                                                         const double * par,
                                                         double * fs, double * d2fs) const 
{


  if (!par) par = params; 
  double logl = 0; 
  double N = 0;


  TDecompChol chol (ndim,ndim) ; 
  TMatrixDSym C(ndim); 
  TVectorD z(ndim); 
  TVectorD u(ndim); 
  TVectorD v(ndim); 

  for (int i = 0; i < n; i++)
  {
    C.Use(ndim, entries[i].covar); 
    chol.SetMatrix(C); 
    if (!chol.Decompose() )
      C.Print(); 

    u[0] = entries[i].phi; 

    for (int d = 1; d < ndim; d++)
    {
        u[d] = entries[i].X[d]; 
    }

    double mean = 0; 
    double rms = 0;
    for (int j = 0; j < ntrials; j++)
    {
//      if (rand.Uniform() < entries[i]->phi_ht)
//      {
//        u[0] = dmtpc::math::normalize(u[0] + M_PI); 
//      }

      bool ok = true; 
      for(unsigned jj = 0; jj  < max_attempts; jj++)
      {
        ok  = true;

        for (int d = 0; d < ndim; d++)
        {
          z[d] = rand.Gaus(); 
        }
        v = u + chol.GetU() * z; 

        for (unsigned d = 1; d < ndim; d++)
        {
          if (X_upper_limits_enable[d-1] && v[d] > X_upper_limits[d-1])
          {
            ok = false; 
          }

          if (X_lower_limits_enable[d-1] && v[d] < X_lower_limits[d-1])
          {
            ok = false; 
          }
        }

        if (ok) break; 
      }

      if (!ok)
      {
        printf("Couldn't get entry within limits in %u attempts... bad limits or entry?\n", max_attempts); 
        u.Print(); 
        C.Print(); 
        v.Print(); 

      }


      double val = ok ? f(v[0], v.GetMatrixArray()+1, entries[i].phi_ht, par) : 0; 

      assert (val < 1); 
      if (isnan(val) || val <=0) // this can happen if either the variable is outside its range (e.g. E goes negative) or bad parameters
      {
        continue; 
      }


      logl += log(val); 
      N++; 
      mean += val; 
      rms += val*val;

    }

    mean/=ntrials;

    if (fs) fs[i] = mean;
    if (d2fs) d2fs[i] = sqrt(rms/ntrials - mean*mean);
  }

  if (N) logl/=N; 
  else
  {
    return DBL_MAX; 
  }
  return logl; 

}


double dmtpc::math::ParametricVonMisesHTTest::minFn(const double * p) const
{
  return -loglikelihood(ntrain,train_data, p,0,0); 
}

double dmtpc::math::ParametricVonMisesHTTest::f(double phi, const double * X, double phi_ht, const double * p ) const 
{
  const double * par = p ? p : params;

  //evalpar should be const... 
  double width = ((TF1*)width_fn)->EvalPar(X, par); 
  double ht = ((TF1*)ht_fn)->EvalPar(X, par+ width_fn->GetNpar()); 
  double norml =((TF1*) normalization_fn)->EvalPar(X, par+ ht_fn->GetNpar() + width_fn->GetNpar()); 
  double mean = mean_fn ? ((TF1*)mean_fn)->EvalPar(X, par+ ht_fn->GetNpar() + width_fn->GetNpar() + normalization_fn->GetNpar()) : 0; 

  if (width < 0) width = 0;
  if (ht < 0.5) ht = 0.5; 
  if (ht > 1) ht = 1; 

  double vm_par[4];  
  vm_par[0] = ht;
  vm_par[1] = norml; 
  vm_par[2] = mean; 
  vm_par[3] = width; 

  /*
  printf("width pars:"); 
  int i = 0;
  for (; i < width_fn->GetNpar(); i++) printf(" (%s:%f)", width_fn->GetParName(i),p[i]);  
  printf("\n");


  printf("vm pars: %f %f %f %f\n", ht,norml,mean,width); 
  fflush(stdout); 
  */

  double alt_phi = phi + TMath::Pi(); 
  double val =  phi_ht * dmtpc::math::angle::von_mises_dist_ht(&phi,vm_par) + (1-phi_ht) * dmtpc::math::angle::von_mises_dist_ht(&alt_phi, vm_par); 


  if (isnan(val))
  {
    printf("width pars:"); 
    int i = 0;
    for (; i < width_fn->GetNpar(); i++) printf(" (%s:%f)", width_fn->GetParName(i),p[i]);  
    printf("\n");


    printf("vm pars: %f %f %f %f\n", ht,norml,mean,width); 
   
  }

  return val; 
}


void dmtpc::math::ParametricVonMisesHTTest::setWidthPars(const double * p, const double *pe)
{
  memcpy(params, p, sizeof(double) * width_fn->GetNpar()); 
  if (pe)
  {
    memcpy(param_errs, pe, sizeof(double) * width_fn->GetNpar()); 
  }
}

void dmtpc::math::ParametricVonMisesHTTest::setHTPars(const double * p,const  double *pe)
{
  memcpy(params + width_fn->GetNpar(), p, sizeof(double) * ht_fn->GetNpar()); 
  if (pe)
  {
    memcpy(param_errs + width_fn->GetNpar(), pe, sizeof(double) * ht_fn->GetNpar()); 
  }
}

void dmtpc::math::ParametricVonMisesHTTest::setNormalizationPars(const double * p, const double *pe)
{
  memcpy(params + width_fn->GetNpar() + ht_fn->GetNpar(), p, sizeof(double) * normalization_fn->GetNpar()); 
  if (pe)
  {
    memcpy(param_errs + width_fn->GetNpar() + ht_fn->GetNpar(), pe, sizeof(double) * normalization_fn->GetNpar()); 
  }
}

void dmtpc::math::ParametricVonMisesHTTest::setMeanPars(const double * p, const double *pe)
{
  memcpy(params + width_fn->GetNpar() + ht_fn->GetNpar() + normalization_fn->GetNpar(),
         p, sizeof(double) * mean_fn->GetNpar()); 
  if (pe)
  {
    memcpy(param_errs + width_fn->GetNpar() + ht_fn->GetNpar() + normalization_fn->GetNpar(), 
        pe, sizeof(double) * mean_fn->GetNpar()); 
  }
}





const double * dmtpc::math::ParametricVonMisesHTTest::getWidthPars() const
{
  return params; 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getWidthParErrs() const
{
  return param_errs; 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getHTPars() const
{
  return params + width_fn->GetNpar(); 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getHTParErrs() const
{
  return param_errs + width_fn->GetNpar();  
}


const double * dmtpc::math::ParametricVonMisesHTTest::getNormalizationPars() const
{
  return params + width_fn->GetNpar() + ht_fn->GetNpar(); 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getNormalizationParErrs() const
{
  return param_errs + width_fn->GetNpar() + ht_fn->GetNpar() ; 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getMeanPars() const
{
  return params + width_fn->GetNpar() + ht_fn->GetNpar() + normalization_fn->GetNpar(); 
}

const double * dmtpc::math::ParametricVonMisesHTTest::getMeanParErrs() const
{
  return param_errs + width_fn->GetNpar() + ht_fn->GetNpar()  + normalization_fn->GetNpar(); 
}

double dmtpc::math::ParametricVonMisesHTTest::train(unsigned n, const Entry * trn) 
{

  ntrain = n; 
  train_data = trn; 

  
  min->Minimize(); 
  min->Hesse(); 

  memcpy(params, min->X(), sizeof(double) * nparams); 
  memcpy(param_errs, min->Errors(), sizeof(double) * nparams); 

}



