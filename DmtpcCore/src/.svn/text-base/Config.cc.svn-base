#include "Config.hh"
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <cstring>

const dmtpc::core::Config::ConfigStore* dmtpc::core::Config::getStore(const char * key) const 
{
  if (!registered.count(key)) return 0; 
  std::tr1::unordered_map<std::string,ConfigStore*>::const_iterator it = registered.find(key); 
  return it->second; 
}



int dmtpc::core::Config::writeOutConfig(char * file)
{
  std::ofstream f; 
  f.open(file); 
  if (!f.is_open())
  {
    std::cerr << "Could not open file " << file << " for some reason." << std::endl;
    return 1; 
  }
  print(f); 
  f.close(); 
  return 0; 
}


static std::string removeWhitespace(std::string in)
{
  std::stringstream out; 

  for (std::string::size_type i = 0; i < in.size(); i++)
  {
    if (in[i]!=' ' && in[i]!='\t')
        out << in[i]; 
  }

  return out.str(); 

}


void dmtpc::core::Config::parseFile(const char * file)
{

  std::ifstream f(file);  
  if (f.is_open()) parseStream(f); 
  else std::cerr << "Could not open file " << file << std::endl; 
}


void dmtpc::core::Config::parseString(const char * s) 
{
  std::string str(s); 
  if (str.find('\n') == std::string::npos)
  {
    str +="\n"; 
  }
  std::istringstream stream(str); 
  parseStream(stream); 
}



void dmtpc::core::Config::parseStream(std::istream & in)
{
  std::string line; 
  int n = 0; 
  while(! in.eof())
  {
    std::getline(in,line); 
    n++;

    //check if we have an include directive
    if (line.find("include ") == 0) 
    {
      parseFile(removeWhitespace(line.substr(sizeof("include"))).c_str()); 
      continue; 
    }


    //Get rid of all white space
    line = removeWhitespace(line); 

    if (line.size() == 0) continue; 
    
    //Get rid of comments
    std::string::size_type comment_loc = line.find("#"); 
    if (comment_loc != std::string::npos)
    {
      line.replace(comment_loc,line.size() - comment_loc, ""); 
    }

    if (line.size() ==0) continue;  
    
    std::string::size_type equal_sign_loc = line.find("="); 
    
    //Make sure there is an equal sign
    if (equal_sign_loc == std::string::npos )
    {
      std::cerr << "No equal sign found in non-empty line " << n
                << ". Skipping." << std::endl;
      continue; 
    }
    //Make sure only one equal sign
    if (equal_sign_loc != line.rfind("="))
    {
      std::cerr << "More than one = sign found in config file line " << n
                <<". Skipping." << std::endl;
      continue; 
    }
    
    //Make sure there is something both before and after the equal sign
    if (equal_sign_loc == 0 || equal_sign_loc == line.size() - 1)
    {
      std::cerr << "No key or no value on line " << n << ". Skipping." 
                << std:: endl;

      continue; 
    }
    

    parsePair(line.substr(0,equal_sign_loc), line.substr(equal_sign_loc+1),n);

  }
}
   

void dmtpc::core::Config::registerDouble(const char * name, double val)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_DBL; 
  store->data.dbl_val = val; 
  registered[name]= store;  
}

void dmtpc::core::Config::registerUInt(const char * name, unsigned val)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_UINT; 
  store->data.uint_val = val; 
  registered[name]=store; 
}

void dmtpc::core::Config::registerInt(const char * name, int val)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_INT; 
  store->data.int_val = val;
  registered[name]=store; 
}

void dmtpc::core::Config::registerBool(const char * name, bool val)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_BOOL; 
  store->data.bool_val = val; 
  registered[name]=store; 
}

void dmtpc::core::Config::registerString(const char * name, const char * ptr)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_STR; 
  store->data.str_val = strdup(ptr); 
  registered[name]=store; 
}

