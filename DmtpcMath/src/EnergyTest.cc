#include "EnergyTest.hh" 
#include <assert.h>
#include "TMath.h"
#include <cmath>
#include "Angular.hh"
#include <cstdio>



dmtpc::math::EnergyTest::EnergyTest(unsigned ndim, double alpha, DistanceMetric metric) 
  : alpha(alpha),MultivariateTest(ndim), metric(metric) 
{


}




double dmtpc::math::EnergyTest::testStatistic() const 
{
  
  double distAA = Adirty ? dist('A','A') : last_AA; 
  printf("\tdist(A,A) = %f\n",distAA); 
  double distBB = Bdirty ? dist('B','B') : last_BB; 
  printf("\tdist(B,B) = %f\n",distBB); 

  double distAB = Adirty || Bdirty ? dist('A','B') : last_AB; 
  double distBA = distAB; // Adirty || Bdirty ? dist('B','A') : last_BA; 

  printf("\tdist(A,B) = %f\n",distAB); 
  printf("\tdist(B,A) = %f\n",distBA); 

  Adirty = false;
  Bdirty = false; 
  last_AA = distAA; 
  last_BB = distBB; 
  last_AB = distAB; 
  last_BA = distBA; 

  return (sum_weights_A * sum_weights_B) / (sum_weights_A + sum_weights_B) * (distBA + distAB - distAA - distBB); 
}

double dmtpc::math::EnergyTest::dist(char x, char y) const
{
  printf("%c%c\n",x,y); 



  const std::vector<const double*> & X = x == 'A' ? vals_A : vals_B; 
  const std::vector<const double*> & Y = y == 'A' ? vals_A : vals_B; 
  const std::vector<const double*> & X_err_dn = x == 'A' ? vals_A_err_dn : vals_B_err_dn; 
  const std::vector<const double*> & Y_err_dn = y == 'A' ? vals_A_err_dn : vals_B_err_dn; 
  const std::vector<const double*> & X_err_up = x == 'A' ? vals_A_err_up : vals_B_err_up; 
  const std::vector<const double*> & Y_err_up = y == 'A' ? vals_A_err_up : vals_B_err_up; 

  const double * wX = x =='A' ? weights_A : weights_B; 
  const double * wY = y =='A' ? weights_A : weights_B; 

  unsigned m = x == 'A' ? num_A : num_B; 
  unsigned n = y == 'A' ? num_A : num_B; 


  double sum = 0; 
  unsigned ndim = vals_A.size();

  double weight_sum = 0;

  for (unsigned i = 0; i < m; i++)
  {

    for (unsigned j = 0; j < n; j++)
    {
      double distance = 0; 
      double weight = 1; 

      if (wX) weight *= wX[i]; 
      if (wY) weight *= wY[j]; 

      if (weight <0)
      {
        fprintf(stderr,"WARNING: negative weight!\n"); 
      }

//      printf("wX: %f wY: %f,",wX[i],wY[j]); 

      for (unsigned d = 0; d < ndim; d++)
      {
        double scale = scales[d]*scales[d]; 

        // see if errors are defined
        double dX = 0; 
        double dY = 0; 

        double diff = X[d][i] - Y[d][j]; 
        
        if (X_err_dn[d] || X_err_up[d])
        {
          if (X_err_dn[d] && X_err_up[d])
          {
            dX = diff < 0 ? fabs(X_err_up[d][i]) : fabs(X_err_dn[d][i]); 
//            printf("j/n,dX: %d/%d,%f\n", j,n,dX); 
//            assert(!isnan(dX)) ; 
          }
          else 
          {
            dX = X_err_dn[d] ? fabs(X_err_dn[d][i]) :  fabs(X_err_up[d][i]); 
          }
        }

        if (Y_err_dn[d] || Y_err_up[d])
        {
          if (Y_err_dn[d] && Y_err_up[d])
          {
            dY = diff > 0 ? fabs(Y_err_up[d][j]) : fabs(Y_err_dn[d][j]); 
//            printf("j/n,dY: %d/%d,%f\n", j,n,dY); 
//            assert(!isnan(dY)) ; 
          }
          else 
          {
            dY = Y_err_dn[d] ? fabs(Y_err_dn[d][j]) :  fabs(Y_err_up[d][j]); 
          }
        }



    

        if (isAngle[d])
        {
          diff = dmtpc::math::angle::normalize(diff,0); 
          //TODO: revisit this 
          if ( fabs(dX) > TMath::Pi()/(2*sqrt(2))) dX = TMath::Pi()/(2*sqrt(2)); 
          if ( fabs(dY) > TMath::Pi()/(2*sqrt(2))) dY = TMath::Pi()/(2*sqrt(2)); 
        }




        if (metric == SIMPLE || !dX && !dY)
        {
          distance += diff * diff / scale; 
        }
        else if (metric == ERRORS_IN_QUADTRATURE)
        {
          distance = diff*diff / (dX*dX +dY*dY) / scale; 
        }
        else if (dX && dY) 
        {
          distance += 0.5 *  diff * diff  * (dX*dX+dY*dY) / (dX*dX * dY*dY) / scale; 
        }
        else if (dX) 
        {
          distance += diff * diff / (dX*dX*scale); 

        }
        else
        {
          distance += diff * diff / (dY*dY*scale); 

        }







        assert(distance >= 0); 

      }

//      if (distance < 0 || sum < 0  || weight_sum < 0) 
//        printf("%f %f %f\n",distance, sum, weight_sum); 
      sum += weight * pow(distance, alpha/2.); 
      weight_sum +=weight; 
    }

    assert(weight_sum > 0); 
    assert(sum > 0); 
  }




  sum /= weight_sum; 


  return sum; 
}



