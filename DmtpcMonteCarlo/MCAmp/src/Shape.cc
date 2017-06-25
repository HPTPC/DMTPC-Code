#include "Shape.hh"
#include <gmsh/GModel.h>
#include <iostream>
#include <gmsh/GEntity.h>
#include <assert.h>

/** Beware all who read it may take a while
 * This code lacks in both comments and style 
 *
 *
 * Note that it requires a patch to gmsh 
 */


int dmtpc::mc::amp::Shape::addToPhysicalVolume(int id)
{
  for (unsigned i = 0; i < vols.size(); i++)
  {
    vols[i]->addPhysicalEntity(id); 
  }
  return vols.size(); 
}


int dmtpc::mc::amp::Shape::addToPhysicalSurface(int id)
{
  for (unsigned i = 0; i < faces.size(); i++)
  {
    for (unsigned j = 0; j < faces[i].size(); j++)
    {
      faces[i][j]->addPhysicalEntity(id); 
    }
  }
  return faces.size(); 
}

//convenience method 
GVertex * addPt(GModel * m, TVector3 pt, double size)
{
  return m->addVertex(pt.x(),pt.y(),pt.z(), size); 
}


GEdge * addLine(GModel *m, dmtpc::mc::amp::Representation * repr,  GVertex *v1, GVertex * v2)
{
  repr->addLine(v1,v2); 
  return m->addLine(v1,v2); 
}

GEdge * addCircleArcCenter(GModel *m,  dmtpc::mc::amp::Representation * repr, GVertex * v1, GVertex *center, GVertex * v2)
{
  repr->addCircleArc(v1,center,v2); 
  return m->addCircleArcCenter(v1,center,v2); 
}



GVertex * addPtRotated(GModel * m, TVector3 pt, double size, double rotation, TVector3 norm)
{
//  printf("addPtRotated(%x, [%g %g %g], %g, %g [%g %g %g]\n", m, pt.x(), pt.y(), pt.z(), size, rotation, norm.x(),norm.y(),norm.z()); 
  pt.Rotate(rotation,norm); 
  return m->addVertex(pt.x(),pt.y(),pt.z(), size); 
}


dmtpc::mc::amp::Box::Box(TVector3 * c, TVector3* w, TVector3 * o, double r)
:  center(0,0,0), widths(1,1,1), orientation(0,0,1), rotation(r) 
{
  if (c) center = *c; 
  if (w) widths = *w; 
  if (o) orientation = *o; 
  size = 0;
}

dmtpc::mc::amp::Disc::Disc(TVector3 * c, double r, double h, TVector3 * o)
:  center(0,0,0), orientation(0,0,1), radius(r), height(h)
{
  if (c) this->center = *c; 
  if (o) this->orientation = *o; 
  size = 0;
}

dmtpc::mc::amp::Ring::Ring(TVector3 * center, double inner_radius, double outer_radius, double height, TVector3 * orientation)
:  center(0,0,0), orientation(0,0,1), inner_radius(inner_radius),outer_radius(outer_radius), height(height)
{
  if (center) this->center = *center; 
  if (orientation) this->orientation = *orientation; 
  size = 0;
}

dmtpc::mc::amp::Torus::Torus(TVector3 * center, double big_radius, double small_radius, TVector3 * orientation)
:  center(0,0,0), orientation(0,0,1), big_radius(big_radius),small_radius(small_radius) 
{
  if (center) this->center = *center; 
  if (orientation) this->orientation = (*orientation).Unit(); 
  size = 0;
}


