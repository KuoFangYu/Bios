.MODEL          small                    ;組譯並連結成PRINTBX.COM
.386
                                    ;使用386指令集，可以用「ROL BX,4」
code SEGMENT
    ASSUME CS:code
    Vendor  DW   0
    Device  DW   0
    SSS     DD   12348086h
    Value       DW      ?

start:        
    MOV  AX, WORD  PTR  SSS
    SHR  SSS,   16
    MOV  Vendor, AX

    MOV  AX, WORD  PTR  SSS
    MOV  Device, AX

    MOV     ES,     Vendor
    MOV     Value,  ES
    CALL    Print16b

    MOV     ES,     Device
    MOV     Value,  ES
    CALL    Print16b

    MOV     AX,         4C00h   ; 给AH设置参数4C00h
    INT     21h                 ; 调用4C00h号功能，结束程序


Print16b:       
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
                ret

code            ENDS
END             start

;data SEGMENT
;    hello  DB 'Hello World!$' ;注意要以$结束
;data ENDS
;code SEGMENT
;    ASSUME CS:CODE,DS:DATA
;start:
;    MOV AX,data  ;将data首地址赋值给AX                
;    MOV DS,AX    ;将AX赋值给DS,使DS指向data
;    LEA DX,hello ;使DX指向hello首地址
;    MOV AH,09h   ;给AH设置参数09H
;    INT 21h      ;执行AH中设置的09H号功能。输出DS指向的DX指向的字符串hello
;    MOV AX,4C00h ;给AH设置参数4C00h
;    int 21h      ;调用4C00h号功能，结束程序
;code ENDS
;END start