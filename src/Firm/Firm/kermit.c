//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// kermit.c - kermit Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define KERMIT_MAXSP		(400)
#define KERMIT_MAXRP		(800)
#define KERMIT_FILENAME_SIZE (256)

#define KERMIT_MAXTRY		(5)

#define KERMIT_TOCHAR(x)	((x) + 32)
#define KERMIT_UNCHAR(x)	((x) - 32)
#define KERMIT_CTL(x)		((x) ^ 64)

#define KERMIT_SRINI		(1)
#define KERMIT_SRFIL		(2)
#define KERMIT_SRDAT		(3)

// 送信
#define KERMIT_SSFIL		(1)
#define KERMIT_SSDAT		(2)
#define KERMIT_SSEOT		(3)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
char *gKermitRbuff;
int gKermitRlen;
unsigned int gKermitRmax;

int gKermitSpsiz = 90;
int gKermitRpsiz = 90;
int gKermitTimint = 5;
int gKermitRtimo = 7;
int gKermitRpadn = 0;
int gKermitSpadn = 0;

char gKermitSpadc = 0;
char gKermitRpadc = 0;
char gKermitSeol = '\r';
char gKermitReol = '\r';
char gKermitRctlq = '#';
char gKermitSctlq = '#';

int gKermitSeq = 0;
int gKermitSize = 0;
int gKermitRln = 0;
int gKermitRsn = 0;
int gKermitSndpkl = 0;

char gKermitSndpkt[KERMIT_MAXSP + 100];
char gKermitRcvpkt[KERMIT_MAXRP+200];
char gKermitFileName[KERMIT_FILENAME_SIZE];

char *gKermitRdatap, gKermitData[KERMIT_MAXRP + 1];
char gKermitSmark = '\1';
char gKermitRmark = '\1';
char gKermitStart = 0;
int gKermitState = 0;

// 送信
char *gkermitIsp = 0;
char *gKermitOsp = 0;
int gKermitIsl = 0;
int gKermiOsize = 0;
int gKermitMaxsiz = 0;
int gKermitLimit = KERMIT_MAXTRY;
int gKermitBctr = 1;
int gKermitPkt = 0;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gConsoleUpdateSelect;


//**********************************************************************************
//	kermit Recive
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBuffer				：受信データを格納するポインタ
//		size				：受信データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitRecv (char *pBuffer, unsigned int size)
{
	int status;
	char type;
	int len;

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Kermit pBuffer NULL Parameter Error.\n");
		return (status);
	}

	// Check size Parameter
	if (size > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff,"Kermit size(0x%x) Parameter Error.(Min=1 / Max=%d)\n", size, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	kermitTinit ();
	kermitRinit ();
	gKermitRtimo = 60;

	gKermitRbuff = (char *)pBuffer;
	gKermitRlen = 0;
	gKermitRmax = size;

	// File Name Clear(Ver.1.2)
	memset (gKermitFileName, 0x00, KERMIT_FILENAME_SIZE);

	while ((type = (char)kermitInput()) != 0)
	{
		if ((type == 'S') && (gKermitState == KERMIT_SRINI))
		{
			kermitSpar (gKermitRdatap);
			kermitAck1 (kermitRpar());
			gKermitState = KERMIT_SRFIL;
		}
		else if ((type == 'B') && (gKermitState == KERMIT_SRFIL))
		{
			kermitAck ();
			gKermitState = 0;
			break;
		}
		else if ((type == 'F') && (gKermitState == KERMIT_SRFIL))
		{
			// Get File Name Size
			len = strlen (gKermitRdatap);

			// Check File Name Size
			if (len < KERMIT_FILENAME_SIZE)
			{
				// File Name Copy
				strcpy (gKermitFileName, gKermitRdatap);
			}

			kermitAck ();
			gKermitState = KERMIT_SRDAT;
		}
		else if ((type == 'D') && (gKermitState == KERMIT_SRDAT))
		{
			if (kermitDecode () < 0)
				return (-1);
			else
				kermitAck ();
		}
		else if ((type == 'Z') && (gKermitState == KERMIT_SRDAT))
		{
			kermitClosof();
			kermitAck ();
			gKermitState = KERMIT_SRFIL;
		}
	}

    return (gKermitState == 0) ? gKermitRlen : -1;
}


