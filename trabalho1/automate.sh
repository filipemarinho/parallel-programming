#!/bin/bash
for file in /home/filipe/git/Grad/parallel-programming/trabalho1/rich-club-results/*
do
   /home/filipe/git/Grad/parallel-programming/trabalho1/trab1.o $file >> output.dat
done
echo Sucess! Verifying output data...
diff -w -s rich-club-results/ rich-club-expected/
echo Done