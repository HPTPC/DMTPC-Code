from ROOT import gROOT, gSystem
from math import *
from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.colors import LinearSegmentedColormap

def Coords(lat,lon):
  TOL = 1e-5
  theta = lat
  if abs(lat-pi/2)<TOL :
    return 0,0.9999
  elif abs(lat+pi/2)<TOL:
    return 0,-0.9999
  thetaLast = 100
  while abs(theta-thetaLast)>TOL:
    thetaLast = theta
    theta -= (2*theta+sin(2*theta)-pi*sin(lat) )/(2+2*cos(2*theta))
  return 2/pi*lon*cos(theta),sin(theta)

def MakePlot(file="data/Wimp100GeVonF19.root",cuts="",suffix="",fig=plt,title=""):

#Violet
  r1 = 0.439
  g1 = 0.131
  b1 = 0.388
#Light blue
  r2 = 117./255
  g2 = 178./255
  b2 = 221./255
#
  f1 = (0,0.5,1,0.7,0.5,0.2,0.,0.)
  f2 = (0,0,0,0.3,0.5,0.8,1,0.5)
  fw = (0,0,0,0,0,0,0,0.5)
  cdict = {'red':((0,f1[0]*r1+f2[0]*r2+fw[0],f1[0]*r1+f2[0]*r2+fw[0]),
                  (0.1,f1[1]*r1+f2[1]*r2+fw[1],f1[1]*r1+f2[1]*r2+fw[1]),
                  (0.3,f1[2]*r1+f2[2]*r2+fw[2],f1[2]*r1+f2[2]*r2+fw[2]),
                  (0.45,f1[3]*r1+f2[3]*r2+fw[3],f1[3]*r1+f2[3]*r2+fw[3]),
                  (0.55,f1[4]*r1+f2[4]*r2+fw[4],f1[4]*r1+f2[4]*r2+fw[4]),
                  (0.75,f1[5]*r1+f2[5]*r2+fw[5],f1[5]*r1+f2[5]*r2+fw[5]),
                  (0.9,f1[6]*r1+f2[6]*r2+fw[6],f1[6]*r1+f2[6]*r2+fw[6]),
                  (1,f1[7]*r1+f2[7]*r2+fw[7],f1[7]*r1+f2[7]*r2+fw[7])),
           'green':((0,f1[0]*g1+f2[0]*g2+fw[0],f1[0]*g1+f2[0]*g2+fw[0]),
                  (0.1,f1[1]*g1+f2[1]*g2+fw[1],f1[1]*g1+f2[1]*g2+fw[1]),
                  (0.3,f1[2]*g1+f2[2]*g2+fw[2],f1[2]*g1+f2[2]*g2+fw[2]),
                  (0.45,f1[3]*g1+f2[3]*g2+fw[3],f1[3]*g1+f2[3]*g2+fw[3]),
                  (0.55,f1[4]*g1+f2[4]*g2+fw[4],f1[4]*g1+f2[4]*g2+fw[4]),
                  (0.75,f1[5]*g1+f2[5]*g2+fw[5],f1[5]*g1+f2[5]*g2+fw[5]),
                  (0.9,f1[6]*g1+f2[6]*g2+fw[6],f1[6]*g1+f2[6]*g2+fw[6]),
                  (1,f1[7]*g1+f2[7]*g2+fw[7],f1[7]*g1+f2[7]*g2+fw[7])),
           'blue':((0,f1[0]*b1+f2[0]*b2+fw[0],f1[0]*b1+f2[0]*b2+fw[0]),
                  (0.1,f1[1]*b1+f2[1]*b2+fw[1],f1[1]*b1+f2[1]*b2+fw[1]),
                  (0.3,f1[2]*b1+f2[2]*b2+fw[2],f1[2]*b1+f2[2]*b2+fw[2]),
                  (0.45,f1[3]*b1+f2[3]*b2+fw[3],f1[3]*b1+f2[3]*b2+fw[3]),
                  (0.55,f1[4]*b1+f2[4]*b2+fw[4],f1[4]*b1+f2[4]*b2+fw[4]),
                  (0.75,f1[5]*b1+f2[5]*b2+fw[5],f1[5]*b1+f2[5]*b2+fw[5]),
                  (0.9,f1[6]*b1+f2[6]*b2+fw[6],f1[6]*b1+f2[6]*b2+fw[6]),
                  (1,f1[7]*b1+f2[7]*b2+fw[7],f1[7]*b1+f2[7]*b2+fw[7]))
          }
  BlueViolet = LinearSegmentedColormap("BlueViolet",cdict)
  plt.register_cmap(cmap=BlueViolet) 
  gSystem.Load("lib/libWimpSpectrum.so")
  from ROOT import Mollweide
   
  moll = Mollweide()
  moll.setBinning(50)
  moll.setNormalize(False)
  moll.projection(file,"Wimps","theta"+suffix,"phi"+suffix,cuts)
  h = moll.getHist()
  ra = np.linspace(-np.pi, np.pi, 40)
  dec= np.linspace(-np.pi/2, np.pi/2, 20)
  X,Y = np.meshgrid(ra,dec)
  Vals = X*Y
  rad = 180./pi
  maxVal = -1
  for i in range(0,len(dec)):
    for j in range(0,len(ra)):
      xm,ym = Coords(Y[i,j],X[i,j])
      Vals[i,j] = h.Interpolate(xm*90,ym*90)
      if Vals[i,j] > maxVal:
        maxVal = Vals[i,j]
     
  X*=rad
  Y*=rad
  Vals/=maxVal
  m = Basemap(projection='moll',lon_0=0,resolution='i')
  v = np.linspace(0,1,200,endpoint=True)
  thePlot = m.contourf(X, Y, Vals,v, cmap=plt.get_cmap('BlueViolet'),latlon=True,vmin=0,vmax=1)

  fig.clim(0,1)
  fig.title(title)
 
  m.drawparallels(np.arange(-90.,120.,30.))
  m.drawmeridians(np.arange(0.,420.,60.))
  return m

def CreatePicture(i,MakeAxes=True,file="image.png"):
  plt.figure(1)
  cut = "vgal<650&&E>"+str(i*10)
  
  plt.subplot(211)

  MakePlot(cuts=cut,title="WIMP Directions",suffix="w")
  if MakeAxes is True:
    cbar = plt.colorbar(orientation="vertical",ticks=[0,0.5,1],pad=0.05,shrink=1)
    cbar.ax.set_xticklabels(['0','0.5','1'])
  plt.subplot(212)
  MakePlot(cuts=cut,title="Recoil Directions, E > "+str(i*10)+" keV")
  if MakeAxes is True:
    cbar = plt.colorbar(orientation="vertical",ticks=[0,0.5,1],pad=0.05,shrink=1)
    cbar.ax.set_xticklabels(['0','0.5','1'])
  plt.savefig(file)

def Animate():
#for some reason the first image created is slightly offset from the others
#Create image 0 twice to get correct orientation
  CreatePicture(0)
  for x in range(16):
    CreatePicture(x,MakeAxes=False,file="1{0:03d}.png".format(x))


