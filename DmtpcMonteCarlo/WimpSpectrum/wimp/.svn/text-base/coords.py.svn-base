import datetime as dt
from math import *
import wimp.time
import numpy as np

rad = np.pi/180

def HourAngle(theTime,ra,lon=0):
  return wimp.time.LocalSiderealTime(theTime,lon)*360./24- ra

def EquatorialToHorizontal(ra,dec,theTime,lat=0,lon=0):
  ra *= rad
  dec *= rad
  lat *= rad
  lon *= rad
  ha = HourAngle(theTime,ra)*rad
  alt = asin( sin(dec)*sin(lat) +cos(dec)*cos(lat)*cos(ha))
  cosA = (sin(dec)-sin(alt)*sin(lat) ) / (cos(alt)*cos(lat) )
  sinA = -sin(ha)*cos(dec)/cos(alt)
  az = atan2(sinA,cosA)
  if az>np.pi:
    az-=2*np.pi
  return alt,az