int dmtpc::mc::amp::WovenMesh::addBraid(GModel * m, TVector3 start, TVector3 dir, int nperiods, bool parity)
{

  newFaceLoop(); 

  int nnodes = nperiods;


  if(!size)  size = wire_radius; // not sure if this might be too coarse... 

  std::vector<std::vector<GVertex*> > circle_pts(nnodes,std::vector<GVertex*>(4)); 
  std::vector<std::vector<GVertex*> > spline_pts_1(nnodes-1,std::vector<GVertex*>(4)); 
  std::vector<std::vector<GVertex*> > spline_pts_2(nnodes-1,std::vector<GVertex*>(4)); 
  dir = dir.Unit(); 
  TVector3 transverse = dir.Cross(orientation); 
  std::vector<std::vector<GEdge*> > transverse_edges(nnodes,std::vector<GEdge*>(4)); 
  std::vector<std::vector<GEdge*> > long_edges(nnodes-1,std::vector<GEdge*>(4)); 

  for (int i = 0; i < nnodes; i++) 
  {


    TVector3 circle_center = start + dir * wire_pitch *i  + orientation * (i %2 == parity ? (1+eps)*wire_radius : -(1+eps)*wire_radius); ; 
    GVertex * cc = addPt(m,circle_center,wire_pitch); 
    points.push_back(cc); 
    circle_pts[i][0] = addPt(m,circle_center + wire_radius * orientation,size); 
    circle_pts[i][1] = addPt(m,circle_center + wire_radius * transverse,size); 
    circle_pts[i][2] = addPt(m,circle_center - wire_radius * orientation,size); 
    circle_pts[i][3] = addPt(m,circle_center - wire_radius * transverse,size); 


    if (i < nnodes -1 )
    {
      spline_pts_1[i][0] = addPt(m,circle_center + wire_radius * orientation + splinedist*dir * wire_radius,size); 
      spline_pts_1[i][1] = addPt(m,circle_center + wire_radius * transverse + splinedist*dir * wire_radius,size); 
      spline_pts_1[i][2] = addPt(m,circle_center - wire_radius * orientation + splinedist*dir * wire_radius,size); 
      spline_pts_1[i][3] = addPt(m,circle_center - wire_radius * transverse + splinedist*dir * wire_radius,size); 
    }

    if (i > 0)
    {

      spline_pts_2[i-1][0] = addPt(m,circle_center + wire_radius * orientation - splinedist*dir * wire_radius,size); 
      spline_pts_2[i-1][1] = addPt(m,circle_center + wire_radius * transverse - splinedist*dir * wire_radius,size); 
      spline_pts_2[i-1][2] = addPt(m,circle_center - wire_radius * orientation - splinedist*dir * wire_radius,size); 
      spline_pts_2[i-1][3] = addPt(m,circle_center - wire_radius * transverse - splinedist*dir * wire_radius,size); 


    }

    for (unsigned j = 0; j <4; j++)
    {
      points.push_back(circle_pts[i][j]); 

      transverse_edges[i][j] = addCircleArcCenter(m,&repr,circle_pts[i][j], cc, circle_pts[i][(j+1)%4]); 
      edges.push_back(transverse_edges[i][j]); 
    }

    if (i>0) 
    {
      for (unsigned j= 0; j < 4; j++)
      {
        std::vector<GVertex*> cpts; 
        
        GVertex * start = circle_pts[i-1][j]; 
        GVertex * s1 = spline_pts_1[i-1][j]; 
        GVertex * s2 = spline_pts_2[i-1][j]; 
        GVertex * end = circle_pts[i][j]; 

        cpts.push_back(start);
        cpts.push_back(s1);
        cpts.push_back(s2);
        cpts.push_back(end);

        /** ignore this... 
        int istart = start->tag(); 
        int is1 = s1->tag(); 
        int is2 = s2->tag(); 
        int iend = end->tag(); 

        //wow this sucks,  gmsh needs to expose its API better. See Geo.cpp in gmsh to help follow
        List_T *ctrl_points = List_Create(4,1,sizeof(int)); 
        List_Write(ctrl_points,0,&istart); 
        List_Write(ctrl_points,1,&is1); 
        List_Write(ctrl_points,2,&is2); 
        List_Write(ctrl_points,3,&iend); 

        Curve* curve = Create_Curve(NEWLINE(), MSH_SEGM_BSPLN, ,2,  ctrl_points,-1,-1,0,1); 
        gmshEdge * edge = new 7
        **/

         long_edges[i-1][j] = m->addBSpline(cpts); 
      }

      for (unsigned j =0; j<4; j++)
      {
         std::vector<std::vector<GEdge*> > loop; 
         std::vector<GEdge*> these_edges; 

         these_edges.push_back(transverse_edges[i-1][j]); 
         these_edges.push_back(long_edges[i-1][j]); 
         these_edges.push_back(transverse_edges[i][j]); 
         these_edges.push_back(long_edges[i-1][(j+1)%4]); 
         loop.push_back(these_edges); 

          std::vector<GFace*> ruled = m->addRuledFaces(loop); 
         newestFaceLoop()->insert(newestFaceLoop()->end(),ruled.begin(),ruled.end());

      }

    }

  }

  //add front and back faces 

  std::vector<std::vector<GEdge*> > front_loop; 
  std::vector<GEdge*> front_edges; 
  std::vector<std::vector<GEdge*> > back_loop; 
  std::vector<GEdge*> back_edges; 
  for (int i = 0; i <4; i++)
  {
    front_edges.push_back(transverse_edges[0][i]); 
    back_edges.push_back(transverse_edges[nnodes-1][i]); 
  }

  front_loop.push_back(front_edges); 
  back_loop.push_back(back_edges); 

  GFace * front_face = m->addPlanarFace(front_loop); 
  GFace * back_face = m->addPlanarFace(back_loop); 
  newestFaceLoop()->push_back(front_face); 
  newestFaceLoop()->push_back(back_face); 
  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(*newestFaceLoop()); 

  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 

  return 0; 

}

dmtpc::mc::amp::WovenMesh::WovenMesh(TVector3 * center, double wire_pitch, double wire_radius,  TVector3 * orientation, unsigned nwiresx,unsigned nwiresy,  double rotation, double separation_eps, double spline_dist ) 
: Mesh(center,wire_pitch,wire_radius,orientation, nwiresx, nwiresy, rotation), eps(separation_eps), splinedist(spline_dist)
{

}




int dmtpc::mc::amp::WovenMesh::build(GModel * m, bool bounding, std::vector<Shape*> *extant_shapes)
{

  if (bounding)
  {
    fprintf(stderr,"WovenMesh not supported as bounded volume\n"); 
  }

  TVector3 norm  = orientation.Unit(); 

  int ret; 
  for (int x = 0; x < nwiresx; x++)
  {
     TVector3 start = center - dirx * (wire_pitch * (nwiresy/2.+0.5 )) + diry * (wire_pitch * (nwiresx/2. -x-0.5)); 


     ret+=addBraid(m,start,dirx,nwiresy+2, x%2 == 0); 
//     printf("x: %d\n",x); 
  }

  for (int y = 0; y < nwiresy; y++)
  {
     TVector3 start = center - diry * (wire_pitch * (nwiresx/2.+0.5)) + dirx * (wire_pitch * (nwiresy/2. -y -0.5)); 
     ret+=addBraid(m,start,diry,nwiresx+2, y%2 == 1); 
//     printf("y: %d\n",y); 
  }

  return ret; 
}


