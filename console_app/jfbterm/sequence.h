/*
 * JFBTERM -
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef INCLUDE_SEQUENCE_H
#define INCLUDE_SEQUENCE_H

#if 0
#define	CHAR_NUL	(0x00)
#define	CHAR_BEL	(0x07)
#define	CHAR_BS		(0x08)
#define	CHAR_HT		(0x09)
#define	CHAR_LF		(0x0A)
#define	CHAR_VT		(0x0B)
#define	CHAR_FF		(0x0C)
#define	CHAR_CR		(0x0D)
#define	CHAR_SO		(0x0E)
#define	CHAR_SI		(0x0F)
#define	CHAR_XON	(0x11)
#define	CHAR_XOFF	(0x12)
#define	CHAR_CAN	(0x18)
#define	CHAR_SUB	(0x1A)
#define	CHAR_ESC	(0x1B)
#define	CHAR_DEL	(0x7F)

#define	CHAR_CSI	(0x9B)
#define	CHAR_SS2	(0x8E)
#endif

#define UNIVERSAL_ESC	(0x1B)	/* UNIVERSAL ESCAPE CODE */ 
#define ISO_DEL		(0x7F)	/*  */ 
#define ISO_SPACE	(0x20)	/*  */ 

/* ======== ISO/IEC 2022 : ============================================== */
#define ISO_ACS		(0x20)	/*  */
#define ISO_CZD		(0x21)	/*  */
#define ISO_C1D		(0x22)	/*  */
#define ISO_PRV		(0x23)	/*  */
#define ISO__MBS	(0x24)	/* DESIGNATE MULTBITE SET */
#define ISO_DOCS	(0x25)	/*  */
#define ISO_IRR		(0x26)	/*  */
			/* 27	 *RESERVED* */
#define ISO_GZD4	(0x28)	/* G0-DESIGNATE 94-SET */
#define ISO_G1D4	(0x29)	/* G1-DESIGNATE 94-SET */
#define ISO_G2D4	(0x2A)	/* G2-DESIGNATE 94-SET */
#define ISO_G3D4	(0x2B)	/* G3-DESIGNATE 94-SET */
#define MULE__GZD6	(0x2C)	/* G0-DESIGNATE 96-SET (MULE EXTENSION) */
#define ISO_G1D6	(0x2D)	/* G1-DESIGNATE 96-SET */
#define ISO_G2D6	(0x2E)	/* G2-DESIGNATE 96-SET */
#define ISO_G3D6	(0x2F)	/* G3-DESIGNATE 96-SET */

