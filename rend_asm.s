			xdef _WToStr

;; Register Map:
; a0 -- Stores return string
; d3 -- Stores value to convert
; d4 -- Stores max length of String

;; Func Constants:
wVal			equ		12
retStr			equ		8
wDvsr			equ		10
bZeroCh			equ		'0'
wMxLngth		equ		$FF

_WToStr:		link		a6,#0
				movem.l		d0-d7/a0-a5,-(sp)
				clr.l		d3
				movea.l		retStr(a6),a1
				lea			wLclBffr,a0
				move.w		wVal(a6),d3
				move.b		#wMxLngth,d4
				subq.b		#1,d4
				clr.b		(a0)+
ws_Start:		divu		#wDvsr,d3
				swap		d3
				move.b		d3,d0
				clr.w		d3
				swap		d3
				add.b		#bZeroCh,d0
				move.b		d0,(a0)+
				tst.w		d3
				beq			ws_Fix
				dbra		d4,ws_Start
ws_Fix:			move.b		-(a0),(a1)+
				beq			ws_Done
				bra			ws_Fix
ws_Done:		movem.l		(sp)+,d0-d7/a0-a5
				unlk		a6
				rts
				
wLclBffr		ds.b		wMxLngth				