int dmtpc::mc::amp::SimpleMesh::build(GModel * m, bool bounding, std::vector<Shape *> * extant_shapes)
{


  if (bounding)
  {
    fprintf(stderr,"SimpleMesh not supported as bounded volume\n"); 
  }

  newFaceLoop(); 

  TVector3 norm  = orientation.Unit(); 

  if (!size) size = wire_radius; 


  std::vector<std::vector<GVertex* > > intersection_centers(nwiresx, std::vector<GVertex*>(nwiresy)); 
  std::vector<std::vector<std::vector<GVertex*> > > ellipse_points(nwiresx, std::vector<std::vector<GVertex*> > (nwiresy, std::vector<GVertex*>(6))); 
  std::vector<std::vector<std::vector<GVertex*> > > circle_points(nwiresx, std::vector<std::vector<GVertex*> > (nwiresy, std::vector<GVertex*>(16))); 
  std::vector<std::vector<std::vector<GVertex*> > > circle_centers(nwiresx, std::vector<std::vector<GVertex*> > (nwiresy, std::vector<GVertex*>(4))); 
  std::vector<std::vector<std::vector<GEdge*> > > ellipse_edges(nwiresx, std::vector<std::vector<GEdge*> > (nwiresy, std::vector<GEdge*>(8))); 
  std::vector<std::vector<std::vector<GEdge*> > > circle_edges(nwiresx, std::vector<std::vector<GEdge*> > (nwiresy, std::vector<GEdge*>(16))); 
  std::vector<std::vector<std::vector<GEdge*> > > long_edges(nwiresx, std::vector<std::vector<GEdge*> > (nwiresy, std::vector<GEdge*>(16))); 

  for (unsigned i = 0; i < nwiresx; i++)
  {
      for (unsigned j = 0; j < nwiresy; j++)
      {
        TVector3 center_pt = center + dirx * (wire_pitch * (nwiresx/2. - j - 0.5)) + diry * (wire_pitch * (nwiresy/2. - i - 0.5));

        
        intersection_centers[i][j] = addPt(m,center_pt ,size); 

        //make circles first
        
        TVector3 dirs_main[4] = { dirx,diry,-dirx,-diry}; 
        TVector3 dirs_orth[4] = { diry,dirx,-diry,-dirx}; 
        
        for (int c = 0; c < 4; c++) 
        {


          int jo= c == 0 && j > 0; 
          int io= c == 1 && i > 0; 

          if (io || jo) 
          {


            circle_centers[i][j][c] = circle_centers[i-io][j-jo][c+2] ; 
            
            for (int p = 0; p < 4; p++)
            {
              int po = (p%2) ? 2 : 0; 
              circle_points[i][j][4*c+ p] = circle_points[i-io][j-jo][4*(c+2)+ (p+po)%4]; 
              circle_edges[i][j][4*c+p] = circle_edges[i-io][j-jo][4*(c+2)+ (3-p)]; 
            }
          }

          else
          {
            circle_centers[i][j][c] = addPt(m, center_pt + wire_pitch/2 * dirs_main[c],size);
            circle_points[i][j][4*c] = addPt(m, center_pt + wire_pitch/2 * dirs_main[c] + wire_radius * norm,size);
            circle_points[i][j][4*c+1] = addPt(m, center_pt + wire_pitch/2 * dirs_main[c] + wire_radius * dirs_orth[c],size);
            circle_points[i][j][4*c+2] = addPt(m, center_pt + wire_pitch/2 * dirs_main[c] - wire_radius * norm,size);
            circle_points[i][j][4*c+3] = addPt(m, center_pt + wire_pitch/2 * dirs_main[c] - wire_radius * dirs_orth[c],size);

            circle_edges[i][j][4*c] = addCircleArcCenter(m,&repr,circle_points[i][j][4*c],
                                                            circle_centers[i][j][c], 
                                                            circle_points[i][j][4*c+1]); 

            circle_edges[i][j][4*c+1] = addCircleArcCenter(m,&repr,circle_points[i][j][4*c+1],
                                                            circle_centers[i][j][c], 
                                                            circle_points[i][j][4*c+2]); 

            circle_edges[i][j][4*c+2] = addCircleArcCenter(m,&repr,circle_points[i][j][4*c+2],
                                                            circle_centers[i][j][c], 
                                                            circle_points[i][j][4*c+3]); 

            circle_edges[i][j][4*c+3] = addCircleArcCenter(m,&repr,circle_points[i][j][4*c+3],
                                                            circle_centers[i][j][c], 
                                                            circle_points[i][j][4*c]); 
          }
        }

        //check if we need to make a circle face 

        if (j == 0)
        {
           std::vector<std::vector<GEdge*> > loop; 
           loop.push_back(std::vector<GEdge*>(circle_edges[i][j].begin()+0, circle_edges[i][j].begin()+4)); 
           faces[0].push_back(m->addPlanarFace(loop)); 
        }

        if (i == 0)
        {
           std::vector<std::vector<GEdge*> > loop; 
           loop.push_back(std::vector<GEdge*>(circle_edges[i][j].begin()+4, circle_edges[i][j].begin()+8)); 
           faces[0].push_back(m->addPlanarFace(loop)); 
        }

        if (i == nwiresy-1)
        {
           std::vector<std::vector<GEdge*> > loop; 
           loop.push_back(std::vector<GEdge*>(circle_edges[i][j].begin()+12, circle_edges[i][j].begin()+16)); 
           faces[0].push_back(m->addPlanarFace(loop)); 
        }

        if (j == nwiresx-1)
        {
           std::vector<std::vector<GEdge*> > loop; 
           loop.push_back(std::vector<GEdge*>(circle_edges[i][j].begin()+8, circle_edges[i][j].begin()+12)); 
           faces[0].push_back(m->addPlanarFace(loop)); 
        }



        //make ellipses 
     
        ellipse_points[i][j][0] = addPt(m,center_pt + norm * wire_radius,size); 
        ellipse_points[i][j][1] = addPt(m,center_pt - norm * wire_radius,size); 
        ellipse_points[i][j][2] = addPt(m,center_pt + (dirx+diry) * wire_radius,size); 
        ellipse_points[i][j][3] = addPt(m,center_pt + (dirx-diry) * wire_radius,size); 
        ellipse_points[i][j][4] = addPt(m,center_pt + (-dirx-diry) * wire_radius,size); 
        ellipse_points[i][j][5] = addPt(m,center_pt + (-dirx+diry) * wire_radius,size); 

        for (int e = 0; e < 4; e++) 
        {
          ellipse_edges[i][j][2*e] = m->addEllipseArc(ellipse_points[i][j][0], intersection_centers[i][j], ellipse_points[i][j][2+e], ellipse_points[i][j][2+e]); 
          ellipse_edges[i][j][2*e+1] = m->addEllipseArc(ellipse_points[i][j][2+e], intersection_centers[i][j], ellipse_points[i][j][2+e], ellipse_points[i][j][1]); 
        }

        //make longitudinal edges and ruled faces 
        //
        
        int ell_order_1[4] =  { 2,2,4,4}; 
        int ell_order_3[4] =  { 3,5,5,3}; 



        

        for (int c = 0; c < 4; c++)
        {
            long_edges[i][j][4*c] = addLine(m,&repr,circle_points[i][j][4*c], ellipse_points[i][j][0]); 
            long_edges[i][j][4*c + 1] = addLine(m,&repr,circle_points[i][j][4*c+1], ellipse_points[i][j][ell_order_1[c]]); 
            long_edges[i][j][4*c + 2] = addLine(m,&repr,circle_points[i][j][4*c+2], ellipse_points[i][j][1]); 
            long_edges[i][j][4*c + 3] = addLine(m,&repr,circle_points[i][j][4*c+3], ellipse_points[i][j][ell_order_3[c]]);



        }

        int face_order[16] = {0,1,3,2,
                              0,1,7,6,
                              4,5,7,6,
                              4,5,3,2} ; 


        //let's make  some faces! 
        for (int c = 0; c < 4; c++)
        {
            for (int f = 0; f < 4; f++) 
            {
              std::vector<GEdge *> es; 
              es.push_back(long_edges[i][j][4*c+f]); 
              es.push_back(circle_edges[i][j][4*c+f]);   
              es.push_back(long_edges[i][j][4*c+(f+1) % 4]); // yep 
              es.push_back(ellipse_edges[i][j][face_order[4*c+f]]);  //black magic here... 

              std::vector<std::vector<GEdge*> > loop; 
              loop.push_back(es); 

              std::vector<GFace*> ruled =m->addRuledFaces(loop) ; 
              newestFaceLoop()->insert(faces[0].end(),ruled.begin(),ruled.end());
            }
        }

      }

  }

  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(*newestFaceLoop()); 

  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 

  return 0; 

}


