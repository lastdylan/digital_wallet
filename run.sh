#!/usr/bin/env bash

#compile the program
echo "Compiling antifraud ... "
g++ -std=c++11 -o antifraud ./src/antifraud.cxx 
if [ $? -eq 0 ]; then 
	echo "Successfully compiled antifraud"

	# I'll execute my programs, with the input directory paymo_input and output the files in the directory paymo_output
	echo "Executing antifraud ... "
	./antifraud ./paymo_input/batch_payment.txt ./paymo_input/stream_payment.txt ./paymo_output/output1.txt ./paymo_output/output2.txt ./paymo_output/output3.txt --depth 3
	#./antifraud ./paymo_input/batch_payment.txt ./paymo_input/stream_payment.txt ./paymo_output/output1.txt ./paymo_output/output2.txt ./paymo_output/output3.txt --debug --depth 3
	echo "All done" 
else 
	echo "Compilation failed"
fi	
