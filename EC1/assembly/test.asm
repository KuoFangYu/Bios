.MODEL          small
.386

data SEGMENT
    BusLp       WORD    0h
    DevLp       WORD    02h
    FuncLp      WORD    0h
data ENDS

code SEGMENT
    ASSUME  CS:code,   DS:data
    PciAddr DD  ?
Start:
    ; bus
    MOV     EAX,        80000000h
    MOVZX   EDX,        BusLp
    SHL     EDX,        16
    OR      EAX,        EDX

    ; dev
    MOVZX   EDX,        DevLp
    SHL     EDX,        11
    OR      EAX,        EDX

    ; func
    MOVZX   EDX,        FuncLp
    SHL     EDX,        12
    OR      EAX,        EDX

    MOV     EDX,        0

    ; Read Bus02:Dev01:Fun00:Offset[0Ch] to eax
    MOV     DX,         0CF8h
    out     DX,         EAX
    MOV     DX,         0CFCh
    in      EAX,        DX
    CALL    Print32b

    MOV     AX,         4C00h   ; 给AH设置参数4C00h
    INT     21h                 ; 调用4C00h号功能，结束程序


;
; print num function
;
Print32b:       MOV     EBX,    EAX        ;把此數存入BX
                MOV     CX,     8            ;BX內有四個位數，所以要印四次
                MOV     AH,     02h          ;DOS服務程式，AH＝2，印出字元
Next:           ROL     EBX,    4            ;旋轉四個位元，即十六進位的一個位數
                MOV     DL,     BL           ;保存BX，用DL運算
                AND     DL,     0fh          ;遮罩DL內較高的四個位元
                ADD     DL,     "0"          ;使DL之值變成阿拉伯數字的ASCII碼
                cmp     DL,     "9"          ;檢查是否為英文字母
                jbe     Print_it             ;如果不是，就跳到print_it
                add     DL,     7            ;如果是，加上7，使DL變成A～F
Print_it:       int     21h                  ;印出字元
                loop    Next                 ;是否已印出四個位數了，若否，跳到next
                ret



code ENDS
END Start