//**********************************************************************************
//	kermit Recive Fill
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitRcvfil (void)
{
	return (1);
}


//**********************************************************************************
//	kermit Close
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitClosof (void)
{
	return (1);
}


//**********************************************************************************
//	kermit Recive
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitInput (void)
{
	int type, try;

	if (gKermitStart != 0)
	{
		type = gKermitStart;
		gKermitStart = 0;
		return (type);
	}

	type = kermitRpack ();
	for (try = 0; (gKermitRsn != gKermitSeq) || (kermitStrchr("TQN", type)); try++)
	{
		if (try > KERMIT_MAXTRY)
		{
			return 0;
		}
		if ((type == 'N') && (gKermitRsn == ((gKermitSeq + 1) & 63)))
		{
			return 'Y';
		}
		else
		{
			kermitResend ();
		}

		type = kermitRpack ();
	}

	kermitTtflui ();
	return (type);
}


//**********************************************************************************
//	kermitResend
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitResend (void)
{
	int x = 0;

	if (*gKermitSndpkt)
		x = kermitTtol (gKermitSndpkt, gKermitSndpkl);
	else
		kermitNak ();

	return x;
}


//**********************************************************************************
//	kermit Next Pkt
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermitNxtpkt (void)
{
	gKermitSeq = (gKermitSeq + 1) & 63;
}


//**********************************************************************************
//	kermit T Init
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermitTinit (void)
{
	gKermitSeq = 0;
}


//**********************************************************************************
//	kermit R Init
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermitRinit (void)
{
	gKermitState = KERMIT_SRINI;
}


//**********************************************************************************
//	kermit Ack(送信データなし)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitAck (void)
{
	int x;

	x = kermitSpack ('Y', gKermitSeq, 0, "");
	kermitNxtpkt ();
	return x;
}


//**********************************************************************************
//	kermit Ack1(送信データあり)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		s					：送信データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitAck1 (char *s)
{
	int x;

	x = kermitSpack ('Y', gKermitSeq, strlen(s), s);
	kermitNxtpkt ();
	return x;
}

//**********************************************************************************
//	kermit Nack
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitNak (void)
{
	int x;

	x = kermitSpack ('N', gKermitSeq, 0, "");
	return x;
}


//**********************************************************************************
//	kermit Spack(Send Pack)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		type				：Y(ACK) or N(NACK)
//		n					：シーケンス番号(0～63)
//		len					：データ長
//		d					：送信データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSpack (char type, int n, int len, char *d)
{
	int i = 0;
	int j, k;

	// データスタートマーク
	gKermitSndpkt[i++] = gKermitSmark;
	k = i;

	// データ長（ASCII)
	gKermitSndpkt[i++] = (char)KERMIT_TOCHAR (len + 3);

	// シーケンス番号(ASCII)
	gKermitSndpkt[i++] = (char)KERMIT_TOCHAR (n);

	// Type(Y/N)
	gKermitSndpkt[i++] = type;

	// 送信データ設定
	for (j = 0; j < len; j++)
		gKermitSndpkt[i++] = *d++;

	// 区切り
	gKermitSndpkt[i] = '\0';

	// チェックサム計算
	gKermitSndpkt[i++] = (char)KERMIT_TOCHAR (kermitChkl(gKermitSndpkt + k));

	// Eol
	gKermitSndpkt[i++] = gKermitSeol;
	
	// NUL
	gKermitSndpkt[i++] = '\0';

	// データ長保存
	gKermitSndpkl = i;

	// データ送信
	i = kermitTtol (gKermitSndpkt, gKermitSndpkl);

	return (i);
}


//**********************************************************************************
//	kermit Rpack
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitRpack (void)
{
	int i, j, type, rlnpos;
	char pbc[4];

	gKermitRsn = gKermitRln = -1;
	*gKermitRcvpkt = '\0';
	
	j = kermitTtinl (gKermitRcvpkt, KERMIT_MAXRP, gKermitReol, gKermitRtimo * 1000);
	if (j < 0)
		return ('T');		// Error

	for (i = 0; gKermitRcvpkt[i] != gKermitRmark && (i < j); i++);

	if (i == j)
		return ('Q');		// Quit

	rlnpos = ++i;
	gKermitRln = KERMIT_UNCHAR(gKermitRcvpkt[i++]) - 3;
	gKermitRsn = KERMIT_UNCHAR(gKermitRcvpkt[i++]);
	type = gKermitRcvpkt[i++];
	gKermitRdatap = gKermitRcvpkt + i;
	*pbc = gKermitRcvpkt[i + gKermitRln];
	gKermitRcvpkt[i + gKermitRln] = '\0';

	if (KERMIT_UNCHAR(*pbc) != kermitChkl(gKermitRcvpkt + rlnpos))
		return ('Q');
	else
		return (type);
}


