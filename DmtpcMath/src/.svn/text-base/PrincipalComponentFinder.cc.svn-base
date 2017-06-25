#include "PrincipalComponentFinder.hh" 
#include "TMath.h"
#include "TDecompSVD.h" 
#include "TPrincipal.h" 
#include <cmath>
#include "TMatrixD.h"
#include <iostream>


dmtpc::math::PrincipalComponentFinder::PrincipalComponentFinder(int ndim, bool fast)
 : points(ndim), components(ndim,std::vector<double>(ndim)), weights(ndim), means(ndim), fast(fast)   
{
  solved = false; 
}

void dmtpc::math::PrincipalComponentFinder::addPoint(const double *x) 
{
  unsigned ndim = points.size(); 
  solved = false; 

  for (unsigned i = 0; i< ndim; i++)
  {
    points[i].push_back(x[i]); 
  }
}


void dmtpc::math::PrincipalComponentFinder::setPointInDim(int dim, int i, double x)
{
  solved = false; 

  points.reserve(i); 
  while(points[dim].size() < i)
  {
    points[dim].push_back(0); 
  }

  points[dim][i] = x; 
}

void dmtpc::math::PrincipalComponentFinder::setPointsInDim(int dim, int n, const double *x) 
{
  solved = false; 
  points[dim].clear(); 
  points[dim].reserve(n); 

  for (int i = 0; i < n; i++)
  {
    points[dim].push_back(x[i]); 
  }
}

void dmtpc::math::PrincipalComponentFinder::reserve(unsigned n)
{
  for (unsigned i = 0; i < points.size(); i++)
  {
    points[i].reserve(n); 
  }
}

void dmtpc::math::PrincipalComponentFinder::clearPoints()
{
  solved = false; 
  for (int dim = 0; dim < points.size(); dim++)
  {
    points[dim].clear(); 
    means[dim] = 0; 
  }
}

double * dmtpc::math::PrincipalComponentFinder::getComponent(int component, double * dest) 
{
  if (!solved) solve(); 
  unsigned dim = points.size();
  if (!dest) dest = new double[dim]; 

  memcpy(dest, &components[component][0], sizeof(*dest) * dim); 

  return dest; 
}

double dmtpc::math::PrincipalComponentFinder::getMean(int dim) 
{
  if (!solved) solve(); 
  return means[dim]; 
}

TVector3 dmtpc::math::PrincipalComponentFinder::asVec(int component) const
{
  TVector3 ans; 

  ans.SetX(components[component][0]); 

  if (ndim() > 1)
  {
    ans.SetY(components[component][1]); 
  }

  if (ndim() > 2)
  {
    ans.SetZ(components[component][2]); 
  }

  return ans; 
}


double dmtpc::math::PrincipalComponentFinder::asPhi(int component) const
{
  if (ndim() == 1) return 0; 
  else return atan2(components[component][1],components[component][0]); 
}


double dmtpc::math::PrincipalComponentFinder::getWeight(int component) 
{
  if (!solved) solve(); 
  return weights[component]; 
}

int dmtpc::math::PrincipalComponentFinder::solveFast() 
{
//  fprintf(stderr,"WARNING: solveFast doesn't seem to work properly... \n"); 
  unsigned ndim = points.size();
  TPrincipal pr(ndim,""); 

  std::vector<double> p(ndim); 
  for (unsigned i =0; i < points[0].size(); i++)
  {
    for (unsigned j = 0; j < ndim; j++)
    {
      p[j] = points[j][i]; 
    }
    pr.AddRow(&p[0]); 
  }

  pr.MakePrincipals(); 

  const TVectorD * ev = pr.GetEigenValues(); 

  std::vector<int> indices(ndim); 
  TMath::Sort(int(ndim),ev->GetMatrixArray(), &indices[0]); 

  for (unsigned i = 0; i < ndim; i++)
  {
    means[i] = (*pr.GetMeanValues())[i]; 
    weights[i] = sqrt((*pr.GetSigmas())[i]); 
    for (unsigned j = 0; j < ndim; j++)
    {
      components[i][j] =  (*pr.GetEigenVectors())[j][indices[i]]; 
    }
  }


  solved = true; 

  return 0; 



}


int dmtpc::math::PrincipalComponentFinder::solve() 
{
  if (fast) return solveFast(); 

  unsigned dim = points.size();



  //need to subtract off means

  for (unsigned n = 0; n < dim; n++)
  {
    means[n] = 0; 
  }

  
  for (unsigned n = 0; n < dim; n++)
  {
    if (points[n].size() != points[0].size())
    {
      std::cerr << "PrincipalComponentFinder::solve():  unequal numbers of points in dimensions" << std::endl; 
      return 1; 
    }

    for (unsigned m = 0; m < points[0].size(); m++)
    {
      means[n] += points[n][m]; 
    }
  }

  for (unsigned n = 0; n < dim; n++)
  {
    means[n] /= points[n].size(); 
  }


  TMatrixD M(points[0].size(),dim); 

  for (unsigned n = 0; n < dim; n++)
  {
    for (unsigned m = 0; m < points[0].size(); m++)
    {
      M[m][n] = points[n][m] - means[n]; 
    }
  }

  TDecompSVD svd(M); 

  svd.Decompose(); 

  for (int i = 0; i < dim; i++)
  {
    for (int j = 0; j < dim; j++)
    {
      components[i][j] = svd.GetV()[j][i]; 
    }
    weights[i] = svd.GetSig()[i]; 
  }

  solved = true; 
  return 0; 
}





