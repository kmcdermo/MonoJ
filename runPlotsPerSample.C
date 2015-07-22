void setupcpp11();

void runPlotsPerSample(){
  setupcpp11();

  gROOT->LoadMacro("Analysis.cpp++g");
  // First is channel
  // Second is MC/Data, if MC, isMC == 1, else isMC == 0

  // Third is output name of directory/rootfile/file plots
  // Fourth is output type of plots if SaveAs
  
  Analysis zmumuData("zmumu",false,"png");
  zmumuData.DoAnalysis();

  std::cout << "on to the next one" << std::endl;

  Analysis zmumuMC("zmumu",true,"png");
  zmumuMC.DoAnalysis();
}

void setupcpp11(){ // customize ACLiC's behavior ...
  TString o;
  // customize MakeSharedLib
  o = TString(gSystem->GetMakeSharedLib());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeSharedLib(o.Data());
  // customize MakeExe
  o = TString(gSystem->GetMakeExe());
  o = o.ReplaceAll(" -c ", " -std=c++0x -c ");
  gSystem->SetMakeExe(o.Data());
} 
