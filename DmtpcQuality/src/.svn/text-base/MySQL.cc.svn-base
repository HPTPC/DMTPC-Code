//#include "/home/dmatter/dmtpc-svn/DmtpcControl/include/MySQLStore.hh"
#include <mysql/mysql.h>
#include <iostream>
#include <string>

#include <TTimeStamp.h>
#include <TSystem.h>

#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>

#include "MySQL.hh"

ClassImp(dmtpc::Quality::MySQL);

dmtpc::Quality::MySQL::MySQL(){
}

dmtpc::Quality::MySQL::~MySQL(){
  mysql_close(_db);
}

void dmtpc::Quality::MySQL::init_access(char *server, char *user, char *pass, char *db_name){
  _db_access.server = server;
  _db_access.user = user;
  _db_access.pass = pass;
  _db_access.database = db_name;
}

void dmtpc::Quality::MySQL::initialiseDB(char *server, char *user, char *pass, char *db_name){

  //initialise mysql connection
  init_access(server,user,pass,db_name);
  _db  =  mysql_init(NULL);

  //connect to database
  int nloop = 0;
  int ntries = 3;
  //  while(mysql_real_connect(_db,"m3slow.lns.mit.edu","dmatter","","DMTPC_TEST",0,NULL,0) == NULL){
  std::cout<<"Connecting to DB: \n"
	   <<"server: "<<_db_access.server<<"\n"
	   <<"user: "<<_db_access.user<<"\n"
	   <<"pass: "<<_db_access.pass<<"\n"
	   <<"database: "<<_db_access.database<<"\n";

  while(mysql_real_connect(_db,_db_access.server, _db_access.user, _db_access.pass, _db_access.database,0,NULL,0) == NULL){
    nloop++;
    gSystem->Sleep(100);
    if(nloop > ntries){
      std::cout<<"can't connect"<<std::endl;
	break;
    }
  }

}

double dmtpc::Quality::MySQL::getValue(TString table, TString column, TString condition){
  
  TString cmd = "SELECT " + column + " from " + table + " WHERE " + condition;
  //  std::cout<<"cmd: "<<cmd<<std::endl;  

  mysql_real_query(_db, (const char*)cmd, cmd.Length());
  MYSQL_RES * res = mysql_store_result(_db);
  MYSQL_ROW row = mysql_fetch_row(res);


  double value;
  if(row != NULL && atof(row[0]) > 0)
    value = atof(row[0]);
  else
    value = 0;
  mysql_free_result(res);
  return value;
}

double dmtpc::Quality::MySQL::getMaxValue(TString table, TString column, TString condition, TTimeStamp &tstamp){
  
  double value;
  
  condition +=  "ORDER BY value DESC LIMIT 1";;
  TString cmd = "SELECT " + column + ", unix_timestamp(timestamp) FROM " + table + " WHERE " + condition;
  //  std::cout<<"cmd: "<<cmd<<std::endl;
  //query the database
  mysql_real_query(_db,(const char*)cmd,cmd.Length());
  //store the result
  MYSQL_RES *res = mysql_store_result(_db);
  MYSQL_ROW row = mysql_fetch_row(res);

  //store the result
  value = atof(row[0]);
  
  TTimeStamp t = TTimeStamp(atoi(row[1]));
  tstamp = t;
  mysql_free_result(res);
  return value;


}

//overloaded to not store timestamp
/*
double getMaxValue(MYSQL * _db, TString table, TString column, TString condition){
  TTimeStamp t;
  return getMaxValue(_db,table,column,t);
}
*/

std::vector<double> dmtpc::Quality::MySQL::getValues(TString table, TString column, TString condition, bool storeTimes,std::vector< std::pair<uint32_t,uint32_t> >&tstamps){
  
  std::vector<double> values;
  
  TString cmd = "SELECT " + column + ", unix_timestamp(timestamp) FROM " + table + " WHERE " + condition;
  //  std::cout<<"cmd: "<<cmd<<std::endl;
  //query the database
  mysql_real_query(_db,(const char*)cmd,cmd.Length());
  //store the results
  MYSQL_RES *res = mysql_store_result(_db);
  MYSQL_ROW row;// = mysql_fetch_row(res);

  //  std::cout<<"result: "<<res<<std::endl;

  //store the results
  int i = 0;
  //  std::cout<<"storing results"<<std::endl;
  while( (row = mysql_fetch_row(res)) ){
    //  std::cout<<"result: "<<i<<" "<<row[0]<<std::endl;
    values.push_back(atof(row[0]));
    if(storeTimes){
      int t = atoi(row[1]);
      tstamps.push_back(std::make_pair(t,0));//second currently zero due to MySQL version not storing microseconds
    }
    i++;
  }
  
  mysql_free_result(res);

  return values;
}
