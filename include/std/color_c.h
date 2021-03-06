

#ifndef _STD_COLOR_C_H_
#define _STD_COLOR_C_H_


//////////////////////////////////////////////////////////////////////////////
// color def
#if 0
#define COLOR_DEFDEF(COLOR_DEF) \
  COLOR_DEF(212,208,200,CLR_SCROLLBAR)\
  COLOR_DEF(58,110,165,CLR_BACKGROUND)\
  COLOR_DEF(10,36,106,CLR_ACTIVECAPTION)\
  COLOR_DEF(128,128,128,CLR_INACTIVECAPTION)\
  COLOR_DEF(212,208,200,CLR_MENU)\
  COLOR_DEF(255,255,255,CLR_WINDOW)\
  COLOR_DEF(0,0,0,CLR_WINDOWFRAME)\
  COLOR_DEF(0,0,0,CLR_MENUTEXT)\
  COLOR_DEF(0,0,0,CLR_WINDOWTEXT)\
  COLOR_DEF(255,255,255,CLR_CAPTIONTEXT)\
  COLOR_DEF(212,208,200,CLR_ACTIVEBORDER)\
  COLOR_DEF(212,208,200,CLR_INACTIVEBORDER)\
  COLOR_DEF(128,128,128,CLR_APPWORKSPACE)\
  COLOR_DEF(10,36,106,CLR_HIGHLIGHT)\
  COLOR_DEF(255,255,255,CLR_HIGHLIGHTTEXT)\
  COLOR_DEF(212,208,200,CLR_BTNFACE)\
  COLOR_DEF(128,128,128,CLR_BTNSHADOW)\
  COLOR_DEF(128,128,128,CLR_GRAYTEXT)\
  COLOR_DEF(0,0,0,CLR_BTNTEXT)\
  COLOR_DEF(212,208,200,CLR_INACTIVECAPTIONTEXT)\
  COLOR_DEF(255,255,255,CLR_BTNHIGHLIGHT)\
  COLOR_DEF(64,64,64,CLR_3DDKSHADOW)\
  COLOR_DEF(212,208,200,CLR_3DLIGHT)\
  COLOR_DEF(0,0,0,CLR_INFOTEXT)\
  COLOR_DEF(255,255,225,CLR_INFOBK)\
  COLOR_DEF(181,181,181,CLR_UNKNOWN)\
  COLOR_DEF(0,0,128,CLR_HOTLIGHT)\
  COLOR_DEF(166,202,240,CLR_GRADIENTACTIVECAPTION)\
  COLOR_DEF(192,192,192,CLR_GRADIENTINACTIVECAPTION)\
  COLOR_DEF(49,106,197,CLR_MENUHILIGHT)\
COLOR_DEF(236,233,216,CLR_MENUBAR)


enum {
#define COLOR_DEF(r, g, b, id) id,
  COLOR_DEFDEF(COLOR_DEF)
#undef COLOR_DEF
  CLR_DESKTOP = CLR_BACKGROUND,
  CLR_3DFACE = CLR_BTNFACE,
  CLR_3DSHADOW = CLR_BTNSHADOW,
  CLR_3DHIGHLIGHT = CLR_BTNHIGHLIGHT,
  CLR_3DHILIGHT = CLR_BTNHIGHLIGHT,
  CLR_BTNHILIGHT = CLR_BTNHIGHLIGHT,
};
static COLOR myGetSysColor(int nIndex)
{
  enum { NUM_SYSCOLORS = 31 };
  static const COLOR SysColors[NUM_SYSCOLORS] = {
#define COLOR_DEF(r, g, b, id) _RGB(r, g, b),
    COLOR_DEFDEF(COLOR_DEF)
#undef COLOR_DEF
  };
  if (nIndex >= 0 && nIndex < NUM_SYSCOLORS) {
    return SysColors[nIndex];
  }
  return 0;
}
#endif

