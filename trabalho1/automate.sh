#!/bin/bash
# Run every file with .net extension in the directory and diff the output with the expected
for file in /home/filipe/git/Grad/parallel-programming/trabalho1/rich-club-results/*; do
    if [ "${file##*.}" = "net" ]; then
        echo "Reading file: " $file
        /home/filipe/git/Grad/parallel-programming/trabalho1/trab1.o $file >> automate.dat
    fi
done
echo "Sucess! Verifying output data..."
diff -w -s rich-club-results/ rich-club-expected/
echo Done
