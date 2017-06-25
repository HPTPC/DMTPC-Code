#ifndef DMTPC_CORE_CONFIG
#define DMTPC_CORE_CONFIG

#include <vector>
#ifndef __CINT__
#include <tr1/unordered_map>
#endif
#include <iostream>

using std::vector; 

namespace dmtpc
{
  namespace core
  {
    class Config
    {
      public: 

      enum ConfigStoreType
      {
        TYPE_DBL, 
        TYPE_INT,
        TYPE_UINT,
        TYPE_STR ,
        TYPE_ENUM,
        TYPE_BOOL,
        TYPE_LIST_DBL, 
        TYPE_LIST_INT,
        TYPE_LIST_UINT,
        TYPE_LIST_STR ,
        TYPE_LIST_ENUM,
        TYPE_LIST_BOOL,
        TYPE_MAP_DBL,
        TYPE_MAP_INT,
        TYPE_MAP_STR,
        TYPE_INVALID
      }; 

      struct ConfigStore
      {

         ConfigStoreType type;    ///<Field type
         union
         {
           double dbl_val;
           int int_val; 
           unsigned uint_val;
           char * str_val; 
           bool bool_val; 
           vector<double> *dbl_list;
           vector<int> *int_list;
           vector<unsigned> *uint_list;
           vector<bool>* bool_list;
           vector<char*>* str_list;
#ifndef __CINT__
           std::tr1::unordered_map<std::string, double>* dbl_map; 
           std::tr1::unordered_map<std::string, int>* int_map; 
           std::tr1::unordered_map<std::string, std::string>* str_map; 
#endif

         }data;

         vector<char*> * enum_keys; 
         vector<int> * enum_indices; 
      };



        /** Parses the given file and replaces any matching
           existing values with values from file 
           @param file filename of the file 
        */
        void parseFile(const char * file);  

        /** Parse a stream 
         * @param in the stream to parse
         * */ 
        void parseStream(std::istream & in); 


        void parseString(const char * in); 
        bool setInt(const char * key, int value); 
        bool setBool(const char * key, bool value); 
        bool setUInt(const char * key, unsigned value); 
        bool setDouble(const char * key, double value); 
        bool setString(const char * key, const char * string); 
        bool setEnum(const char * key, int value); 
        bool setEnum(const char * key, const char * value_name); 

        bool setIntList(const char * key, const vector<int> * value); 
        bool setBoolList(const char * key, const vector<bool> * value); 
        bool setUIntList(const char * key, const vector<unsigned>* value); 
        bool setDoubleList(const char * key, const vector<double>* value); 
        bool setStringList(const char * key, const vector<const char *> * strings); 
        bool setEnumList(const char * key, const vector<int>* values); 
        bool setEnumList(const char * key, const vector<const char *> * value_names); 

        bool setIntMap(const char * key, const char * key2, int val); 
        bool setStringMap(const char * key, const char * key2, const char *  val); 
        bool setDoubleMap(const char * key, const char * key2, double val);  

        int getInt(const char * key) const { return getStore(key)->data.int_val; }
        unsigned getUInt(const char * key) const { return getStore(key)->data.uint_val;} 
        double getDouble(const char * key) const  { return getStore(key)->data.dbl_val;} 
        const char * getString(const char * key) const  { return getStore(key)->data.str_val;} 
        int getEnum(const char * key) const  { return getStore(key)->data.int_val;} 
        const char * getEnumName(const char * key, int val) const; 
        bool getBool(const char * key) const { return getStore(key)->data.bool_val; }

        const vector<int> * getIntList(const char * key) const  { return getStore(key)->data.int_list;} 
        const vector<unsigned> * getUIntList(const char * key) const  { return getStore(key)->data.uint_list;} 
        const vector<double> * getDoubleList(const char * key) const  { return getStore(key)->data.dbl_list;} 
        const vector<const char *> * getStringList(const char * key) const  { return (vector<const char *> *)getStore(key)->data.str_list;} 
        const char * getStringList(const char * key, int i) const  { return getStore(key)->data.str_list->operator[](i);} 
        const vector<int> * getEnumList(const char * key) const  { return getStore(key)->data.int_list;} 
        const vector<bool> * getBoolList(const char * key) const  { return getStore(key)->data.bool_list;} 