#define COLOR_DEFDEF(SYSCOLORDEF) \
  SYSCOLORDEF(SCROLLBAR, FFD4D0C8) \
  SYSCOLORDEF(BACKGROUND, FF3A6EA5) \
  SYSCOLORDEF(ACTIVECAPTION, FF0A246A) \
  SYSCOLORDEF(INACTIVECAPTION, FF808080) \
  SYSCOLORDEF(MENU, FFD4D0C8) \
  SYSCOLORDEF(WINDOW, FFFFFFFF) \
  SYSCOLORDEF(WINDOWFRAME, FF000000) \
  SYSCOLORDEF(MENUTEXT, FF000000) \
  SYSCOLORDEF(WINDOWTEXT, FF000000) \
  SYSCOLORDEF(CAPTIONTEXT, FFFFFFFF) \
  SYSCOLORDEF(ACTIVEBORDER, FFD4D0C8) \
  SYSCOLORDEF(INACTIVEBORDER, FFD4D0C8) \
  SYSCOLORDEF(APPWORKSPACE, FF808080) \
  SYSCOLORDEF(HIGHLIGHT, FF0A246A) \
  SYSCOLORDEF(HIGHLIGHTTEXT, FFFFFFFF) \
  SYSCOLORDEF(BTNFACE, FFD4D0C8) \
  SYSCOLORDEF(BTNSHADOW, FF808080) \
  SYSCOLORDEF(GRAYTEXT, FF808080) \
  SYSCOLORDEF(BTNTEXT, FF000000) \
  SYSCOLORDEF(INACTIVECAPTIONTEXT, FFD4D0C8) \
  SYSCOLORDEF(BTNHIGHLIGHT, FFFFFFFF) \
  SYSCOLORDEF(3DDKSHADOW, FF404040) \
  SYSCOLORDEF(3DLIGHT, FFD4D0C8) \
  SYSCOLORDEF(INFOTEXT, FF000000) \
  SYSCOLORDEF(INFOBK, FFFFFFE1) \
  SYSCOLORDEF(HOTLIGHT, FF000080) \
  SYSCOLORDEF(GRADIENTACTIVECAPTION, FFA6CAF0) \
  SYSCOLORDEF(GRADIENTINACTIVECAPTION, FFC0C0C0) \
  SYSCOLORDEF(DESKTOP, FF3A6EA5) \
  SYSCOLORDEF(3DFACE, FFD4D0C8) \
  SYSCOLORDEF(3DSHADOW, FF808080) \
  SYSCOLORDEF(3DHIGHLIGHT, FFFFFFFF) \
  SYSCOLORDEF(3DHILIGHT, FFFFFFFF) \
SYSCOLORDEF(BTNHILIGHT, FFFFFFFF)

enum {
#define SYSCOLORDEF(a, b)  CLR_##a=0x##b,
  COLOR_DEFDEF(SYSCOLORDEF)
#undef SYSCOLORDEF
};
#define _GetSysColor(_COL)  (uint32)(_COL)
#define myGetSysColor(_COL)  (uint32)(_COL)