int dmtpc::mc::amp::Disc::build(GModel * m, bool bounding, std::vector<Shape*>* extant_shapes)
{
  newFaceLoop(); 
  if(!size) size = TMath::Min(height,radius/4); 
//  GVertex * vcenter = addPt(m,center,size);
//  points.push_back(vcenter); 

  TVector3 norm  = orientation.Unit(); 
  TVector3 orth = norm.Orthogonal(); 
  TVector3 orth2 = norm.Cross(orth); 

  GVertex * center_top = addPt(m,center + height/2*norm,size);
  GVertex * center_bottom = addPt(m,center-height/2*norm,size);

  GVertex * v1 = addPt(m, (center + radius * orth  + height/2*norm), size); 
  GVertex * v2 = addPt(m, (center + radius * orth2  + height/2*norm), size); 
  GVertex * v3 = addPt(m, (center - radius * orth  + height/2*norm), size); 
  GVertex * v4 = addPt(m, (center - radius * orth2  + height/2*norm), size); 

  GVertex * v5 = addPt(m, (center + radius * orth  - height/2*norm), size); 
  GVertex * v6 = addPt(m, (center + radius * orth2  - height/2*norm), size); 
  GVertex * v7 = addPt(m, (center - radius * orth  - height/2*norm), size); 
  GVertex * v8 = addPt(m, (center - radius * orth2  - height/2*norm), size); 

  points.push_back(v1); points.push_back(v2); points.push_back(v3); points.push_back(v4); 
  points.push_back(v5); points.push_back(v6); points.push_back(v7); points.push_back(v8); 

  GEdge * v1_v5 = addLine(m,&repr,v1,v5); 
  GEdge * v2_v6 = addLine(m,&repr,v2,v6); 
  GEdge * v3_v7 = addLine(m,&repr,v3,v7); 
  GEdge * v4_v8 = addLine(m,&repr,v4,v8); 

  GEdge * v1_v2 = addCircleArcCenter(m,&repr,v1,center_top,v2); 
  GEdge * v2_v3 = addCircleArcCenter(m,&repr,v2,center_top,v3); 
  GEdge * v3_v4 = addCircleArcCenter(m,&repr,v3,center_top,v4); 
  GEdge * v4_v1 = addCircleArcCenter(m,&repr,v4,center_top,v1); 
  GEdge * v5_v6 = addCircleArcCenter(m,&repr,v5,center_bottom,v6); 
  GEdge * v6_v7 = addCircleArcCenter(m,&repr,v6,center_bottom,v7); 
  GEdge * v7_v8 = addCircleArcCenter(m,&repr,v7,center_bottom,v8); 
  GEdge * v8_v5 = addCircleArcCenter(m,&repr,v8,center_bottom,v5); 

  GEdge* top_edges_v[4] = {v1_v2,v2_v3,v3_v4,v4_v1}; 
  GEdge* bottom_edges_v[4] = {v5_v6,v6_v7,v7_v8,v8_v5}; 
  GEdge* front_edges_v[4] = {v1_v2,v2_v6,v5_v6,v1_v5}; 
  GEdge* right_edges_v[4] = {v2_v3,v3_v7,v6_v7,v2_v6}; 
  GEdge* back_edges_v[4] = {v3_v4,v4_v8,v7_v8,v3_v7}; 
  GEdge* left_edges_v[4] = {v4_v1,v1_v5,v8_v5,v4_v8}; 

  GEdge** edges_v[6] = {top_edges_v,bottom_edges_v,front_edges_v,right_edges_v,back_edges_v,left_edges_v}; 

  edges.insert(edges.end(),top_edges_v,top_edges_v+4); 
  edges.insert(edges.end(),bottom_edges_v,bottom_edges_v+4); 
  edges.push_back(v1_v5); edges.push_back(v2_v6); edges.push_back(v3_v7); edges.push_back(v4_v8); 

  for (int i = 0; i < 6; i++)
  {
    std::vector<GEdge*> these_edges(edges_v[i], edges_v[i]+4); 
    std::vector<std::vector<GEdge*> > loop; 
    loop.push_back(these_edges); 
    if (i < 2)
    {
     faces[0].push_back(m->addPlanarFace(loop)); 
    }
    else
    {
      std::vector<GFace*> ruled =m->addRuledFaces(loop) ; 
      faces[0].insert(faces[0].end(),ruled.begin(),ruled.end());
    }
  }

  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(faces[0]); 

  if (bounding)
  {
    for (unsigned i = 0; i < extant_shapes->size(); i++)
    {
      for (unsigned j = 0; j < extant_shapes->at(i)->getFaceLoops()->size(); j++)
      {
        surfloop.push_back(extant_shapes->at(i)->getFaceLoops()->at(j)); 
      }
  }
  }


  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 
  return 0; 
}



