# Digital Wallet challenge (Insight Data Engineering)
A full description of this challenge is at https://github.com/InsightDataScience/digital-wallet.

I responded to this challenge in C++. See src/antifraud.cxx.

I read transactions from paymo_input/batch_payment.txt. While reading each transaction, I map each user's ID to a set of their immediate friends. Call this level of friendship O(1). This operation is done once by a function called get_records(<inputs>). This map is then passed (by reference) multiple times as the program runs. 

The program runs through transactions from paymo_input/stream_payment.txt, zipping through ~4M of them in less than 4 minutes. For each transaction, I assess O(1) friendship between payer and payee in a function called friendSearch(<input>). To assess O(2) and O(4) friendships the transaction is handed over to friendTwoSearch(<input>) and friendThreeSearch(<input>) respectively.

The slowest event in paymo_input/stream_payment.txt takes ~0.1 seconds to assess friendship at O(4) level. 

Output files are stored in paymo_output/, where output1.txt->O(1), output2.txt->O(2) and output3.txt->O(4). 

Please send concerns to lastdylan@gmail.com

