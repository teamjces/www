' This work is released to the public domain under CC0 1.0
' https://creativecommons.org/publicdomain/zero/1.0/

10 A=INT(RND(0)*100)+1
20 INPUT "アナタノヨソウハ？",X
30 IF X<A THEN PRINT "チイサスギマス！": GOTO 20
40 IF X>A THEN PRINT "オオキスギマス！": GOTO 20
50 PRINT "アタリ！！"
