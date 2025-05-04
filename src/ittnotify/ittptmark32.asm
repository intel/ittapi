COMMENT @
  Copyright (C) 2017-2025 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
@

;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; ;;; Intel Processor Trace Marker Functionality
;;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.686p
.xmm
.model  FLAT

_TEXT	segment

        public	___itt_pt_mark
        public	___itt_pt_event
        public	___itt_pt_mark_event
        public  ___itt_pt_mark_threshold
        public  ___itt_pt_write
        public  ___itt_pt_byte

        align   10h

;;; void __itt_pt_mark(unsigned char index);

___itt_pt_mark  proc    near

        movzx   eax,byte ptr [esp + 4]
        lea     eax,__itt_pt_mark_call_table[eax * 4]
        jmp	eax

        align   04h

        dd      0, 1, 2, 3      ;;; GUID

        dd      0fadefadeh      ;;; magic marker

__itt_pt_mark_call_table:

        dd      256 dup(0000c2c3h OR (( $ - offset __itt_pt_mark_call_table) SHL 14))

___itt_pt_mark  endp


___itt_pt_byte   proc   near

        mov      ecx,[esp + 4]

___itt_pt_byte_::

        and     ecx,0ffh
        lea     ecx,__itt_pt_byte_call_table[ecx]
        jmp	ecx

        align   04h

        dd      0, 1, 2, 3      ;;; GUID

        dd      0fadedeafh      ;;; magic marker

__itt_pt_byte_call_table:

        db      256 dup(0c3h)

___itt_pt_byte   endp

        align   10h

___itt_pt_event  proc   near

        push    ecx
        mov     ecx,[esp + 8]
        rdpmc

        mov     cl,al
        call    ___itt_pt_byte_
        mov     cl,ah
        call    ___itt_pt_byte_
        shr     eax,16
        mov     cl,al
        call    ___itt_pt_byte_
        mov     cl,ah
        call    ___itt_pt_byte_

        mov     cl,dl
        call    ___itt_pt_byte_
        mov     cl,dh
        call    ___itt_pt_byte_
        shr     edx,16
        mov     cl,dl
        call    ___itt_pt_byte_
        mov     cl,dh
        call    ___itt_pt_byte_

        pop     ecx
        ret

___itt_pt_event  endp

        align   10h

___itt_pt_mark_event    proc    near

        test    byte ptr [esp + 4],1
        jnz     odd
        push    0
        call    ___itt_pt_event
        add     esp,4
        jmp     ___itt_pt_mark

odd:
        push    dword ptr [esp + 4]
        call    ___itt_pt_mark
        add     esp,4
        mov     dword ptr [esp + 4],0
        jmp     ___itt_pt_event

___itt_pt_mark_event    endp

        align   10h

___itt_pt_flush proc    near

        lea     eax,offset __itt_pt_mark_flush_1
        jmp     eax

        align   10h
        nop
__itt_pt_mark_flush_1:
        lea     eax,offset __itt_pt_mark_flush_2
        jmp     eax

        align   10h
        nop
        nop
__itt_pt_mark_flush_2:
        lea     eax,offset __itt_pt_mark_flush_3
        jmp     eax

        align   10h
        nop
        nop
        nop
__itt_pt_mark_flush_3:
        ret

___itt_pt_flush endp

        align   10h

;;; int __itt_pt_mark_threshold(unsigned char index, unsigned long long* tmp, int threshold);

___itt_pt_mark_threshold        proc    near
        test    byte ptr [esp + 4],1    ;;; index
        jnz     mark_end
mark_begin:
        mov     ecx,(1 SHL 30) + 1
        rdpmc
        mov     ecx,[esp + 8]   ;;; tmp
        mov     [ecx + 0],eax
        mov     [ecx + 4],edx
        jmp     ___itt_pt_mark
mark_end:
        mov     ecx,(1 SHL 30) + 1
        rdpmc
        mov     ecx,[esp + 8]   ;;; tmp
        sub     eax,[ecx + 0]
        sbb     edx,[ecx + 4]
        or      edx,edx
        jnz     found
        cmp     edx,[esp + 12]  ;;; threshold
        jnc     found
        jmp     ___itt_pt_mark
found:
        call    ___itt_pt_mark
        jmp     ___itt_pt_flush

___itt_pt_mark_threshold        endp

;;; PTWRITE

        align   10h

;;; void __itt_pt_write(unsigned long long value);

        dd      0, 1, 2, 3      ;;; GUID

___itt_pt_write proc

;;;        ptwrite dword ptr [esp + 4]
        db      0F3h, 0Fh, 0AEh, 64h, 24h, 04h
        ret

___itt_pt_write endp

;;;

_TEXT	ends
        end