#define COLORDEF_DEF(COLORDEF) \
  COLORDEF(AliceBlue,_RGB(240,248,255))\
  COLORDEF(AntiqueWhite,_RGB(250,235,215))\
  COLORDEF(Aqua,_RGB(0,255,255))\
  COLORDEF(Aquamarine,_RGB(127,255,212))\
  COLORDEF(Azure,_RGB(240,255,255))\
  COLORDEF(Beige,_RGB(245,245,220))\
  COLORDEF(Bisque,_RGB(255,228,196))\
  COLORDEF(Black,_RGB(0,0,0))\
  COLORDEF(BlanchedAlmond,_RGB(255,255,205))\
  COLORDEF(Blue,_RGB(0,0,255))\
  COLORDEF(BlueViolet,_RGB(138,43,226))\
  COLORDEF(Brown,_RGB(165,42,42))\
  COLORDEF(Burlywood,_RGB(222,184,135))\
  COLORDEF(CadetBlue,_RGB(95,158,160))\
  COLORDEF(Chartreuse,_RGB(127,255,0))\
  COLORDEF(Chocolate,_RGB(210,105,30))\
  COLORDEF(Coral,_RGB(255,127,80))\
  COLORDEF(CornflowerBlue,_RGB(100,149,237))\
  COLORDEF(Cornsilk,_RGB(255,248,220))\
  COLORDEF(Crimson,_RGB(220,20,60))\
  COLORDEF(Cyan,_RGB(0,255,255))\
  COLORDEF(DarkBlue,_RGB(0,0,139))\
  COLORDEF(DarkCyan,_RGB(0,139,139))\
  COLORDEF(DarkGoldenRod,_RGB(184,134,11))\
  COLORDEF(DarkGray,_RGB(169,169,169))\
  COLORDEF(DarkGreen,_RGB(0,100,0))\
  COLORDEF(DarkKhaki,_RGB(189,183,107))\
  COLORDEF(DarkMagenta,_RGB(139,0,139))\
  COLORDEF(DarkOliveGreen,_RGB(85,107,47))\
  COLORDEF(DarkOrange,_RGB(255,140,0))\
  COLORDEF(DarkOrchid,_RGB(153,50,204))\
  COLORDEF(DarkRed,_RGB(139,0,0))\
  COLORDEF(DarkSalmon,_RGB(233,150,122))\
  COLORDEF(DarkSeaGreen,_RGB(143,188,143))\
  COLORDEF(DarkSlateBlue,_RGB(72,61,139))\
  COLORDEF(DarkSlateGray,_RGB(47,79,79))\
  COLORDEF(DarkTurquoise,_RGB(0,206,209))\
  COLORDEF(DarkViolet,_RGB(148,0,211))\
  COLORDEF(DeepPink,_RGB(255,20,147))\
  COLORDEF(DeepSkyBlue,_RGB(0,191,255))\
  COLORDEF(DimGray,_RGB(105,105,105))\
  COLORDEF(DodgerBlue,_RGB(30,144,255))\
  COLORDEF(FireBrick,_RGB(178,34,34))\
  COLORDEF(FloralWhite,_RGB(255,250,240))\
  COLORDEF(ForestGreen,_RGB(34,139,34))\
  COLORDEF(Fuchsia,_RGB(255,0,255))\
  COLORDEF(Gainsboro,_RGB(220,220,220))\
  COLORDEF(GhostWhite,_RGB(248,248,255))\
  COLORDEF(Gold,_RGB(255,215,0))\
  COLORDEF(GoldenRod,_RGB(218,165,32))\
  COLORDEF(Gray,_RGB(127,127,127))\
  COLORDEF(Green,_RGB(0,128,0))\
  COLORDEF(GreenYellow,_RGB(173,255,47))\
  COLORDEF(HoneyDew,_RGB(240,255,240))\
  COLORDEF(HotPink,_RGB(255,105,180))\
  COLORDEF(IndianRed,_RGB(205,92,92))\
  COLORDEF(Indigo,_RGB(75,0,130))\
  COLORDEF(Ivory,_RGB(255,255,240))\
  COLORDEF(Khaki,_RGB(240,230,140))\
  COLORDEF(Lavender,_RGB(230,230,250))\
  COLORDEF(LavenderBlush,_RGB(255,240,245))\
  COLORDEF(Lawngreen,_RGB(124,252,0))\
  COLORDEF(LemonChiffon,_RGB(255,250,205))\
  COLORDEF(LightBlue,_RGB(173,216,230))\
  COLORDEF(LightCoral,_RGB(240,128,128))\
  COLORDEF(LightCyan,_RGB(224,255,255))\
  COLORDEF(LightGoldenRodYellow,_RGB(250,250,210))\
  COLORDEF(LightGreen,_RGB(144,238,144))\
  COLORDEF(LightGrey,_RGB(211,211,211))\
  COLORDEF(LightPink,_RGB(255,182,193))\
  COLORDEF(LightSalmon,_RGB(255,160,122))\
  COLORDEF(LightSeaGreen,_RGB(32,178,170))\
  COLORDEF(LightSkyBlue,_RGB(135,206,250))\
  COLORDEF(LightSlateGray,_RGB(119,136,153))\
  COLORDEF(LightSteelBlue,_RGB(176,196,222))\
  COLORDEF(LightYellow,_RGB(255,255,224))\
  COLORDEF(Lime,_RGB(0,255,0))\
  COLORDEF(LimeGreen,_RGB(50,205,50))\
  COLORDEF(Linen,_RGB(250,240,230))\
  COLORDEF(Magenta,_RGB(255,0,255))\
  COLORDEF(Maroon,_RGB(128,0,0))\
  COLORDEF(MediumAquamarine,_RGB(102,205,170))\
  COLORDEF(MediumBlue,_RGB(0,0,205))\
  COLORDEF(MediumOrchid,_RGB(186,85,211))\
  COLORDEF(MediumPurple,_RGB(147,112,219))\
  COLORDEF(MediumSeaGreen,_RGB(60,179,113))\
  COLORDEF(MediumSlateBlue,_RGB(123,104,238))\
  COLORDEF(MediumSpringGreen,_RGB(0,250,154))\
  COLORDEF(MediumTurquoise,_RGB(72,209,204))\
  COLORDEF(MediumVioletRed,_RGB(199,21,133))\
  COLORDEF(MidnightBlue,_RGB(25,25,112))\
  COLORDEF(MintCream,_RGB(245,255,250))\
  COLORDEF(MistyRose,_RGB(255,228,225))\
  COLORDEF(Moccasin,_RGB(255,228,181))\
  COLORDEF(NavajoWhite,_RGB(255,222,173))\
  COLORDEF(Navy,_RGB(0,0,128))\
  COLORDEF(OldLace,_RGB(253,245,230))\
  COLORDEF(Olive,_RGB(128,128,0))\
  COLORDEF(OliveDrab,_RGB(107,142,35))\
  COLORDEF(Orange,_RGB(255,165,0))\
  COLORDEF(OrangeRed,_RGB(255,69,0))\
  COLORDEF(Orchid,_RGB(218,112,214))\
  COLORDEF(PaleGoldenRod,_RGB(238,232,170))\
  COLORDEF(PaleGreen,_RGB(152,251,152))\
  COLORDEF(PaleTurquoise,_RGB(175,238,238))\
  COLORDEF(PaleVioletRed,_RGB(219,112,147))\
  COLORDEF(PapayaWhip,_RGB(255,239,213))\
  COLORDEF(PeachPuff,_RGB(255,218,185))\
  COLORDEF(Peru,_RGB(205,133,63))\
  COLORDEF(Pink,_RGB(255,192,203))\
  COLORDEF(Plum,_RGB(221,160,221))\
  COLORDEF(PowderBlue,_RGB(176,224,230))\
  COLORDEF(Purple,_RGB(128,0,128))\
  COLORDEF(Red,_RGB(255,0,0))\
  COLORDEF(RosyBrown,_RGB(188,143,143))\
  COLORDEF(RoyalBlue,_RGB(65,105,225))\
  COLORDEF(SaddleBrown,_RGB(139,69,19))\
  COLORDEF(Salmon,_RGB(250,128,114))\
  COLORDEF(SandyBrown,_RGB(244,164,96))\
  COLORDEF(SeaGreen,_RGB(46,139,87))\
  COLORDEF(SeaShell,_RGB(255,245,238))\
  COLORDEF(Sienna,_RGB(160,82,45))\
  COLORDEF(Silver,_RGB(192,192,192))\
  COLORDEF(SkyBlue,_RGB(135,206,235))\
  COLORDEF(SlateBlue,_RGB(106,90,205))\
  COLORDEF(SlateGray,_RGB(112,128,144))\
  COLORDEF(Snow,_RGB(255,250,250))\
  COLORDEF(SpringGreen,_RGB(0,255,127))\
  COLORDEF(SteelBlue,_RGB(70,130,180))\
  COLORDEF(Tan,_RGB(210,180,140))\
  COLORDEF(Teal,_RGB(0,128,128))\
  COLORDEF(Thistle,_RGB(216,191,216))\
  COLORDEF(Tomato,_RGB(255,99,71))\
  COLORDEF(Turquoise,_RGB(64,224,208))\
  COLORDEF(Violet,_RGB(238,130,238))\
  COLORDEF(Wheat,_RGB(245,222,179))\
  COLORDEF(White,_RGB(255,255,255))\
  COLORDEF(WhiteSmoke,_RGB(245,245,245))\
  COLORDEF(Yellow,_RGB(255,255,0))\
  COLORDEF(YellowGreen,_RGB(139,205,50))

