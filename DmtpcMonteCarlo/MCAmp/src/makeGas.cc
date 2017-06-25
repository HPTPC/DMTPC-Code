#include "Gas.hh" 
#include <cstdlib>


int main(int nargs, char ** args) 
{

  double pressure = atof(args[1]); 
  double temp = nargs>2 ? atof(args[2]) : 298; 

  dmtpc::mc::amp::GasLoader gas; 
  gas.getCF4(pressure,temp)->PrintGas(); 

}
