
	.include awp-fp.inc

operation:
	lf r7+ARG1
	nrf
	uj r5

tests:
	.word 0, ?Z, 0
	.float 0
	.float 0
	.float 0
fin:

; XPCT sr : 0
; XPCT ir&0x3f : 0o77