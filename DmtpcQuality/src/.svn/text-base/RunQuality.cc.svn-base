#include "RunQuality.hh"
#include "Dataset.hh"
#include "DatasetHeader.hh"
#include "Event.hh"

#include <string>
#include <sstream>
#include <algorithm>


ClassImp(dmtpc::Quality::RunQuality);

dmtpc::Quality::RunQuality::RunQuality(TString run, TString DBname)
{

  if(run.Contains(".root")){
    _use_raw_file = true;
  _d.open(run);
  }
  else{
    _use_raw_file = false;
    _run_id = run;
  }

  _db_name = DBname;
  init();
}

dmtpc::Quality::RunQuality::RunQuality(int run, TString DBname){

  //  _run_id = TString::Itoa(run,10);
  _run_id = Form("%d",run);
  _use_raw_file = false;
  _db_name = DBname;
  init();
}
  

dmtpc::Quality::RunQuality::~RunQuality()
{
  if(_use_raw_file)
    _d.close();
}

//---Initialisation--//
void dmtpc::Quality::RunQuality::init(){

  //hardcoded for now
  _min_current = 0;
  _readtime = 3;
  _nchannel = 5;//4sh + 1sh (cameras only)
  
  //set db conditions
  _run_condition = "run_id = " + _run_id;
  //  if(!_sql.isOpen())
  _sql.open(_db_name);
  //run start and end times
  dmtpc::Quality::RunQuality::setRunTimes();
  //exposure
  dmtpc::Quality::RunQuality::setExposure();
  //number of events
  dmtpc::Quality::RunQuality::setNEvents();
  dmtpc::Quality::RunQuality::setNBias();
  //debug default off
  debug = false;
  

}
/*
void dmtpc::Quality::RunQuality::changeRun(int runId){
  
  _run_id = TString::Itoa(runId,10);
  init();
}
*/
void dmtpc::Quality::RunQuality::setRunTimes(){
  
  if(_use_raw_file){
    _run_start_s = _d.getHeader()->starttime->GetSec();
    _run_end_s = _d.getHeader()->endtime->GetSec();
  }
  else{
    _run_start_s = _sql.getIValue("run","run_start_s",_run_condition); 
    _run_end_s = _sql.getIValue("run","run_end_s",_run_condition); 
  }
}

void dmtpc::Quality::RunQuality::setExposure(){

  //get exposure using time between events
  if(_use_raw_file){
    _d.getEvent(0);
    int ev_start = _d.event()->getTimestamp()->GetSec();
    _d.getEvent(1);
    int ev_end = _d.event()->getTimestamp()->GetSec();
    _exp = ev_end - ev_start;
  }
  else{
    _exp = _sql.getIValue("run","exp4",_run_condition);
  }
  if(debug)
    std::cout<<"exposure: "<<_exp<<std::endl;
}

void dmtpc::Quality::RunQuality::setNEvents(){
  if(_use_raw_file)
    _nevents = _d.nevents();
  else{
    _nevents = _sql.getIValue("run","nexp",_run_condition);
  }
}

void dmtpc::Quality::RunQuality::setNBias(){
  if(_use_raw_file)
    _nbias = _d.nbias();
  else
    _nbias = _sql.getIValue("run","nbias",_run_condition);
}

void dmtpc::Quality::RunQuality::setDataFile(TString dataFileName){
  _d.close();
  _d.open(dataFileName);
  _use_raw_file = true;
  setExposure();
}
//-----end of initialisation----//

//--getters/output--//

void dmtpc::Quality::RunQuality::printRunInfo(){

  std::cout<<"---------------------------------"<<std::endl;
  std::cout<<"Run ID: "<<_run_id<<std::endl;
  std::cout<<"N Events: "<<_nevents<<std::endl;
  std::cout<<"N Bias: "<<_nbias<<std::endl;
  std::cout<<"Run Start: "<<_run_start_s<<std::endl;
  std::cout<<"Run End: "<<_run_end_s<<std::endl;
  std::cout<<"Exposure: "<<_exp<<std::endl;
  std::cout<<"Read Time: "<<_readtime<<std::endl;

}

//check the run info exists in the database provided
bool dmtpc::Quality::RunQuality::noInfo(){
  
  _table = "run";
  _column = "run_id";
  _condition = "run_id = " + _run_id;

  int value = _sql.getIValue(_table,_column,_condition);
  
  if(value < 0){
    return true;
  }
  
  return false;
  
}


//---functions for checking quality----//  