enum {
#define COLORDEF(name, a) Color_##name = a,
  COLORDEF_DEF(COLORDEF)
#undef COLORDEF
};


// Color
// Construct an opaque Color object with
// the specified Red, Green, Blue values.
// Color values are not premultiplied.
// Common color constants
#define COLORTABLEDEF_DEF(COLORTABLEDEF) \
COLORTABLEDEF(AliceBlue, 0xFFF0F8FF) \
COLORTABLEDEF(AntiqueWhite, 0xFFFAEBD7) \
COLORTABLEDEF(Aqua, 0xFF00FFFF) \
COLORTABLEDEF(Aquamarine, 0xFF7FFFD4) \
COLORTABLEDEF(Azure, 0xFFF0FFFF) \
COLORTABLEDEF(Beige, 0xFFF5F5DC) \
COLORTABLEDEF(Bisque, 0xFFFFE4C4) \
COLORTABLEDEF(BlanchedAlmond, 0xFFFFEBCD) \
COLORTABLEDEF(Blue, 0xFF0000FF) \
COLORTABLEDEF(BlueViolet, 0xFF8A2BE2) \
COLORTABLEDEF(Brown, 0xFFA52A2A) \
COLORTABLEDEF(BurlyWood, 0xFFDEB887) \
COLORTABLEDEF(CadetBlue, 0xFF5F9EA0) \
COLORTABLEDEF(Chartreuse, 0xFF7FFF00) \
COLORTABLEDEF(Chocolate, 0xFFD2691E) \
COLORTABLEDEF(Coral, 0xFFFF7F50) \
COLORTABLEDEF(CornflowerBlue, 0xFF6495ED) \
COLORTABLEDEF(Cornsilk, 0xFFFFF8DC) \
COLORTABLEDEF(Crimson, 0xFFDC143C) \
COLORTABLEDEF(Cyan, 0xFF00FFFF) \
COLORTABLEDEF(DarkBlue, 0xFF00008B) \
COLORTABLEDEF(DarkCyan, 0xFF008B8B) \
COLORTABLEDEF(DarkGoldenrod, 0xFFB8860B) \
COLORTABLEDEF(DarkGray, 0xFFA9A9A9) \
COLORTABLEDEF(DarkGreen, 0xFF006400) \
COLORTABLEDEF(DarkKhaki, 0xFFBDB76B) \
COLORTABLEDEF(DarkMagenta, 0xFF8B008B) \
COLORTABLEDEF(DarkOliveGreen, 0xFF556B2F) \
COLORTABLEDEF(DarkOrange, 0xFFFF8C00) \
COLORTABLEDEF(DarkOrchid, 0xFF9932CC) \
COLORTABLEDEF(DarkRed, 0xFF8B0000) \
COLORTABLEDEF(DarkSalmon, 0xFFE9967A) \
COLORTABLEDEF(DarkSeaGreen, 0xFF8FBC8B) \
COLORTABLEDEF(DarkSlateBlue, 0xFF483D8B) \
COLORTABLEDEF(DarkSlateGray, 0xFF2F4F4F) \
COLORTABLEDEF(DarkTurquoise, 0xFF00CED1) \
COLORTABLEDEF(DarkViolet, 0xFF9400D3) \
COLORTABLEDEF(DeepPink, 0xFFFF1493) \
COLORTABLEDEF(DeepSkyBlue, 0xFF00BFFF) \
COLORTABLEDEF(DimGray, 0xFF696969) \
COLORTABLEDEF(DodgerBlue, 0xFF1E90FF) \
COLORTABLEDEF(Firebrick, 0xFFB22222) \
COLORTABLEDEF(FloralWhite, 0xFFFFFAF0) \
COLORTABLEDEF(ForestGreen, 0xFF228B22) \
COLORTABLEDEF(Fuchsia, 0xFFFF00FF) \
COLORTABLEDEF(Gainsboro, 0xFFDCDCDC) \
COLORTABLEDEF(GhostWhite, 0xFFF8F8FF) \
COLORTABLEDEF(Gold, 0xFFFFD700) \
COLORTABLEDEF(Goldenrod, 0xFFDAA520) \
COLORTABLEDEF(Gray, 0xFF808080) \
COLORTABLEDEF(Green, 0xFF008000) \
COLORTABLEDEF(GreenYellow, 0xFFADFF2F) \
COLORTABLEDEF(Honeydew, 0xFFF0FFF0) \
COLORTABLEDEF(HotPink, 0xFFFF69B4) \
COLORTABLEDEF(IndianRed, 0xFFCD5C5C) \
COLORTABLEDEF(Indigo, 0xFF4B0082) \
COLORTABLEDEF(Ivory, 0xFFFFFFF0) \
COLORTABLEDEF(Khaki, 0xFFF0E68C) \
COLORTABLEDEF(Lavender, 0xFFE6E6FA) \
COLORTABLEDEF(LavenderBlush, 0xFFFFF0F5) \
COLORTABLEDEF(LawnGreen, 0xFF7CFC00) \
COLORTABLEDEF(LemonChiffon, 0xFFFFFACD) \
COLORTABLEDEF(LightBlue, 0xFFADD8E6) \
COLORTABLEDEF(LightCoral, 0xFFF08080) \
COLORTABLEDEF(LightCyan, 0xFFE0FFFF) \
COLORTABLEDEF(LightGoldenrodYellow, 0xFFFAFAD2) \
COLORTABLEDEF(LightGray, 0xFFD3D3D3) \
COLORTABLEDEF(LightGreen, 0xFF90EE90) \
COLORTABLEDEF(LightPink, 0xFFFFB6C1) \
COLORTABLEDEF(LightSalmon, 0xFFFFA07A) \
COLORTABLEDEF(LightSeaGreen, 0xFF20B2AA) \
COLORTABLEDEF(LightSkyBlue, 0xFF87CEFA) \
COLORTABLEDEF(LightSlateGray, 0xFF778899) \
COLORTABLEDEF(LightSteelBlue, 0xFFB0C4DE) \
COLORTABLEDEF(LightYellow, 0xFFFFFFE0) \
COLORTABLEDEF(Lime, 0xFF00FF00) \
COLORTABLEDEF(LimeGreen, 0xFF32CD32) \
COLORTABLEDEF(Linen, 0xFFFAF0E6) \
COLORTABLEDEF(Magenta, 0xFFFF00FF) \
COLORTABLEDEF(Maroon, 0xFF800000) \
COLORTABLEDEF(MediumAquamarine, 0xFF66CDAA) \
COLORTABLEDEF(MediumBlue, 0xFF0000CD) \
COLORTABLEDEF(MediumOrchid, 0xFFBA55D3) \
COLORTABLEDEF(MediumPurple, 0xFF9370DB) \
COLORTABLEDEF(MediumSeaGreen, 0xFF3CB371) \
COLORTABLEDEF(MediumSlateBlue, 0xFF7B68EE) \
COLORTABLEDEF(MediumSpringGreen, 0xFF00FA9A) \
COLORTABLEDEF(MediumTurquoise, 0xFF48D1CC) \
COLORTABLEDEF(MediumVioletRed, 0xFFC71585) \
COLORTABLEDEF(MidnightBlue, 0xFF191970) \
COLORTABLEDEF(MintCream, 0xFFF5FFFA) \
COLORTABLEDEF(MistyRose, 0xFFFFE4E1) \
COLORTABLEDEF(Moccasin, 0xFFFFE4B5) \
COLORTABLEDEF(NavajoWhite, 0xFFFFDEAD) \
COLORTABLEDEF(Navy, 0xFF000080) \
COLORTABLEDEF(OldLace, 0xFFFDF5E6) \
COLORTABLEDEF(Olive, 0xFF808000) \
COLORTABLEDEF(OliveDrab, 0xFF6B8E23) \
COLORTABLEDEF(Orange, 0xFFFFA500) \
COLORTABLEDEF(OrangeRed, 0xFFFF4500) \
COLORTABLEDEF(Orchid, 0xFFDA70D6) \
COLORTABLEDEF(PaleGoldenrod, 0xFFEEE8AA) \
COLORTABLEDEF(PaleGreen, 0xFF98FB98) \
COLORTABLEDEF(PaleTurquoise, 0xFFAFEEEE) \
COLORTABLEDEF(PaleVioletRed, 0xFFDB7093) \
COLORTABLEDEF(PapayaWhip, 0xFFFFEFD5) \
COLORTABLEDEF(PeachPuff, 0xFFFFDAB9) \
COLORTABLEDEF(Peru, 0xFFCD853F) \
COLORTABLEDEF(Pink, 0xFFFFC0CB) \
COLORTABLEDEF(Plum, 0xFFDDA0DD) \
COLORTABLEDEF(PowderBlue, 0xFFB0E0E6) \
COLORTABLEDEF(Purple, 0xFF800080) \
COLORTABLEDEF(Red, 0xFFFF0000) \
COLORTABLEDEF(RosyBrown, 0xFFBC8F8F) \
COLORTABLEDEF(RoyalBlue, 0xFF4169E1) \
COLORTABLEDEF(SaddleBrown, 0xFF8B4513) \
COLORTABLEDEF(Salmon, 0xFFFA8072) \
COLORTABLEDEF(SandyBrown, 0xFFF4A460) \
COLORTABLEDEF(SeaGreen, 0xFF2E8B57) \
COLORTABLEDEF(SeaShell, 0xFFFFF5EE) \
COLORTABLEDEF(Sienna, 0xFFA0522D) \
COLORTABLEDEF(Silver, 0xFFC0C0C0) \
COLORTABLEDEF(SkyBlue, 0xFF87CEEB) \
COLORTABLEDEF(SlateBlue, 0xFF6A5ACD) \
COLORTABLEDEF(SlateGray, 0xFF708090) \
COLORTABLEDEF(Snow, 0xFFFFFAFA) \
COLORTABLEDEF(SpringGreen, 0xFF00FF7F) \
COLORTABLEDEF(SteelBlue, 0xFF4682B4) \
COLORTABLEDEF(Tan, 0xFFD2B48C) \
COLORTABLEDEF(Teal, 0xFF008080) \
COLORTABLEDEF(Thistle, 0xFFD8BFD8) \
COLORTABLEDEF(Tomato, 0xFFFF6347) \
COLORTABLEDEF(Transparent, 0x00FFFFFF) \
COLORTABLEDEF(Turquoise, 0xFF40E0D0) \
COLORTABLEDEF(Violet, 0xFFEE82EE) \
COLORTABLEDEF(Wheat, 0xFFF5DEB3) \
COLORTABLEDEF(White, 0xFFFFFFFF) \
COLORTABLEDEF(WhiteSmoke, 0xFFF5F5F5) \
COLORTABLEDEF(Yellow, 0xFFFFFF00) \
COLORTABLEDEF(YellowGreen, 0xFF9ACD32) \
COLORTABLEDEF(PureGreen, 0xFF00FF00) \
COLORTABLEDEF(DeepGreen, 0xFF00C000) \
COLORTABLEDEF(Black, 0xFF000000)

