NUMTESTS=$2
a=$1

while [ $a -le "$NUMTESTS" ]
  do 
  (if [ $a -le 10 ]
   then
     sed -e 's/snapshot_\([0-9]*\)/snapshot_00'$a'/g' < parameters.txt > temp.txt
   elif [ $a -le 100 ]
   then 
     sed -e 's/snapshot_\([0-9]*\)/snapshot_0'$a'/g' < parameters.txt > temp.txt
   else
     sed -e 's/snapshot_\([0-9]*\)/snapshot_'$a'/g' < parameters.txt > temp.txt
   fi
   mv temp.txt parameters.txt
   ./Snapshot
  )

  a=$(($a+1))
done
