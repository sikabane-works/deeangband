#!/bin/sh

for file in ls ./*.txt src/*.h src/*.c lib/apex/* lib/pref/* lib/edit/* lib/help/* lib/file/*
do
   echo $file
   nkf -e $file > tmp
   mv tmp $file
done

chmod 755 bootstrap