enum {
#define COLORTABLEDEF(name, b)  Color##name = b,
  COLORTABLEDEF_DEF(COLORTABLEDEF)
#undef COLORTABLEDEF
};


#define UICOLORDEF_DEF(COLORDEF) \
  COLORDEF(Col_Text,0.90f,0.90f,0.90f,1.00f)\
  COLORDEF(Col_TextDisabled,0.60f,0.60f,0.60f,1.00f)\
  COLORDEF(Col_WindowBg,0.00f,0.00f,0.00f,0.70f)\
  COLORDEF(Col_ChildWindowBg,0.00f,0.00f,0.00f,0.00f)\
  COLORDEF(Col_PopupBg,0.05f,0.05f,0.10f,0.90f)\
  COLORDEF(Col_Border,0.70f,0.70f,0.70f,0.65f)\
  COLORDEF(Col_BorderShadow,0.00f,0.00f,0.00f,0.00f)\
  COLORDEF(Col_FrameBg,0.80f,0.80f,0.80f,0.30f)\
  COLORDEF(Col_FrameBgHovered,0.90f,0.80f,0.80f,0.40f)\
  COLORDEF(Col_FrameBgActive,0.90f,0.65f,0.65f,0.45f)\
  COLORDEF(Col_TitleBg,0.27f,0.27f,0.54f,0.83f)\
  COLORDEF(Col_TitleBgCollapsed,0.40f,0.40f,0.80f,0.20f)\
  COLORDEF(Col_TitleBgActive,0.32f,0.32f,0.63f,0.87f)\
  COLORDEF(Col_MenuBarBg,0.40f,0.40f,0.55f,0.80f)\
  COLORDEF(Col_ScrollbarBg,0.20f,0.25f,0.30f,0.60f)\
  COLORDEF(Col_ScrollbarGrab,0.40f,0.40f,0.80f,0.30f)\
  COLORDEF(Col_ScrollbarGrabHovered,0.40f,0.40f,0.80f,0.40f)\
  COLORDEF(Col_ScrollbarGrabActive,0.80f,0.50f,0.50f,0.40f)\
  COLORDEF(Col_ComboBg,0.20f,0.20f,0.20f,0.99f)\
  COLORDEF(Col_CheckMark,0.90f,0.90f,0.90f,0.50f)\
  COLORDEF(Col_SliderGrab,1.00f,1.00f,1.00f,0.30f)\
  COLORDEF(Col_SliderGrabHovered,0.80f,0.50f,0.50f,0.50f)\
  COLORDEF(Col_SliderGrabActive,0.80f,0.50f,0.50f,1.00f)\
  COLORDEF(Col_LayoutSplit,1.00f,1.00f,1.00f,0.30f)\
  COLORDEF(Col_LayoutSplitHovered,1.0f,1.0f,1.0f,0.60f)\
  COLORDEF(Col_LayoutSplitActive,1.0f,1.0f,1.0f,0.90f)\
  COLORDEF(Col_Button,0.67f,0.40f,0.40f,0.60f)\
  COLORDEF(Col_ButtonHovered,0.67f,0.40f,0.40f,1.00f)\
  COLORDEF(Col_ButtonActive,0.80f,0.50f,0.50f,1.00f)\
  COLORDEF(Col_Header,0.40f,0.40f,0.90f,0.45f)\
  COLORDEF(Col_HeaderHovered,0.45f,0.45f,0.90f,0.80f)\
  COLORDEF(Col_HeaderActive,0.53f,0.53f,0.87f,0.80f)\
  COLORDEF(Col_Column,0.50f,0.50f,0.50f,1.00f)\
  COLORDEF(Col_ColumnHovered,0.70f,0.60f,0.60f,1.00f)\
  COLORDEF(Col_ColumnActive,0.90f,0.70f,0.70f,1.00f)\
  COLORDEF(Col_ResizeGrip,1.00f,1.00f,1.00f,0.30f)\
  COLORDEF(Col_ResizeGripHovered,1.00f,1.00f,1.00f,0.60f)\
  COLORDEF(Col_ResizeGripActive,1.00f,1.00f,1.00f,0.90f)\
  COLORDEF(Col_CloseButton,0.50f,0.50f,0.90f,0.50f)\
  COLORDEF(Col_CloseButtonHovered,0.70f,0.70f,0.90f,0.60f)\
  COLORDEF(Col_CloseButtonActive,0.70f,0.70f,0.70f,1.00f)\
  COLORDEF(Col_PlotLines,1.00f,1.00f,1.00f,1.00f)\
  COLORDEF(Col_PlotLinesHovered,0.90f,0.70f,0.00f,1.00f)\
  COLORDEF(Col_PlotHistogram,0.90f,0.70f,0.00f,1.00f)\
  COLORDEF(Col_PlotHistogramHovered,1.00f,0.60f,0.00f,1.00f)\
  COLORDEF(Col_TextSelectedBg,0.00f,0.00f,1.00f,0.35f)\
  COLORDEF(Col_ModalWindowDarkening,0.20f,0.20f,0.20f,0.35f)
enum GuiCol_ {
#define COLORDEF(id, r, g, b, a) id = _rgba(r, g, b, a),
  UICOLORDEF_DEF(COLORDEF)
#undef COLORDEF
  ImGuiCol_COUNT
};

#endif // _STD_COLOR_C_H_
