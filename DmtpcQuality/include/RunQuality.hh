#ifndef DMTPC_RUN_QUALITY_HH
#define DMTPC_RUN_QUALITY_HH

#include "SQLite.hh"
#include "../DmtpcCore/include/Dataset.hh"
#include "TString.h"
#include "TObject.h"

namespace dmtpc
{
  namespace core 
  {
    //    class Dataset;
    class Event;
  }
  namespace Quality
  {
    class SQLite;
    class RunQuality : public TObject
    {
    public:
      RunQuality(){_use_raw_file = false;};
      RunQuality(TString run, TString DBName);
      RunQuality(int run, TString DBname);
      virtual ~RunQuality();

      //---setter/getter/class initialisation----  
      void setRunId(TString runId){_run_id = runId;}
      void init();
      void setDataFile(TString dataFileName);
      void setNChannels(int nchan){_nchannel = nchan;}
      void openDB(TString dbName){_db_name = dbName; _sql.open(_db_name);}
      dmtpc::core::Dataset getDataset(){return _d;}

      //--setter/getter info---//
      void setDebug(){debug = true;}
      //      void changeRun(int runId);
      
      int runid(){return _run_id.Atoi();}
      int estart(int ev_num){return getEStart(ev_num);}
      int nevents(){return _nevents;}
      int nbias(){return _nbias;}

      void printRunInfo();
      bool noInfo();//true if run not in provided database

      //----quality functions----//		
      int isSpark(int ev_num, int channel);
      //all channels
      bool isSpark(int ev_num);
      void loadSparkTaggedEvents();
      std::vector< std::vector< int > > getSparkTaggedEvents();
      double getLastCurrent(int ev, int ch);
      int getLastCurrentTime(int ev, int ch);


    private:
      //class setup
      dmtpc::core::Dataset _d;
      SQLite _sql;
      TString _db_name;
      TString _cmd;
      int _nchannel;
      bool _use_raw_file;
      void setRunTimes();
      void setExposure();
      void setNEvents();
      void setNBias();
      //----run parameters-----
      TString _run_id;
      int _nevents;
      int _nbias;
      //time variables (unix_timestamp) ns for futureproof MySQL upgrade
      int _run_start_s, _run_start_ns;
      int _run_end_s, _run_end_ns;
      int _exp;
      int _readtime;
      bool debug;
      int getEStart(int ev_num);

      //---database query variables---
      TString _run_condition;
      TString _table;
      TString _column;
      TString _condition;
      //quality variables (as expands might want to set with config file)
      double _min_current;
      //results
      std::vector< std::vector< int > > _spark_events; 
      int isSpark(const dmtpc::core::Event * ev, int channel);
      //      int isSpark(int ev_num, int channel);
      
      ClassDef(RunQuality,1);
      
    };
  }
}
#endif
