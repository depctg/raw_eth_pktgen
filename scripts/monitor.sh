WATCHED_PID=$({ ./L data/CA_ROAD.txt >log.stdout 2>log.stderr & } && echo $!);
while ps -p $WATCHED_PID --no-headers --format "etime pid %cpu %mem rss"; do 
   sleep 1 
done
