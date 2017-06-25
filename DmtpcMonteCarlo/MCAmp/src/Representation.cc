#include "Representation.hh" 
#include "gmsh/GVertex.h"
#include "TMath.h"
#include "TVector3.h"


#define NCIRCLESEGS 32 
#define NSPLINESEGS 4

//ClassImp(dmtpc::mc::amp::Representation); 

void dmtpc::mc::amp::Representation::draw(const char * opt) 
{

  for (unsigned i = 0; i < repr.size(); i++) 
  {
    repr[i]->Draw(opt); 
  }
}

void dmtpc::mc::amp::Representation::setLineColor(int color) 
{
  for (unsigned i = 0; i < repr.size(); i++) 
  {
    repr[i]->SetLineColor(color);
  }
}

void dmtpc::mc::amp::Representation::addLine(const GVertex * v1, const GVertex * v2)
{
  TPolyLine3D * l = new TPolyLine3D(2); 
  l->SetPoint(0,v1->x(), v1->y(), v1->z()); 
  l->SetPoint(1,v2->x(), v2->y(), v2->z()); 
  add(l); 
}

void dmtpc::mc::amp::Representation::addCircleArc(const GVertex * v1, const GVertex * center, const GVertex * v2) 
{
  TVector3 p1(v1->x(),v1->y(),v1->z()); 
  TVector3 p2(v2->x(),v2->y(),v2->z()); 
  TVector3 pc(center->x(),center->y(),center->z()); 

  double phi = ((p1-pc).Angle(p2-pc)); 

  int np = 8;//int(NCIRCLESEGS * fabs(phi) / (2*TMath::Pi()) + 0.1);

  TPolyLine3D * l = new TPolyLine3D(np); 

  TVector3 norm = (p1-pc).Cross(p2-pc); 

  TVector3 pt = p1-pc; 

  for (unsigned i = 0; i < np; i++) 
  {
    pt.Rotate(phi/(np-1), norm); 

    TVector3 pt2 = pc + pt; 

    l->SetPoint(i, pt2.x(), pt2.y(), pt2.z()); 
  }

  add(l); 
}


void dmtpc::mc::amp::Representation::addBSpline(std::vector<GVertex *> * cpts) 
{

  unsigned s = cpts->size(); 
  int np = s * NSPLINESEGS; 

  std::vector<double> x(s + 2); 
  std::vector<double> y(s + 2); 
  std::vector<double> z(s + 2); 

  x[0] = cpts->at(0)->x(); 
  y[0] = cpts->at(0)->y(); 
  z[0] = cpts->at(0)->z(); 

  for (int i = 0; i < s; i++)
  {
    x[i+1] = cpts->at(i)->x(); 
    y[i+1] = cpts->at(i)->y(); 
    z[i+1] = cpts->at(i)->z(); 
  }

  x[s+1] = cpts->at(s-1)->x(); 
  y[s+1] = cpts->at(s-1)->y(); 
  z[s+1] = cpts->at(s-1)->z(); 

  TPolyLine3D * l = new TPolyLine3D(np); 



  for (int i = 0; i < np ; i++)
  {
    double u = double(np) / NSPLINESEGS + 2; 
    double val = 0; 

    for (unsigned j = 0; j < s+2; j++)
    {
      double weight = fabs(u - j) ; 


    }
  }




  add(l); 


}

