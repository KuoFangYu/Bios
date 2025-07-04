.MODEL          small
.386

data SEGMENT
    Field       DB    'BUS   DEV   FUN   VenderID   DeviceID$'
    Space       DB    '  $'
    LSpace      DB    '       $'
    ChangeLine  DB    10, 13, '$'
    BusLp       DW    0
    DevLp       DW    0
    FuncLp      DW    0
data ENDS

code SEGMENT
    ASSUME CS:code, DS:data
    Value   DW      ?       ;要在螢光幕上印出來的十六進位數
    Temp    DD      0
    Vendor  DW      0
    Device  DW      0
    VenDev  DD      0

Start:
    MOV     AX,     data    ;将data首地址赋值给AX                
    MOV     DS,     AX      ;将AX赋值给DS,使DS指向data
    LEA     DX,     Field   ;使DX指向Field首地址
    MOV     AH,     09h     ;给AH设置参数09H
    INT     21h

    CALL    change_line

Bus:
    CMP     BusLp,  2
    JGE     Done            ; >= 256 -> DONE
    MOV     DevLp,  0

Dev:
    CMP     DevLp,  32
    JGE     BusPlus         ; >= 32 -> BusPlus
    MOV     FuncLp, 0

Func:
    ; get address and get vender id, device id  
    CALL    GetAddr
    CMP     Vendor,     65535   ;ffff
    JZ      FuncPlus

    ; print bus num
    MOV     ES,     BusLp
    MOV     Value,  ES
    CALL    Print

    ; print space
    LEA     DX,     Space
    MOV     AH,     09h
    INT     21h

    ; print Dev num
    MOV     ES,     DevLp
    MOV     Value,  ES
    CALL    Print

    ; print space
    LEA     DX,     Space
    MOV     AH,     09h
    INT     21h

    ; print func num
    MOV     ES,     FuncLp
    MOV     Value,  ES
    CALL    Print

    ; print space
    LEA     DX,     Space
    MOV     AH,     09h
    INT     21h

    ; print Vendor num
    MOV     ES,     Vendor
    MOV     Value,  ES
    CALL    Print

    ; print space
    LEA     DX,     LSpace
    MOV     AH,     09h
    INT     21h

    ; print Device num
    MOV     ES,     Device
    MOV     Value,  ES
    CALL    Print

    CALL    change_line

FuncPlus:
    INC     FuncLp      ; FuncLp++
    CMP     FuncLp, 8   
    JGE     DevPlus     ; >= 8 -> DevPlus
    JMP     Func

DevPlus:
    INC     DevLp       ; DevLp++
    JMP     Dev

BusPlus:
    INC     BusLp       ; BusLp++
    JMP     Bus

Done:
    MOV     AX,     4C00h  ; 给AH设置参数4C00h
    INT     21h            ; 调用4C00h号功能，结束程序

;
; Print function
;
Print:       
    MOV     BX,     Value        ;把此數存入BX
    MOV     CX,     4            ;BX內有四個位數，所以要印四次
    MOV     AH,     02h          ;DOS服務程式，AH＝2，印出字元
Next:           
    ROL     BX,     4            ;旋轉四個位元，即十六進位的一個位數
    MOV     DL,     BL           ;保存BX，用DL運算
    AND     DL,     0fh          ;遮罩DL內較高的四個位元
    ADD     DL,     "0"          ;使DL之值變成阿拉伯數字的ASCII碼
    cmp     DL,     "9"          ;檢查是否為英文字母
    jbe     Print_it             ;如果不是，就跳到print_it
    add     DL,     7            ;如果是，加上7，使DL變成A～F
Print_it:       
    int     21h                  ;印出字元
    loop    Next                 ;是否已印出四個位數了，若否，跳到next
    RET


GetAddr:
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
    SHL     EDX,        8
    OR      EAX,        EDX

    MOV     EDX,        0

    ; Read Bus02:Dev01:Fun00:Offset[0Ch] to eax
    MOV     DX,         0CF8h
    OUT     DX,         EAX
    MOV     DX,         0CFCh
    IN      EAX,        DX
    MOV     VenDev,     EAX

    ; devide to vender and device
    MOV  AX, WORD  PTR  VenDev
    SHR  VenDev,    16
    MOV  Vendor,    AX

    MOV  AX, WORD  PTR  VenDev
    MOV  Device,    AX
    RET

change_line:
    LEA     DX,     ChangeLine  
    MOV     AH,     09h
    INT     21h
    RET


code ENDS
END Start