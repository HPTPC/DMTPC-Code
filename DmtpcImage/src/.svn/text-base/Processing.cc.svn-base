#include "ImageProcessing.hh"
#include <algorithm>
#include "TMath.h"
#include "ImageCalculations.hh"
#include <assert.h>
#include <math.h>
#include "ImageTools.hh" 
//#include "TThread.h"
#include "TRandom3.h"
#include "TH2.h" 
#include <iostream>
#include "TVirtualFFT.h"

/** Functions which either modify an existing image or calculate a 
 *  new image that is a filtered version of an existing image go here. 
 *  Note that there is also a source file for image transforms, 
 *  which produce a completely different image. 
 */ 

void
dmtpc::image::processing::applyThreshold(TH2 *image, float threshold) {
    // Apply threshold...
    //
    int nx = image->GetNbinsX();
    int ny = image->GetNbinsY();
    
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
            const Double_t &y=image->GetBinContent(i,j);
            if (y<threshold) image->SetBinContent(i,j,0);
        }
    }
}


void
dmtpc::image::processing::applyCeiling(TH2 *image, float ceiling) {
  // Apply threshold...                                                                                                                                                                                                                
  //                                                                                                                                                                                                                                   
  int nx = image->GetNbinsX();
  int ny = image->GetNbinsY();

  for (int i=1; i<=nx; i++) {
    for (int j=1; j<=ny; j++) {
      const Double_t &y=image->GetBinContent(i,j);
      if (y>ceiling) image->SetBinContent(i,j,0);
    }
  }
}

void
dmtpc::image::processing::subtractPedestal(TH2* image, float pedestal)
{
   int nx = image->GetNbinsX();
   int ny = image->GetNbinsY();
   for(int i=0; i<=nx; i++)
   {
      for(int j=0; j <=ny; j++)
      {
         float content = image->GetBinContent(i,j) - pedestal;
         image->SetBinContent(i,j,content);
      }
   }
}

int
dmtpc::image::processing::killRow(TH2* image, int irow) {
    // set pixels in one row to zero.
    
    //cout << irow << "  " << image->GetNbinsY()<<endl;
    assert(irow>=1 && irow<=image->GetNbinsY());
    int count = 0;
    for (int i=1; i<=image->GetNbinsX(); i++) {
      image->SetBinContent(i, irow, 0);
      count++;
    }
    return count;
}

int
dmtpc::image::processing::killColumn(TH2* image, int icolumn) {
    // set pixels in one column to zero.
    
    assert(icolumn>=1 && icolumn<=image->GetNbinsX());
    int count = 0;
    for (int i=1; i<=image->GetNbinsY(); i++) {
      image->SetBinContent(icolumn, i, 0);
      count++;
    }
    return count;
}

TH2 * dmtpc::image::processing::convolve(const TH2* img, const double *kernel, int width, int height, bool separable , BLUR_EDGE_BEHAVIOR edge_action  , const char * new_name  )
{
  double mag = 0; 

  if (edge_action == RENORMALIZE) 
  {
      for (int i = 0; i < (separable ? width : width*height); i++) mag += kernel[i]; 
  }
//  TThread::Lock(); 
  TString name =  new_name == 0 ? TString::Format("%s_convolved",img->GetName()) : TString(new_name); 
  TH2 * copy = (TH2*) img->Clone(separable ? "tmp" : name); 
  //TThread::UnLock();

  int nx = copy ->GetNbinsX(); 
  int ny = copy ->GetNbinsY(); 

  if (!separable)
  {
    for (int i = 1; i <= nx; i++)
    {
      for (int j = 1; j <=ny; j++)
      {
        double newval = 0; 
        double norml = 0; 
        for (int k = -width/2; k < (width+1)/2; k++)
        {
          for (int h = -height/2; h < (height+1)/2; h++)
          {
             double weight =kernel[(width/2 + k) * (height/2 + h)]; 

             if (i+k >=1 && i+k <= nx && j + h >=1 && j+ h <= ny)
             {
                newval += weight * img->GetBinContent(i+k,j+h); 
                norml += weight; 
             }
             else if (i + k < 1)
             {
               switch(edge_action)
               {
                case EXTEND_EDGES:
                  newval += weight*img->GetBinContent(1,j);  
                  break;
                case MIRROR_EDGES:
                  newval += weight*img->GetBinContent(i-k,j);
                  break;
                default: break; 
               }
             }

             else if (i + k  > nx)
             {
               switch(edge_action)
               {
                case EXTEND_EDGES:
                  newval += weight*img->GetBinContent(nx,j);  
                  break;
                case MIRROR_EDGES:
                  newval += weight*img->GetBinContent(i-k,j);
                  break;
                default: break; 
               }
             }
             else if (j + h < 1)
             {
               switch(edge_action)
               {
                case EXTEND_EDGES:
                  newval += weight*img->GetBinContent(i,1);  
                  break;
                case MIRROR_EDGES:
                  newval += weight*img->GetBinContent(i,j-h);
                  break;
                default: break; 
               }
             }

             else if (j + h > ny)
             {
               switch(edge_action)
               {
                case EXTEND_EDGES:
                  newval += weight*img->GetBinContent(i,ny);  
                  break;
                case MIRROR_EDGES:
                  newval += weight*img->GetBinContent(i,j-h);
                  break;
                default: break; 
               }
             }
          }
        }

        if (edge_action == RENORMALIZE) newval/=(norml/mag); 
        copy->SetBinContent(i,j,newval); 

      }
    }

    return copy;
  }


  //otherwise do two convolutions since it's faster
  //Horizontal convolution
  //
  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <=ny; j++)
    {
      double newval = 0;   
      double normalization = 0; 
      for (int k = -width/2; k< (width+1)/2; k++)
      {
        double weight = kernel[width/2 + k]; 

        if (i+k >=1 && i+k <= nx)
        {
          newval += weight*img->GetBinContent(i+k,j); 
          normalization += weight;
        }
        else if (i+k < 1)
        {
          switch (edge_action)
          {
            case EXTEND_EDGES:
              newval += weight*img->GetBinContent(1,j);  
              break;
            case MIRROR_EDGES:
              newval += weight*img->GetBinContent(i-k,j);
              break;
            default:
              break;
          }
          
        }
        else
        {
          switch(edge_action)
          {
            case EXTEND_EDGES:
              newval += weight*img->GetBinContent(nx,j);  
              break;
            case MIRROR_EDGES:
              newval += weight*img->GetBinContent(i-k,j);
              break;
            default:
              break;
          }
        }
      }
      if (edge_action == RENORMALIZE) newval/=(normalization/mag); 
      copy->SetBinContent(i,j,newval); 
    }
  }

  //TThread::Lock();
  TH2 * final = (TH2*) copy->Clone(name); 
  //TThread::UnLock();

  //Vertical  convolution
  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <=ny; j++)
    {
      double newval = 0;   
      double normalization = 0; 
      for (int k = -width/2; k< (width+1)/2; k++)
      {
        double weight = kernel[width/2 + k]; 
        if (j+k >=1 && j+k <= ny)
        {
          newval += weight*copy->GetBinContent(i,j+k); 
          normalization += weight; 
        }
        else if (j+k < 1)
        {
          switch (edge_action)
          {
            case EXTEND_EDGES:
              newval += weight*copy->GetBinContent(i,1);  
              break;
            case MIRROR_EDGES:
              newval += weight*copy->GetBinContent(i,j-k);
              break;
            default:
              break;
          }
          
        }
        else
        {
          switch(edge_action)
          {
            case EXTEND_EDGES:
              newval += weight*copy->GetBinContent(i,ny);  
              break;
            case MIRROR_EDGES:
              newval += weight*copy->GetBinContent(i,j-k);
              break;
            default:
              break;
          }
        }
      }
      if (edge_action == RENORMALIZE) newval /= (normalization/mag); 
      final->SetBinContent(i,j,newval); 
    }
  }
 
  copy->Delete();  


  return final; 

}

