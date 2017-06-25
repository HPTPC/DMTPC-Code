//header file: for connecting to and transferring data between MySQL databases
//initialised: for extracting power supply current draws
//intended use: to extract max value as spark checker
//NB. different use of MYSQL and MySQL
//Author: Richard Eggleston Feb 2016

#ifndef MYSQL_HH
#define MYSQL_HH

#include <mysql/mysql.h>
#include <vector>
#include <stdint.h>

#include "TTimeStamp.h"
#include "TString.h"
#include "TObject.h"

//const char* dbfilename = "/etc/dbaccess.txt";


namespace dmtpc
{
  namespace Quality
  {    
    class MySQL : public TObject
    {

    public:
      MySQL();
      virtual ~MySQL();
      

      void initialiseDB(char *server, char *user, char *pass, char *db_name);
      double getValue(TString table, TString column, TString condition);
      double getMaxValue(TString channel, TString minTime, TString maxTime, TTimeStamp &timestamp);
      std::vector<double> getValues(TString table, TString column, TString condition, bool storeTimes, std::vector< std::pair<uint32_t, uint32_t > > &timestamps);
      
    private: 
      
      MYSQL * _db;
      
      struct db_access{
	char * server;
	char * user;
	char * pass;
	char * database;
      };
      db_access _db_access;

      void init_access(char *server, char *user, char *pass, char *db_name);

      
      ClassDef(MySQL,1);
      //not working
      //std::vector<double> getValues(MYSQL * db, TString channel, TString minTime, TString maxTime);
      
      
      //double getMaxValue(MYSQL * db, TString channel, TString minTime, TString maxTime);
    };
  }
}

#endif
