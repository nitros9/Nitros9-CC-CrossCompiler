 psect atof_c,0,0,0,0,0
 nam atof_c
 ttl atof
atof: pshs u
 ldu 4,s
 leas -19,s
 bra _2
_3 leau 1,u
_2 ldb 0,u
 cmpb #32
 beq _3
_4 ldb 0,u
 cmpb #9
 lbeq _3
_1 ldb 0,u
 cmpb #45
 bne _6
_5 ldd #1
 bra _$1
_6 clra
 clrb
_$1 std 7,s
_7 ldb 0,u
 cmpb #45
 beq _8
_10 ldb 0,u
 cmpb #43
 bne _9
_8 leau 1,u
_9 leax 11,s
 pshs x
 bsr _11
 fdb 0,0,0,0
_11 puls x
 lbsr _dmove
 bra _13
_14 ldb 2,s
 sex
 pshs d
 leax 13,s
 pshs x
 lbsr L0178
 leas 4,s
 leau 1,u
_13 ldb 0,u
 stb 2,s
 sex
 leax _chcodes,y
 leax d,x
 ldb 0,x
 clra
 andb #8
 bne _14
_12 clra
 clrb
 std 3,s
 ldb 0,u
 cmpb #46
 bne _16
_15 leau 1,u
 bra _18
_19 ldb 2,s
 sex
 pshs d
 leax 13,s
 pshs x
 lbsr L0178
 leas 4,s
 leau 1,u
 ldd 3,s
 addd #1
 std 3,s
_18 ldb 0,u
 stb 2,s
 sex
 leax _chcodes,y
 leax d,x
 ldb 0,x
 clra
 andb #8
 bne _19
_16
_17 leax 11,s
 stx 0,s
 ldd #184
 ldx 0,s
 stb 7,x
 leax 11,s
 pshs x
 leax 13,s
 pshs x
 lbsr _dnorm
 leas 2,s
 lbsr _dmove
 ldb 0,u
 stb 2,s
 cmpb #101
 beq _20
_22 ldb 2,s
 cmpb #69
 lbne _21
_20 ldd #1
 std 5,s
 leau 1,u
 ldb 0,u
 cmpb #43
 bne _24
_23 leau 1,u
 bra _25
_24 ldb 0,u
 cmpb #45
 bne _27
_26 leau 1,u
 clra
 clrb
 std 5,s
_25
_27 clra
 clrb
 bra _$2
_30 ldd 9,s
 pshs d
 ldd #10
 lbsr ccmult
 pshs d
 ldb 4,s
 sex
 addd ,s++
 addd #-48
_$2 std 9,s
_29 ldb ,u+
 stb 2,s
 sex
 leax _chcodes,y
 leax d,x
 ldb 0,x
 clra
 andb #8
 bne _30
_28 ldd 3,s
 pshs d
 ldd 7,s
 beq _31
_32 ldd 11,s
 nega
 negb
 sbca #0
 bra _33
_31 ldd 11,s
_33 addd ,s++
 std 3,s
_21 ldd 3,s
 bge _35
_34 ldd 3,s
 nega
 negb
 sbca #0
 std 3,s
 ldd #1
 bra _$3
_35 clra
 clrb
_$3 std 5,s
 leax 11,s
 pshs x
 ldd 7,s
 pshs d
 ldd 7,s
 pshs d
 leax 17,s
 lbsr _dstack
 lbsr scale
 leas 12,s
 lbsr _dmove
_36 ldd 7,s
 beq _38
_37 leax _flacc,y
 pshs x
 leax 13,s
 lbsr _dneg
 bra _$4
_38 leax _flacc,y
 pshs x
 leax 13,s
_$4 lbsr _dmove
_39 leas 19,s
 puls u,pc
L0178 ldx 2,s 
 leas -8,s 
 ldd 5,x 
 lslb
 rola
 std 5,x 
 std 5,s 
 ldd 3,x 
 rolb
 rola
 std 3,x 
 std 3,s 
 ldd 1,x 
 rolb
 rola
 std 1,x 
 std 1,s 
 lda ,x 
 rola
 sta ,x 
 sta ,s 
 asl 6,x 
 rol 5,x 
 rol 4,x 
 rol 3,x 
 rol 2,x 
 rol 1,x 
 rol ,x 
 lblo L0205
 asl 6,x 
 rol 5,x 
 rol 4,x 
 rol 3,x 
 rol 2,x 
 rol 1,x 
 rol ,x 
 lblo L0205
 ldd 5,x 
 addd 5,s 
 std 5,x 
 ldd 3,x 
 adcb 4,s 
 adca 3,s 
 std 3,x 
 ldd 1,x 
 adcb 2,s 
 adca 1,s 
 std 1,x 
 ldb ,x 
 adcb ,s 
 stb ,x 
 bcs L0205
 ldb 13,s 
 andb #$0f 
 clra
 addd 5,x 
 std 5,x 
 ldd #0 
 adcb 4,x 
 adca 3,x 
 std 3,x 
 ldd #0 
 adcb 2,x 
 adca 1,x 
 std 1,x 
 lda #0 
 adca ,x 
 sta ,x 
 bcs L0205
 leas 8,s 
 clra
 clrb
 rts
L0205 ldd #1 
 leas 8,s 
 rts
 ends
 endsect
