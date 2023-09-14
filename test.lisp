(progn
  (print "the length of (1 2 3 4) is: ")
  (print (length (quote (1 2 3 4))))
  (terpri)
  (print "so the double is 4 + 4 = ")
  (print (+ 4 4))
  (terpri)
  (print "and 4 - 4 - 4 = ")
  (print (- 4 4 4))
  (terpri)
  (set (quote i) (cons 1 2))
  (print "the value of i is: ")
  (print i)
  (terpri)
  (print "the car of i is: ")
  (print (car i))
  (terpri)
  (print "the cdr of i is: ")
  (print (cdr i))
  (terpri)
  (rcar! i 3)
  (rcdr! i 4)
  (print "now the value of i is: ")
  (print i)
  (terpri)
  (print (memstats))
  (terpri))