int dmtpc::mc::amp::Torus::build(GModel * m, bool bounding, std::vector<Shape*>* extant_shapes)
{

  if (bounding)
  {
    fprintf(stderr,"Torus not supported as bounded volume\n"); 
  }

  newFaceLoop(); 

  assert(big_radius > small_radius); 

  if(!size)  size = small_radius; 

  TVector3 norm  = orientation.Unit(); 
  TVector3 orth = norm.Orthogonal(); 
  TVector3 orth2 = norm.Cross(orth); 

  GVertex * vic = addPt(m,center,size); 
  points.push_back(vic);

  GVertex * vic_high = addPt(m,center+small_radius*norm,size); 
  points.push_back(vic_high);

  GVertex * vic_low = addPt(m,center-small_radius*norm,size); 
  points.push_back(vic_low);


  GVertex * voc[4]; 


  TVector3 circle_centers[4]; 

  circle_centers[0] = center + big_radius * orth; 
  circle_centers[1] = center + big_radius * orth2; 
  circle_centers[2] = center - big_radius * orth; 
  circle_centers[3] = center - big_radius * orth2; 

  for (int i = 0; i < 4; i++)
  {
    voc[i] = addPt(m, circle_centers[i], size); 
    points.push_back(voc[i]); 
  }


  GVertex * o[4][4]; //circle outer points

  GEdge * es[4][4];  //small edges, first index is around loop normal
  GEdge * eb[4][4];  //big edges

  //make the outer points and the small edges
  for (int i = 0; i < 4; i++)
  {
     o[i][0] = addPt(m, circle_centers[i] + small_radius * norm, size); 
     o[i][1] = addPt(m, circle_centers[i] + (small_radius/big_radius) * (circle_centers[i]-center), size); 
     o[i][2] = addPt(m, circle_centers[i] - small_radius * norm, size); 
     o[i][3] = addPt(m, circle_centers[i] - (small_radius/big_radius) * (circle_centers[i]-center), size); 
     for (int j = 0; j < 4; j++)
     {
        points.push_back(o[i][j]); 
        es[i][j] = addCircleArcCenter(m,&repr,o[i][j], voc[i], o[i][(j+1)%4]); 
        edges.push_back(es[i][j]); 
     }
  }

  //add the remaining edges
  for (int i = 0; i <4; i++)
  {
    eb[i][0] = addCircleArcCenter(m,&repr,o[i][0], vic_high, o[(i+1)%4][0]);  
    eb[i][1] = addCircleArcCenter(m,&repr,o[i][1], vic, o[(i+1)%4][1]);  
    eb[i][2] = addCircleArcCenter(m,&repr,o[i][2], vic_low, o[(i+1)%4][2]);  
    eb[i][3] = addCircleArcCenter(m,&repr,o[i][3], vic, o[(i+1)%4][3]);  
    for (int j = 0; j <4; j++)
    {
       edges.push_back(eb[i][j]); 
    }
  }

  //create the surfaces 

  for (int i = 0; i <4; i++)
  {
    for (int j = 0; j <4; j++)
    {
      std::vector<std::vector<GEdge*> > loop; 
      std::vector<GEdge*> these_edges; 
      these_edges.push_back(es[i][j]);
      these_edges.push_back(eb[i][j]);
      these_edges.push_back(es[(i+1)%4][j]);
      these_edges.push_back(eb[i][(j+1) %4]);
      loop.push_back(these_edges); 

      std::vector<GFace*> ruled = m->addRuledFaces(loop); 
      faces[0].insert(faces[0].end(),ruled.begin(),ruled.end());
    }
  }

  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(faces[0]); 

  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 

  return 0; 
}