//**********************************************************************************
//	kermit Chkl
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitChkl (char *packet)
{
	int s, t;

	s = kermitChksum (packet);
	t = (((s & 192) >> 6) + s) & 63;

	return (t);
}


//**********************************************************************************
//	kermit Check Sum
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitChksum (char *p)
{
	unsigned int s;

	for (s = 0; *p != 0; p++)
		s += *p;

	return (s & 07777);
}


//**********************************************************************************
//	kermit Encode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermitEncode (char a)
{
	int a7;

	a7 = a & 127;
	if (a7 < 32 || a7 == 127)
	{
		gKermitData[gKermitSize++] = gKermitSctlq;
		a = (char)KERMIT_CTL(a);
	}
	else if (a7 == gKermitSctlq)
	{
		gKermitData[gKermitSize++] = gKermitSctlq;
	}

	gKermitData[gKermitSize++] = a;
	gKermitData[gKermitSize] = '\0';
}


//**********************************************************************************
//	kermit Decode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitDecode (void)
{
	char a, a7;

	while ((a = *gKermitRdatap++) != '\0')
	{
		if (a == gKermitRctlq)
		{
			// 受信データデコード
			a = *gKermitRdatap++;
			a7 = (char)(a & 127);
			if ((a7 > 62) && (a7 < 96))
				a = (char)KERMIT_CTL(a);
		}

		// 受信データ保存
		if (kermitPnchar (a) < 0)
			return (-1);
	}

	return (0);
}


//**********************************************************************************
//	受信データをバッファへ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitPnchar (char c)
{
	// 受信バッファサイズチェック
	if (gKermitRlen >= gKermitRmax)
		return (0);

	// 受信データをバッファへ格納
	gKermitRbuff[gKermitRlen++] = c;
		return (1);
}


//**********************************************************************************
//	kermit Spar
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermitSpar (char *s)
{
	int x;

	s--;

	// Limit on size of outbound packets
	x = (gKermitRln >= 1) ? KERMIT_UNCHAR(s[1]) : 80;
	gKermitSpsiz = (x < 10) ? 80 : x;

	// Timeout on inbound packets
	x = (gKermitRln >= 2) ? KERMIT_UNCHAR(s[2]) : 5;
	gKermitTimint = (x < 0) ? 5 : x;

	gKermitRtimo = 7;

	// Outbound Padding
	gKermitSpadn = 0; gKermitSpadc = '\0';
	if (gKermitRln >= 3)
	{
		gKermitSpadn = KERMIT_UNCHAR(s[3]);
		if (gKermitRln >= 4)
			gKermitSpadc = (char)KERMIT_CTL(s[4]);
		else
			gKermitSpadc = 0;
	}

	// Outbound Packet Terminator
	gKermitSeol = (char)((gKermitRln >= 5) ? KERMIT_UNCHAR(s[5]) : '\r');
	if ((gKermitSeol < 2) || (gKermitSeol > 31))
		gKermitSeol = '\r';

	// Control prefix
	x = (gKermitRln >= 6) ? s[6] : '#';
	gKermitRctlq = (char)(((x > 32 && x < 63) || (x > 95 && x < 127)) ? x : '#');
}


//**********************************************************************************
//	kermit Rpar
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
char *kermitRpar (void)
{
	gKermitData[1] = (char)KERMIT_TOCHAR (gKermitRpsiz);
	gKermitData[2] = (char)KERMIT_TOCHAR (gKermitRtimo);
	gKermitData[3] = (char)KERMIT_TOCHAR (gKermitRpadn);
	gKermitData[4] = (char)KERMIT_CTL (gKermitRpadc);
	gKermitData[5] = (char)KERMIT_TOCHAR (gKermitReol);
	gKermitData[6] = '#';
	gKermitData[7] = 0;
	return (gKermitData + 1);
}


