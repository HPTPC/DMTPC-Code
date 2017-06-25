#include "Config.hh" 

const char * my_enum[] = { "bread","eggs","cake"}; 

class MyConfig : public dmtpc::core::Config
{

  public:
    MyConfig(const char * f = 0)
    {
      registerDouble("x",10.3); 
      registerInt("i",42); 
      registerIntList("v"); 
      registerEnum("lunch",3,my_enum,0,0); 
      registerDouble("three",0); 
      registerDouble("pi",0); 

      if (f) parseFile(f); 
    }

    virtual ~MyConfig(){;}
};


int main(int nargs, char ** args)
{
  MyConfig cfg(args[1]); 

  int ret = 0;

  if (cfg.getInt("i") != 42)
  {
    std::cerr << "REASON: INT TEST FAILED" << std::endl;
    ret++;
  }

 cfg.setInt("i",84); 

  if (cfg.getInt("i") != 84)
  {
    std::cerr << "REASON: INT REASSIGNMENT TEST FAILED" << std::endl;
    ret++;
  }

  cfg.parseString("i = 900\n"); 

  if (cfg.getInt("i") != 900)
  {
    std::cerr << "REASON: INT REASSIGNMENT STRING TEST FAILED" << std::endl;
    std::cout << cfg.getInt("i"); 
    ret++;

  }

  cfg.setEnum("lunch","eggs"); 



  if (strcmp("eggs", cfg.getEnumName("lunch",cfg.getEnum("lunch"))))
  {
    std::cout << cfg.getEnum("lunch") << std::endl; 
    std::cout << cfg.getEnumName("lunch",cfg.getEnum("lunch")) << std::endl; 
    std::cerr << "REASON: ENUM ASSIGNMENT TEST FAILED" << std::endl;
    ret++;
  }

  cfg.parseString("v = 10,20,30"); 

  if (cfg.getIntList("v")->at(2) != 30)
  {
    std::cerr << "REASON: VECTOR TEST FAILED" << std::endl;
    ret++; 
  }

  if (cfg.getDouble("pi") != 3.14 || cfg.getDouble("three")!= 3.) 
  {
    std::cerr << "REASON: INCLUDE TEST FAILED" << std::endl;
    ret++; 
  }

  cfg.print(); 

  return ret; 
}