static double laplace_kernel[9] = { 0, -1, 0, 
                            -1, 4, -1,
                             0, -1, 0}; 

TH2* dmtpc::image::processing::laplacian(const TH2 * img)
{
  return convolve(img, laplace_kernel, 3, 3, false, EXTEND_EDGES); 
}


TH2* dmtpc::image::processing::laplacianOfGaussian(const TH2 * img, double sigma, int width)
{
    if (width < 0) width = -width; 
    if (width == 0) return 0; 
    double * kernel = new double[(2*width+1)]; 
    for (int i = -width; i <= width; i++)
    {
      kernel[i+width] = 1./sqrt(2*M_PI)/pow(sigma,5) * ( i*i - sigma*sigma) * exp(-i*i/(2*sigma*sigma)); 
    }

    return convolve(img, kernel, 2 * width+1, 1, true, EXTEND_EDGES); 
}

static __thread int last_blurn = -1; 
static __thread double last_sigma; 
static __thread double * last_kernel = NULL; 

static __thread double last_value_sigma; 
static __thread dmtpc::image::processing::BILATERAL_VALUE_FN last_fn; 
static __thread double * last_lookup = NULL; 
static __thread size_t last_lookup_size; 
static __thread int last_scale_exp; 

#define CONVERT_TO_INT

#ifdef CONVERT_TO_INT
static __thread int * last_int_img = 0; 
static __thread int last_int_img_nx; 
static __thread int last_int_img_ny; 
#endif 

