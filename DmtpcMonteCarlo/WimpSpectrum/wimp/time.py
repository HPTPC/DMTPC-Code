import datetime as dt


def J2000(theTime):
  j2k = dt.datetime(2000,1,1,12)
  return (theTime-j2k).total_seconds()/(3600*24)

def JulianDate(theTime):
  return J2000(theTime)+2451545.0

def ModifiedJulianDate(theTime):
  return J2000(theTime)+2400000.5

def B1950(theTime):
  j2k = dt.datetime(1949,12,31,22,9)
  date = dt.datetime(theTime)
  return (date-j2k).total_seconds()/(3600*24)
  
def GMSiderealTime(theTime):
  return  18.697374558 \
          + 24.06570982441908 \
          * J2000(theTime)

def LocalSiderealTime(theTime,longitude):
  return GMSiderealTime(theTime) + longitude *24./ 360.



