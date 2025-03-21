;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (Linux)
;--------------------------------------------------------
; PIC port for the 14-bit core
;--------------------------------------------------------
;	.file	"dice_src_code.c"
	list	p=12f683
	radix dec
	include "p12f683.inc"
;--------------------------------------------------------
; external declarations
;--------------------------------------------------------
	extern	_TRISIO
	extern	_GPIO
	extern	_WDTCONbits
	extern	_GPIObits
	extern	__sdcc_gsinit_startup
;--------------------------------------------------------
; global declarations
;--------------------------------------------------------
	global	_main
	global	_numero_aleatorio
	global	_mostrar_numero
	global	_delay

	global PSAVE
	global SSAVE
	global WSAVE
	global STK12
	global STK11
	global STK10
	global STK09
	global STK08
	global STK07
	global STK06
	global STK05
	global STK04
	global STK03
	global STK02
	global STK01
	global STK00

sharebank udata_ovr 0x0070
PSAVE	res 1
SSAVE	res 1
WSAVE	res 1
STK12	res 1
STK11	res 1
STK10	res 1
STK09	res 1
STK08	res 1
STK07	res 1
STK06	res 1
STK05	res 1
STK04	res 1
STK03	res 1
STK02	res 1
STK01	res 1
STK00	res 1

;--------------------------------------------------------
; global definitions
;--------------------------------------------------------
UD_dice_src_code_0	udata
_reg_num_gen	res	20

UD_dice_src_code_1	udata
_i	res	2

;--------------------------------------------------------
; absolute symbol definitions
;--------------------------------------------------------
;--------------------------------------------------------
; compiler-defined variables
;--------------------------------------------------------
UDL_dice_src_code_0	udata
r0x1010	res	1
r0x1011	res	1
r0x1012	res	1
r0x1013	res	1
r0x1014	res	1
r0x1015	res	1
r0x100C	res	1
r0x100E	res	1
r0x100F	res	1
r0x1005	res	1
r0x1004	res	1
r0x1006	res	1
r0x1007	res	1
r0x1008	res	1
r0x1009	res	1
r0x100A	res	1
r0x100B	res	1
;--------------------------------------------------------
; initialized data
;--------------------------------------------------------

IDD_dice_src_code_0	idata
_click_dtc
	db	0x00, 0x00	;  0


IDD_dice_src_code_1	idata
_indice
	db	0x00, 0x00	;  0


IDD_dice_src_code_2	idata
_num_inv
	db	0x00, 0x00	;  0


IDD_dice_src_code_3	idata
_counter
	db	0x01, 0x00	;  1


IDD_dice_src_code_4	idata
_time
	db	0x96, 0x00	; 150


IDD_dice_src_code_5	idata
_numero_aleatorio
	db	0x01, 0x00	;  1

;--------------------------------------------------------
; initialized absolute data
;--------------------------------------------------------
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
;	udata_ovr
;--------------------------------------------------------
; reset vector 
;--------------------------------------------------------
STARTUP	code 0x0000
	nop
	pagesel __sdcc_gsinit_startup
	goto	__sdcc_gsinit_startup
;--------------------------------------------------------
; code
;--------------------------------------------------------
code_dice_src_code	code
;***
;  pBlock Stats: dbName = M
;***
;has an exit
;6 compiler assigned registers:
;   r0x1010
;   r0x1011
;   r0x1012
;   r0x1013
;   r0x1014
;   r0x1015
;; Starting pCode block
S_dice_src_code__main	code
_main:
; 2 exit points
;	.line	21; "dice_src_code.c"	TRISIO = 0b00100000; //Poner todos los pines como salidas expecto P5
	MOVLW	0x20
	BANKSEL	_TRISIO
	MOVWF	_TRISIO
;	.line	22; "dice_src_code.c"	GPIO=0b000000;       //Apagar todas las salidas
	BANKSEL	_GPIO
	CLRF	_GPIO
;	.line	23; "dice_src_code.c"	WDTPS0=0b1;          //Configurar la cantidad de ciclos de espera del WD timer
	BSF	_WDTCONbits,1
;	.line	24; "dice_src_code.c"	WDTPS1=0b1;
	BSF	_WDTCONbits,2
;	.line	25; "dice_src_code.c"	WDTPS2=0b0;
	BCF	_WDTCONbits,3
;	.line	26; "dice_src_code.c"	WDTPS3=0b1;
	BSF	_WDTCONbits,4
