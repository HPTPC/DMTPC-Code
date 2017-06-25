/** \file DmtpcAstro.hh
Header file for the dmtpc::physics::astro namespace
\author Asher Kaboth
*/
#ifndef DMTPC_ASTRO_HH
#define DMTPC_ASTRO_HH
#include "TROOT.h"
#include "TVector3.h"
#include "TRotation.h"
namespace dmtpc {
namespace physics{

/** Conversion utilities for astronomy stuff 
ALL ASTRO ANGLES to be expressed in decimal DEGREES. This includes sidereal
time. ALL TIMES are to be UTC. Longitude is +E, -W. Hour angle expressed
westwards from south. Azimuth measured eastwards from north. Detector
coordinates are standard azimuthal angle phi and zenith angle theta; alpha,
beta, and gamma are Euler angles that rotate FROM a "default" earth-based
system TO the detector coordinates, using the Goldstein (X) convention. The
default is y = north, x= east,z=radially outward from the surface (right hand
rule). Detector and euler angles are given in radians. Most algorithms from
Astronomical Algorithms by Jan Meeus. \author Asher Kaboth
*/

namespace astro{
   /** mod360 \param angle Reduces an angle to 360 degrees*/
   void mod360(double& angle);
   /** localSiderealTime finds the local siderial time for a time and a longitude using the formula: 100.46061837+36000.770053608*T+0.000387933*T*T+15.0*1.002737909*timeut+long, where T is the julian day
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \param lon longitude
       \returns the local sidereal time
   */
   double localSiderealTime(int yyyy, int mm, int dd, double timeut, double lon);
   /** jdOfDate finds the julian day of a date 
       \param yyyy year
       \param mm month
       \param dd day
       \returns the julian day
   */
   double jdOfDate(int yyyy, int mm, double dd);
   /** julianDay finds the julian day of a date 
       \param yyyy year
       \param mm month
       \param dd day
       \returns the julian day
   */
   double julianDay(int yyyy, int mm, double dd);
   /** hmsToFractionalDay gives the fraction of a day that has passed 
       \param hh hour
       \param mm minute
       \param ss second			       
       \return a number between 0 and 1
       
   */
   double hmsToFractionalDay(int hh, int mm, int ss);
   /** hmsToDecimalHours gives a decimal number of hours that have passed
       \param hh hour
       \param mm minute
       \param ss second
       \return a number between 0 and 24
   */
   double hmsToDecimalHours(int hh, int mm, int ss);
   /** HA to RA hour angle to right ascention 
       \param ha hour angle
       \param lst local sidereal time
       \return right ascention
      
    */
   double localHourAngleToRA(double ha, double lst);
   /** RA to HA right ascention to hour angle
       \param ra right ascention 
       \param lst local sidereal time
       \return hour angle
   */
   double raToLocalHourAngle(double ra, double lst);
   /** precess coordinates to epoch B1950
       \param ra right ascention
       \param dec declination
       \param jd julian day
       \param ra1950 variable in which to place the ra from 1950
       \param dec1950 variable in which to place the dec from 1950
   */
   void precessToB1950(double ra, double dec, double jd, 
		       double& ra1950, double& dec1950);
   /** precess coordinates from epoch B1950
       \param ra1950 right ascention in 1950
       \param dec1950 declination in 1950
       \param jd julian day
       \param ra variable in which to place the ra 
       \param dec variable in which to place the dec
   */
   void precessFromB1950(double ra1950, double dec1950, double jd,
			 double& ra, double& dec);
   
