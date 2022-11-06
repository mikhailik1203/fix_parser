# FIX Parser

Prototype of the low latency FIX parser. It supports messages, groups, blocks and plain tags. Library supports folloiwng types for FIX tags: Bool, Char, Int, Double, String, Date, Datetime, RawData.

Performance of the FIX Parser library:
 - Creation of the empty FIX message, nsec:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 162 | 169 | 178 | 179 | 1034348

 - Serialization of the FIX message (plain) to the raw text (155 bytes), nsec:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 287 | 295 | 310 | 319 | 9667

Example of the message:
8=FIX.4.49=13235=D49=BenchSenderId56=BenchTargetId34=77752=20221005 12:44:55.12345611=98765432154=138=10.044=100.055=MSFT18=640=259=110=183

 - Parse of the raw text (155 bytes) to the FIX message (plain), nsec:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 653 | 665 | 690 | 718 | 12629

 - Serialization of the FIX message (with 2 groups) to the raw text (180 bytes), nsec:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
MDSubscription(V) | 375 | 385 | 398 | 410 | 12020

Example of the message:
8=FIX.4.49=15735=V49=BenchSenderId56=BenchTargetId34=77752=20221005 12:44:55.123456262=123456263=1264=0265=1267=3269=0269=1269=2146=355=MSFT55=NVDA55=AMZN10=024

 - Parse of the raw text (155 bytes) to the FIX message (with 2 groups), nsec:

 Message | min | 50% | 95% | 99% | max
--- | --- | --- | --- |--- |--- 
NewOrderSingle(D) | 1408 | 1465 | 1543 | 1605 | 22476


ToDo:
 - FIX generator is required to generate definition for all FIX44 messages or other protocols
 - Implement mechanizm for adding other FIX protocols (FIXT, FIX50, etc)

