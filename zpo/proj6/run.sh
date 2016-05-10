#!/bin/bash  
rm -r out
mkdir out

#ref image
ref=data/clean.png
#input image
input=$1
#window radius 5 mean window 11x11
radius=40
#param1
param1=(none 5 5 10 40 5)
#param2
param2=-0.5;

output=(none mean median midgray bernsen niblack)
for i in {1..5}
do
  ./thresholding-cv $input out/${output[i]} $i $radius ${param1[i]} $param2
done
