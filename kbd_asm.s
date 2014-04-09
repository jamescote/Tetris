					xdef	_kybd_isr
					xref	_do_kybd_isr
					
					
;; ISR function to set into vector #70

_kybd_isr:			movem.l	d0-2/a0-2,-(sp)
					jsr		_do_kybd_isr
					movem.l	(sp)+,d0-2/a0-2
					rte