void dmtpc::core::Config::registerEnum(const char * name, int n, const char ** keys, const int * indices, int default_val)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_ENUM; 
  store->data.int_val = default_val; 

  store->enum_keys = new vector<char*>(n); 
  for (int i = 0; i < n; i++) { (*store->enum_keys)[i] = strdup(keys[i]); }
  if (indices)
  {
    store->enum_indices = new vector<int>(n); 
    for (int i = 0; i < n; i++) { (*store->enum_indices)[i] = indices[i]; }
  }
  else store->enum_indices = NULL; 

  registered[name]=store; 
}

void dmtpc::core::Config::registerStringMap(const char * name, const vector<const char *> *k, const vector<const char *> *v)
{

  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_MAP_STR; 
  store->data.str_map = new std::tr1::unordered_map<std::string,std::string>; 

  registered[name]=store; 

  if (k && v)
  {
    for (unsigned i = 0; i < k->size(); i++)
    {
      setStringMap(name,k->at(i), v->at(i)); 
    }
  }

}
void dmtpc::core::Config::registerIntMap(const char * name, const vector<const char *> *k, const vector<int> *v)
{

  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_MAP_INT; 
  store->data.int_map = new std::tr1::unordered_map<std::string,int>; 

  registered[name]=store; 
  if (k && v)
  {
    for (unsigned i = 0; i < k->size(); i++)
    {
      setIntMap(name,k->at(i), v->at(i)); 
    }
  }
}

void dmtpc::core::Config::registerDoubleMap(const char * name, const vector<const char *> *k, const vector<double> *v)
{

  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_MAP_DBL; 
  store->data.dbl_map = new std::tr1::unordered_map<std::string,double>; 

  registered[name]=store; 

  if (k && v)
  {
    for (unsigned i = 0; i < k->size(); i++)
    {
      setDoubleMap(name,k->at(i), v->at(i)); 
    }
  }

}

void dmtpc::core::Config::registerDoubleList(const char * name, unsigned n,  const double * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_DBL; 
  store->data.dbl_list = v ? new vector<double>(v, v+n) : new vector<double>;  
  registered[name]=store; 
}


void dmtpc::core::Config::registerDoubleList(const char * name, const vector<double> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_DBL; 
  store->data.dbl_list = v ? new vector<double>(*v) : new vector<double>;  
  registered[name]=store; 
}

void dmtpc::core::Config::registerIntList(const char * name, const vector<int> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_INT; 
  store->data.int_list = v ? new vector<int>(*v) : new vector<int>;  
  registered[name]=store; 
}

void dmtpc::core::Config::registerUIntList(const char * name, const vector<unsigned> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_UINT; 
  store->data.uint_list = v ? new vector<unsigned>(*v) : new vector<unsigned>;  
  registered[name]=store; 
}

void dmtpc::core::Config::registerBoolList(const char * name, const vector<bool> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_BOOL; 
  store->data.bool_list = v ? new vector<bool>(*v) : new vector<bool>;  
  registered[name]=store; 
}

void dmtpc::core::Config::registerStringList(const char * name, const vector<const char *> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_STR; 
  store->data.str_list =  new vector<char *>;  
  if (v) setStringList(name,v); 
  registered[name]=store; 
}

void dmtpc::core::Config::registerEnumList(const char * name, int n, const char ** keys, const int * indices, const vector<int> * v)
{
  ConfigStore * store = new ConfigStore; 
  store->type = TYPE_LIST_ENUM; 

  store->data.int_list = v ? new vector<int>(*v) : new vector<int>;  

  store->enum_keys = new vector<char*>(n); 

  for (int i = 0; i < n; i++) { (*store->enum_keys)[i] = strdup(keys[i]); }

  if (indices)
  {
    store->enum_indices = new vector<int>(n); 
    for (int i = 0; i < n; i++) { (*store->enum_indices)[i] = indices[i]; }
  }
  else store->enum_indices = NULL; 

  registered[name]=store; 
}





void dmtpc::core::Config::parsePair(std::string key, std::string value, int n)
{
//  std::cout << key << " :: " << value << std::endl; 
  parseRegistered(key,value,n); 
}

bool dmtpc::core::Config::setDoubleMap(const char * key, const char * key2, double val )
{

  if (checkType(key,TYPE_MAP_DBL))
  {

    (*(registered[key]->data.dbl_map))[std::string(key2)]= val; 

    return true;
  }
  return false;
}

