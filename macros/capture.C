#include <vector>

void capture(){
  gROOT->ProcessLine(".L pureweight.C");
  std::vector<Double_t> that = pureweight();

  

  std::cout<< "captured: " << std::endl;  
  for (int i = 0; i < 50; i++){
    std::cout<< that[i] << std::endl;

  }





}