TH2 * dmtpc::image::processing::fastBilateralFilter(const TH2* img, double space_sigma, double value_sigma, double nsigma, BILATERAL_VALUE_FN fn, int scale_exp, bool cache_lookup_table)
{
  int blurn = (int) (nsigma * space_sigma + 1); 
  size_t row_size = 2*blurn+1; 
  double * space_kernel;
  size_t arr_size = row_size*row_size * sizeof(*space_kernel); 

  int nx = img->GetNbinsX();
  int ny = img->GetNbinsY();

#ifdef CONVERT_TO_INT
  int * intimg; 
  if (last_int_img && nx == last_int_img_nx && ny == last_int_img_ny)
  {
    intimg = last_int_img; 
  }
  else
  {
    if (last_int_img) free(last_int_img); 
    intimg = (int*) malloc(nx*ny*sizeof(*intimg)); 
    last_int_img = intimg; 
    last_int_img_nx = nx; 
    last_int_img_ny = ny; 
  }

  int img_i = 0; 
  for (int j = 1; j <= ny; j++)
  {
    for (int i = 1; i <=nx; i++)
    {
      intimg[img_i++] = lrint(ldexp(img->GetBinContent(i,j),scale_exp)); 
    }
  }
  
  unsigned value_sigma_int = lrint(ldexp(value_sigma,scale_exp)); 

  img_i = 0; 
#endif
 

  if(last_kernel && blurn == last_blurn && space_sigma == last_sigma)
  {
    space_kernel = last_kernel; 
  }
  else
  {
    space_kernel =  (double*) malloc (arr_size); 
    if(last_kernel)
    {
      free(last_kernel);  
    }

    for (int j = -blurn; j<= blurn; j++)
    {
      for (int i = -blurn; i <= blurn; i++)
      {
        double val = exp(-(i*i+j*j) / (2*space_sigma*space_sigma));  
        space_kernel[blurn + i + (row_size)*(blurn+j)] = val; 
      }
    }
    last_blurn = blurn; 
    last_sigma = space_sigma; 
    last_kernel = space_kernel; 
  }

  //compute necessary size of lookup table, if gaussian or lorentzian

  //require minimum to be 1/(2*arr_size); 
  size_t lookup_size = 0; 
  double * lookup_arr = 0; 

  if (fn == BILATERAL_GAUSSIAN || fn == BILATERAL_CAUCHY)
  {
    if (cache_lookup_table && last_lookup && fn == last_fn && last_sigma == space_sigma && last_value_sigma == value_sigma && last_blurn == blurn && last_scale_exp == scale_exp)
    {
      lookup_size = last_lookup_size;  
      lookup_arr = last_lookup; 
    }

    else
    {
      if (last_lookup && cache_lookup_table)
      {
        free(last_lookup); 
      }

      double value_sigma_mod = ldexp(value_sigma,scale_exp); 

      if (fn == BILATERAL_GAUSSIAN)
      {
        lookup_size = (size_t) (ceil(sqrt(2*log(2*row_size*row_size))*value_sigma_mod) + 1); 
        lookup_arr = (double*) (cache_lookup_table ? malloc(lookup_size * sizeof(*lookup_arr)) : alloca(lookup_size * sizeof(*lookup_arr))); 
        for (unsigned i = 0; i < lookup_size; i++)
        {
          lookup_arr[i] = exp(-double(i*i)/(2.*value_sigma_mod*value_sigma_mod));   
        }
      }
      else 
      {
        lookup_size =  value_sigma_mod < 2*row_size*row_size ? (size_t) ceil(sqrt(value_sigma_mod*(2*row_size*row_size -value_sigma_mod))) + 1 : 2; 
        lookup_arr = (double*) (cache_lookup_table ? malloc(lookup_size * sizeof(*lookup_arr)) : alloca(lookup_size * sizeof(*lookup_arr))); 
        for (size_t i = 0; i < lookup_size; i++)
        {
          lookup_arr[i] = value_sigma_mod/(double(i*i)+ value_sigma_mod*value_sigma_mod); 
        }
      }
      std::cout <<"lookup_size: " << lookup_size << std::endl; 

      if (cache_lookup_table) 
      {
        last_lookup_size = lookup_size; 
        last_value_sigma = last_sigma; 
        last_fn = fn; 
        last_lookup = lookup_arr; 
        last_scale_exp = scale_exp; 
      }
    }
  }

  TH2 * blurred = (TH2*) img->Clone(img->GetName() + TString("_bilateral")); 


 
  //convolution
  for (int j = 1; j <= ny; j++)
  {
    for (int i = 1; i <=nx; i++)
    {
      double newval = 0;   
      double normalization = 0; 
      
      #ifdef CONVERT_TO_INT
      int center_val = intimg[img_i]; 
      #else
      double center_val = img->GetBinContent(i,j); 
      #endif
      int kernel_idx = 0; 
      int second_idx; 
      for (int k = -blurn; k<= blurn; k++)
      {
        int jk = j+k; 
        if (unsigned(jk-1) >= unsigned(ny)) continue; 
        second_idx = img_i + k*nx - blurn;  

        for (int h = -blurn; h<= blurn; h++)
        {
  //        if (i+k >=1 && i+k <= nx && j + h >=1 && j+ h <= ny)   //reduce to two comparisons
  
          int ih = h+i; 
          if (unsigned(ih-1) >=unsigned(nx)) continue; 

          #ifdef CONVERT_TO_INT
          int val = intimg[second_idx++]; 
          unsigned diff = abs(center_val-val); 
          #else
          double val = img->GetBinContent(ih,jk); 
          double diff = fabs(center_val - val); 
          unsigned rounded_diff; 
          #endif

          double value_weight; 

          switch (fn)
          {
            case BILATERAL_GAUSSIAN:   
            case BILATERAL_CAUCHY:   
              #ifdef CONVERT_TO_INT
              value_weight = diff < lookup_size ? lookup_arr[diff] : 0; 
              #else
              rounded_diff = lrint(ldexp(diff,scale_exp)); 
              value_weight = rounded_diff < lookup_size ? lookup_arr[rounded_diff] : 0; 
              #endif
            //  std::cout << diff << " " << rounded_diff << " " << value_weight << std::endl; 
              break; 
            case BILATERAL_BOX: 
              #ifdef CONVERT_TO_INT
              value_weight = diff <=value_sigma_int ? 1 : 0;  
              #else
              value_weight = diff <= value_sigma ? 1 : 0; 
              #endif
              break; 
            case BILATERAL_TRIANGLE:
              #ifdef CONVERT_TO_INT
              value_weight = diff > value_sigma_int ? 0 : 1 - double(diff)/value_sigma_int; 
              #else
              value_weight = diff > value_sigma ? 0 : 1 - diff/value_sigma; 
              #endif
              break; 
            case BILATERAL_TUKEY:
              #ifdef CONVERT_TO_INT
              value_weight = diff > value_sigma_int ? 0 : 0.5 * pow(1 - pow(double(diff)/value_sigma_int,2),2); 
              #else
              value_weight = diff > value_sigma ? 0 : 0.5 * pow(1 - pow(diff/value_sigma,2),2); 
              #endif
              break; 
            default:
              value_weight = 1; 
          }

          double weight = space_kernel[kernel_idx++] * value_weight; 
          newval += weight * val; 
          normalization += weight; 
        }
      }
      newval/=normalization; 
      #ifdef CONVERT_TO_INT
      blurred->SetBinContent(i,j,ldexp(newval,-scale_exp)); 
      img_i++; 
      #else
      blurred->SetBinContent(i,j,newval); 
      #endif
    }
  }

  return blurred; 

}




