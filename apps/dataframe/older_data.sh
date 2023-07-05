#!/bin/bash

# links=(
#        https://s3.amazonaws.com/nyc-tlc/csv_backup/yellow_tripdata_2014-01.csv \
#        https://s3.amazonaws.com/nyc-tlc/csv_backup/yellow_tripdata_2014-02.csv \
#        https://s3.amazonaws.com/nyc-tlc/csv_backup/yellow_tripdata_2014-03.csv \
#        https://s3.amazonaws.com/nyc-tlc/csv_backup/yellow_tripdata_2014-04.csv \
#        https://s3.amazonaws.com/nyc-tlc/csv_backup/yellow_tripdata_2014-05.csv
#        )

# head=yellow_tripdata_2016-01.csv

cd /mnt/data/df_csv

for link in "${links[@]}"
do
    wget "$link"
done

# cat $head > all.csv

# for file in `ls *.csv`
# do
#     if [ "$file" = "all.csv" ]; then
# 	continue
#     fi
#     if [ "$file" = "$head" ]; then
# 	continue
#     fi
#     awk '{if (NR > 1) print $0}' $file >> all.csv
# done