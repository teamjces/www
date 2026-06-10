10 A=INT(RND(0)*100)+1
20 INPUT "アナタノヨソウハ？",X
30 IF X<A THEN PRINT "チイサスギマス！": GOTO 20
40 IF X>A THEN PRINT "オオキスギマス！": GOTO 20
50 PRINT "アタリ！！"
