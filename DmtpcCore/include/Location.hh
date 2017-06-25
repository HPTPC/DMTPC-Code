#ifndef DMTPC_CORE_LOCATION_HH
#define DMTPC_CORE_LOCATION_HH

#include "TObjString.h" 


namespace dmtpc
{
  namespace core
  {
    class Location  : public TObject 
    {

      public: 

        Location() { ; }
        Location(double longitude, double latitude, double north_angle, double elevation = 0, double horizon_angle = 0, const char * description = "")
          : longitude(longitude), latitude(latitude), north_angle(north_angle), elevation(elevation), horizon_angle(horizon_angle), description(description)
        {}

        double getNorthAngle() const { return north_angle; } 
        double getHorizonAngle() const { return horizon_angle; } 
        double getLatitude() const { return latitude; } 
        double getLongitude() const { return longitude; } 

        //get elevation in m above sea level 
        double getElevation() const { return elevation; } 

        const char * getDescription() { return description.GetString().Data() ; } 

      private:

        double longitude; 
        double latitude; 
        double north_angle; 
        double horizon_angle; 
        double elevation; 
        TObjString description; 
        ClassDef(Location,1); 

    }; 
  }
}
#endif