bool dmtpc::core::Config::setStringMap(const char * key, const char * key2, const char * val)
{

  if (checkType(key,TYPE_MAP_STR))
  {

    (*(registered[key]->data.str_map))[std::string(key2)]= std::string(val); 

    return true;
  }
  return false;
}

bool dmtpc::core::Config::setIntMap(const char * key, const char * key2, int val)
{

  if (checkType(key,TYPE_MAP_INT))
  {
    (*(registered[key]->data.int_map))[std::string(key2)]= val;

    return true;
  }
  return false;

}

bool dmtpc::core::Config::setInt(const char * key, int value) 
{
  if (checkType(key,TYPE_INT))
  {
    registered[key]->data.int_val = value; 
    return true; 
  }
  return false;
}



bool dmtpc::core::Config::setIntList(const char * key, const vector<int> * value) 
{
  if (checkType(key,TYPE_LIST_INT))
  {
    registered[key]->data.int_list->clear(); 
    registered[key]->data.int_list->insert(registered[key]->data.int_list->begin(), value->begin(), value->end()); 
    return true; 
  }
  return false;
}

bool dmtpc::core::Config::setBoolList(const char * key, const vector<bool> * value) 
{
  if (checkType(key,TYPE_LIST_BOOL))
  {
    registered[key]->data.bool_list->clear(); 
    registered[key]->data.bool_list->insert(registered[key]->data.bool_list->begin(), value->begin(), value->end()); 
    return true; 
  }
  return false;
}


bool dmtpc::core::Config::setUInt(const char * key, unsigned value) 
{
  if (checkType(key,TYPE_UINT)) 
  {
    registered[key]->data.uint_val = value; 
    return true; 

  }
  return false;
}

bool dmtpc::core::Config::setUIntList(const char * key, const vector<unsigned> * value) 
{
  if (checkType(key,TYPE_LIST_UINT))
  {
    registered[key]->data.uint_list->clear(); 
    registered[key]->data.uint_list->insert(registered[key]->data.uint_list->begin(), value->begin(), value->end()); 
    return true; 
  }
  return false;
}


bool dmtpc::core::Config::setBool(const char * key, bool value) 
{
  if (checkType(key,TYPE_BOOL)) 
  {
    registered[key]->data.bool_val = value; 
    return true; 

  }
  return false;
}

bool dmtpc::core::Config::setDouble(const char * key, double value) 
{
  if (checkType(key,TYPE_DBL)) 
  {
    registered[key]->data.dbl_val = value; 
    return true; 

  }
  return false;
}

bool dmtpc::core::Config::setDoubleList(const char * key, const vector<double> * value) 
{
  if (checkType(key,TYPE_LIST_DBL))
  {
    registered[key]->data.dbl_list->clear(); 
    registered[key]->data.dbl_list->insert(registered[key]->data.dbl_list->begin(), value->begin(), value->end()); 
    return true; 
  }
  return false;
}


bool dmtpc::core::Config::setEnum(const char * key, int value) 
{
  if (checkType(key,TYPE_ENUM))
  {
    registered[key]->data.int_val = value; 
    return true; 
  }
  return false;
}

bool dmtpc::core::Config::setEnumList(const char * key, const vector<int> * value) 
{
  if (checkType(key,TYPE_LIST_ENUM))
  {
    registered[key]->data.int_list->clear(); 
    registered[key]->data.int_list->insert(registered[key]->data.int_list->begin(), value->begin(), value->end()); 
    return true; 
  }
  return false;
}



bool dmtpc::core::Config::setEnum(const char * key, const char * value) 
{
  if (checkType(key,TYPE_ENUM) )
  {
    ConfigStore *store = registered[key]; 
    for (unsigned j = 0; j < store->enum_keys->size(); j++)
    {
      if ( strcasecmp(value, store->enum_keys->at(j)) ==0)
      {
        if (store->enum_indices==NULL)
          store->data.int_val = j; 
        else 
          store->data.int_val = store->enum_indices->at(j); 
        break; 
      }

      if (j == store->enum_keys->size() - 1)
      {
        std::cerr << "Warning, don't understand value \""<<value<<"\". Skipping." << std::endl; 
        store->type = TYPE_INVALID; 
      }
    }
 
    return true; 

  }
  return false;
}


