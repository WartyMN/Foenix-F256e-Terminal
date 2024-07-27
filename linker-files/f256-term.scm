(define memories
  '((memory LoMem (address (#x0000 . #xcfff)) (type ANY))
    (memory Vector (address (#xffe4 . #xffff)))
    (memory Banks (address (#x10000 . #x7dfff)) (type ANY))

    (memory buffers (address (#x7e000 . #x7ffff))
	    (section (uart_buffer #x7e000) ))

    (memory palettes (address (#xf03000 . #xf03fff))
	    (section (palette0 #xf03000) (palette1 #xf03400))
	    (section (palette2 #xf03800) (palette3 #xf03c00)))
    ))