TH2 * dmtpc::image::processing::bilateralFilter(const TH2 * img, double space_sigma, double value_sigma, double nsigma, BILATERAL_VALUE_FN fn)  
{
  assert (nsigma > 0); 
  int blurn = (int) (nsigma * space_sigma+1); 
  if (space_sigma == 0) 
  {
    //TThread::Lock(); 
    TH2 * ret =  (TH2*) img->Clone(img->GetName() + TString("_bilateral")); 
    //TThread::UnLock(); 
    return ret; 
  }

  std::vector<std::vector<double> > space_kernel( 2*blurn+1, std::vector<double> (2*blurn+1)); 

  for (int i = -blurn; i <=blurn; i++)
  {
    for (int j = -blurn; j <= blurn; j++)
    {
      space_kernel[i+blurn][j+blurn] = exp(-(i*i+j*j)/(2.*space_sigma*space_sigma));
//      std::cout << space_kernel[i+blurn][j+blurn] << " " ; 
    }
//      std::cout << std::endl; 
  }


  
  //TThread::Lock();
  TH2 * blurred = (TH2*) img->Clone(img->GetName() + TString("_bilateral")); 
  //TThread::UnLock(); 
  int nx = blurred->GetNbinsX();
  int ny = blurred->GetNbinsY();
  
  //convolution
  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <=ny; j++)
    {
      double newval = 0;   
      double normalization = 0; 
      double center_val = img->GetBinContent(i,j); 
      for (int k = -blurn; k<= blurn; k++)
      {
        for (int h = -blurn; h<= blurn; h++)
        {
          if (i+k >=1 && i+k <= nx && j + h >=1 && j+ h <= ny)
          {
            double val = img->GetBinContent(i+k,j+h); 
            double diff = center_val - val; 
            double value_weight = 1; 
            if (diff != 0)
            {
              switch (fn)
              {
                case BILATERAL_GAUSSIAN:   
                  value_weight = exp(-diff*diff/(2*value_sigma*value_sigma)); 
                  break; 
                case BILATERAL_CAUCHY:   
                  value_weight = value_sigma/(diff*diff + value_sigma*value_sigma); 
                  break; 
                case BILATERAL_BOX: 
                  value_weight = fabs(diff) <= value_sigma ? 1 : 0; 
                  break; 
                case BILATERAL_TRIANGLE:
                  value_weight = fabs(diff) > value_sigma ? 0 : 1 - diff/value_sigma; 
                  break; 
                case BILATERAL_TUKEY:
                  value_weight = fabs(diff) > value_sigma ? 0 : 0.5 * pow(1 - pow(diff/value_sigma,2),2); 
                  break; 
              }
            }

            double weight = space_kernel[blurn+k][blurn+h] * value_weight; 
            newval += weight * val; 
            normalization += weight; 
          }
        }
      }
      newval/=normalization; 
      blurred->SetBinContent(i,j,newval); 
    }
  }

  return blurred; 

}


TH2* dmtpc::image::processing::gaussianBlur(const TH2* img, int blurn, double sigma, double ** kernel_ptr, BLUR_EDGE_BEHAVIOR edge_action)
{
  //Create gaussian kernel
  double * kernel; 
  assert(blurn>0); 

  if (sigma == 0)
  {
   // TThread::Lock(); 
    TH2 * ret =  (TH2*) img->Clone(img->GetName() + TString("_blur")); 
   // TThread::UnLock(); 

    return ret;
  }

  //If we were given a kernel to use, use it
  if (kernel_ptr != NULL && *kernel_ptr !=NULL)
  {
    kernel = *kernel_ptr; 
  }

  //otherwise, if the parameters match the last parameters, used the same last kernel
  else if(last_blurn == blurn && last_sigma == sigma && last_kernel!=NULL)
  {
    kernel = last_kernel; 
  }

  //otherwise, allocate and compute a new kernel, freeing the last_kernel if it has been allocated.
  else
  {
    if (last_kernel!=NULL) free(last_kernel); 
    kernel = (double*) malloc(sizeof(double) * (2*blurn+1)); 
    for (int i = -blurn; i <=blurn; i++)
    {
      kernel[i+blurn] = 1./sqrt(M_PI*2*sigma*sigma)*exp(-i*i/(2.*sigma*sigma));
    }
    last_kernel = kernel;   
    last_blurn = blurn; 
    last_sigma = sigma; 
  }

  if (kernel_ptr && *kernel_ptr == NULL)
  {
    //Make a copy because last_kernel may be deleted by another instance 
    memcpy(*kernel_ptr,kernel,sizeof(double)*(2*blurn+1));  
  }


  char new_name[128]; 
  sprintf(new_name,"%s_blur",img->GetName()); 
 
  return (TH2*) convolve(img, kernel, 2*blurn+1, 0, true, edge_action, new_name); 
}


TH2* dmtpc::image::processing::topHatBlur(TH2* image, int blurn, double blurfrac)
{
   TH2* copy = (TH2*)image->Clone("copy");
   int nx = copy->GetNbinsX();
   int ny = copy->GetNbinsY();

   for(int i=1; i<=nx; i++)
   {
      for(int j=1; j<=ny; j++)
      {   
         double sum=0;
         int nin=0;

         for(int k=i-blurn; k<= i+blurn; k++)
         {
            for(int l=j-blurn; l<=j+blurn; l++)
            {
               if(k > 0 && l > 0 && k <=nx && l <= ny)
               {
            nin++;
            if(k==i && l==j) sum+=image->GetBinContent(k,l);
            else sum+=image->GetBinContent(k,l)*blurfrac;
               } 

            }
         }

         sum = sum/(1+(nin-1)*blurfrac);
         copy->SetBinContent(i,j,sum);
      }
   }
   
   return copy;
}



