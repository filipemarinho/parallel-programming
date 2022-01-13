#!/bin/bash
#Requires a rich-club-results directory with the .net files and a rich-club-results directory 
# with the .rcb expected results with the exact same name as the output file

path="$HOME/git/Grad/parallel-programming/MPI"

#Run every file with .net extension in the directory and diff the output with the expected
for file in $path/rich-club-results/*; do
    if [ "${file##*.}" = "net" ]; then
        echo "Reading file: " $file
        mpirun "$path/trab3.o" $file >> automate.dat
    fi
done
echo "Sucess! Verifying output data..."
# diff -w -s rich-club-results/ rich-club-expected/
echo Done

