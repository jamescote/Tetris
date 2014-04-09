		xdef	_vblank
		xref	_do_vbl
		xdef	_get_vid
		xdef	_set_vid
			
; Constants:
pVBBaseHi	equ		$FFFF8201
pVBBaseLo	equ		$FFFF8203

;; get_video_base:
;; Returns the address of the original frame buffer.

_get_vid:	link	a6,#0
			movem.l d1-d7/a0-a5,-(sp)
			move.l	#$1,-(sp)			; Check to see if we're in sp mode
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
			move.l	d0,d6
			bne		gvParse
			clr.l	-(sp)				; switch to sp if not in it already.
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
			move.l	d0,d6
gvParse:	movea.l	#pVBBaseHi,a0
			movea.l	#pVBBaseLo,a1
			clr.l	d0
			move.b	(a0),d0
			asl.l	#8,d0
			move.b	(a1),d0
			asl.l	#8,d0
			move.l	d0,d5
			cmp.l	#-1,d6
			beq		gvExit
			move.l	d6,-(sp)			; set the system back to user mode.
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
			move	d5,d0
gvExit:		movem.l	(sp)+,d1-7/a0-5
			unlk	a6
			rts
		
;; set_video_base( UINT16* )
; sets the video base register on the mmu to a specified
; address.  Assuming the address is 256 byte aligned.
		
svNewVB		equ		8
		
_set_vid:	link	a6,#0
			movem.l d0-d7/a0-a5,-(sp)
			move.l	#$1,-(sp)			; Check to see if we're in sp mode
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
			move.l	d0,d6
			bne		svParse
			clr.l	-(sp)				; switch to sp if not in it already.
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
			move.l	d0,d6
svParse:	movea.l	#pVBBaseHi,a0
			move.l	svNewVB(a6),d0
			lsr.l	#8,d0
			movep.w	d0,0(a0)
			cmp.l	#-1,d6
			beq		svExit
			move.l	d6,-(sp)			; set the system back to user mode.
			move.w	#32,-(sp)
			trap	#1
			addq	#6,sp
svExit:		movem.l	(sp)+,d0-d7/a0-a5
			unlk	a6
			rts
			

;; New Vector for vertical blank
;; calls the do_vbl subroutine from tet_main.c	
_vblank:		movem.l	d0-2/a0-2,-(sp)
				jsr _do_vbl
				movem.l	(sp)+,d0-2/a0-2
				rte