TH2 * dmtpc::image::processing::anisotropicDiffusion(const TH2 * img, double lambda, double K, ANISOTROPIC_DIFFUSION_FN fn, double gradient_t, GRADIENT_OPERATOR gradop, bool use_diagonals)
{
  TH2 * out = (TH2F*) img->Clone(img->GetName() +TString("_diffused")); 

  TH2F * gmag2 = (TH2F*) dmtpc::image::tools::newTH2StealSize(img,'F',"magtemp","magtemp"); 

  gradient(img,gmag2,0,gradient_t, gradop, (int) (gradient_t*3+1)); 


  const int NORTH = 0; 
  const int EAST = 1; 
  const int SOUTH = 2; 
  const int WEST = 3; 
  const int NW = 4; 
  const int NE = 5; 
  const int SW = 6; 
  const int SE = 7; 

  if (use_diagonals) 
  {
    std::cout << " WARNING: USING DIAGONALS DOESN'T WORK!!! NUMERICALLY UNSTABLE.. IF YOU FIGURE  OUT HOW TO FIX IT PLEASE DO" << std::endl; 
  }
  
  if (lambda < 0) lambda = 0; 
  if (lambda > 1) lambda = 1; 

  int rown = out->GetNbinsX() + 2; 

  //Take square root 
  for (int i = 1; i <= out->GetNbinsX(); i++)
  {
    for (int j = 1; j <= out->GetNbinsY(); j++) 
    {
      int b = j*rown + i; 
      double content = gmag2->GetBinContent(b); 
      gmag2->SetBinContent(b, TMath::Sqrt(content)); 
    }
  }



  for (int i = 1; i <= out->GetNbinsX(); i++)
  {
    for (int j = 1; j <= out->GetNbinsY(); j++) 
    {
      double nd = 0; 
      double dI[8] = {0,0,0,0,0,0,0,0}; 
      double gI[8] = {0,0,0,0,0,0,0,0}; 
      double c[8]; 
      int b = j*rown + i; 
      double content = img->GetBinContent(b); 


//      std::cout << i << " " << j << std::endl; 
      if (i > 1)
      {
        nd++;  
        dI[WEST]= img->GetBinContent(b-1) - content; 
        gI[WEST]= 0.5 * (gmag2->GetBinContent(b-1) + gmag2->GetBinContent(b)) ; 
      }

      if (j>1)
      {
        nd++; 
        dI[SOUTH]= img->GetBinContent(b-rown) - content; 
        gI[SOUTH]= 0.5 * (gmag2->GetBinContent(b-rown) + gmag2->GetBinContent(b)) ; 
      }

      if (use_diagonals && i > 1 && j > 1)
      {
        nd++; 
        dI[SW] = 1./2 * ( img->GetBinContent(b-rown-1) - content); 
        gI[SW] = 0.5 * TMath::Sqrt2() * ( gmag2->GetBinContent(b-rown-1) - gmag2->GetBinContent(b)); 

      }

      if (i < out->GetNbinsX())
      {
        nd++; 
        dI[EAST] = img->GetBinContent(b+1) - content; 
        gI[EAST]= 0.5 * (gmag2->GetBinContent(b+1) + gmag2->GetBinContent(b)) ; 
      }

      if (use_diagonals && i < out->GetNbinsX() && j > 1)
      {
        nd++;
        dI[SE] = 1./2 * ( img->GetBinContent(b-rown+1) - content); 
        gI[SE] = 0.5*TMath::Sqrt2() *  ( gmag2->GetBinContent(b-rown+1) - gmag2->GetBinContent(b)); 
      }

      if (j < out->GetNbinsY())
      {
        nd++; 
        dI[NORTH]= img->GetBinContent(b+rown) - content; 
        gI[NORTH]= 0.5 * (gmag2->GetBinContent(b+rown) + gmag2->GetBinContent(b)); 
      }

      if (use_diagonals && i < out->GetNbinsX() && j < out->GetNbinsX())
      {
        nd++;
        dI[NE] =1./2 * ( img->GetBinContent(b+rown+1) - content); 
        gI[NE] = 0.5* TMath::Sqrt2() * ( gmag2->GetBinContent(b+rown+1) - gmag2->GetBinContent(b)); 
      }

      if (use_diagonals && i > 1 && j < out->GetNbinsX())
      {
        nd++;
        dI[NW] = 1./2 * ( img->GetBinContent(b+rown-1) - content); 
        gI[NW] = 0.5 *TMath::Sqrt2() *  ( gmag2->GetBinContent(b+rown-1) - gmag2->GetBinContent(b)); 
      }

      double res = content; 
      
      for (int d = 0; d < 8; d++)
      {
        if (!use_diagonals && d>3) break; 

        switch (fn)
        {
          case GAUSSIAN:
            c[d] = TMath::Exp(-TMath::Power((gI[d]/K),2)); 
            break; 
          case LORENTZIAN:
            c[d] = 1./(1+ TMath::Power(gI[d]/K,2)); 
            break; 
          case TUKEY:
            c[d] = gI[d] > K ? 0 : 0.5*TMath::Power(1 - TMath::Power(gI[d]/K,2),2); 
            break; 
        }

        res += lambda / nd * c[d] * dI[d]; 
      }
    
      out->SetBinContent(i,j,res); 
    }
  }

  gmag2->Delete(); 
  return out; 
}



TH2 * dmtpc::image::processing::nonMaximumSuppress(const TH2* in, int n, double setval, const char * suffix)
{
  TH2 * out = (TH2*) in->Clone(TString(in->GetName()) + TString(suffix)); 


  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    for (int j = 1; j <= in->GetNbinsY(); j++)
    {

      double this_val = in->GetBinContent(i,j); 
      bool max = true; 
      for (int ii = -n; ii <=n; ii++)
      {
        if (!max) break; 
        for (int jj = -n; jj <=n; jj++)
        {
            if (ii == 0 && jj == 0) continue; 
            if (i + ii < 1 || j + jj < 1 || i + ii >in->GetNbinsX() || j + jj > in->GetNbinsY()) continue; 

            if (in->GetBinContent(i+ii, j+jj) > this_val)
            {
              max = false; 
              break; 
            }
        }
      }

      if (!max) out->SetBinContent(i,j,setval); 
    }
  }

  return out; 
}


TH2C * dmtpc::image::processing::binaryThreshold(const TH2 * in, double thresh)
{
  TH2C * out = (TH2C*) dmtpc::image::tools::newTH2StealSize(in, 'C', in->GetName() + TString("_twotone"), in->GetTitle() + TString (" (two tone)"));

  for (int i = 1; i <= in->GetNbinsX();i++)
  {
    for (int j = 1; j <= in->GetNbinsY();j++)
    {
      out->SetBinContent(i,j,in->GetBinContent(i,j) >= thresh ? 1 : 0); 
    }
  }

  return out; 
}


void dmtpc::image::processing::fillEdges(TH2 * in, unsigned width, double setval)
{

  for (int x = 1; x <= in->GetNbinsX(); x++)
  {
    for (unsigned n = 1; n <= width; n++)
    {
      in->SetBinContent(x,n,setval); 
      in->SetBinContent(x,in->GetNbinsY()-n+1,setval); 
    }
  }

  for (int y = 1; y <= in->GetNbinsY(); y++)
  {
    for (unsigned n = 1; n <= width; n++)
    {
      in->SetBinContent(n,y,setval); 
      in->SetBinContent(in->GetNbinsX()-n+1,y,setval); 
    }
  }
}