int dmtpc::mc::amp::Ring::build(GModel * m, bool bounding, std::vector<Shape*>* extant_shapes)
{

  if (bounding)
  {
    fprintf(stderr,"Ring not supported as bounded volume\n"); 
  }

  newFaceLoop(); 

  assert(outer_radius > inner_radius); 

  if (!size) 
  {
    size = TMath::Min(height,inner_radius); 
    size = TMath::Min(size,(outer_radius - inner_radius)); 
  }
//  GVertex * cpt = addPt(m,center,size);
 // points.push_back(cpt); 
  

  TVector3 norm  = orientation.Unit(); 
  TVector3 orth = norm.Orthogonal(); 
  TVector3 orth2 = norm.Cross(orth); 

  GVertex * center_top = addPt(m,center + height/2*norm,size);
  GVertex * center_bottom = addPt(m,center-height/2*norm,size);

  GVertex * v1 = addPt(m, (center + outer_radius * orth + height/2*norm), size); 
  GVertex * v2 = addPt(m, (center + outer_radius * orth2 + height/2*norm), size); 
  GVertex * v3 = addPt(m, (center - outer_radius * orth + height/2*norm), size); 
  GVertex * v4 = addPt(m, (center - outer_radius * orth2 + height/2*norm), size); 

  GVertex * v5 = addPt(m, (center + inner_radius * orth + height/2*norm), size); 
  GVertex * v6 = addPt(m, (center + inner_radius * orth2 + height/2*norm), size); 
  GVertex * v7 = addPt(m, (center - inner_radius * orth + height/2*norm), size); 
  GVertex * v8 = addPt(m, (center - inner_radius * orth2 + height/2*norm), size); 

  GVertex * v9 = addPt(m, (center + outer_radius * orth - height/2*norm), size); 
  GVertex * v10 = addPt(m, (center + outer_radius * orth2 - height/2*norm), size); 
  GVertex * v11 = addPt(m, (center - outer_radius * orth - height/2*norm), size); 
  GVertex * v12 = addPt(m, (center - outer_radius * orth2 - height/2*norm), size); 

  GVertex * v13 = addPt(m, (center + inner_radius * orth - height/2*norm), size); 
  GVertex * v14 = addPt(m, (center + inner_radius * orth2 - height/2*norm), size); 
  GVertex * v15 = addPt(m, (center - inner_radius * orth - height/2*norm), size); 
  GVertex * v16 = addPt(m, (center - inner_radius * orth2 - height/2*norm), size); 


  points.push_back(v1); points.push_back(v2); points.push_back(v3); points.push_back(v4); 
  points.push_back(v5); points.push_back(v6); points.push_back(v7); points.push_back(v8); 
  points.push_back(v9); points.push_back(v10); points.push_back(v11); points.push_back(v12); 
  points.push_back(v13); points.push_back(v14); points.push_back(v15); points.push_back(v16); 

  GEdge * v1_v9 = addLine(m,&repr,v1,v9); 
  GEdge * v2_v10 = addLine(m,&repr,v2,v10); 
  GEdge * v3_v11 = addLine(m,&repr,v3,v11); 
  GEdge * v4_v12 = addLine(m,&repr,v4,v12); 
  GEdge * v5_v13 = addLine(m,&repr,v5,v13); 
  GEdge * v6_v14 = addLine(m,&repr,v6,v14); 
  GEdge * v7_v15 = addLine(m,&repr,v7,v15); 
  GEdge * v8_v16 = addLine(m,&repr,v8,v16); 

  GEdge * v1_v2 = addCircleArcCenter(m,&repr,v1,center_top,v2); 
  GEdge * v2_v3 = addCircleArcCenter(m,&repr,v2,center_top,v3); 
  GEdge * v3_v4 = addCircleArcCenter(m,&repr,v3,center_top,v4); 
  GEdge * v4_v1 = addCircleArcCenter(m,&repr,v4,center_top,v1); 
  GEdge * v5_v6 = addCircleArcCenter(m,&repr,v5,center_top,v6); 
  GEdge * v6_v7 = addCircleArcCenter(m,&repr,v6,center_top,v7); 
  GEdge * v7_v8 = addCircleArcCenter(m,&repr,v7,center_top,v8); 
  GEdge * v8_v5 = addCircleArcCenter(m,&repr,v8,center_top,v5); 

  GEdge * v9_v10 = addCircleArcCenter(m,&repr,v9,center_bottom,v10); 
  GEdge * v10_v11 = addCircleArcCenter(m,&repr,v10,center_bottom,v11); 
  GEdge * v11_v12 = addCircleArcCenter(m,&repr,v11,center_bottom,v12); 
  GEdge * v12_v9 = addCircleArcCenter(m,&repr,v12,center_bottom,v9); 
  GEdge * v13_v14 = addCircleArcCenter(m,&repr,v13,center_bottom,v14); 
  GEdge * v14_v15 = addCircleArcCenter(m,&repr,v14,center_bottom,v15); 
  GEdge * v15_v16 = addCircleArcCenter(m,&repr,v15,center_bottom,v16); 
  GEdge * v16_v13 = addCircleArcCenter(m,&repr,v16,center_bottom,v13); 


  GEdge* top_edges_outer_v[4] = {v1_v2,v2_v3,v3_v4,v4_v1}; 
  GEdge* top_edges_inner_v[4] = {v5_v6,v6_v7,v7_v8,v8_v5}; 
  GEdge* bottom_edges_outer_v[4] = {v9_v10,v10_v11,v11_v12,v12_v9}; 
  GEdge* bottom_edges_inner_v[4] = {v13_v14,v14_v15,v15_v16,v16_v13}; 

  GEdge* front_edges_inner_v[4] = {v1_v2,v2_v10,v9_v10,v1_v9}; 
  GEdge* right_edges_inner_v[4] = {v2_v3,v3_v11,v10_v11,v2_v10}; 
  GEdge* back_edges_inner_v[4] = {v3_v4,v4_v12,v11_v12,v3_v11}; 
  GEdge* left_edges_inner_v[4] = {v4_v1,v1_v9,v12_v9,v4_v12}; 

  GEdge* front_edges_outer_v[4] = {v5_v6,v6_v14,v13_v14,v5_v13}; 
  GEdge* right_edges_outer_v[4] = {v6_v7,v7_v15,v14_v15,v6_v14}; 
  GEdge* back_edges_outer_v[4] = {v7_v8,v8_v16,v15_v16,v7_v15}; 
  GEdge* left_edges_outer_v[4] = {v8_v5,v5_v13,v16_v13,v8_v16}; 

  GEdge* vertical_edges[8] = {v1_v9,v2_v10,v3_v11,v4_v12,v5_v13,v6_v14,v7_v15,v8_v16}; 

  GEdge** edges_v[8] = { front_edges_inner_v,right_edges_inner_v,back_edges_inner_v,left_edges_inner_v,
                           front_edges_outer_v,right_edges_outer_v,back_edges_outer_v,left_edges_outer_v }; 

  edges.insert(edges.end(),top_edges_inner_v,top_edges_inner_v+4); 
  edges.insert(edges.end(),top_edges_outer_v,top_edges_outer_v+4); 
  edges.insert(edges.end(),bottom_edges_inner_v,bottom_edges_inner_v+4); 
  edges.insert(edges.end(),bottom_edges_outer_v,bottom_edges_outer_v+4); 
  edges.insert(edges.end(),vertical_edges,vertical_edges+8); 

  std::vector<std::vector<GEdge*> > top_loop; 
  top_loop.push_back(std::vector<GEdge*>(top_edges_outer_v,top_edges_outer_v+4)); 
  top_loop.push_back(std::vector<GEdge*>(top_edges_inner_v,top_edges_inner_v+4)); 

  std::vector<std::vector<GEdge*> > bottom_loop; 
  bottom_loop.push_back(std::vector<GEdge*>(bottom_edges_outer_v,bottom_edges_outer_v+4)); 
  bottom_loop.push_back(std::vector<GEdge*>(bottom_edges_inner_v,bottom_edges_inner_v+4)); 

  faces[0].push_back(m->addPlanarFace(top_loop)); 
  faces[0].push_back(m->addPlanarFace(bottom_loop)); 

  for (int i = 0; i < 8; i++)
  {
    std::vector<GEdge*> these_edges(edges_v[i], edges_v[i]+4); 
    std::vector<std::vector<GEdge*> > loop; 
    loop.push_back(these_edges); 
    std::vector<GFace*> ruled =m->addRuledFaces(loop) ; 
    faces[0].insert(faces[0].end(),ruled.begin(),ruled.end());
  }

  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(faces[0]); 

  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 

  return 0; 
}


