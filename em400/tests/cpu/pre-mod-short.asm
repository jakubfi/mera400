.program "cpu/pre-mod-short"

	md 100
	lwt r1, 1
	md -1
	lwt r2, 1
	md -16
	lwt r3, 0
	md -1024
	lwt r4, -63
	md 0xffff
	lwt r5, 1
	md -63
	lwt r6, -63

	hlt 077

.endprog

; XPCT int(rz(6)) : 0
; XPCT int(sr) : 0

; XPCT int(r1): 101
; XPCT int(r2): 0
; XPCT int(r3): -16
; XPCT int(r4): -1087
; XPCT int(r5): 0
; XPCT int(r6): -126

