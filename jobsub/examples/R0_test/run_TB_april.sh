#!/bin/sh

# no traling 00s here for consistency, as with the array they are kept, with seq not
array=( 1474 )


alias js="jobsub -g -c config.cfg -csv runlist.csv"
#alias js="jobsub -g -c config/config_R0temp.cfg -csv runlist/runlist_april.csv"

for i in "${array[@]}"; do
   
   # run for alignment stuff
   #cp gear/gear-beforeAlign-R0.xml  gear/tempGear-$i.xml
   #js noisypixel $i
   #js clustering $i
   #js hitmaker $i
   cp gear/tempGear-${i}_pre.xml  gear/tempGear-$i.xml
   #js hitmaker $i
	 
   js alignGBL1 $i
   #cp gear/tempGear-${i}_GBL1.xml gear/tempGear-$i.xml
   #js alignGBL2 $i
	 #js fitGBL $i


done
