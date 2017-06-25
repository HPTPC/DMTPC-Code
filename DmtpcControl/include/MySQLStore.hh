#ifndef DMTPC_CONTROL_MYSQL_STORE_HH
#define DMTPC_CONTROL_MYSQL_STORE_HH

#include "Store.hh"
#include "TMutex.h"
#include "TTimeStamp.h"
#include <mysql/mysql.h>

namespace dmtpc
{
  namespace control
  {
    class MySQLStore : public Store
    {
    public:
      
      /** 
       * Creates a MySQLStore and provides primitive access to a MYSQL DB
       * with the given parameters
       @param server host that the database is on
       @param user username for the database
       @param pw password for the database
       @param db name of the database
      */  
      //MySQLStore(const char *server, const char *user, const char *pw, const char *db);
      
      /**
       * Creates a MySQLStore and provides primitive access to a MYSQL DB
       * with the db parameters stored in a text file
       @param filename_dbaccess name of the file containing database access information
      */
      MySQLStore(const char *filename_dbaccess);


      /** 
       * Reads channel value from database
       @param table name of the table to be read
       @param ifield number in table to read
       @param doPrint whether to print the row or not
       @param db_handle if not NULL, the _MYSQL connection associated with that pointer is used instead of making a new connection
       @return read value of the field
      */
      double readValueFromDB(const char* table, unsigned int ifield=0, bool doPrint=false);


      /**
       * Saves the given value and timestamp to the specified database
       @param table name of the table to save to
       @param value value to save 
       @param db_handle if not NULL, the _MYSQL connection associated with that pointer is used instead of making a new connection
       @return 0 on success, 1 on failure
      */
      int saveValueToDB(const char* table, double value); //, MYSQL * db_handle = NULL);
	
      
      /**
       * Saves the currentValue and timestamp to the specified database
       * and checks that they were actually written. If they weren't, it tries
       * indefinitely
       @param table name of the table to save to
       @param value value to save
       @param db_handle if not NULL, the _MYSQL connection associated with that pointer is used instead of making a new connection
       @return return value of saveValueToDB
      */
      int saveValueToDBandCheck(const char* table, double value); // , MYSQL* db_handle = NULL);
      

      /** 
       * Implementation of Store class method 'save'
       @param key name of the table to save to
       @param value value to save
       @return 0 on success, 1 on failure
       */ 
      int save(const char* key, double value) { return saveValueToDB(key,value); }

      /** 
       * Implementation of Store class method 'getValue'
       @param key name of the table to read
       @param when currently unused
       @return 0 on success, 1 on failure
       */ 
      // TODO: actually use TTimeStamp! for now, just reads most recent value
      double getValue(const char* key, TTimeStamp* when = 0) { return readValueFromDB(key); }

       /** 
	* Print out to stdout the name and currentValue
	* of the channel
	*/ 
      //void print(const char* table);

       /** 
	* Returns the average of the last n entries in the specified table
	* @param table name of the table 
	* @param n number of entries to average
	* @return average value of the entries
	*/
      //double getAverage(const char *table, int n);

      //void synchronizeDB(TMutex * m) { mutex_db = m; }

      
    private:

      /** 
	* Connects to db 
	*/
      bool connectToDB(int ntries = 10);

      bool checkMySQLHandle(void);

      MYSQL m_mysql_struct;
      MYSQL * m_mysql;
      TMutex *mutex_db;

    };
  }
}


#endif
