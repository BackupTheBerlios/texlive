unsigned int rop3code[256] = {
  /* 00 */ 0x00000042, /* 0 BLACKNESS */
  /* 01 */ 0x00010289, /* DPSoon */
  /* 02 */ 0x00020C89, /* DPSona */
  /* 03 */ 0x000300AA, /* PSon */
  /* 04 */ 0x00040C88, /* SDPona */
  /* 05 */ 0x000500A9, /* DPon */
  /* 06 */ 0x00060865, /* PDSxnon */
  /* 07 */ 0x000702C5, /* PDSaon */
  /* 08 */ 0x00080F08, /* SDPnaa */
  /* 09 */ 0x00090245, /* PDSxon */
  /* 0A */ 0x000A0329, /* DPna */
  /* 0B */ 0x000B0B2A, /* PSDnaon */
  /* 0C */ 0x000C0324, /* SPna */
  /* 0D */ 0x000D0B25, /* PDSnaon */
  /* 0E */ 0x000E08A5, /* PDSonon */
  /* 0F */ 0x000F0001, /* Pn */
  /* 10 */ 0x00100C85, /* PDSona */
  /* 11 */ 0x001100A6, /* DSon NOTSRCERASE */
  /* 12 */ 0x00120868, /* SDPxnon */
  /* 13 */ 0x001302C8, /* SDPaon */
  /* 14 */ 0x00140869, /* DPSxnon */
  /* 15 */ 0x001502C9, /* DPSaon */
  /* 16 */ 0x00165CCA, /* PSDPSanaxx */
  /* 17 */ 0x00171D54, /* SSPxDSxaxn */
  /* 18 */ 0x00180D59, /* SPxPDxa */
  /* 19 */ 0x00191CC8, /* SDPSanaxn */
  /* 1A */ 0x001A06C5, /* PDSPaox */
  /* 1B */ 0x001B0768, /* SDPSxaxn */
  /* 1C */ 0x001C06CA, /* PSDPaox */
  /* 1D */ 0x001D0766, /* DSPDxaxn */
  /* 1E */ 0x001E01A5, /* PDSox */
  /* 1F */ 0x001F0385, /* PDSoan */
  /* 20 */ 0x00200F09, /* DPSnaa */
  /* 21 */ 0x00210248, /* SDPxon */
  /* 22 */ 0x00220326, /* DSna */
  /* 23 */ 0x00230B24, /* SPDnaon */
  /* 24 */ 0x00240D55, /* SPxDSxa */
  /* 25 */ 0x00251CC5, /* PDSPanaxn */
  /* 26 */ 0x002606C8, /* SDPSaox */
  /* 27 */ 0x00271868, /* SDPSxnox */
  /* 28 */ 0x00280369, /* DPSxa */
  /* 29 */ 0x002916CA, /* PSDPSaoxxn */
  /* 2A */ 0x002A0CC9, /* DPSana */
  /* 2B */ 0x002B1D58, /* SSPxPDxaxn */
  /* 2C */ 0x002C0784, /* SPDSoax */
  /* 2D */ 0x002D060A, /* PSDnox */
  /* 2E */ 0x002E064A, /* PSDPxox */
  /* 2F */ 0x002F0E2A, /* PSDnoan */
  /* 30 */ 0x0030032A, /* PSna */
  /* 31 */ 0x00310B28, /* SDPnaon */
  /* 32 */ 0x00320688, /* SDPSoox */
  /* 33 */ 0x00330008, /* Sn NOTSRCCOPY */
  /* 34 */ 0x003406C4, /* SPDSaox */
  /* 35 */ 0x00351864, /* SPDSxnox */
  /* 36 */ 0x003601A8, /* SDPox */
  /* 37 */ 0x00370388, /* SDPoan */
  /* 38 */ 0x0038078A, /* PSDPoax */
  /* 39 */ 0x00390604, /* SPDnox */
  /* 3A */ 0x003A0644, /* SPDSxox */
  /* 3B */ 0x003B0E24, /* SPDnoan */
  /* 3C */ 0x003C004A, /* PSx */
  /* 3D */ 0x003D18A4, /* SPDSonox */
  /* 3E */ 0x003E1B24, /* SPDSnaox */
  /* 3F */ 0x003F00EA, /* PSan */
  /* 40 */ 0x00400F0A, /* PSDnaa */
  /* 41 */ 0x00410249, /* DPSxon */
  /* 42 */ 0x00420D5D, /* SDxPDxa */
  /* 43 */ 0x00431CC4, /* SPDSanaxn */
  /* 44 */ 0x00440328, /* SDna SRCERASE */
  /* 45 */ 0x00450B29, /* DPSnaon */
  /* 46 */ 0x004606C6, /* DSPDaox */
  /* 47 */ 0x0047076A, /* PSDPxaxn */
  /* 48 */ 0x00480368, /* SDPxa */
  /* 49 */ 0x004916C5, /* PDSPDaoxxn */
  /* 4A */ 0x004A0789, /* DPSDoax */
  /* 4B */ 0x004B0605, /* PDSnox */
  /* 4C */ 0x004C0CC8, /* SDPana */
  /* 4D */ 0x004D1954, /* SSPxDSxoxn */
  /* 4E */ 0x004E0645, /* PDSPxox */
  /* 4F */ 0x004F0E25, /* PDSnoan */
  /* 50 */ 0x00500325, /* PDna */
  /* 51 */ 0x00510B26, /* DSPnaon */
  /* 52 */ 0x005206C9, /* DPSDaox */
  /* 53 */ 0x00530764, /* SPDSxaxn */
  /* 54 */ 0x005408A9, /* DPSonon */
  /* 55 */ 0x00550009, /* Dn DSTINVERT */
  /* 56 */ 0x005601A9, /* DPSox */
  /* 57 */ 0x00570389, /* DPSoan */
  /* 58 */ 0x00580785, /* PDSPoax */
  /* 59 */ 0x00590609, /* DPSnox */
  /* 5A */ 0x005A0049, /* DPx PATINVERT */
  /* 5B */ 0x005B18A9, /* DPSDonox */
  /* 5C */ 0x005C0649, /* DPSDxox */
  /* 5D */ 0x005D0E29, /* DPSnoan */
  /* 5E */ 0x005E1B29, /* DPSDnaox */
  /* 5F */ 0x005F00E9, /* DPan */
  /* 60 */ 0x00600365, /* PDSxa */
  /* 61 */ 0x006116C6, /* DSPDSaoxxn */
  /* 62 */ 0x00620786, /* DSPDoax */
  /* 63 */ 0x00630608, /* SDPnox */
  /* 64 */ 0x00640788, /* SDPSoax */
  /* 65 */ 0x00650606, /* DSPnox */
  /* 66 */ 0x00660046, /* DSx SRCINVERT */
  /* 67 */ 0x006718A8, /* SDPSonox */
  /* 68 */ 0x006858A6, /* DSPDSonoxxn */
  /* 69 */ 0x00690145, /* PDSxxn */
  /* 6A */ 0x006A01E9, /* DPSax */
  /* 6B */ 0x006B178A, /* PSDPSoaxxn */
  /* 6C */ 0x006C01E8, /* SDPax */
  /* 6D */ 0x006D1785, /* PDSPDoaxxn */
  /* 6E */ 0x006E1E28, /* SDPSnoax */
  /* 6F */ 0x006F0C65, /* PDSxnan */
  /* 70 */ 0x00700CC5, /* PDSana */
  /* 71 */ 0x00711D5C, /* SSDxPDxaxn */
  /* 72 */ 0x00720648, /* SDPSxox */
  /* 73 */ 0x00730E28, /* SDPnoan */
  /* 74 */ 0x00740646, /* DSPDxox */
  /* 75 */ 0x00750E26, /* DSPnoan */
  /* 76 */ 0x00761B28, /* SDPSnaox */
  /* 77 */ 0x007700E6, /* DSan */
  /* 78 */ 0x007801E5, /* PDSax */
  /* 79 */ 0x00791786, /* DSPDSoaxxn */
  /* 7A */ 0x007A1E29, /* DPSDnoax */
  /* 7B */ 0x007B0C68, /* SDPxnan */
  /* 7C */ 0x007C1E24, /* SPDSnoax */
  /* 7D */ 0x007D0C69, /* DPSxnan */
  /* 7E */ 0x007E0955, /* SPxDSxo */
  /* 7F */ 0x007F03C9, /* DPSaan */
  /* 80 */ 0x008003E9, /* DPSaa */
  /* 81 */ 0x00810975, /* SPxDSxon */
  /* 82 */ 0x00820C49, /* DPSxna */
  /* 83 */ 0x00831E04, /* SPDSnoaxn */
  /* 84 */ 0x00840C48, /* SDPxna */
  /* 85 */ 0x00851E05, /* PDSPnoaxn */
  /* 86 */ 0x008617A6, /* DSPDSoaxx */
  /* 87 */ 0x008701C5, /* PDSaxn */
  /* 88 */ 0x008800C6, /* DSa SRCAND */
  /* 89 */ 0x00891B08, /* SDPSnaoxn */
  /* 8A */ 0x008A0E06, /* DSPnoa */
  /* 8B */ 0x008B0666, /* DSPDxoxn */
  /* 8C */ 0x008C0E08, /* SDPnoa */
  /* 8D */ 0x008D0668, /* SDPSxoxn */
  /* 8E */ 0x008E1D7C, /* SSDxPDxax */
  /* 8F */ 0x008F0CE5, /* PDSanan */
  /* 90 */ 0x00900C45, /* PDSxna */
  /* 91 */ 0x00911E08, /* SDPSnoaxn */
  /* 92 */ 0x009217A9, /* DPSDPoaxx */
  /* 93 */ 0x009301C4, /* SPDaxn */
  /* 94 */ 0x009417AA, /* PSDPSoaxx */
  /* 95 */ 0x009501C9, /* DPSaxn */
  /* 96 */ 0x00960169, /* DPSxx */
  /* 97 */ 0x0097588A, /* PSDPSonoxx */
  /* 98 */ 0x00981888, /* SDPSonoxn */
  /* 99 */ 0x00990066, /* DSxn */
  /* 9A */ 0x009A0709, /* DPSnax */
  /* 9B */ 0x009B07A8, /* SDPSoaxn */
  /* 9C */ 0x009C0704, /* SPDnax */
  /* 9D */ 0x009D07A6, /* DSPDoaxn */
  /* 9E */ 0x009E16E6, /* DSPDSaoxx */
  /* 9F */ 0x009F0345, /* PDSxan */
  /* A0 */ 0x00A000C9, /* DPa */
  /* A1 */ 0x00A11B05, /* PDSPnaoxn */
  /* A2 */ 0x00A20E09, /* DPSnoa */
  /* A3 */ 0x00A30669, /* DPSDxoxn */
  /* A4 */ 0x00A41885, /* PDSPonoxn */
  /* A5 */ 0x00A50065, /* PDxn */
  /* A6 */ 0x00A60706, /* DSPnax */
  /* A7 */ 0x00A707A5, /* PDSPoaxn */
  /* A8 */ 0x00A803A9, /* DPSoa */
  /* A9 */ 0x00A90189, /* DPSoxn */
  /* AA */ 0x00AA0029, /* D */
  /* AB */ 0x00AB0889, /* DPSono */
  /* AC */ 0x00AC0744, /* SPDSxax */
  /* AD */ 0x00AD06E9, /* DPSDaoxn */
  /* AE */ 0x00AE0B06, /* DSPnao */
  /* AF */ 0x00AF0229, /* DPno */
  /* B0 */ 0x00B00E05, /* PDSnoa */
  /* B1 */ 0x00B10665, /* PDSPxoxn */
  /* B2 */ 0x00B21974, /* SSPxDSxox */
  /* B3 */ 0x00B30CE8, /* SDPanan */
  /* B4 */ 0x00B4070A, /* PSDnax */
  /* B5 */ 0x00B507A9, /* DPSDoaxn */
  /* B6 */ 0x00B616E9, /* DPSDPaoxx */
  /* B7 */ 0x00B70348, /* SDPxan */
  /* B8 */ 0x00B8074A, /* PSDPxax */
  /* B9 */ 0x00B906E6, /* DSPDaoxn */
  /* BA */ 0x00BA0B09, /* DPSnao */
  /* BB */ 0x00BB0226, /* DSno MERGEPAINT */
  /* BC */ 0x00BC1CE4, /* SPDSanax */
  /* BD */ 0x00BD0D7D, /* SDxPDxan */
  /* BE */ 0x00BE0269, /* DPSxo */
  /* BF */ 0x00BF08C9, /* DPSano */
  /* C0 */ 0x00C000CA, /* PSa MERGECOPY */
  /* C1 */ 0x00C11B04, /* SPDSnaoxn */
  /* C2 */ 0x00C21884, /* SPDSonoxn */
  /* C3 */ 0x00C3006A, /* PSxn */
  /* C4 */ 0x00C40E04, /* SPDnoa */
  /* C5 */ 0x00C50664, /* SPDSxoxn */
  /* C6 */ 0x00C60708, /* SDPnax */
  /* C7 */ 0x00C707AA, /* PSDPoaxn */
  /* C8 */ 0x00C803A8, /* SDPoa */
  /* C9 */ 0x00C90184, /* SPDoxn */
  /* CA */ 0x00CA0749, /* DPSDxax */
  /* CB */ 0x00CB06E4, /* SPDSaoxn */
  /* CC */ 0x00CC0020, /* S SRCCOPY */
  /* CD */ 0x00CD0888, /* SDPono */
  /* CE */ 0x00CE0B08, /* SDPnao */
  /* CF */ 0x00CF0224, /* SPno */
  /* D0 */ 0x00D00E0A, /* PSDnoa */
  /* D1 */ 0x00D1066A, /* PSDPxoxn */
  /* D2 */ 0x00D20705, /* PDSnax */
  /* D3 */ 0x00D307A4, /* SPDSoaxn */
  /* D4 */ 0x00D41D78, /* SSPxPDxax */
  /* D5 */ 0x00D50CE9, /* DPSanan */
  /* D6 */ 0x00D616EA, /* PSDPSaoxx */
  /* D7 */ 0x00D70349, /* DPSxan */
  /* D8 */ 0x00D80745, /* PDSPxax */
  /* D9 */ 0x00D906E8, /* SDPSaoxn */
  /* DA */ 0x00DA1CE9, /* DPSDanax */
  /* DB */ 0x00DB0D75, /* SPxDSxan */
  /* DC */ 0x00DC0B04, /* SPDnao */
  /* DD */ 0x00DD0228, /* SDno */
  /* DE */ 0x00DE0268, /* SDPxo */
  /* DF */ 0x00DF08C8, /* SDPano */
  /* E0 */ 0x00E003A5, /* PDSoa */
  /* E1 */ 0x00E10185, /* PDSoxn */
  /* E2 */ 0x00E20746, /* DSPDxax */
  /* E3 */ 0x00E306EA, /* PSDPaoxn */
  /* E4 */ 0x00E40748, /* SDPSxax */
  /* E5 */ 0x00E506E5, /* PDSPaoxn */
  /* E6 */ 0x00E61CE8, /* SDPSanax */
  /* E7 */ 0x00E70D79, /* SPxPDxan */
  /* E8 */ 0x00E81D74, /* SSPxDSxax */
  /* E9 */ 0x00E95CE6, /* DSPDSanaxxn */
  /* EA */ 0x00EA02E9, /* DPSao */
  /* EB */ 0x00EB0849, /* DPSxno */
  /* EC */ 0x00EC02E8, /* SDPao */
  /* ED */ 0x00ED0848, /* SDPxno */
  /* EE */ 0x00EE0086, /* DSo SRCPAINT */
  /* EF */ 0x00EF0A08, /* SDPnoo */
  /* F0 */ 0x00F00021, /* P PATCOPY */
  /* F1 */ 0x00F10885, /* PDSono */
  /* F2 */ 0x00F20B05, /* PDSnao */
  /* F3 */ 0x00F3022A, /* PSno */
  /* F4 */ 0x00F40B0A, /* PSDnao */
  /* F5 */ 0x00F50225, /* PDno */
  /* F6 */ 0x00F60265, /* PDSxo */
  /* F7 */ 0x00F708C5, /* PDSano */
  /* F8 */ 0x00F802E5, /* PDSao */
  /* F9 */ 0x00F90845, /* PDSxno */
  /* FA */ 0x00FA0089, /* DPo */
  /* FB */ 0x00FB0A09, /* DPSnoo PATPAINT */
  /* FC */ 0x00FC008A, /* PSo */
  /* FD */ 0x00FD0A0A, /* PSDnoo */
  /* FE */ 0x00FE02A9, /* DPSoo */
  /* FF */ 0x00FF0062 /* 1 WHITENESS */
};