/* ======== ISO/IEC 6429 : C0 SET ======================================= */
#define ISO_NUL		(0x00)	/*  */
#define ISO_SOH		(0x01)	/*  */
#define ISO_STX		(0x02)	/*  */
#define ISO_ETX		(0x03)	/*  */
#define ISO_EOT		(0x04)	/*  */
#define ISO_ENQ		(0x05)	/*  */
#define ISO_ACK		(0x06)	/*  */
#define ISO_BEL		(0x07)	/*  */
#define ISO_BS		(0x08)	/* (C0)		BACKSPACE */
#define ISO_HT		(0x09)	/* (C0)		CHARACTER TABULATION */
#define ISO_LF		(0x0A)	/* (C0)		LINE FEED */
#define ISO_VT		(0x0B)	/* (C0)		LINE TABULATION */
#define ISO_FF		(0x0C)	/* (C0)		FORM FEED */
#define ISO_CR		(0x0D)	/* (C0)		CARRIAGE RETURN */
#define ISO_LS1		(0x0E)	/* (C0)		LOCKING-SHIFT ONE */
#define ISO_LS0		(0x0F)	/* (C0)		LOCKING-SHIFT ZERO */
#define ISO_DLE		(0x10)	/*  */
#define ISO_DC1		(0x11)	/*  */
#define ISO_DC2		(0x12)	/*  */
#define ISO_DC3		(0x13)	/*  */
#define ISO_DC4		(0x14)	/*  */
#define ISO_NAK		(0x15)	/*  */
#define ISO_SYN		(0x16)	/*  */
#define ISO_ETB		(0x17)	/*  */
#define ISO_CAN		(0x18)	/*  */
#define ISO_EM		(0x19)	/*  */
#define ISO_SUB		(0x1A)	/*  */
#if 0	/* ESC is defined as UNIVERSAL_ESC */
#define ISO_ESC		(0x1B)	/* (C0)		ESCAPE */ 
#endif
#define ISO_IS4		(0x1C)	/*  */
#define ISO_IS3		(0x1D)	/*  */
#define ISO_IS2		(0x1E)	/*  */
#define ISO_IS1		(0x1F)	/*  */
/* ======== ISO/IEC 6429 : C1 SET ======================================= */
#define ISO_UNDEF_80	(0x80)	/*  */
#define ISO_UNDEF_81	(0x81)	/*  */
#define ISO_BPH		(0x82)	/*  */
#define ISO_NBH		(0x83)	/*  */
#define ISO_UNDEF_83	(0x84)	/*  */
#define ISO_NEL		(0x85)	/* (C1)		NEXT LINE */
#define ISO_SSA		(0x86)	/*  */
#define ISO_ESA		(0x87)	/*  */
#define ISO_HTS		(0x88)	/*  */
#define ISO_HTJ		(0x89)	/*  */
#define ISO_VTS		(0x8A)	/*  */
#define ISO_PLD		(0x8B)	/*  */
#define ISO_PLU		(0x8C)	/*  */
#define ISO_RI		(0x8D)	/* (C1)		REVERSE LINE FEED */
#define ISO_SS2		(0x8E)	/*  */
#define ISO_SS3		(0x8F)	/*  */
#define ISO_DCS		(0x90)	/*  */
#define ISO_PU1		(0x91)	/*  */
#define ISO_PU2		(0x92)	/*  */
#define ISO_STS		(0x93)	/*  */
#define ISO_CCH		(0x94)	/*  */
#define ISO_MW		(0x95)	/*  */
#define ISO_SPA		(0x96)	/*  */
#define ISO_EPA		(0x97)	/*  */
#define ISO_SOS		(0x98)	/*  */
#define ISO_UNDEF_99	(0x99)	/*  */
#define ISO_SCI		(0x9A)	/*  */
#define ISO_CSI		(0x9B)	/* (C1)	CONTROL SEQUENCE INTRODUCER */
#define ISO_ST		(0x9C)	/*  */
#define ISO_OSC		(0x9D)	/*  */
#define ISO_PM		(0x9E)	/*  */
#define ISO_APC		(0x9F)	/*  */
/* ======== ISO/IEC 6429 : CONTROL SEQUENCE WITHOUT I-BYTE ========== */
#define ISO_CS_NO_ICH	(0x40)	/*  */
#define ISO_CS_NO_CUU	(0x41)	/* (Pn)		CURSOR UP */
#define ISO_CS_NO_CUD	(0x42)	/* (Pn)		CURSOR DOWN */
#define ISO_CS_NO_CUF	(0x43)	/* (Pn)		CURSOR RIGHT */
#define ISO_CS_NO_CUB	(0x44)	/* (Pn)		CURSOR LEFT */
#define ISO_CS_NO_CNL	(0x45)	/*  */
#define ISO_CS_NO_CPL	(0x46)	/*  */
#define ISO_CS_NO_CHA	(0x47)	/*  */
#define ISO_CS_NO_CUP	(0x48)	/*  */
#define ISO_CS_NO_CHT	(0x49)	/*  */
#define ISO_CS_NO_ED	(0x4A)	/*  */
#define ISO_CS_NO_EL	(0x4B)	/*  */
#define ISO_CS_NO_IL	(0x4C)	/*  */
#define ISO_CS_NO_DL	(0x4D)	/*  */
#define ISO_CS_NO_EF	(0x4E)	/*  */
#define ISO_CS_NO_EA	(0x4F)	/*  */
#define ISO_CS_NO_DCH	(0x50)	/*  */
#define ISO_CS_NO_SEE	(0x51)	/*  */
#define ISO_CS_NO_CPR	(0x52)	/*  */
#define ISO_CS_NO_SU	(0x53)	/*  */
#define ISO_CS_NO_SD	(0x54)	/*  */
#define ISO_CS_NO_NP	(0x55)	/*  */
#define ISO_CS_NO_PP	(0x56)	/*  */
#define ISO_CS_NO_CTC	(0x57)	/*  */
#define ISO_CS_NO_ECH	(0x58)	/*  */
#define ISO_CS_NO_CVT	(0x59)	/*  */
#define ISO_CS_NO_CBT	(0x5A)	/*  */
#define ISO_CS_NO_SRS	(0x5B)	/*  */
#define ISO_CS_NO_PTX	(0x5C)	/*  */
#define ISO_CS_NO_SDS	(0x5D)	/*  */
#define ISO_CS_NO_SIMD	(0x5E)	/*  */
#define ISO_CS_NO_UNDEF_5F	(0x5F)	/*  */
#define ISO_CS_NO_HPA	(0x60)	/*  */
#define ISO_CS_NO_HPR	(0x61)	/*  */
#define ISO_CS_NO_REP	(0x62)	/*  */
#define ISO_CS_NO_DA	(0x63)	/*  */
#define ISO_CS_NO_VPA	(0x64)	/*  */
#define ISO_CS_NO_VPR	(0x65)	/*  */
#define ISO_CS_NO_HVP	(0x66)	/*  */
#define ISO_CS_NO_TBC	(0x67)	/*  */
#define ISO_CS_NO_SM	(0x68)	/*  */
#define ISO_CS_NO_MC	(0x69)	/*  */
#define ISO_CS_NO_BPB	(0x6A)	/*  */
#define ISO_CS_NO_VPB	(0x6B)	/*  */
#define ISO_CS_NO_RM	(0x6C)	/*  */
#define ISO_CS_NO_SGR	(0x6D)	/*  */
#define ISO_CS_NO_DSR	(0x6E)	/*  */
#define ISO_CS_NO_DAQ	(0x6F)	/*  */
/* ======== ISO/IEC 6429 : CONTROL SEQUENCE WITH I-BYTE 02/00  ====== */
#define ISO_CS_20_SL	(0x40)	/*  */
#define ISO_CS_20_SR	(0x41)	/*  */
#define ISO_CS_20_GSM	(0x42)	/*  */
#define ISO_CS_20_GSS	(0x43)	/*  */
#define ISO_CS_20_FNT	(0x44)	/*  */
#define ISO_CS_20_TSS	(0x45)	/*  */
#define ISO_CS_20_JFY	(0x46)	/*  */
#define ISO_CS_20_SPI	(0x47)	/*  */
#define ISO_CS_20_QUAD	(0x48)	/*  */
#define ISO_CS_20_SSU	(0x49)	/*  */
#define ISO_CS_20_PFS	(0x4A)	/*  */
#define ISO_CS_20_SHS	(0x4B)	/*  */
#define ISO_CS_20_SVS	(0x4C)	/*  */
#define ISO_CS_20_IGS	(0x4D)	/*  */
#define ISO_CS_20_UNDEF_4E	(0x4E)	/*  */
#define ISO_CS_20_IDCS	(0x4F)	/*  */
#define ISO_CS_20_PPA	(0x50)	/*  */
#define ISO_CS_20_PPR	(0x51)	/*  */
#define ISO_CS_20_PPB	(0x52)	/*  */
#define ISO_CS_20_SPD	(0x53)	/*  */
#define ISO_CS_20_DTA	(0x54)	/*  */
#define ISO_CS_20_SLH	(0x55)	/*  */
#define ISO_CS_20_SLL	(0x56)	/*  */
#define ISO_CS_20_FNK	(0x57)	/*  */
#define ISO_CS_20_SPQR	(0x58)	/*  */
#define ISO_CS_20_SEF	(0x59)	/*  */
#define ISO_CS_20_PEC	(0x5A)	/*  */
#define ISO_CS_20_SSW	(0x5B)	/*  */
#define ISO_CS_20_SACS	(0x5C)	/*  */
#define ISO_CS_20_SAPV	(0x5D)	/*  */
#define ISO_CS_20_STAB	(0x5E)	/*  */
#define ISO_CS_20_GCC	(0x5F)	/*  */
#define ISO_CS_20_TATE	(0x60)	/*  */
#define ISO_CS_20_TALE	(0x61)	/*  */
#define ISO_CS_20_TAC	(0x62)	/*  */
#define ISO_CS_20_TCC	(0x63)	/*  */
#define ISO_CS_20_TSR	(0x64)	/*  */
#define ISO_CS_20_SCO	(0x65)	/*  */
#define ISO_CS_20_SRCS	(0x66)	/*  */
#define ISO_CS_20_SCS	(0x67)	/*  */
#define ISO_CS_20_SLS	(0x68)	/*  */
#define ISO_CS_20_SPH	(0x69)	/*  */
#define ISO_CS_20_SPL	(0x6A)	/*  */
#define ISO_CS_20_SCP	(0x6B)	/*  */
#define ISO_CS_20_UNDEF_6C	(0x6C)	/*  */
#define ISO_CS_20_UNDEF_6D	(0x6D)	/*  */
#define ISO_CS_20_UNDEF_6E	(0x6E)	/*  */
#define ISO_CS_20_UNDEF_6F	(0x6F)	/*  */
/* ======== ISO/IEC 6429 : ========================================== */
#define ISO_DMI		(0x60)	/*  */
#define ISO_INT		(0x61)	/*  */
#define ISO_EMI		(0x62)	/*  */
#define ISO_RIS		(0x63)	/* (Fs)		RESET TO INITIAL STATE */
#define ISO_CMD		(0x64)	/*  */
#define ISO_LS2		(0x6E)	/*  */
#define ISO_LS3		(0x6F)	/*  */
#define ISO_LS3R	(0x7C)	/*  */
#define ISO_LS2R	(0x7D)	/*  */
#define ISO_LS1R	(0x7E)	/*  */

#define TERM_IND	(0x84)	/* (TERM C1) INDEX (DOWN AND SCROLL IF REQ) */
#define TERM_CAH	(0x2A)	/* (PRIVATE) CLS AND HOME */
#define DEC_RC		(0x38)	/* (PRIVATE) RESTORE CURSOR */
#define DEC_SC		(0x37)	/* (PRIVATE) SAVE CURSOR */

#endif /* INCLUDE_SEQUENCE_H */
