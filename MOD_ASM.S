			xdef		_rot_R
			xdef		_rot_L
; [TO DO] delete this example function

;; TETRIS CONSTANTS
mapArrySz		equ		3

;; Register Map: 
; a0 -- oldMap to Parse
; a1 -- newMap to read into
; d0 -- index for oldMap
; d1 -- index for newMap
; d2 -- manipulating words in the old map
; d3 -- setting words in the new map

newMap			equ		8
oldMap			equ		12

_rot_R:		link		a6,#0
			movem.l		d0-d7/a0-a5,-(sp)
			movea.l		oldMap(a6),a0
			movea.l		newMap(a6),a1
			move.w		#mapArrySz,d0
			move.w		#mapArrySz,d1
			addq.l		#6,a1
			addq.l		#6,a0
			move.w		(a1),d3
rrStart		move.w		(a0),d2	
			lsl.w		#1,d3
			lsr.w		#1,d2
			bcc		skipRPush
			addq.w		#1,d3
skipRPush:		move.w		d2,(a0)
			subq.l		#2,a0
			dbra		d0,rrStart
			move.w		#mapArrySz,d0
			movea.l		oldMap(a6),a0
			addq.l		#6,a0
			move.w		d3,(a1)
			move.w		-(a1),d3
			dbra		d1,rrStart
			movem.l		(sp)+,d0-d7/a0-a5
			unlk		a6
			rts

			
;; Register Map: 
; a0 -- oldMap to Parse
; a1 -- newMap to read into
; d0 -- index for oldMap
; d1 -- index for newMap
; d2 -- manipulating words in the old map
; d3 -- setting words in the new map

_rot_L:		link		a6,#0
			movem.l		d0-d7/a0-a5,-(sp)
			movea.l		oldMap(a6),a0
			movea.l		newMap(a6),a1
			move.w		#mapArrySz,d0
			move.w		#mapArrySz,d1
			move.w		(a1),d3
rlStart		move.w		(a0),d2	
			lsl.w		#1,d3
			lsr.w		#1,d2
			bcc			skipLPush
			addq.w		#1,d3
skipLPush:		move.w		d2,(a0)
			addq.l		#2,a0
			dbra		d0,rlStart
			move.w		#mapArrySz,d0
			movea.l		oldMap(a6),a0
			move.w		d3,(a1)
			addq.l		#2,a1
			move.w		(a1),d3
			dbra		d1,rlStart
			movem.l		(sp)+,d0-d7/a0-a5
			unlk		a6
			rts

; recall: cc68x assumes d0-2/a0-2 are *not* preserved across subroutine calls!