TH2 * dmtpc::image::processing::medianFilter(const TH2 * in, unsigned nbins, unsigned niter)
{
  if (!(nbins % 2)) nbins +=1;  //odd bins only 

  const TH2 * last = in; 
  TH2 * todelete = 0; 
  TH2 * current;
  
  std::vector<double> sortme; 


  for (unsigned i = 0; i < niter; i++)
  {
    //TThread::Lock(); 
    current = (TH2*) last->Clone(TString::Format("%s_median_iter%d",in->GetName(),i)); 
    //TThread::UnLock(); 

    
    for (int x = 1; x <= in->GetNbinsX();  x++)
    {
      for (int y = 1; y <= in->GetNbinsY();  y++)
      {
        sortme.clear(); 

        for (int xx = x - (int) nbins/2; xx <= x + (int) nbins/2; xx++)
        {
          if (xx < 1 || xx > in->GetNbinsX()) continue; 
          for (int yy = y - (int) nbins/2; yy<= y +(int)  nbins/2; yy++)
          {
            if (yy < 1 || yy > in->GetNbinsY()) continue; 
            sortme.push_back(last->GetBinContent(xx,yy)); 
            
          }
        }
    
        std::sort(sortme.begin(), sortme.end()); 
       // std::cout << sortme.size() << std::endl; 
        current->SetBinContent(x,y, sortme[sortme.size()/2 + 1]);

      }
    }
    if (todelete) delete todelete; 
    if (i > 0) todelete = current; 
    last = current; 
  }

  return (TH2*) last; 
}

int
dmtpc::image::processing::killLonePixels(TH2* image, float threshold, int minNeighbors) {
    // Set to zero all pixels that are below the threshold, or
    // have no neighbors above the threshold.

    int nx = image->GetNbinsX();
    int ny = image->GetNbinsY();
    int count = 0; 
    // kill lone pixels
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
//             static int multi=0; if (multi++<10) cout <<"# of neighbors changed!"<<endl;
          if (!dmtpc::image::calculations::hasNeighbor(image,i,j, threshold, minNeighbors)){
            image->SetBinContent(i,j,0);
            count++;
          }
        }
    }
    return count;
}

int
dmtpc::image::processing::killLonePixels2(TH2* image, float threshold,std::vector<int>* killedPixels) {
    // Set to average all pixels that have no neighbors and are
   //  above threshold

    int nx = image->GetNbinsX();
    int ny = image->GetNbinsY();

    double mean, rms;
    dmtpc::image::calculations::meanRMSNoOutliers(image,mean,rms);
    int count=0; 

    // kill lone pixels
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
//             static int multi=0; if (multi++<10) cout <<"# of neighbors changed!"<<endl;
              if ( (!dmtpc::image::calculations::hasNeighbor(image,i,j, threshold*0.95) &&
                     image->GetBinContent(i,j)>threshold) ) {
            //       cout << "dmtpc::image::processing::killLonePixels2 killing pixel : "
            //       << i << "," << j << ":" << image->GetBin(i,j)
            //       << "\n\t current value: " << image->GetBinContent(i,j) << endl;
                   if(killedPixels)
                      killedPixels->push_back(image->GetBin(i,j));
            //        image->SetBinContent(i,j,mean);
                   int nin=0;
                   double sum=0;
                   for(int k=i-1; k<= i+1; k++)
                   {
                      for(int l=j-1; l<=j+1; l++)
                      {
                         if(k > 0 && l > 0 && k <=nx && l <= ny && k!=i && l!=j)
                         {
                            nin++;
                            sum+=image->GetBinContent(k,l);
                         } 
                         
                      }
                   }
                   sum = sum/double(nin);
            //       cout << "\t setting value to: " <<sum << endl;
                   image->SetBinContent(i,j,sum);
                   count++;
                }
        }
    }
    //cout << "dmtpc::image::processing:: number of lone pixels killed: " << count << endl;
    return count; 
}

int
dmtpc::image::processing::killPixels(TH2* image, float threshold,std::vector<int>* killedPixels) {
    // Set to average all pixels that are above threshold

    int nx = image->GetNbinsX();
    int ny = image->GetNbinsY();
    int count = 0;

    double mean, rms;
    dmtpc::image::calculations::meanRMSNoOutliers(image,mean,rms);

    // kill pixels above threshold
    for (int i=1; i<=nx; i++) {
      for (int j=1; j<=ny; j++) {
        if (image->GetBinContent(i,j)>threshold) {
          std::cout << "dmtpc::image::processing::killPixels killing pixel " 
           << i << "," << j << ":" << image->GetBin(i,j)
           << "\n\t current value: " << image->GetBinContent(i,j) << std::endl;
             if(killedPixels)
               killedPixels->push_back(image->GetBin(i,j));
         //    image->SetBinContent(i,j,mean);
           int nin=0;
           double sum=0;
           for(int k=i-1; k<= i+1; k++)
           {
              for(int l=j-1; l<=j+1; l++)
              {
                 if(k > 0 && l > 0 && k <=nx && l <= ny && k!=i && l!=j)
                 {
                    nin++;
                    sum+=image->GetBinContent(k,l);
                 } 
              }
           }
           sum = sum/double(nin);
           std::cout << "\t setting value to: " <<sum <<std::endl;
           image->SetBinContent(i,j,sum);
           count++;
        }
      }
    }

    std::cout << "dmtpc::image::processing:: number of pixels killed: " << count << std::endl;
    return count;
}

int
dmtpc::image::processing::killPixelList(TH2* image, std::vector<int> pixellist)
{
   for(int i=0; i<int(pixellist.size()); i++)
   {
      image->SetBinContent(pixellist[i],0);
   }
   return pixellist.size();
}

int
dmtpc::image::processing::killUnusedPixels(TH2* image, std::vector<int> &roi, int d, TString opt) {
    
    if (roi.size()<1) return 0;
    opt.ToLower();
    bool isX=opt.Contains("x");
    int n = isX ? image->GetNbinsX() : image->GetNbinsY();
    //cout << "isX="<<isX<<endl;
    int count = 0;
    for (int i=1; i<=n; i++) {
        bool isInside=false;
        for (unsigned int l=0; l<roi.size(); l++) if (i>=roi[l]-d && i<=roi[l]+d) { isInside=true; break; }
        //cout << i << "   isInside=" << isInside << endl;
        if (!isInside && isX) count+=killColumn(image, i);
        else if (!isInside) count+=killRow(image, i); 
    }
    return count;
}

