#include "MultiNormalSampler.hh" 

dmtpc::math::MultiNormalSampler::MultiNormalSampler(unsigned ndim, unsigned nsamples, long seed)
  : chol(ndim), C(ndim), z(ndim), u(ndim), v(ndim), rand(seed), nsamples (nsamples), ndim(ndim) 
{




}
  

bool dmtpc::math::MultiNormalSampler::sample()
{

  chol.SetMatrix(C); 
  if (!chol.Decompose() )
  {
    printf("Warning in MultiNormalSampler: Cholesky Decomposition failed. Input matrix follows\n"); 
    C.Print(); 
    return false; 
  }

  for(unsigned i = 0; i < nsamples; i++)
  {

    for (unsigned d = 0; d < ndim; d++)
    {
      z[d] = rand.Gaus(); 
    }
    v = u + chol.GetU() * z; 
  }



  return true; 
}

const double * dmtpc::math::MultiNormalSampler::getPoints() const
{
  return v.GetMatrixArray(); 
}

void dmtpc::math::MultiNormalSampler::fillPoints(double * vv) const
{
  memcpy(vv,v.GetMatrixArray(), sizeof(double) * ndim); 
}

bool dmtpc::math::MultiNormalSampler::sample(const double * means, const double * covar)
{
  C.Use(ndim,covar); 
  u.SetElements(means); 
  return sample(); 
}

bool dmtpc::math::MultiNormalSampler::sample(const TVectorD * means, const TMatrixDSym * c)
{
  C.Use(*c); 
  u.SetElements(means->GetMatrixArray()); 
  return sample(); 
}

