#!/usr/bin/env bash

#compile the program
echo "Compiling antifraud ... "
g++ -o antifraud ./src/antifraud.cxx 
if [ $? -eq 0 ]; then 
	echo "Successfully compiled antifraud"

	# I'll execute my programs, with the input directory paymo_input and output the files in the directory paymo_output
	echo "Executing antifraud ... "
	./antifraud ./paymo_input/batch_payment.csv ./insight_testsuite/tests/my-paymo-trans/paymo_input/stream_payment.txt 0 insight_testsuite/tests/my-paymo-trans/paymo_output/output0.txt
	echo "All done" 
else 
	echo "Compilation failed"
fi	
