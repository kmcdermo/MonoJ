# MonoJ

To run this code, must make plots for each sample first using the Analysis object.  Then once all plots are made, the StackPlots object will compile all histos from data/MC to produce stack plots.

Specify input samples in main.	  
Specify isMC == true with the sample.
Samples are stored on local EOS, and are called in implementation files via xrootd.

Specify output directory for whole schema.
Analysis object individual plots will be placed in output directory and will be saved under {user outputdir}/{sample name}_{MC/data}.  Saved as user defined output type.  Also stored in root file.
StackPlots output stored in {user outputdir}/stacks.

User is given control of colors for MC sample stacking, specified in map.  

Sample MC strings to be used:
zmumu (derived from zll)
ttbar

Sample Data strings to be used:
doublemu