bool dmtpc::core::Config::setEnumList(const char * key, const vector<const char *> * value) 
{
  if (checkType(key,TYPE_LIST_ENUM))
  {

    ConfigStore* store = registered[key]; 
    registered[key]->data.int_list->clear(); 
    for (unsigned i = 0; i < value->size(); i++)
    {
      for (unsigned j = 0; j < store->enum_keys->size(); j++)
      {
        if ( strcasecmp(value->at(i), store->enum_keys->at(j)) ==0)
        {
          if (store->enum_indices==NULL)
          {
            store->data.int_list->push_back(j); 
          }
          else 
          {
            store->data.int_list->push_back(store->enum_indices->at(j)); 
          }
          break; 
        }

        if (j == store->enum_keys->size() - 1)
        {
          std::cerr << "Warning, don't understand value \""<<value->at(i)<<"\". Skipping value in list" << std::endl; 
        }
      }
    }
    return true; 
  }
  return false;
}


bool dmtpc::core::Config::setString(const char * key, const char * value) 
{
  if (checkType(key,TYPE_STR))
  {
    if (registered[key]->data.str_val) free(registered[key]->data.str_val);
    registered[key]->data.str_val = strdup(value); 
    return true; 
  }
  return false;
}

bool dmtpc::core::Config::setStringList(const char * key, const vector<const char *> * value) 
{
  if (checkType(key,TYPE_LIST_STR))
  {
    for (unsigned i = 0; i < registered[key]->data.str_list->size(); i++)
    {
      free(registered[key]->data.str_list->at(i)); 
    }

    registered[key]->data.str_list->clear();
    registered[key]->data.str_list->reserve(value->size()); 

    for (unsigned i = 0; i < value->size(); i++)
      registered[key]->data.str_list->push_back(strdup(value->at(i)));

    return true; 
  }
  return false;
}



