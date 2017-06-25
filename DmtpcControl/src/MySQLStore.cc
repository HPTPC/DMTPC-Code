#include "MySQLStore.hh"

#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using std::cout;
using std::endl;
using std::cerr;
using std::setw;

static const double ERROR_VALUE = -9999.99;

/** 
 * Used to read credentials from
 * a dbaccess file
 */
namespace DBaccess {
  char server[256];   // host that the database is on
  char user[256];     // username for the database
  char pass[256];     // password for the database
  char database[256]; // database name
  /** 
   * Set the database info fields from the given file.
   *
   * @param fname the file to read from
   */
  void set(const char *fname) { 
    ifstream infile(fname);
    infile >> server >> user >> pass >> database;
  }
};

dmtpc::control::MySQLStore::MySQLStore(const char *filename_dbaccess)
{
  DBaccess::set(filename_dbaccess);
  m_mysql = 0;
  mutex_db = 0;
}

/* Returns true on success */ 
bool dmtpc::control::MySQLStore::connectToDB(int ntries)
{
  cout << "connecting to mysql DB" << endl;
  int nloop = 0;
  while (!mysql_real_connect( m_mysql, DBaccess::server, DBaccess::user, 
			      DBaccess::pass, DBaccess::database, 0, NULL, 0 )) 
    {
      nloop++;
      cout << "Cannot connect to mysql DB after " << nloop << " trials " << endl;
      gSystem->Sleep(100);
      if (nloop>ntries) {
	cout << "Cannot connect to mysql DB" << endl;
	return false;
      }
    }
  return true; 
}

/*
void dmtpc::control::MySQLStore::print(const char *table) 
{
  cout << "running print" << endl;
  cout << setw(12) << table << ": "
       << setw(12) << readValueFromDB(table) << endl;
}
*/

bool dmtpc::control::MySQLStore::checkMySQLHandle(void)
{
  //bool lock = mutex_db !=0; 
  //if (lock) mutex_db->Lock(); 
  if (!m_mysql)
    {
      m_mysql = &m_mysql_struct;
      mysql_init(m_mysql);
      if (!connectToDB()) return false;
      /*
	{
	  if (lock) mutex_db->UnLock(); 
	  return false; 
	}
      */
    }
  else mysql_ping(m_mysql);
   //{
  //  cout << "found handle" << endl;
  //  mysql =(MYSQL*) handle;
  //  mysql_ping(mysql);
  // }

  return true;
}
double dmtpc::control::MySQLStore::readValueFromDB(const char *table, unsigned int ifield,
						   bool doPrint) 
{
  // Reads last value in DB table
  // ifield 0 = value;
  // ifield 1 = busy (if defined in table)

  double ret=0;

  //MYSQL mysql_struct; 
  
  //bool lock = mutex_db !=0; 
  //if (lock) mutex_db->Lock(); 

  if (checkMySQLHandle())
    {

      TString sql=TString("SELECT * FROM ") + table + TString(" ORDER BY timestamp DESC LIMIT 1");
      mysql_real_query(m_mysql, (const char*)sql, sql.Length());

      //cout << sql << endl;

      MYSQL_RES *res=mysql_store_result(m_mysql);

      //cout << "table = " << table;
      if (!mysql_num_rows(res)) {
	cout << table << ": No entries in table " << table << endl;
	//if (!handle)
	//  mysql_close(m_mysql);
	//if (lock) mutex_db->UnLock(); 
	return 0;
      }

      MYSQL_ROW  row=mysql_fetch_row(res);

      if (ifield<0 || ifield>mysql_num_fields(res)-1) { 
	cout << "MySQLStore:: from field = " << ifield << endl;
	//if (!handle)
	//  mysql_close(m_mysql);
	//if (lock)
	// mutex_db->UnLock(); 
	return -1; 
      }

      unsigned int  num_fields = mysql_num_fields(res);
      MYSQL_FIELD *fields = mysql_fetch_fields(res);

      //currentValue = atof( row[0] );
      //currentValueRMS = atof( row[1] );
      //setValue = atof( row[2] );
      ret = atof( row[ifield] );
  
      
      if (doPrint) {
	cout << setw(12) << table << ":";
	for(unsigned int i = 0; i < num_fields; i++) {
	  cout <<  setw(12) << fields[i].name << "=" << setw(12) << row[i] ; 
	}
	cout << endl;
      }

      mysql_free_result(res);
      //if (!handle)
      //  mysql_close(mysql);
      //if (lock) mutex_db->UnLock(); 

      return ret;
    }
 
  cout << "Unable to connect to mysql DB to read value" << endl;
  return ERROR_VALUE;
}