// gradient operators 
static const int sobel[3][3] = {{-1,0,1},
                                {-2,0,2},
                                {-1,0,1}}; 

static const int prewitt[3][3] = {{-1,0,1},
                                  {-1,0,1},
                                  {-1,0,1}}; 

static const int scharr[3][3] = {{-3,0,3}, 
                                {-10,0,10},
                                 {-3,0,3}}; 

void dmtpc::image::processing::gradient(const TH2 * img, TH2 * M2, TH2S * A, double blurlevel, GRADIENT_OPERATOR gradop , unsigned int kernel_size ) 
{

  //Blur with nxn kernel 
  TH2 * blurred = gaussianBlur(img,kernel_size,blurlevel);
  int nx = img->GetNbinsX(); 
  int ny = img->GetNbinsY(); 


  //Compute the gradient histograms
  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <= ny; j++)
    {
      //gradients in x and y direction
      double gx = 0; 
      double gy = 0;

      //Convolve using chosen gradient operators
      for (int ii = -1; ii <= 1; ii++)   
      {
        int bx = i+ii; 
        if (bx == 0 || bx > nx) continue; 

        for (int jj = -1; jj <= 1; jj++)
        {
          int by = j+jj; 
          if (by == 0 || by > ny) continue; 

          switch(gradop)
          {
            case SOBEL:
               gy += sobel[ii+1][jj+1]*blurred->GetBinContent(bx,by); 
               gx += sobel[jj+1][ii+1]*blurred->GetBinContent(bx,by); 
               break; 
            case PREWITT:
               gy += prewitt[ii+1][jj+1]*blurred->GetBinContent(bx,by); 
               gx += prewitt[jj+1][ii+1]*blurred->GetBinContent(bx,by); 
               break; 
            case SCHARR:
               gy += scharr[ii+1][jj+1]*blurred->GetBinContent(bx,by); 
               gx += scharr[jj+1][ii+1]*blurred->GetBinContent(bx,by); 
               break; 
            default: break; 
          }
        }
      }

      double m2 = gx*gx+gy*gy; 

     // cout << m2 << endl; 

      if(M2)
        M2->SetBinContent(i,j,m2);

      if (!A) continue;
      double slope = gy/gx; 

      //Round to nearest orientation
      short ori;      

      //horizontal 
      if (slope < 0.41421 && slope >-0.41421)
      {
        ori = 0;
      }
      //pos 
      else if (slope > 0.41421 && slope < 2.4142)
      {
        ori = 45; 
      }
      //neg
      else if (slope < -0.41421 && slope > -2.4142)
      {
        ori = 135; 
      }
      //else vertical
      else
      {
        ori = 90; 
      }


//      std::cout <<  slope << " " << ori <<  " " << TMath::ATan(slope) * 180 / TMath::Pi() << std::endl; 
      A->SetBinContent(i,j,ori); 
    }
  }

  blurred->Delete(); 
}

static void hysteresisConnect(int i, int j, TH2C * out, const TH2C * hi, const TH2C * low)
{
  for (int ii = i-1; ii <=i+1; ii++)
  {
    if (ii == 0 || ii > out->GetNbinsX()) continue; 

    for (int jj = j-1; jj<=j+1; jj++)
    {
      if (jj == 0 || jj > out->GetNbinsY()) continue; 

      if (ii==i && jj==j) continue; 

      if (out->GetBinContent(ii,jj) == 0 && low->GetBinContent(ii,jj) ==1)
      {
        out->SetBinContent(ii,jj,1); 
        hysteresisConnect(ii,jj,out,hi,low); 
      }
    }
  }
}


TH2C * dmtpc::image::processing::edgeDetect(const TH2 * img, double blurlevel, double th_low, double th_hi, GRADIENT_OPERATOR gradop, unsigned int kernel_size) 
{


  int nx = img->GetNbinsX(); 
  int ny = img->GetNbinsY(); 
  int xmin = (int) img->GetXaxis()->GetXmin();
  int ymin = (int) img->GetYaxis()->GetXmin();
  int xmax = (int) img->GetXaxis()->GetXmax();
  int ymax = (int) img->GetYaxis()->GetXmax();

  //Store the magnitude (squared) of the gradient
  TH2 * M2 = dmtpc::image::tools::newTH2StealType(img,"gMag2","gMag2",nx,xmin,xmax,ny,ymin,ymax); 

  //Store the rounded orientation of the gradient
  //. 0 = vertical. 45 = diagonal pos slope, 90 = vertical,  135 = diagonal neg slope
  TH2S * A = new TH2S("gAng","gAng",nx,xmin,xmax,ny,ymin,ymax); 

  TString histname = img->GetName(); 
  histname+="_thresh"; 
 
  gradient(img,M2,A,blurlevel,gradop,kernel_size);   

  //Low and high thresholds
  TH2C * Tl = new TH2C("thresh_low","thresh_low",nx,xmin,xmax,ny,ymin,ymax);
  TH2C * Th = new TH2C("thresh_hi","thresh_hi",nx,xmin,xmax,ny,ymin,ymax);

  //Final product
  TH2C * T = new TH2C(histname,histname,nx,xmin,xmax,ny,ymin,ymax);


  //Now, do non-maximum suppression to thin edges to one pixel
  //(require that 
  // the gradient is local maximum along the computed estimated 
  // orientation)

  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <= ny; j++)
    {
      bool ismax = true; 
    
      short ori = (short) A->GetBinContent(i,j); 

      if (ori == 0)
      {
        if ((i>1 &&  M2->GetBinContent(i-1,j) > M2->GetBinContent(i,j))
             || (i < nx &&  M2->GetBinContent(i+1,j) > M2->GetBinContent(i,j)) )
        {
          ismax = false;
        }
      }
      else if (ori == 90)
      {
        if ((j>1 &&  M2->GetBinContent(i,j-1) > M2->GetBinContent(i,j))
             || (j < ny &&  M2->GetBinContent(i,j+1) > M2->GetBinContent(i,j)) )
        {
          ismax = false;
        }
      }
      else if (ori == 45)
      {
        if ((i>1 && j>1 && M2->GetBinContent(i-1,j-1) > M2->GetBinContent(i,j))
            || (i<nx && j < ny && M2->GetBinContent(i+1,j+1) > M2->GetBinContent(i,j)))
        {
          ismax = false; 
        }
      }
      else if (ori == 135)
      {
        if ((i>1 && j<ny && M2->GetBinContent(i-1,j+1) > M2->GetBinContent(i,j))
            || (i<nx && j >1 && M2->GetBinContent(i+1,j-1) > M2->GetBinContent(i,j)))
        {
          ismax = false; 
        }
      }
      

      //If it is a local maximum, we can can consider this for hysteresis thresholding
      if (ismax)
      {
        if (M2->GetBinContent(i,j) > th_low*th_low)
        {
          Tl->SetBinContent(i,j,1);      
          if (M2->GetBinContent(i,j) > th_hi*th_hi)
          {
            Th->SetBinContent(i,j,1);      
          }
        }
      }
    }
  }
  

  M2->Delete(); 
  A->Delete(); 

  //Now, perform the hysteresis thresholding 

  for (int i = 1; i <= nx; i++)
  {
    for (int j = 1; j <= ny; j++)
    {
      if (Th->GetBinContent(i,j)==1)
      {
        T->SetBinContent(i,j,1); 
        hysteresisConnect(i,j,T,Th,Tl); 
      }
    }
  }

  Th->Delete(); 
  Tl->Delete(); 

  return T; 
}