_00107_DS_:
;	.line	33; "dice_src_code.c"	while (click_dtc == 0){
	BANKSEL	_click_dtc
	MOVF	(_click_dtc + 1),W
	IORWF	_click_dtc,W
	BTFSS	STATUS,2
	GOTO	_00109_DS_
;	.line	37; "dice_src_code.c"	counter++;
	BANKSEL	_counter
	INCF	_counter,F
	BTFSC	STATUS,2
	INCF	(_counter + 1),F
;	.line	38; "dice_src_code.c"	if (counter==7){
	MOVF	_counter,W
	BANKSEL	r0x1010
	MOVWF	r0x1010
	BANKSEL	_counter
	MOVF	(_counter + 1),W
	BANKSEL	r0x1011
	MOVWF	r0x1011
	MOVF	r0x1010,W
	XORLW	0x07
	BTFSS	STATUS,2
	GOTO	_00106_DS_
	MOVF	r0x1011,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00106_DS_
;	.line	39; "dice_src_code.c"	counter=1;
	MOVLW	0x01
	BANKSEL	_counter
	MOVWF	_counter
	CLRF	(_counter + 1)
_00106_DS_:
;	.line	41; "dice_src_code.c"	click_dtc = GP5;
	BANKSEL	r0x1010
	CLRF	r0x1010
	BANKSEL	_GPIObits
	BTFSS	_GPIObits,5
	GOTO	_00001_DS_
	BANKSEL	r0x1010
	INCF	r0x1010,F
_00001_DS_:
	BANKSEL	r0x1010
	MOVF	r0x1010,W
	BANKSEL	_click_dtc
	MOVWF	_click_dtc
	CLRF	(_click_dtc + 1)
	GOTO	_00107_DS_
;;signed compare: left < lit(0x1=1), size=2, mask=ffff
_00109_DS_:
;	.line	45; "dice_src_code.c"	switch (counter){
	BANKSEL	_counter
	MOVF	(_counter + 1),W
	ADDLW	0x80
	ADDLW	0x80
	BTFSS	STATUS,2
	GOTO	_00173_DS_
	MOVLW	0x01
	SUBWF	_counter,W
_00173_DS_:
	BTFSS	STATUS,0
	GOTO	_00116_DS_
;;genSkipc:3307: created from rifx:0x7ffef3df2820
;;swapping arguments (AOP_TYPEs 1/3)
;;signed compare: left >= lit(0x7=7), size=2, mask=ffff
	BANKSEL	_counter
	MOVF	(_counter + 1),W
	ADDLW	0x80
	ADDLW	0x80
	BTFSS	STATUS,2
	GOTO	_00174_DS_
	MOVLW	0x07
	SUBWF	_counter,W
_00174_DS_:
	BTFSC	STATUS,0
	GOTO	_00116_DS_
;;genSkipc:3307: created from rifx:0x7ffef3df2820
	BANKSEL	_counter
	DECF	_counter,W
	BANKSEL	r0x1010
	MOVWF	r0x1010
	MOVLW	HIGH(_00175_DS_)
	BANKSEL	PCLATH
	MOVWF	PCLATH
	MOVLW	_00175_DS_
	BANKSEL	r0x1010
	ADDWF	r0x1010,W
	BTFSS	STATUS,0
	GOTO	_00002_DS_
	BANKSEL	PCLATH
	INCF	PCLATH,F
_00002_DS_:
	MOVWF	PCL
_00175_DS_:
	GOTO	_00110_DS_
	GOTO	_00111_DS_
	GOTO	_00112_DS_
	GOTO	_00113_DS_
	GOTO	_00114_DS_
	GOTO	_00115_DS_
_00110_DS_:
;	.line	46; "dice_src_code.c"	case 1: GPIO = 0b000001; break;
	MOVLW	0x01
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00111_DS_:
;	.line	47; "dice_src_code.c"	case 2: GPIO = 0b000010; break;
	MOVLW	0x02
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00112_DS_:
;	.line	48; "dice_src_code.c"	case 3: GPIO = 0b000011; break;
	MOVLW	0x03
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00113_DS_:
;	.line	49; "dice_src_code.c"	case 4: GPIO = 0b000100; break;
	MOVLW	0x04
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00114_DS_:
;	.line	50; "dice_src_code.c"	case 5: GPIO = 0b000101; break; 
	MOVLW	0x05
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00115_DS_:
;	.line	51; "dice_src_code.c"	case 6: GPIO = 0b000110; break;
	MOVLW	0x06
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00140_DS_
_00116_DS_:
;	.line	52; "dice_src_code.c"	default: GPIO = 0b000000;
	BANKSEL	_GPIO
	CLRF	_GPIO
_00140_DS_:
;	.line	56; "dice_src_code.c"	for(i=0;i<1000;i++){
	BANKSEL	r0x1010
	CLRF	r0x1010
	CLRF	r0x1011
_00128_DS_:
;	.line	57; "dice_src_code.c"	for(j=0;j<1275;j++){
	MOVLW	0xfb
	BANKSEL	r0x1012
	MOVWF	r0x1012
	MOVLW	0x04
	MOVWF	r0x1013
_00127_DS_:
;	.line	58; "dice_src_code.c"	counter++;
	BANKSEL	_counter
	INCF	_counter,F
	BTFSC	STATUS,2
	INCF	(_counter + 1),F
;	.line	59; "dice_src_code.c"	if (counter==7){
	MOVF	_counter,W
	BANKSEL	r0x1014
	MOVWF	r0x1014
	BANKSEL	_counter
	MOVF	(_counter + 1),W
	BANKSEL	r0x1015
	MOVWF	r0x1015
	MOVF	r0x1014,W
	XORLW	0x07
	BTFSS	STATUS,2
	GOTO	_00119_DS_
	MOVF	r0x1015,W
	XORLW	0x00
	BTFSS	STATUS,2
	GOTO	_00119_DS_
;	.line	60; "dice_src_code.c"	counter=1;
	MOVLW	0x01
	BANKSEL	_counter
	MOVWF	_counter
	CLRF	(_counter + 1)
_00119_DS_:
	MOVLW	0xff
	BANKSEL	r0x1012
	ADDWF	r0x1012,W
	MOVWF	r0x1014
	MOVLW	0xff
	MOVWF	r0x1015
	MOVF	r0x1013,W
	BTFSC	STATUS,0
	INCFSZ	r0x1013,W
	ADDWF	r0x1015,F
	MOVF	r0x1014,W
	MOVWF	r0x1012
	MOVF	r0x1015,W
	MOVWF	r0x1013
;	.line	57; "dice_src_code.c"	for(j=0;j<1275;j++){
	MOVF	r0x1015,W
	IORWF	r0x1014,W
	BTFSS	STATUS,2
	GOTO	_00127_DS_
;	.line	56; "dice_src_code.c"	for(i=0;i<1000;i++){
	INCF	r0x1010,F
	BTFSC	STATUS,2
	INCF	r0x1011,F
;;unsigned compare: left < lit(0x3E8=1000), size=2
	MOVLW	0x03
	SUBWF	r0x1011,W
	BTFSS	STATUS,2
	GOTO	_00176_DS_
	MOVLW	0xe8
	SUBWF	r0x1010,W
_00176_DS_:
	BTFSS	STATUS,0
	GOTO	_00128_DS_
;;genSkipc:3307: created from rifx:0x7ffef3df2820
;	.line	65; "dice_src_code.c"	GPIO = 0b000000;       //Apagar todas las salidas
	BANKSEL	_GPIO
	CLRF	_GPIO
;	.line	66; "dice_src_code.c"	click_dtc = 0;
	BANKSEL	_click_dtc
	CLRF	_click_dtc
	CLRF	(_click_dtc + 1)
	GOTO	_00107_DS_
;	.line	151; "dice_src_code.c"	}
	RETURN	
; exit point of _main

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;9 compiler assigned registers:
;   r0x1004
;   STK00
;   r0x1005
;   r0x1006
;   r0x1007
;   r0x1008
;   r0x1009
;   r0x100A
;   r0x100B
;; Starting pCode block
S_dice_src_code__delay	code
_delay:
; 2 exit points
;	.line	170; "dice_src_code.c"	void delay(unsigned int tiempo)
	BANKSEL	r0x1004
	MOVWF	r0x1004
	MOVF	STK00,W
	MOVWF	r0x1005
;	.line	175; "dice_src_code.c"	for(i=0;i<tiempo;i++)
	CLRF	r0x1006
	CLRF	r0x1007
_00261_DS_:
	BANKSEL	r0x1004
	MOVF	r0x1004,W
	SUBWF	r0x1007,W
	BTFSS	STATUS,2
	GOTO	_00282_DS_
	MOVF	r0x1005,W
	SUBWF	r0x1006,W
_00282_DS_:
	BTFSC	STATUS,0
	GOTO	_00263_DS_
;;genSkipc:3307: created from rifx:0x7ffef3df2820
;	.line	176; "dice_src_code.c"	for(j=0;j<1275;j++);
	MOVLW	0xfb
	BANKSEL	r0x1008
	MOVWF	r0x1008
	MOVLW	0x04
	MOVWF	r0x1009
_00259_DS_:
	MOVLW	0xff
	BANKSEL	r0x1008
	ADDWF	r0x1008,W
	MOVWF	r0x100A
	MOVLW	0xff
	MOVWF	r0x100B
	MOVF	r0x1009,W
	BTFSC	STATUS,0
	INCFSZ	r0x1009,W
	ADDWF	r0x100B,F
	MOVF	r0x100A,W
	MOVWF	r0x1008
	MOVF	r0x100B,W
	MOVWF	r0x1009
	MOVF	r0x100B,W
	IORWF	r0x100A,W
	BTFSS	STATUS,2
	GOTO	_00259_DS_
;	.line	175; "dice_src_code.c"	for(i=0;i<tiempo;i++)
	INCF	r0x1006,F
	BTFSC	STATUS,2
	INCF	r0x1007,F
	GOTO	_00261_DS_
_00263_DS_:
;	.line	177; "dice_src_code.c"	}
	RETURN	
; exit point of _delay

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;5 compiler assigned registers:
;   r0x100C
;   STK00
;   r0x100D
;   r0x100E
;   r0x100F
;; Starting pCode block
S_dice_src_code__mostrar_numero	code
_mostrar_numero:
; 2 exit points
;	.line	153; "dice_src_code.c"	void mostrar_numero(short int num){
	BANKSEL	r0x100C
	MOVWF	r0x100C
	MOVF	STK00,W
;;1	MOVWF	r0x100D
;	.line	154; "dice_src_code.c"	switch (num){
	MOVWF	r0x100E
	MOVF	r0x100C,W
	MOVWF	r0x100F
	IORWF	r0x100E,W
	BTFSC	STATUS,2
	GOTO	_00181_DS_
	MOVF	r0x100E,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00241_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00182_DS_
_00241_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x02
	BTFSS	STATUS,2
	GOTO	_00242_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00183_DS_
_00242_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x03
	BTFSS	STATUS,2
	GOTO	_00243_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00184_DS_
_00243_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x04
	BTFSS	STATUS,2
	GOTO	_00244_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00185_DS_
_00244_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x05
	BTFSS	STATUS,2
	GOTO	_00245_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00186_DS_
_00245_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x06
	BTFSS	STATUS,2
	GOTO	_00246_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00187_DS_
_00246_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x07
	BTFSS	STATUS,2
	GOTO	_00247_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00188_DS_
_00247_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x08
	BTFSS	STATUS,2
	GOTO	_00248_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00189_DS_
_00248_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x09
	BTFSS	STATUS,2
	GOTO	_00249_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00190_DS_
_00249_DS_:
	BANKSEL	r0x100E
	MOVF	r0x100E,W
	XORLW	0x63
	BTFSS	STATUS,2
	GOTO	_00250_DS_
	MOVF	r0x100F,W
	XORLW	0x00
	BTFSC	STATUS,2
	GOTO	_00191_DS_
_00250_DS_:
	GOTO	_00192_DS_
_00181_DS_:
;	.line	155; "dice_src_code.c"	case 0: GPIO = 0b000000; break;
	BANKSEL	_GPIO
	CLRF	_GPIO
	GOTO	_00194_DS_
_00182_DS_:
;	.line	156; "dice_src_code.c"	case 1: GPIO = 0b000001; break;
	MOVLW	0x01
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00183_DS_:
;	.line	157; "dice_src_code.c"	case 2: GPIO = 0b000010; break;
	MOVLW	0x02
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00184_DS_:
;	.line	158; "dice_src_code.c"	case 3: GPIO = 0b000011; break;
	MOVLW	0x03
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00185_DS_:
;	.line	159; "dice_src_code.c"	case 4: GPIO = 0b000100; break; 
	MOVLW	0x04
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00186_DS_:
;	.line	160; "dice_src_code.c"	case 5: GPIO = 0b000101; break;
	MOVLW	0x05
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00187_DS_:
;	.line	161; "dice_src_code.c"	case 6: GPIO = 0b000110; break;
	MOVLW	0x06
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00188_DS_:
;	.line	162; "dice_src_code.c"	case 7: GPIO = 0b000111; break;
	MOVLW	0x07
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00189_DS_:
;	.line	163; "dice_src_code.c"	case 8: GPIO = 0b010000; break;
	MOVLW	0x10
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00190_DS_:
;	.line	164; "dice_src_code.c"	case 9: GPIO = 0b010001; break;
	MOVLW	0x11
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00191_DS_:
;	.line	165; "dice_src_code.c"	case 99: GPIO = 0b010010; break; 
	MOVLW	0x12
	BANKSEL	_GPIO
	MOVWF	_GPIO
	GOTO	_00194_DS_
_00192_DS_:
;	.line	166; "dice_src_code.c"	default: GPIO = 0b000000;
	BANKSEL	_GPIO
	CLRF	_GPIO
_00194_DS_:
;	.line	168; "dice_src_code.c"	}
	RETURN	
; exit point of _mostrar_numero


;	code size estimation:
;	  309+   63 =   372 instructions (  870 byte)

	end
