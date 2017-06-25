#ifndef SQLITE_FUNCTIONS_HH
#define SQLITE_FUNCTIONS_HH

#ifndef __MAKECINT__
#include <sqlite3.h>
#endif
#include <TString.h>
#include <TObject.h>
#include <TTimeStamp.h>

#include <iostream>
#include <cstdio>



namespace dmtpc
{
  namespace core
  {
    class Dataset;
    class Event;
  }
  namespace Quality
  {

    class SQLite// : public TObject
      {


      public:
	
	SQLite();
	SQLite(TString db_name);
	virtual ~SQLite();

	void setName(TString db_name){_db_name = db_name;}
	void createDatabase();
	void createTable();
	void createSparkTable();
	void createRunTable();
	void createEventTable();
	void createSparkEventTable();
	void storeValues(TString cmd);
	void open()
	{
	  if(_db_name.Length() == 0)
	    std::cout<<"Set the database name"<<std::endl;
	  else
	    dmtpc::Quality::SQLite::createDatabase();

	}
	void open(TString dbName){
	  _db_name = dbName;
	  dmtpc::Quality::SQLite::createDatabase();
	}
	//	void close(){sqlite3_close(_db);} - in destructor
	bool isOpen();

	//update so user doesn't have to know which type they're getting (template)
	//additional functionality might be useful here
	int getIValue(TString table, TString column, TString condition);
	double getDValue(TString table, TString column, TString condition);
	void setCommand(TString cmd){_cmd = cmd;}
	std::vector<int> getIValues(TString table, TString column, TString condition);
	int getMaxIValue(TString table, TString column, TString condition);
	std::vector<double> getDValues(TString table, TString column, TString condition);


      private:
	TString _db_name;
#ifndef __MAKECINT__
	sqlite3 * _db;
	sqlite3_stmt * _res;
#endif
	int _rc;
	TString _cmd;
	int _exp;
	const char * _tail;//!
	int getIResult();
	double getDResult();
	static int callback(void * notUsed, int argc, char ** argv, char ** azColName){
	  int i;
	  for(i=0; i < argc; i++){
	    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	  }
	  printf("\n");
	  return 0;
	}



	ClassDef(SQLite,1);
    };
  }
}
#endif 