dmtpc::mc::amp::Mesh::Mesh(TVector3 * center , double wire_pitch, double wire_radius, TVector3 * orientation , unsigned nwiresx , unsigned nwiresy, double rotation) 
:  center(0,0,0), orientation(0,0,1), wire_pitch(wire_pitch),wire_radius(wire_radius), nwiresx(nwiresx),nwiresy(nwiresy), rotation(rotation)
{
  if (center) this->center = *center; 
  if (orientation) this->orientation = (*orientation).Unit(); 

  TVector3 norm  = this->orientation.Unit(); 
  dirx = norm.Orthogonal(); 
  diry =norm.Cross(dirx);
  dirx.Rotate(rotation,norm);
  diry.Rotate(rotation,norm);

  size = 0;
}

dmtpc::mc::amp::SimpleMesh::SimpleMesh(TVector3 * center , double wire_pitch, double wire_radius, TVector3 * orientation , unsigned nwiresx , unsigned nwiresy, double rotation) 
: Mesh(center,wire_pitch,wire_radius,orientation, nwiresx, nwiresy, rotation)
{
}

int dmtpc::mc::amp::Box::build(GModel * m, bool bounding, std::vector<Shape*> * extant_shapes) 
{
  double w = widths.x()/2.; 
  double l = widths.y()/2.; 
  double h = widths.z()/2.; 
  TVector3 norm  = orientation.Unit(); 
  TVector3 orth = norm.Orthogonal(); 
  TVector3 orth2 = norm.Cross(orth); 

  if (!size)
  {
    size = TMath::Min(w,h); 
    size = TMath::Min(size,l); 
  }

  newFaceLoop(); 

  GVertex * v1 = addPtRotated(m,(center + w * orth + l * orth2 + h * norm),size,rotation,norm); 
  GVertex * v2 = addPtRotated(m,(center + w * orth - l * orth2 + h * norm),size,rotation,norm); 
  GVertex * v3 = addPtRotated(m,(center - w * orth - l * orth2 + h * norm),size,rotation,norm); 
  GVertex * v4 = addPtRotated(m,(center - w * orth + l * orth2 + h * norm),size,rotation,norm); 
  GVertex * v5 = addPtRotated(m,(center + w * orth + l * orth2 - h * norm),size,rotation,norm); 
  GVertex * v6 = addPtRotated(m,(center + w * orth - l * orth2 - h * norm),size,rotation,norm); 
  GVertex * v7 = addPtRotated(m,(center - w * orth - l * orth2 - h * norm),size,rotation,norm); 
  GVertex * v8 = addPtRotated(m,(center - w * orth + l * orth2 - h * norm),size,rotation,norm); 

  points.push_back(v1); points.push_back(v2); points.push_back(v3); points.push_back(v4); 
  points.push_back(v5); points.push_back(v6); points.push_back(v7); points.push_back(v8); 

  GEdge * v1_v2 = addLine(m,&repr,v1,v2); 
  GEdge * v2_v3 = addLine(m,&repr,v2,v3); 
  GEdge * v3_v4 = addLine(m,&repr,v3,v4); 
  GEdge * v4_v1 = addLine(m,&repr,v4,v1); 
  GEdge * v5_v6 = addLine(m,&repr,v5,v6); 
  GEdge * v6_v7 = addLine(m,&repr,v6,v7); 
  GEdge * v7_v8 = addLine(m,&repr,v7,v8); 
  GEdge * v8_v5 = addLine(m,&repr,v8,v5); 
  GEdge * v1_v5 = addLine(m,&repr,v1,v5); 
  GEdge * v2_v6 = addLine(m,&repr,v2,v6); 
  GEdge * v3_v7 = addLine(m,&repr,v3,v7); 
  GEdge * v4_v8 = addLine(m,&repr,v4,v8); 

  GEdge* top_edges_v[4] = {v1_v2,v2_v3,v3_v4,v4_v1}; 
  GEdge* bottom_edges_v[4] = {v5_v6,v6_v7,v7_v8,v8_v5}; 
  GEdge* front_edges_v[4] = {v1_v2,v2_v6,v5_v6,v1_v5}; 
  GEdge* right_edges_v[4] = {v2_v3,v3_v7,v6_v7,v2_v6}; 
  GEdge* back_edges_v[4] = {v3_v4,v4_v8,v7_v8,v3_v7}; 
  GEdge* left_edges_v[4] = {v4_v1,v1_v5,v8_v5,v4_v8}; 

  GEdge** edges_v[6] = {top_edges_v,bottom_edges_v,front_edges_v,right_edges_v,back_edges_v,left_edges_v}; 

  edges.insert(edges.end(),top_edges_v,top_edges_v+4); 
  edges.insert(edges.end(),bottom_edges_v,bottom_edges_v+4); 
  edges.push_back(v1_v5); edges.push_back(v2_v6); edges.push_back(v3_v7); edges.push_back(v4_v8); 

  for (int i = 0; i < 6; i++)
  {
    std::vector<GEdge*> these_edges(edges_v[i], edges_v[i]+4); 
    std::vector<std::vector<GEdge*> > loop; 
    loop.push_back(these_edges); 
    faces[0].push_back(m->addPlanarFace(loop)); 
  }


  std::vector<std::vector<GFace*> > surfloop;
  surfloop.push_back(faces[0]) ; 

  if (bounding)
  {
    for (unsigned i = 0; i < extant_shapes->size(); i++)
    {
      for (unsigned j = 0; j < extant_shapes->at(i)->getFaceLoops()->size(); j++)
      {
        surfloop.push_back(extant_shapes->at(i)->getFaceLoops()->at(j)); 
      }
    }
  }


  GRegion * vol = m->addVolume(surfloop); 
  vols.push_back(vol); 

  return 0; 
}



