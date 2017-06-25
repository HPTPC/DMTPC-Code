#include "MultivariateTest.hh" 
#include "TMath.h"
#include <cmath>
#include "TTree.h"



#include <iostream>
int dmtpc::math::MultivariateTest::setAValues(TTree * t, const char * const * xnames, const char * const * xerr_dn_names,
                                               const char * const * xerr_up_names, const char * weight_names, const char * cut, bool append)
{
  return setValues('A', t, xnames, xerr_dn_names, xerr_up_names, weight_names, cut, append); 
}

int dmtpc::math::MultivariateTest::setBValues(TTree * t, const char * const * xnames, const char * const * xerr_dn_names , const char * const * xerr_up_names, const char * weight_names , const char * cut , bool append) 
{
  return setValues('B', t, xnames, xerr_dn_names, xerr_up_names, weight_names, cut, append); 
}


int dmtpc::math::MultivariateTest::setValues(char which, TTree * t, const char * const * xnames, const char * const * xerr_dn_names,
                                               const char * const * xerr_up_names, const char * weight_names, const char * cut, bool append)
{

  which == 'A' ? Adirty : Bdirty = true; 

  unsigned & num = which == 'A' ? num_A : num_B; 
  double & sum_weights = which == 'A' ? sum_weights_A : sum_weights_B; 
  std::vector<const double *> & vals = which == 'A' ? vals_A : vals_B; 
  std::vector<const double *> & vals_err_dn = which == 'A' ? vals_A_err_dn : vals_B_err_dn; 
  std::vector<const double *> & vals_err_up = which == 'A' ? vals_A_err_up : vals_B_err_up; 
  const double * & weights = which == 'A' ? weights_A : weights_B; 
  std::vector<double *> & tree_store = which == 'A' ? tree_store_A : tree_store_B; 

  t->SetEstimate(append ? num + t->GetEntries() : t->GetEntries()); 
  if (vals.size() != ndim)  vals.resize(ndim); 


  if (!append)
  {
    for (unsigned i = 0; i < tree_store.size(); i++)
    {
      delete tree_store[i];
    }
    tree_store.clear(); 
  }



  unsigned store_idx = 0; 
  int n = 0;

  for (unsigned i = 0; i < ndim; i++)
  {


    TString draw_string = TString::Format("%s: %s : %s", 
                                           xnames[i], 
                                           xerr_dn_names ? xerr_dn_names[i] : "0", 
                                           xerr_up_names ? xerr_up_names[i] : "0" 
        ); 
    

    std::cout << "draw: " <<draw_string << std::endl; 
    std::cout << "cut: " <<cut << std::endl; 
    n = t->Draw(draw_string, cut,"goff"); 


    if (!append)
    {
      double *x = new double[n]; 
      memcpy(x,t->GetV1(), n * sizeof(*x));
      tree_store.push_back(x); 
      vals[i] = x; 
    }
    else
    {
      double *x = new double[num+n]; 
      memcpy(x,vals[i], sizeof(*x) * num); 
      memcpy(x+num,t->GetV1(), n * sizeof(*x));
      delete tree_store[store_idx]; 
      tree_store[store_idx++] = x;
      vals[i] = x; 
    }


    if (xerr_dn_names)
    {
      if (!append)
      {
        double *xdn = new double[n]; 
        memcpy(xdn,t->GetV2(), n * sizeof(*xdn));
        tree_store.push_back(xdn); 
        vals_err_dn[i] = xdn; 
      }
      else
      {
        double *xdn =  new double[num+n]; 
        memcpy(xdn, vals_err_dn[i], num* sizeof(*xdn)); 
        memcpy(xdn + num, t->GetV2(), n * sizeof(*xdn));
        delete tree_store[store_idx]; 
        tree_store[store_idx++] = xdn;
        vals_err_dn[i] = xdn; 
      }

    }

    if (xerr_up_names)
    {
      if (!append)
      {
        double *xup = new double[n]; 
        memcpy(xup,t->GetV3(), n * sizeof(*xup));
        tree_store.push_back(xup); 
        vals_err_up[i] = xup; 
      }
      else
      {
        double *xup =  new double[num +n]; 
        memcpy(xup, vals_err_up[i], num * sizeof(*xup)); 
        memcpy(xup + num, t->GetV3(), n * sizeof(*xup));
        delete tree_store[store_idx]; 
        tree_store[store_idx++] = xup;
        vals_err_up[i] = xup; 
      }
    }

  }


  if (weight_names)
  {
      n = t->Draw(weight_names, cut, "goff"); 

      if (!append)
      {
        double *w = new double[n]; 
        memcpy(w,t->GetV1(), n * sizeof(*w));
        tree_store.push_back(w); 
        weights = w; 
      }
      else
      {
        double *w = new double[num + n]; 
        memcpy(w,weights, num * sizeof(*w));
        memcpy(w+num,t->GetV1(), n * sizeof(*w));
        delete tree_store[store_idx]; 
        tree_store[store_idx++] = w; 
        weights = w; 
      }
  }


  num = append ? num + n : n; 


  sum_weights = 0; 
  if (weights)
  {
    for (unsigned j = 0; j < num; j++) sum_weights += weights[j]; 
  }
  else
  {
    sum_weights = num; 

  }

  return num; 

}


void dmtpc::math::MultivariateTest::setAValues(unsigned n, const double * const *x, const double * const * dx_dn, const double * const * dx_up, const double * ws)
{
  Adirty = true;
  if (vals_A.size() != ndim)  vals_A.resize(ndim); 
  for (unsigned j = 0; j < ndim; j++)
  {
    vals_A[j] = x[j]; 
    vals_A_err_dn[j] = dx_dn ? dx_dn[j]: 0; 
    vals_A_err_up[j] = dx_up ? dx_up[j]: 0; 
  }
  num_A = n; 
  weights_A = ws; 

  sum_weights_A = 0; 
  if (ws)
  {
    for (unsigned j = 0; j < n; j++) sum_weights_A += ws[j]; 
  }
  else
  {
    sum_weights_A = num_A; 

  }

}


void dmtpc::math::MultivariateTest::setBValues(unsigned n, const double * const *x, const double * const * dx_dn, const double * const * dx_up, const double * ws)
{
  Bdirty = true; 
  if (vals_A.size() != ndim)  vals_B.resize(ndim); 

  for (unsigned j = 0; j <ndim; j++)
  {
    vals_B[j] = x[j]; 
    vals_B_err_dn[j] = dx_dn ? dx_dn[j]: 0; 
    vals_B_err_up[j] = dx_up ? dx_up[j]: 0; 
  }
  num_B = n;
  weights_B = ws; 

  sum_weights_B = 0; 
  if (ws)
  {
    for (unsigned j = 0; j < n; j++) sum_weights_B += ws[j]; 
  }
  else
  {
    sum_weights_B = num_B; 
  }

}


dmtpc::math::MultivariateTest::MultivariateTest(unsigned ndim) 
  : ndim(ndim), vals_A(ndim), vals_B(ndim), vals_A_err_dn(ndim), vals_A_err_up(ndim), 
    vals_B_err_dn(ndim), vals_B_err_up(ndim), isAngle(ndim,false),scales(ndim,1)
{

  weights_A = 0; 
  num_A = 0; 
  sum_weights_A = 0; 

  weights_B = 0; 
  num_B = 0; 
  sum_weights_B = 0; 
  Adirty = true;
  Bdirty = true; 
}