bool dmtpc::core::Config::parseRegistered(std::string key, std::string value, int n)
{
  
//  std::cout << "HERE: \"" << key.c_str() << "\" :: \"" << value.c_str() << "\"" <<  std::endl; 
  
  const char * key_cstr = key.c_str(); 
  const char * value_cstr = value.c_str(); 

  if (registered.count(key_cstr) > 0)
  {
    ConfigStore *store = registered[key_cstr]; 

    if (store->type == TYPE_DBL)
    {
      setDouble(key_cstr,atof(value_cstr));   
    }
            
    else if (store->type == TYPE_INT)
    {
      setInt(key_cstr,atoi(value_cstr));  
    }

    else if (store->type == TYPE_BOOL)
    {
      setBool(key_cstr,!strcasecmp(value_cstr,"true") || atoi(value_cstr)!=0);  
    }
    else if (store->type == TYPE_UINT)
    {
      setUInt(key_cstr,atoi(value_cstr));  
    }
            
    else if (store->type == TYPE_STR)
    {
      setString(key_cstr, value_cstr);  
    }
            
    else if (store->type == TYPE_ENUM)
    {
      setEnum(key_cstr, value_cstr);  
    }

    else //list
    {
      size_t last_pos = 0; 
      vector<char *> tokens; 

      
      while(true)
      {
        size_t pos = value.find(',',last_pos); 
        
        if (pos!= std::string::npos)
        {
          std::string val =  value.substr(last_pos,pos);
          tokens.push_back(strdup(val.c_str())); 
//          std::cout <<" Added list token " << val.c_str() << std::endl; 
        }
        else
        {
          break;
        }

        last_pos = pos+1; 
      }
      if (last_pos!=value.length())
      {
          std::string val =  value.substr(last_pos);
          tokens.push_back(strdup(val.c_str())); 
 //         std::cout <<" Added list token " << val.c_str() << std::endl; 
      }



      if (store->type == TYPE_LIST_INT)
      {
        store->data.int_list->clear(); 
        store->data.int_list->reserve(tokens.size()); 
        for (unsigned i = 0; i < tokens.size(); i++) store->data.int_list->push_back(atoi(tokens[i])); 
      }

      else if (store->type == TYPE_LIST_UINT)
      {
        store->data.uint_list->clear(); 
        store->data.uint_list->reserve(tokens.size()); 
        for (unsigned i = 0; i < tokens.size(); i++) store->data.uint_list->push_back(atoi(tokens[i])); 
      }

      else if (store->type == TYPE_LIST_BOOL)
      {
        store->data.bool_list->clear(); 
        store->data.bool_list->reserve(tokens.size()); 
        for (unsigned i = 0; i < tokens.size(); i++) store->data.bool_list->push_back(!strcasecmp(tokens[i],"true") || atoi(tokens[i])); 
      }

      else if (store->type == TYPE_LIST_DBL)
      {
        store->data.dbl_list->clear(); 
        store->data.dbl_list->reserve(tokens.size()); 
        for (unsigned i = 0; i < tokens.size(); i++) store->data.dbl_list->push_back(atof(tokens[i])); 
      }
      else if (store->type == TYPE_LIST_STR)
      {
        setStringList(key_cstr, (const vector<const char *> *)&tokens); 
      }
      else if (store->type == TYPE_LIST_ENUM)
      {
        setEnumList(key_cstr, (const vector<const char *> *)&tokens); 
      }

      // map 
      

      for (unsigned i = 0; i < tokens.size(); i++)
      {
        if (char * valval = strstr(tokens[i],":"))
        {
          int n = valval - tokens[i]; 
          std::string key2(tokens[i],n); 

          if (store->type == TYPE_MAP_DBL)
          {
            setDoubleMap(key_cstr, key2.c_str(), atof(valval+1)); 
          }
          else if (store->type == TYPE_MAP_STR)
          {
            setStringMap(key_cstr, key2.c_str(), valval+1); 
          }
          else if (store->type == TYPE_MAP_INT)
          {
            setIntMap(key_cstr, key2.c_str(), atoi(valval+1)); 
          }
        }
      }

      for (unsigned i = 0; i < tokens.size(); i++) free(tokens[i]);

    }

    return true;
  }

  return false; 
}


void dmtpc::core::Config::print(std::ostream & out) const
{
  printRegistered(out); 
}