//**********************************************************************************
//	kermit Flush Input Buffer
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitTtflui (void)
{
	return (0);
}


//**********************************************************************************
//	kermitデータ送信
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitTtol (char *s, int n)
{
	int i;
	for (i=0;i<n;i++)
	{
		kermiSendByte (*s);
		s++;
	}

	return (n);
}


//**********************************************************************************
//	kermit Tranmit Packet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitTtinl (char *dest, int max, char eol, int timo)
{
	int ccn = 0;
	char ch;
	int len;
	len = 0;
	*dest = 0;

	while (len < max)
	{
		dest[len++] = ch = kermitRecvByte ();
		if (ch == eol)
		{
			break;
		}
		else if (ch == 3)
		{
			// control c
			if (++ccn > 1)
				return (-1);
		}
		else
		{
			ccn = 0;
		}
	}

	dest[len]= '\0';
	return (len);
}


//**********************************************************************************
//	一致文字検索
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		s					：検索対象
//		c					：検索文字
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
char *kermitStrchr (char *s, int c)
{
    while (*s)
	{
        if(*s == (char)c)
            return (char*)s;
        s++;
	}

    return (NULL);
}


//**********************************************************************************
//	kermit UART Send
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
void kermiSendByte (char byte)
{
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		if (gConsoleUpdateSelect == UART_PORT0)
			DEBUG_OUTPUT (byte);
		else
			outputGigE (byte);
	}
	else
	{
		DEBUG_OUTPUT (byte);
	}
}


//**********************************************************************************
//	kermit UART Recv
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
char kermitRecvByte (void)
{
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		if (gConsoleUpdateSelect == UART_PORT0)
			return (DEBUG_INPUT ());
		else
			return (inputGigE ());
	}
	else
	{
		return (DEBUG_INPUT ());
	}
}


//**********************************************************************************
//	kermit UART Send
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFileName			：送信データを保存するファイル名を格納するポインタ
//		pBuffer				：送信データを格納するポインタ
//		size				：送信データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSend (char *pFileName, char *pBuffer, int size)
{
	char type;
	int filcnt = 0;

	kermitTinit ();
	gKermitRtimo = 10;
	kermitSinit ('S');
	gKermitState = KERMIT_SSFIL;

	while ((type = (char)kermitInput ()) != 0)
	{
		if ((type == 'Y') && (gKermitState == KERMIT_SSFIL))
		{
			if (filcnt++ == 0)
			{
				kermitSpar (gKermitRdatap);
				kermitSfile (pFileName, pBuffer, size);
				gKermitState = KERMIT_SSDAT;
			}
			else
			{
				kermitSeot ();
				gKermitState = KERMIT_SSEOT;
			}
		}
		else if ((type == 'Y') && (gKermitState == KERMIT_SSDAT))
		{
			if (kermitSdata () == 0)
			{
				kermitSeof ("");
				gKermitState = KERMIT_SSFIL;
			}
		}
		else if ((type == 'Y') && (gKermitState == KERMIT_SSEOT))
			return (0);
	}

	return (0);
}


//**********************************************************************************
//	kermitファイル名送信
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFilename			：送信データを保存するファイル名を格納するポインタ
//		pBuffer				：送信データを格納するポインタ
//		size				：送信データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSfile (char *pFilename, char *pBuffer, int size)
{
	int x;

	x = kermitEncstr (pFilename);

	gkermitIsp = pBuffer;
	gKermitIsl = size;

	gKermitMaxsiz = gKermitSpsiz - (gKermitBctr + 3);
	kermitNxtpkt ();

	return (kermitSpack((gKermitPkt ? 'X' : 'F'), gKermitSeq, x, gKermitData));
}


//**********************************************************************************
//	kermit UART Send Data
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSdata (void)
{
	int x;

	if ((x = kermitGetpkt (gKermitMaxsiz)) == 0)
	return (0);
	
	kermitNxtpkt ();
	return (kermitSpack ('D', gKermitSeq, x, gKermitData));
}


