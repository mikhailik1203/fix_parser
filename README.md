# FIX Parser

Prototype of the low latency FIX parser. It supports messages, groups, blocks and plain tags. Library supports folloiwng types for FIX tags: Bool, Char, Int, Double, String, Date, Datetime, RawData.

Performance of the FIX Parser library:
 - Creation of the empty FIX message:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 160 | 168 | 182 | 190 | 643747

 - Serialization of the FIX message (plain) to the raw text (155 bytes):

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 402 | 411 | 432 | 451 | 12442

Example of the message:
8=FIX.4.49=13235=D49=BenchSenderId56=BenchTargetId34=77752=20221005 12:44:55.12345611=98765432154=138=10.044=100.055=MSFT18=640=259=110=183

 - Parse of the raw text (155 bytes) to the FIX message (plain):

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 619 | 632 | 971 | 1107 | 12926

 - Serialization of the FIX message (with 2 groups) to the raw text (180 bytes):

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
MDSubscription(V) | 501 | 511 | 537 | 564 | 12534

Example of the message:
8=FIX.4.49=15735=V49=BenchSenderId56=BenchTargetId34=77752=20221005 12:44:55.123456262=123456263=1264=0265=1267=3269=0269=1269=2146=355=MSFT55=NVDA55=AMZN10=024

 - Parse of the raw text (155 bytes) to the FIX message (with 2 groups) :

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 1463 | 1479 | 1536 | 1583 | 42186


ToDo:
 - FIX generator is required to generate definition for all FIX44 messages or other protocols
 - Implement mechanizm for adding other FIX protocols (FIXT, FIX50, etc)