        int getIntMap(const char * key, const char * key2, int notfound = 0) const; 
        double getDoubleMap(const char * key, const char * key2, double notfound = 0) const; 
        const char *  getStringMap(const char * key, const char * key2, const char * notfound = 0) const; 

#ifndef __CINT__
        const std::tr1::unordered_map<std::string, int> * getIntMap(const char * key) const { return getStore(key)->data.int_map; }
        const std::tr1::unordered_map<std::string, double> * getDoubleMap(const char * key) const { return getStore(key)->data.dbl_map; }
        const std::tr1::unordered_map<std::string, const char *> * getStringMap(const char * key) const { return  (std::tr1::unordered_map<std::string, const char *> *) getStore(key)->data.str_map; }
#endif

        /** Write out the current config to the named file
         @param file the filename of the file
         */
        int writeOutConfig(char * file); 

        /** Prints out the current config to the given output stream 
         *
         * @param out the output stream to print to (default is stdout)
         * */
        virtual void print(std::ostream & out = std::cout) const; 

        /** Destructor. Does nothing */
        virtual ~Config(){;}

      protected:
        /** This method is attempts to parse a key value pair from a file 
         * using the registered fields. 
         *
         * @param key the key to parse (the thing on the left hand side of the = )
         * @param value the key to value (the thing on the right hand side of the = ) 
         * @param line the line number of the line to be parsed.
         * @return true if the key was parsed, false otherwise
         *
         */
        bool parseRegistered(std::string key, std::string value, int line); 

        /** prints out all registered fields to the given output stream 
         * in the config format
         * @param out the output stream to print to (default is stdout)
         */
        void printRegistered(std::ostream & out = std::cout) const; 


        void registerDouble(const char * name, double default_val = 0); 
        void registerInt(const char * name, int default_val = 0); 
        void registerUInt(const char * name, unsigned default_val = 0); 
        void registerString(const char * name, const char * default_val = ""); 
        void registerEnum(const  char * name, int n, const char ** keys, const int * indices = 0, int default_val = 0); 
        void registerBool(const char * name, bool  default_val = false); 

        void registerDoubleList(const char * name, const vector<double> * default_val = 0 ); 
        void registerDoubleList(const char * name, unsigned n, const double * v ); 
        void registerIntList(const char * name, const vector<int>  *default_val = 0); 
        void registerUIntList(const char * name, const vector<unsigned> *default_val = 0); 
        void registerStringList(const char * name, const vector<const char*>* default_val = 0); 
        void registerEnumList(const  char * name, int n, const char ** keys, const int * indices = 0, const vector<int>* default_val = 0); 
        void registerBoolList(const char * name, const vector<bool> * default_val = 0); 

        void registerDoubleMap(const char * name, const vector<const char *> * default_keys = 0, const vector<double> * default_vals = 0); 
        void registerIntMap(const char * name, const vector<const char *> * default_keys = 0, const vector<int> * default_vals = 0); 
        void registerStringMap(const char * name, const vector<const char *> * default_keys = 0, const vector<const char *> * default_vals = 0); 

        std::string asString(); 
        
      private:
        /** This method attempts to parse a key value pair from a file. 
         * By default it just calls parseRegistered to attempt to parse a registered
         * field but can be overriden to do something more complicated
         *
         * @param key the key to parse (the thing on the left hand side of the = )
         * @param value the key to value (the thing on the right hand side of the = ) 
         * @param line the line number of the line to be parsed.
         *
         */
        virtual void parsePair(std::string key, std::string value, int line) ; 
        
        /** Stores the registered fields */
#ifndef __CINT__
        std::tr1::unordered_map<std::string, ConfigStore *> registered; 
#endif
        bool checkType(const char * key, ConfigStoreType type) const { return registered.count(key) > 0 && getStore(key)->type == type; }
        const ConfigStore* getStore(const char * key) const;   

    };


  }
}

#endif
