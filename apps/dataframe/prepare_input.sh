#!/bin/bash
cd /mnt/data/df_csv

head=yellow_tripdata_2016-01.csv
head -n 1 $head > all_15.csv
head -n 1 $head > all_16.csv

for file in `ls *.csv`
do
  if [[ $file =~ "2015" ]]; then
    awk '{if (NR > 1) print $0}' $file >> all_15.csv
  elif [[ $file =~ "2016" ]]; then
    awk '{if (NR > 1) print $0}' $file >> all_16.csv
  else
    continue
  fi
done