TH2 * dmtpc::image::processing:: neighborRatio(const TH2 * in, bool abs, bool median, bool difference)
{
  double mean_kernel[9] =  {1,1,1,  1,0,1,  1,1,1}; 

  TH2 * out = median ? medianFilter(in,3,1) : convolve(in, mean_kernel,3,3,false); 

  for (int x = 1; x <= in->GetNbinsX(); x++) 
  {
    for (int y = 1; y <= in->GetNbinsY(); y++) 
    {
      double divideby = median ? 1 : 8; 
      double new_val = difference ? in->GetBinContent(x,y) - out->GetBinContent(x,y) / divideby : in->GetBinContent(x,y) / out->GetBinContent(x,y) / divideby ;  
      if (abs) new_val = fabs(new_val); 
      out->SetBinContent(x,y, new_val); 
    }
  }

  return out; 
}

int dmtpc::image::processing::killLonePixelsMedianDifference(TH2 * tokill, double threshold)
{
  TH2 * median = medianFilter(tokill,3,1); 
  int nkill = 0; 
  for (int x = 1; x <= tokill->GetNbinsX(); x++) 
  {
    for (int y = 1; y <= tokill->GetNbinsY(); y++) 
    {
      if (fabs(tokill->GetBinContent(x,y) - median->GetBinContent(x,y)) > threshold)
      {
        tokill->SetBinContent(x,y,median->GetBinContent(x,y)); 
        nkill++; 
      }
    }
  }
  median->Delete(); 
  return nkill; 
}


TH2 * dmtpc::image::processing::WienerGaussDeconvolve(const TH2 * s, double sigma, double noise_sigma)
{

    double x_w = s->GetXaxis()->GetXmax() - s->GetXaxis()->GetXmin(); 
    double y_w = s->GetYaxis()->GetXmax() - s->GetYaxis()->GetXmin(); 
    
    TVirtualFFT::SetTransform(0); 
    TH2 * S_m = 0; 
    TH2 * S_p = 0; 

    TH2 * ret= (TH2*) s->Clone(TString(s->GetName()) + TString("_wiener_decon")); 

    S_m = (TH2*) ret->FFT(S_m,"MAG"); 
    S_p = (TH2*) ret->FFT(S_p,"PH"); 

    TH2 * psd = (TH2*) S_m->Clone("psd_temp"); 


    int nx = s->GetNbinsX(); 
    int ny = s->GetNbinsY(); 

    int dim[2]; 
    dim[0] = nx; 
    dim[1] = ny; 
    TVirtualFFT * t = TVirtualFFT::FFT(2,dim,"C2R K"); 

 
    for (int x = 1; x <= nx; x++)
    {
      for (int y = 1; y <= ny/2+1; y++)
      {
        double fx = x <= nx/2 ? s->GetXaxis()->GetBinLowEdge(x) / x_w : -s->GetXaxis()->GetBinLowEdge(nx - x + 1) / x_w; 
        double fy = y <= ny/2 ? s->GetYaxis()->GetBinLowEdge(y) / y_w : -s->GetYaxis()->GetBinLowEdge(ny - y + 1) / y_w;

        double h = exp(-(sigma*sigma*M_PI)/2*(fx*fx+fy*fy)); 

        double sig = psd->GetBinContent(x,y); 
        double noi = noise_sigma * noise_sigma; 
        sig = (sig*sig)/(nx*ny) - noi;
        if (sig < 0) sig = 0; 


        double mag =  S_m->GetBinContent(x,y)/(nx*ny) * h * sig / (h*h*sig + noi); 
        double ph = S_p->GetBinContent(x,y); 

        int ind[2]; 
        ind[0] = x-1; 
        ind[1] = y-1; 

        t->SetPoint(ind,mag*cos(ph), mag * sin(ph)); 
      }
    }

   t->Transform(); 


   TH1::TransformHisto(t,ret,"RE"); 


   //cleanup
   delete t; 
   delete S_m; 
   delete S_p; 
   delete psd; 

   return ret;
}


void dmtpc::image::processing::addGaussianNoise(TH2 * in, double sigma) 
{

  static TRandom3 rand; 
  rand.SetSeed(0); 

  for (int x =1; x<=in->GetNbinsX(); x++)
  {
    for (int y =1; y<=in->GetNbinsX(); y++)
    {
      in->SetBinContent(x,y,in->GetBinContent(x,y) + rand.Gaus(0,sigma)); 
    }
  }
}