//**********************************************************************************
//	kermit Encstr
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		s					：
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitEncstr (char *s)
{
	gkermitIsp = s;
	gKermitIsl = strlen (s);
	kermitGetpkt (gKermitSpsiz);
	gkermitIsp = 0;

	return (gKermitSize);
}


//**********************************************************************************
//	kermit Get Packet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		maxlen
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitGetpkt (int maxlen)
{
	int i, c;
	static char remain[6] = { '\0', '\0', '\0', '\0', '\0', '\0' };

	for (gKermitSize = 0; (gKermitData[gKermitSize] = remain[gKermitSize]) != '\0'; gKermitSize++);
		*remain = '\0';

	while ((c = kermitGnchar ()) > -1)
	{
		gKermiOsize = gKermitSize;
		kermitEncode (c);

		if (gKermitSize == maxlen)
			return (gKermitSize);

		if (gKermitSize > maxlen)
		{
			for (i = 0; (remain[i] = gKermitData[gKermiOsize + i]) != '\0'; i++);
			gKermitSize = gKermiOsize;
			gKermitData[gKermitSize] = '\0';
			return (gKermitSize);
		}
	}
	
	return (gKermitSize);
}


//**********************************************************************************
//	kermit Eof
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSeof (char *s)
{
	kermitNxtpkt ();
	return (kermitSpack('Z', gKermitSeq, strlen(s), s));
}


//**********************************************************************************
//	kermit Eot
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSeot (void)
{
	kermitNxtpkt ();
	return (kermitSpack('B', gKermitSeq, 0, ""));
}


//**********************************************************************************
//	kermit Eot
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitGnchar (void)
{
	unsigned char c;

	if (gKermitIsl > 0)
	{
		c = *gkermitIsp++;
		gKermitIsl--;
		return (c);
	}

	return (-1);
}


//**********************************************************************************
//	kermit kermitRpar
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//=================================================================================
int kermitSinit (char c)
{
	char *s;

	s = kermitRpar ();
	usDelay(1000);
	return (kermitSpack (c, gKermitSeq, strlen(s), s));
}


//**********************************************************************************
//	gZipデータCheck
//  ※malloc処理を関数内で使用する為、別途開放処理必要
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pUnCompress			：解凍データを格納するポインタ
//		pUncomprLen			：解凍データサイズを格納するポインタ
//		pCompress			：圧縮データを格納するポインタ
//		uncomprLen			：圧縮データサイズ
//		mallocSize			：mallocするサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int kermitGzipDataCheck (unsigned char **pUnCompress, int *pUncomprLen, unsigned char *pCompress, int comprLen, int mallocSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int len;
	unsigned char *pTemp;

	// Check pUncomprLen Parameter
	if (pUncomprLen == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Kermit Data Check pUncomprLen NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pCompress Parameter
	if (pCompress == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Kermit Data Check pCompress NULL Parameter Error.\n");
		goto _DONE;
	}

#ifdef COMPRESS_MODE
	// 初期化
	//*pUnCompress = pCompress;
	//*pUncomprLen = comprLen;

	if ((pCompress[0] == COMPRESS_GZIP_ID1) && (pCompress[1] == COMPRESS_GZIP_ID2))
	{
		// Get Size
		len = strlen (gKermitFileName);

		if (len > 3)
		{
			// Check File Name
			if (strncmp (&gKermitFileName[len-3], UPDATE_FILE_NAME_CL_GZIP_EXTENSION, 3) == 0)
			{
				#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

				// Malloc
				if ((pTemp = malloc (mallocSize)) == NULL)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
					sprintf (gLogMsgBuff, "Uncompress Malloc Error. Size = %d\n", mallocSize);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}

				#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

				//@@@@1pTemp = (unsigned char *)FFC_BLACK_MEMORY_ADRS;
				pTemp = (unsigned char *)0x8a000000;

				#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

				// Uncompress
				*pUncomprLen = mallocSize;
				if ((status = gzipUncomp (pTemp, pUncomprLen, (unsigned char *)pCompress, comprLen)) != 0)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
					sprintf (gLogMsgBuff, "Uncompress Error. Status = %d\n", status);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

					#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
					free (*pUnCompress);
					#endif
					goto _DONE;
				}

				// バッファコピー
				*pUnCompress = pTemp;
			}
		}
	}
#endif // COMPRESS_MODE

_DONE:
	return (status);
}

//eof