   /** getAltfromEq gets the altitude from equitorial (ra/dec) coordinates
       \param ra right ascention
       \param dec declination
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return altitude
    */
   double getAltFromEq(double ra, double dec, double lat, double lon, int yyyy, int mm, int dd, double timeut);
      /** getAzfromEq gets the azimuth from equitorial (ra/dec) coordinates
       \param ra right ascention
       \param dec declination
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return azimuth
    */
   double getAzFromEq(double ra, double dec, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** getAltfromGal gets the altitude from galactic coordinates
       \param ra right ascention
       \param dec declination
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return altitude
    */
   double getAltFromGal(double l, double b, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** getAzfromEq gets the azimuth from equitorial (ra/dec) coordinates
       \param ra right ascention
       \param dec declination
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return azimuth
    */
   double getAzFromGal(double l, double b, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** getRA gets the right ascention from galactic coordinates
       \param l galactic longitude
       \param b galactic latitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return right ascention
    */
   double getRA(double l, double b, int yyyy, int mm, int dd, double timeut);
   /** getDec gets the right declination from galactic coordinates
       \param l galactic longitude
       \param b galactic latitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return declination
    */
   double getDec(double l, double b, int yyyy, int mm, int dd, double timeut);
   /** getRA gets the right ascention from horizon coordinates
       \param alt altitude
       \param az azimuth
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return right ascention
    */
   double getRA(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** getDec gets the declination from horizon coordinates
       \param alt altitude
       \param az azimuth
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return declination
    */
   double getDec(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** Gets the galactic longitude from equitorial coordinates
       \param ra right ascention
       \param dec declination
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic longitude
    */
   double getL(double ra, double dec, int yyyy, int mm, int dd, double timeut);
   /** Gets the galactic latitude from equitorial coordinates
       \param ra right ascention
       \param dec declination
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic latitude
    */

   double getB(double ra, double dec, int yyyy, int mm, int dd, double timeut);
   /** Gets the galactic longitude from equitorial coordinates
       \param ra right ascention
       \param dec declination
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic longitude
    */
   double getGalLongitude(double ra, double dec, int yyyy, int mm, int dd, double timeut);

   /** Gets the galactic latitude from equitorial coordinates
       \param ra right ascention
       \param dec declination
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic latitude
    */
   double getGalLatitude(double ra, double dec, int yyyy, int mm, int dd, double timeut);

   /** Gets the galactic longitude from horizion coordinates
       \param alt altitude
       \param az azimuth
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic longitude
    */
   double getL(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** Gets the galactic latitude from horizion coordinates
       \param alt altitude
       \param az azimuth
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic latitude
    */
   double getB(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);
   /** Gets the galactic longitude from horizion coordinates
       \param alt altitude
       \param az azimuth
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic longitude
    */
   double getGalLongitude(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);

   /** Gets the galactic latitude from horizion coordinates
       \param alt altitude
       \param az azimuth
       \param lat latitude
       \param lon longitude
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time			
       \return galactic latitude
    */
   double getGalLatitude(double alt, double az, double lat, double lon, int yyyy, int mm, int dd, double timeut);

      /** getAltfromDetector gets the altitude from the detector coordinates
       \param phi detector phi
       \param theta detector theta
       \param alpha 1st euler angle
       \param beta 2nd euler angle
       \param gamma 3rd euler angle
       \return altitude
    */
   double getAltFromDetector(double phi, double theta, double alpha, double beta, double gamma);
      /** getAzfromDetector gets the azimuth from the detector coordinates
       \param phi detector phi
       \param theta detector theta
       \param alpha 1st euler angle
       \param beta 2nd euler angle
       \param gamma 3rd euler angle
       \return azimuth
    */
   double getAzFromDetector(double phi, double theta, double alpha, double beta, double gamma);
    /** getPhifromHorizon gets the detector phi from the horizon coordinates
       \param alt altitude
       \param az azimuth
       \param alpha 1st euler angle
       \param beta 2nd euler angle
       \param gamma 3rd euler angle
       \return phi
    */

   double getPhiFromHorizon(double alt, double az, double alpha, double beta, double gamma);
       /** getThetafromHorizon gets the detector theta from the horizon coordinates
       \param alt altitude
       \param az azimuth
       \param alpha 1st euler angle
       \param beta 2nd euler angle
       \param gamma 3rd euler angle
       \return theta
    */

   double getThetaFromHorizon(double alt, double az, double alpha, double beta, double gamma);
   /** getWimpWindL gets the wimp wind galactic longitude as a function of time from Lewin & Smith appendix
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return galactic longitude
   */
   double getWimpWindL(int yyyy, int mm, int dd, double timeut);
   /** getWimpWindB gets the wimp wind galactic latitude as a function of time from Lewin & Smith appendix
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return galactic latitude
   */
   double getWimpWindB(int yyyy, int mm, int dd, double timeut);
   /** getWimpTime returns the Lewin & Smith time defintion for getWimpWindL and getWimpWindB
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \return time
   */
   double getWimpTime(int yyyy, int mm, int dd, double timeut);
   /** getWimpGalCoord gets the galactic latitude and longitude as a function of time
       \param yyyy year
       \param mm month
       \param dd day
       \param timeut UTC time
       \param l variable in which to put galactic longitude
       \param b variable in which to put galactic latitude
       \return the velocity in km/s
   */
   double getWimpGalCoord(int yyyy, int mm, int dd, double timeut,double& l, double& b);
   /** getWimpGalCoord gets the galactic latitude and longitude as a function of time
       \param time the time since noon, 31/12/1999
       \param l variable in which to put galactic longitude
       \param b variable in which to put galactic latitude
       \return the velocity in km/s
   */   
   double getWimpGalCoord(double time, double& l, double& b);

   /** getWimpGalCoord gets the galactic latitude and longitude as a function of time
      \param utc the Unix timestamp
      \param nsec nanoseconds after the timestamp
      \param l variable in which to put galactic longitude
      \param b variable in which to put galactic latitude
      \return the velocity in km/s
   */
   double getWimpGalCoord(time_t utc, int nsec, double& l, double& b);
  
  
   /** Get WimpDir in detector coordinates  (by Cosmin) 
    * @param phi  pointer to where phi will be placed 
    * @param theta  pointer to where theta will be placed 
    * @param t   UTC Unix time (with potential fractional part) 
    * @param latitude
    * @param longitude
    * @param detector_z   the detector z axis relative to horizon coordinates (where x = North, y = East, z = up). If zero, assume detector axis is up
    * @param detector_x    The detector x axis relative to horizon coordinates. If zero, assume no rotation about detector z is necessary (potentially leading to an additive offset)
    */
   void getWimpDirInDetectorCoords(double *phi, double *theta, double time, double latitude, double longitude, const TVector3 * detector_z, const TVector3 * detector_x); 

}//astro
}//physics
}//dmtpc

#endif

