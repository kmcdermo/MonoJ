# MonoJ

Before trying to run this code, first execute scripts/setupdirs.sh. Makefile depends on it.

Then, out of the box, compile by 'make main'.

'main' is the name of the executable produced from four source files: Common, PUReweight, Analysis, StackPlots

In main, first specify samples and selection for PU reweighting.  Standard is "zmumu" selection, and samples are just data = doublemu, MC = zll.

The Analysis object produces all plots for a given sample and selection.  need to break it up into pieces.  Run over top and qcd separately, then call hadd on individual files.

After all Analysis is done, and top and qcd are added to samples, run over the plots to make stack plots with CMS formats with StackPlots  object.  Have to add plots by hand (see constructor).

Will eventually add list of selections and samples.  NOTE: Selection is mostly by hand, so if it changes in analysis, needs to be propagated back to purw!

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