#ifndef DMTPC_MATH_GEOMETRY_HH
#define DMTPC_MATH_GEOMETRY_HH


class TVector3; 

namespace dmtpc
{
  namespace math
   {

     /** compute the intersection between the lines defined by (x1,y1)-(x2,y2) and (x3,y3)-(x4,y4). 
      *
      * Returns 1 if they don't intersect, otherwise returns 0 and populates xint and yint
      */

      int lineLineIntersection(double x1,double y1, double x2, double y2, double x3, double y3, double x4, double y4,double * xint, double * yint); 

      /** calculate where a line hits a rectangle with sides orthogonal to coordinate system. Returns false if no intersection. Otherwise, x0,y0,x1,y1 are populated with the intersection points. 
        parameterization: y = -cotan(theta) + r / sin(theta) 
      */
      bool lineRectIntersectionPoints(double line_theta, double line_r, double rect_xmin, double rect_xmax, double rect_ymin, double rect_ymax, double * x0, double *y0, double * x1, double * y1); 



      /* Computes the intersection point between a line defined by l0 + l*t and a plane defined by p0 and n. Returns 0 if no intersection, 1 if intersection and 2 if intersection everywhere (i.e. line is in plane).  */ 
      int linePlaneIntersection(const TVector3 * l0, const TVector3 * l, const TVector3 * p0, const TVector3 * n, double * t); 

      /* Computes the intersection points between a line defined by l0 + l*t and cartesian aligned 3d rect 
       * with corners r0 and r1. Returns 0 if no intersection, 1 if one intersection, 2 if two intersections 
       * and -N if the line is collinear with one of the planes (where N = 1-6 number each of the 6 planes). 
       * In the case of one or two intersections, t0 and t1 are populated 
       *
       * If any component of r0 and r1 are equal, then that dimension is ignored. 
       *
       * */ 

      int line3DRectIntersection(const TVector3 * l0, const TVector3 * l, const TVector3 * r0, const TVector3 * r1, double *t0, double *t1); 
      
      // projection of a onto b 
      double scalarProjection(const TVector3 * a, const TVector3 * b); 

      double scalarRejection(const TVector3 * a, const TVector3 * b); 

      // projection of a onto b, changes a, returns scalar projection as byproduct
      void vectorProjection(TVector3 * a, const TVector3 * b); 


      /* line defined as s0 + vt */ 
      double linePointDistance(const TVector3 * pt, const TVector3 * s0, const TVector3 * v); 


      /* plane with point p0, normal pn */ 
      double pointPlaneDistance(const TVector3* pt, const TVector3 * p0, const TVector3* pn); 
   }
}

#endif
