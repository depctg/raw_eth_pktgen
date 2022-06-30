WATCHED_PID=$({ ./../build/bin/graph_alg-SSSP ../apps/graph_alg/data/roadNet-PA.txt 1 1 >log.stdout 2>log.stderr & } && echo $!);
while ps -p $WATCHED_PID --no-headers --format "etime pid %cpu %mem rss"; do 
   sleep 0.1
done