bool dmtpc::mc::amp::Shape::intersectsSegment(double x0, double y0, double z0, double x1, double y1, double z1) const
{
  std::cerr << "intersectsSegment not implemented! " << std::endl; 
  return false; 
}


/** Simplify by calculating intersection with plane and assuming 2d. Should be ok as long as camera is far enough away **/ 

bool dmtpc::mc::amp::Mesh::intersectsSegment(double x0, double y0, double z0, double x1, double y1, double z1) const
{

 //find intersection with mesh plane
  TVector3 p0(x0,y0,z0); 
  TVector3 u(x1-x0, y1-y0,z1-z0); 

  if (u.Dot(orientation) == 0)
  {
    return (p0 - center).Mag() < 2*wire_radius; 
  }

  double s =  orientation.Dot(center - p0) / orientation.Dot(u); 
  if (s < 0 || s > 1) return false; 

  TVector3 p = p0 + s * u ; //intersection point
  p-= center;  
  double x = p.Dot(dirx); 
  double y = p.Dot(diry); 


  double xn = x/wire_pitch; 

  if (fabs(xn) > nwiresx/2) return false; 

  double yn = y/wire_pitch; 

  if (fabs(yn) > nwiresy/2) return false; 

//  printf("xn,yn: %f,%f\n",xn,yn); 

  double adjustx = nwiresx % 2 ? 0 : 0.5; 
  double adjusty = nwiresy % 2 ? 0 : 0.5; 

  return fabs((xn - adjustx) - round(xn))  < wire_radius/wire_pitch  ||  fabs((yn - adjusty) - round(yn))  < wire_radius/wire_pitch ; 
}