void dmtpc::core::Config::printRegistered(std::ostream & out) const
{
  for (std::tr1::unordered_map<std::string, ConfigStore*>::const_iterator it = registered.begin(); it !=registered.end(); it++  )
  {
    char * printme; 
    int val; 
    Config::ConfigStore * store = it->second; 
    const char * name = it->first.c_str(); 

    switch(store->type)
    {
      case TYPE_DBL: 
        out << name << " = " << store->data.dbl_val<< std::endl; break; 
      case TYPE_INT: 
        out << name << " = " << store->data.int_val<< std::endl; break; 
      case TYPE_UINT: 
        out << name << " = " << store->data.uint_val<< std::endl; break; 
      case TYPE_STR:
        out << name << " = " << store->data.str_val<< std::endl; break; 
      case TYPE_BOOL: 
        printme = store->data.bool_val ? strdup("true") : strdup("false");
        out << name << " = " << printme << std::endl; 
        break; 
      case TYPE_ENUM:
        out << name << " = " << getEnumName(name, store->data.int_val) << std::endl; break; 

      case TYPE_LIST_DBL: 
       out << name << " = "; 
       if (store->data.dbl_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.dbl_list->size()-1; i++)
         {
           out << store->data.dbl_list->at(i); 
           out << " , "; 
         }
         out << store->data.dbl_list->at(store->data.dbl_list->size()-1) << std::endl; 
       }
       break;

      case TYPE_LIST_INT: 
       out << name << " = "; 
       if (store->data.int_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.int_list->size()-1; i++)
         {
           out << store->data.int_list->at(i); 
           out << " , "; 
         }
         out << store->data.int_list->at(store->data.int_list->size()-1) << std::endl; 
       }
       break;

      case TYPE_LIST_UINT: 
       out << name << " = "; 
       if (store->data.uint_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.uint_list->size()-1; i++)
         {
           out << store->data.uint_list->at(i); 
           out << " , "; 
         }
         out << store->data.uint_list->at(store->data.uint_list->size()-1) << std::endl; 
       }
       break;

      case TYPE_LIST_STR: 
       out << name << " = "; 
       if (store->data.str_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.str_list->size()-1; i++)
         {
           out << store->data.str_list->at(i); 
           out << " , "; 
         }
         out << store->data.str_list->at(store->data.str_list->size()-1) << std::endl; 
       }
       break;

      case TYPE_LIST_BOOL: 
       out << name << " = "; 
       if (store->data.bool_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.bool_list->size()-1; i++)
         {
           out << store->data.bool_list->at(i) ? "true" : "false" ; 
           out << " , "; 
         }
         out << (store->data.bool_list->at(store->data.bool_list->size()-1) ? "true" : "false")  << std::endl; 
       }
       break;

       case TYPE_LIST_ENUM: 
       out << name << " = "; 
       if (store->data.int_list->size() > 0)
       {
         for (unsigned i = 0; i < store->data.int_list->size()-1; i++)
         {
           out << getEnumName(name,store->data.int_list->at(i)); 
           out << " , "; 
         }
         out << getEnumName(name,store->data.int_list->at(store->data.int_list->size()-1)) << std::endl; 
       }
       break;


       case TYPE_MAP_DBL:
       out << name <<" = "; 
       for (std::tr1::unordered_map<std::string,double>::iterator it = store->data.dbl_map->begin(); it!= store->data.dbl_map->end(); it++)
       {
         if (it!= store->data.dbl_map->begin()) 
         {
           out << " , "; 
         }

         out << (*it).first<< " : " << (*it).second;
       }
       out << std::endl; 
       break;

       case TYPE_MAP_INT:
       out << name <<" = "; 
       for (std::tr1::unordered_map<std::string,int>::iterator it = store->data.int_map->begin(); it!= store->data.int_map->end(); it++)
       {
         if (it!= store->data.int_map->begin()) 
         {
           out << " , "; 
         }

         out << (*it).first<< " : " << (*it).second;
       }
       out << std::endl; 
       break;

       case TYPE_MAP_STR:
       out << name << " = "; 
       for (std::tr1::unordered_map<std::string,std::string>::iterator it = store->data.str_map->begin(); it!= store->data.str_map->end(); it++)
       {
         if (it!= store->data.str_map->begin()) 
         {
           out << " , "; 
         }

         out << (*it).first<< " : " << (*it).second;
       }
       out << std::endl; 
       break;




      case TYPE_INVALID:
        break; 

    }
  }
  
}

const char * dmtpc::core::Config::getStringMap(const char * key, const char * key2, const char * notfound) const
{

  if (checkType(key, TYPE_MAP_STR) && getStore(key)->data.str_map->count(key2))
  {
    return getStore(key)->data.str_map->operator[](key2).c_str(); 
  }
  return notfound; 
}


double dmtpc::core::Config::getDoubleMap(const char * key, const char * key2, double notfound) const
{

  if (checkType(key, TYPE_MAP_DBL) && getStore(key)->data.dbl_map->count(key2))
  {
    return getStore(key)->data.dbl_map->operator[](key2); 
  }
  return notfound; 
}



int dmtpc::core::Config::getIntMap(const char * key, const char * key2, int notfound) const 
{

  if (checkType(key, TYPE_MAP_INT) && getStore(key)->data.int_map->count(key2))
  {
    return getStore(key)->data.int_map->operator[](key2); 
  }
  return notfound; 
}


const char * dmtpc::core::Config::getEnumName(const char * key, int val) const
{
  const ConfigStore * store = getStore(key); 
  if (!store) return 0; 

  if (!store->enum_indices && val < store->enum_keys->size())
  {
    return store->enum_keys->at(val); 
  }
  else if (store->enum_indices)
  {
    for (unsigned k = 0; k < store->enum_indices->size(); k++)
    {
      if (store->enum_indices->at(k) == val)
      {
         return store->enum_keys->at(k);  
      }
    }
  }

  return 0; 
}

