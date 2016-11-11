# Digital Wallet challenge (Insight Data Engineering)
A full description of this challenge is at https://github.com/InsightDataScience/digital-wallet.

I respond to the challenge in C++. For the finer details, feel free to dig into src/antifraud.cxx.

I read transactions from paymo_input/batch_payment.txt, a file that contains about 4 million trusted transactions. While reading each transaction, I map each user's ID to a set of his/her immediate friends. Call this level of friendship O(1). This operation is done once by a function called get_records(<inputs>), taking at most 14 seconds. I then pass this map, by reference, multiple times to assess O(1, 2 and 4) as the program runs. 

The program runs through transactions from paymo_input/stream_payment.txt, zipping through ~4 million of them in less than 4 minutes. For each transaction, I assess O(1) friendship between payer and payee in a function called friendSearch(<input>). To assess O(2) and O(4) friendships I successively hand over the transaction to friendTwoSearch(<input>) and friendThreeSearch(<input>) respectively.

While the most stubborn transaction in paymo_input/stream_payment.txt takes about 0.1 seconds to assess friendship at O(4) level, some transactions are processed as fast as in 0.3 micro-seconds!

I store output files in paymo_output/, where output1.txt->O(1), output2.txt->O(2) and output3.txt->O(4). 
