@ God knows..., Satoru Kosaki

	.equ	tempo,155	@ num of quarternotes per minute
	.equ	tick, 32	@ num of ticks per quarternote

	.include "note.s"

    .macro Ref000 x=A4
	.byte	Gs5,12,4,\x,5,3
	.byte	Fs5, 8,0,\x,5,3,\x,5,3
	.byte	E5,  8,0,\x,5,3,\x,5,3
	.byte	Fs5,12,4,\x,5,3
	.byte	E5,10,6
    .endm
    .macro Ref00 x=A4
	Ref000	\x
	.byte	Fs5,5,3,\x,5,3
    .endm
    .macro Ref1
	Ref00  A4
	Ref00  B3
    .endm
    .macro Refx00 u=Gs5 v=Fs5 w=Cs4 x=Gs5 y=Fs5 z=Cs4
	.byte	\u,6,0,\v,5,0,\w,5,0, \x,6,0,\y,5,0,\z,5,0
    .endm
    .macro Refx1 u=Gs5 v=Fs5 w=Cs4 x=Gs5 y=Fs5 z=Cs4
	Refx00  \u,\v,\w,\x,\y,\z
	Refx00  \u,\v,\w,\x,\y,\z	
	Refx00  \u,\v,\w,\x,\y,\z
    .endm

	.section .data
	.global	music_data
music_data:
	.byte	E5,4,28,E5,4,28,E5,4,28,E5,4,28

	Ref1
	Ref000	Gs4
	.byte	Ds5,20,4,Cs4,5,3
	.byte	E5,8,0,Cs4,5,3,Ds5,8,0,Cs4,5,3
	.byte	B4,20,4,Cs4,5,3
	.byte	Cs5,8,0,Cs4,5,3,Ds5,8,0,Gs4,5,3
	.byte	E5,8,0,Cs4,5,3

	Ref1
	Refx1
	Refx00
	Refx1	Gs5,Fs5,E5, Ds5,Cs5,B3
	.byte	Ds5,8,0,B3,8,0,Cs4,8,0,E4,8,0

	Ref1
	.byte	B5,10,6,Gs4,5,3
	.byte	B5,10,6,Gs4,5,3
	.byte	B5,10,6,Gs4,5,3
	.byte	A5,8,0,Gs4,5,3,Gs4,5,3
	.byte	Gs5,10,6,A5,5,3,Gs4,5,3
	.byte	Fs5,12,4,Cs4,5,3
	.byte	E5,  8,0,Cs4,5,3,Cs4,5,3
	.byte	Fs5,12,4,Cs4,5,3
	.byte	Ds5,7,1, Ds5,12,4
	.byte	E5,10,6, Fs5,5,3,Cs4,5,3

	Ref1
	Refx1	Gs5,Fs5,F5, Gs5,Fs5,Cs4
	Refx00	Gs5,Fs5,F5, Gs5,Fs5,Cs4
	.byte	F5,16,0,Fs5,8,0,Cs4,8,0
	.byte	F5,8,0, Cs4,8,0,Cs5,60,20

	.byte	0
        .end
