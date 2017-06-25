from math import *
from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import datetime as dt
import wimp.coords as coords

def CygnusAngle(lat,lon,title=""):
  ra = 20.62/24*360
  dec = 42.03
#  ra = 0
#  dec = 80
  rad = np.pi/180.

  T = np.arange(0,24.,1)
  X = np.arange(0,24.,1)
  Y = np.arange(0,24.,1)
  i=0
  for t in T:
    hrs = int(t)
    min = int( (t-hrs)*60 )
    sec = int( ((t-hrs)*60-min)*60 )
    time = dt.datetime(2013,5,20,hrs,min,sec)

    alt,az = coords.EquatorialToHorizontal(ra,dec,time,lat=lat,lon=lon)

    Y[i] = alt/rad
    X[i] = 90.-az/rad
    print time,Y[i],X[i]
    i += 1 
  m = Basemap(projection='moll',lon_0=0,resolution='c')
  X,Y = m(X,Y)
  thePlot = m.scatter(X, Y, s=150, c=T,cmap=plt.cm.jet)

  plt.clim(0,24)
  plt.title(title)
 
  m.drawparallels(np.arange(-90.,120.,30.))
  m.drawmeridians(np.arange(0.,420.,60.))
  plt.savefig("ani/CygnusAngleWIPP.png")
  return m

