#include "Geometry.hh" 
#include <cmath>
#include "TVector3.h" 

int dmtpc::math::lineLineIntersection(double x1,double y1, double x2, double y2, double x3, double y3, double x4, double y4,double * xint, double * yint)
{
    double denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4); 

    if (denom == 0) return 1; 

    *xint = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))/denom; 
    *yint = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4))/denom; 

    return 0; 
}



bool  dmtpc::math::lineRectIntersectionPoints(double line_th, double line_r, double rect_xmin, double rect_xmax, double rect_ymin, double rect_ymax, double * x0, double * y0, double * x1, double * y1)
{
  double sinth = sin(line_th); 

  //Check case that m = infinity
  if (sinth == 0)
  {
    if (line_r > rect_xmin && line_r < rect_xmax)
    {
     *x0 = line_r; 
     *x1 = line_r; 
     *y0 = rect_ymin; 
     *y1 = rect_ymax; 
     return true; 
    }
    else
    {
      return false; 
    }
  }

  double costh = cos(line_th); 

  //case that m = 0
  if (costh == 0)
  {
    if (line_r > rect_ymin && line_r < rect_ymin) 
    {
       *y0 = line_r; 
       *y1 = line_r; 
       *x0 = rect_xmin; 
       *x1 = rect_xmax; 
       return true; 
    }
  }
  
 //evaluate line 
  double m =  -1/tan(line_th); 
  double b = line_r/sin(line_th); 
  

  double y_at_xmin = m * rect_xmin + b; 
  double y_at_xmax = m * rect_xmax + b; 
  double x_at_ymax = (rect_ymax - b ) / m; 
  double x_at_ymin = (rect_ymin - b ) / m;

  
  bool hits_W = (y_at_xmin > rect_ymin && y_at_xmin < rect_ymax); 
  bool hits_E = (y_at_xmax > rect_ymin && y_at_xmax < rect_ymax); 
  bool hits_S = (x_at_ymin > rect_xmin && x_at_ymin < rect_xmax); 
  bool hits_N = (x_at_ymax > rect_xmin && x_at_ymax < rect_xmax); 



  if (hits_W && hits_E)
  {
    *x0 = rect_xmin; 
    *y0 = y_at_xmin; 
    *x1 = rect_xmax; 
    *y1 = y_at_xmax; 
  }
  else if (hits_W && hits_N)
  {
    *x0 = rect_xmin; 
    *y0 = y_at_xmin; 
    *x1 = x_at_ymax; 
    *y1 = rect_ymax;
  }
  else if (hits_W && hits_S)
  {
    *x0 = rect_xmin; 
    *y0 = y_at_xmin; 
    *x1 = x_at_ymin; 
    *y1 = rect_ymin;
  }
  else if (hits_S && hits_N)
  {
    if (x_at_ymin < x_at_ymax)
    {
      *x0 = x_at_ymin;  
      *y0 = rect_ymin; 
      *x1 = x_at_ymax; 
      *y1 = rect_ymax;
    }
    else
    {
      *x1 = x_at_ymin;  
      *y1 = rect_ymin; 
      *x0 = x_at_ymax; 
      *y0 = rect_ymax;
    }
  }


  else if (hits_S && hits_E)
  {
    *x0 = x_at_ymin;  
    *y0 = rect_ymin; 
    *x1 = rect_xmax; 
    *y1 = y_at_xmax; 
  }
  else if (hits_N && hits_E)
  {
    *x0 = x_at_ymax;  
    *y0 = rect_ymax; 
    *x1 = rect_xmax; 
    *y1 = y_at_xmax; 
  }
  else return false; 

  return true; 
}

static double between(double x, double x0, double x1) 
{
  return (x > x0 && x < x1) || (x > x1 && x < x0) || (x0 == x1); 
}


int dmtpc::math::linePlaneIntersection(const TVector3 * l0, const TVector3 * l, const TVector3 * p0, const TVector3 * n, double * t_intersection) 
{
  double denom = l->Dot(*n); 
  double num = (*p0-*l0).Dot(*n); 
  if (denom ==0 && num !=0)
    return 0; 
  else if (denom == 0 && num ==0)
  {
    *t_intersection = 0; 
    return 2; 
  }
   
  *t_intersection = num / denom; 

  return 1; 
}



int dmtpc::math::line3DRectIntersection(const TVector3 * l0, const TVector3 * l, const TVector3 * r0, const TVector3 * r1, double * t0, double *t1) 
{

  //check intersection with each plane
  TVector3 z(0,0,1); 
  TVector3 x(1,0,0); 
  TVector3 y(0,1,0); 

  const TVector3 * p[6] = {r0,r0,r0,r1,r1,r1}; 
  TVector3 * n[6] = {&x,&y,&z,&x,&y,&z}; 

  int nint = 0; 

  *t0 = 0; 
  *t1 = 0; 
  for (int i = 0; i < 6; i++)
  {

    double t;
    int r = linePlaneIntersection(l0,l,p[i],n[i],&t); 
    if (r == 2) 
    {
      return -(i+1); 
    }
    if (r==1) 
    {
      //check to see if within bounds
      
      TVector3 pt = *l0 + (*l)*t; 
      bool bound = false; 

      if (i %3 == 0)//x-normal 
      {
        bound = between(pt.y(), r0->y(), r1->y()) && 
                between(pt.z(), r0->z(), r1->z()); 

      }
      else if (i %3 == 1)//y-normal 
      {
        bound = between(pt.x(), r0->x(), r1->x()) && 
                between(pt.z(), r0->z(), r1->z()); 

      }
      else //z-normal
      {
        bound = between(pt.x(), r0->x(), r1->x()) && 
                between(pt.y(), r0->y(), r1->y()); 

      }

      if (!bound) continue; 

      if (nint == 0) 
      {
        *t0 = t; 
        nint++; 
      }
      else
      {
        *t1 = t; 
        return 2; 
      }
    }
  }


  return nint; 
}

double dmtpc::math::scalarRejection(const TVector3 * a, const TVector3 *b)
{
  return  sqrt(a->Mag2() - pow(a->Dot(*b),2) / b->Mag2()); 
}
double dmtpc::math::scalarProjection(const TVector3 * a, const TVector3 *b)
{
  return a->Dot(*b) / b->Mag(); 
}

void dmtpc::math::vectorProjection(TVector3 * a, const TVector3 *b)
{
  double c = a->Dot(*b) / b->Mag2(); 
  *a=c**b; //hey, at least it's not *a*=*c**b 
}

double dmtpc::math::linePointDistance(const TVector3 * p, const TVector3 * s0, const TVector3 *v)
{
  return (*s0 - *p).Dot(*v) / (v->Mag2()); 
}

double dmtpc::math::pointPlaneDistance(const TVector3 *p, const TVector3* p0, const TVector3* pn)
{
  TVector3 displacement = *p - *p0;
  return scalarProjection(&displacement, pn); 
}
