#!/bin/sh

# no traling 00s here for consistency, as with the array they are kept, with seq not
array=( 1474 )


alias js="jobsub -g -c config.cfg -csv runlist.csv"
#alias js="jobsub -g -c config/config_R0temp.cfg -csv runlist/runlist_april.csv"

for i in "${array[@]}"; do
   
   # run for alignment stuff
   #cp geometry/gear-beforeAlign-R0.xml  geometry/tempGear-$i.xml
   #js noisypixel $i
   #js clustering $i
   #js hitmaker $i
   #cp geometry/tempGear-${i}_pre.xml  geometry/tempGear-$i.xml
	 
   js alignGBL1 $i
   #cp geometry/alignedGear-iter_temp.xml geometry/tempGear-$i.xml
	 #js fitGBL $i


done