//function for using raw root file
//needs updating - using _spark_events after loadin
int dmtpc::Quality::RunQuality::isSpark (const dmtpc::core::Event * ev, int channel){

  if(!_sql.isOpen())
    _sql.open();

    
  int e_start = ev->getTimestamp()->GetSec();
  std::stringstream sminc;
  sminc << _min_current;
  std::string minc = sminc.str();
  TString ev_end = TString::Itoa(e_start+_exp,10);
  TString ch = TString::Itoa(channel,10);

  TString ev_start = TString::Itoa(e_start,10);
  _table = "sparks";
  _column = "current";
  _condition = ev_start;
  _condition+= " and timestamp_s <= ";
  _condition+= ev_end;
  _condition+= " and current > ";
  _condition+= minc;
  _condition+= " and channel = ";
  _condition+= ch;

  //  int value = _sql.getIResult();
  int value = _sql.getIValue(_table, _column, _condition);
  if(value > 0){
    return 1;
  }
  else{
    return 0;
  }


}

//get event time for not using raw file
int dmtpc::Quality::RunQuality::getEStart(int ev_num){

  //  return _run_start_s + ev_num*_exp;
  TString evCondition = "ev = " + TString::Itoa(ev_num+_nbias,10);
  int t_end = _sql.getIValue("events","timestamp_s",evCondition);
  int e_start = t_end - _exp - _readtime;
  if(debug){
    std::cout<<"evCondition: "<<evCondition<<std::endl;
    std::cout<<"t_end: "<<t_end;
    std::cout<<"E start: "<<e_start<<std::endl;
  }
  return e_start;

}

//overloaded for not using raw file
//needs updating for using _spark_events after loading
int dmtpc::Quality::RunQuality::isSpark(int ev_num, int channel){

  if(!_sql.isOpen())
    _sql.open();

  _table = "spark_events";
  _column = "spark";
  _condition = Form("run_id = %d and ev = %d and channel = %d", _run_id.Atoi(), ev_num, channel);

  int isSpark = _sql.getIValue(_table,_column,_condition);
  
  if(isSpark > 0){
    return isSpark;
  }
  else{
    return 0;
  }
}

bool dmtpc::Quality::RunQuality::isSpark(int ev){
  if(_spark_events.size() == 0)
    loadSparkTaggedEvents();

  int isSpark = 0;
  for(int ch = 0; ch < _nchannel; ch++){
    if(std::find(_spark_events.at(ch).begin(), _spark_events.at(ch).end(), ev) != _spark_events.at(ch).end()){
      //for(int i = 0;  i < _spark_events.at(ch).size(); i++){
      //if(_spark_events.at(ch).at(i) == ev){
	return true;
    }
  }

  return false;
}

  /* ---deprecated, used for v1 of extractDatabase  
  int e_start = getEStart(ev_num);
  std::stringstream sminc;
  sminc << _min_current;
  std::string minc = sminc.str();
  TString ev_end = TString::Itoa(e_start+_exp,10);
  TString ch = TString::Itoa(channel,10);


  TString ev_start = TString::Itoa(e_start,10);
  _table = "sparks";
  _column = "current";
  _condition = "timestamp_s >= " + ev_start;
  condition+= " and timestamp_s <= ";
  condition+= ev_end;
  condition+= " and current > ";
  condition+= minc;
  condition+= " and channel = ";
  condition+= ch;
  */
    
  /*==deprecated
  std::vector<int> values = _sql.getIValues(table, column, condition);
  if(debug){
  std::cout<<"condition: "<<condition<<std::endl;
    std::cout<<"values size: "<<values.size()<<std::endl;  
  }

  
  if(values.size() > 0){
    return 1;
  }
  else{
    return 0;
  }
  */

std::vector< std::vector< int >  > dmtpc::Quality::RunQuality::getSparkTaggedEvents(){
 
  if(_spark_events.size() == 0)
    loadSparkTaggedEvents();

  return _spark_events;

}

void dmtpc::Quality::RunQuality::loadSparkTaggedEvents(){


  if(debug)
    printRunInfo(); 

  //new method, only get spark tagged from, not ev by ev check
  _table = "spark_events";
  _column = "ev";

  for(int channel = 0; channel < _nchannel; channel++){
    _condition = Form("run_id = %d  and channel = %d and spark = 1", _run_id.Atoi(), channel);
    
    std::vector<int> events =  _sql.getIValues(_table,_column,_condition);
    _spark_events.push_back(events);
  }

  if(debug) std::cout<<"checking done"<<std::endl ;

}

double dmtpc::Quality::RunQuality::getLastCurrent(int ev, int ch){

  _table = "spark_events";
  _column = "last_spark_value";
  _condition = Form("ev = %d and channel = %d", ev, ch);

  double value = _sql.getDValue(_table,_column,_condition);
  if(value > 0){
    return value;
  }
  else{
    return 0;
  }
}

int dmtpc::Quality::RunQuality::getLastCurrentTime(int ev, int ch){
  
  _table = "spark_events";
  _column = "last_spark_time";
  _condition = Form("ev = %d and channel = %d", ev, ch);
  
  int value = _sql.getIValue(_table,_column,_condition);
  if(value > 0){
    return value;
  }
  else{
    return 0;
  }
}
  