int dmtpc::control::MySQLStore::saveValueToDB(const char *table, double value) {
  // A primitive access to a DB containing slow control data.

  //MYSQL *  mysql;
  //bool lock = (mutex_db!=0); 
  //if (lock) mutex_db->Lock(); 
  //MYSQL mysql_struct; 
  
  if (checkMySQLHandle())
    {
      TString sql = ""; 
      /*
	double set = setValue; 
	if (!save_set)
	{
	sql = TString("SELECT setval FROM ") + table + TString(" ORDER BY timestamp DESC LIMIT 1"); 
	mysql_real_query(mysql, sql.Data(), sql.Length()); 
	MYSQL_RES * res = mysql_store_result(mysql); 
	MYSQL_ROW row = mysql_fetch_row(res); 
	set = atof(row[0]); 
	mysql_free_result(res); 
	}
      */

      sql=TString("INSERT INTO ") + table 
	+ TString(" ( value, timestamp) VALUES( ");

      sql += value;
      sql += TString(", NOW() )");
       
      //cout << sql << endl;

      mysql_real_query(m_mysql, (const char*)sql, sql.Length());

      return 1;
    }
  
  cout << "Unable to connect to mysql DB to save value" << endl;
  return -1;

  //if (!handle)
  //  mysql_close(mysql);
  //if (lock) mutex_db->UnLock(); 

  //return 0;
}


int dmtpc::control::MySQLStore::saveValueToDBandCheck(const char *table, double value) {

  double currentValue=readValueFromDB(table);
 
  int ret=0;
  double delta=1e-5;
  while (1) {

    ret=saveValueToDB(table,value);

    double newValue = readValueFromDB(table,0);
    double diff = fabs(newValue - currentValue);
    
    if ( diff<delta ) break;

    cout << table << ": Setting values again (delta=" << delta << ")" << endl;
    if (diff>delta)        
      cout << newValue << "!=" << currentValue << "  delta=" << diff << endl;
    
    //currentValue=currentValue0;

  }

  return ret;
}

/*
double dmtpc::control::MySQLStore::getAverage(const char* table, int n)
{

  double avg=0;
  
  MYSQL mysql;
  
  bool lock = mutex_db != NULL; 
  if (lock) mutex_db->Lock(); 

  mysql_init(&mysql);
  bool success = connectToDB(&mysql,"get values", table); 
  if (!success) 
  {
    if (lock) mutex_db->UnLock(); 
    return -1; 
  }

  TString sql=TString("SELECT value FROM ") + table + TString(" ORDER BY timestamp DESC LIMIT ");
  sql+=n;
  int ret = mysql_real_query(&mysql, (const char*)sql, sql.Length());
  cout << "query: " << sql << " returned " << ret << endl;

  MYSQL_RES *res=mysql_store_result(&mysql);

  if ((int)mysql_num_rows(res)<n) {
    cout << table << ": Not enough data  " << endl;
    mysql_close(&mysql);
    if (lock) mutex_db->UnLock(); 
    return -1;
  }

  for(int i=0; i<n; i++)
    {
      MYSQL_ROW  row=mysql_fetch_row(res);
      avg+=atof(row[0]);
    }

  avg=avg/double(n);

  mysql_free_result(res);
  mysql_close(&mysql);
  if (lock) mutex_db->UnLock(); 

  return avg;

}
*/
