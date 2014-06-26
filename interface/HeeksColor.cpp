// HeeksColor.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "HeeksColor.h"

HeeksColor::HeeksColor(unsigned char r, unsigned char g, unsigned char b){
	red = r;
	green = g;
	blue = b;
}

HeeksColor::HeeksColor(long color)
{
	red   = (unsigned char)( color      & 0xff);
	green = (unsigned char)((color>> 8) & 0xff);
	blue  = (unsigned char)((color>>16) & 0xff);
}

HeeksColor HeeksColor::best_black_or_white(void)const{
	if(red + green + blue > 0x17e)return HeeksColor(0, 0, 0);
	else return HeeksColor(255, 255, 255);
}

void HeeksColor::glColor(void)const{
	::glColor3ub(red, green, blue);
}

void HeeksColor::glClearColor(GLclampf alpha)const{
	::glClearColor((float)red/255, (float)green/255, (float)blue/255, alpha);
}

long HeeksColor::COLORREF_color(void)const{
	return red | (green << 8) | (blue << 16);
}


// AutoCad color index -> RGB translate table
// from http://autolisp.mapcar.net/acifarben.html

typedef struct  {   
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} rgb_t;

static rgb_t aci2rgb[256] = {
//	 R,	G,	B ,	ACI
	{ 0,	0,	0 },	// 0
	{ 255,	0,	0 },	// 1
	{ 255,	255,	0 },	// 2
	{ 0,	255,	0 },	// 3
	{ 0,	255,	255 },	// 4
	{ 0,	0,	255 },	// 5
	{ 255,	0,	255 },	// 6
	{ 255,	255,	255 },	// 7
	{ 128,	128,	128 },	// 8
	{ 192,	192,	192 },	// 9
	{ 255,	0,	0 },	// 10
	{ 255,	127,	127 },	// 11
	{ 204,	0,	0 },	// 12
	{ 204,	102,	102 },	// 13
	{ 153,	0,	0 },	// 14
	{ 153,	76,	76 },	// 15
	{ 127,	0,	0 },	// 16
	{ 127,	63,	63 },	// 17
	{ 76,	0,	0 },	// 18
	{ 76,	38,	38 },	// 19
	{ 255,	63,	0 },	// 20
	{ 255,	159,	127 },	// 21
	{ 204,	51,	0 },	// 22
	{ 204,	127,	102 },	// 23
	{ 153,	38,	0 },	// 24
	{ 153,	95,	76 },	// 25
	{ 127,	31,	0 },	// 26
	{ 127,	79,	63 },	// 27
	{ 76,	19,	0 },	// 28
	{ 76,	47,	38 },	// 29
	{ 255,	127,	0 },	// 30
	{ 255,	191,	127 },	// 31
	{ 204,	102,	0 },	// 32
	{ 204,	153,	102 },	// 33
	{ 153,	76,	0 },	// 34
	{ 153,	114,	76 },	// 35
	{ 127,	63,	0 },	// 36
	{ 127,	95,	63 },	// 37
	{ 76,	38,	0 },	// 38
	{ 76,	57,	38 },	// 39
	{ 255,	191,	0 },	// 40
	{ 255,	223,	127 },	// 41
	{ 204,	153,	0 },	// 42
	{ 204,	178,	102 },	// 43
	{ 153,	114,	0 },	// 44
	{ 153,	133,	76 },	// 45
	{ 127,	95,	0 },	// 46
	{ 127,	111,	63 },	// 47
	{ 76,	57,	0 },	// 48
	{ 76,	66,	38 },	// 49
	{ 255,	255,	0 },	// 50
	{ 255,	255,	127 },	// 51
	{ 204,	204,	0 },	// 52
	{ 204,	204,	102 },	// 53
	{ 153,	153,	0 },	// 54
	{ 153,	153,	76 },	// 55
	{ 127,	127,	0 },	// 56
	{ 127,	127,	63 },	// 57
	{ 76,	76,	0 },	// 58
	{ 76,	76,	38 },	// 59
	{ 191,	255,	0 },	// 60
	{ 223,	255,	127 },	// 61
	{ 153,	204,	0 },	// 62
	{ 178,	204,	102 },	// 63
	{ 114,	153,	0 },	// 64
	{ 133,	153,	76 },	// 65
	{ 95,	127,	0 },	// 66
	{ 111,	127,	63 },	// 67
	{ 57,	76,	0 },	// 68
	{ 66,	76,	38 },	// 69
	{ 127,	255,	0 },	// 70
	{ 191,	255,	127 },	// 71
	{ 102,	204,	0 },	// 72
	{ 153,	204,	102 },	// 73
	{ 76,	153,	0 },	// 74
	{ 114,	153,	76 },	// 75
	{ 63,	127,	0 },	// 76
	{ 95,	127,	63 },	// 77
	{ 38,	76,	0 },	// 78
	{ 57,	76,	38 },	// 79
	{ 63,	255,	0 },	// 80
	{ 159,	255,	127 },	// 81
	{ 51,	204,	0 },	// 82
	{ 127,	204,	102 },	// 83
	{ 38,	153,	0 },	// 84
	{ 95,	153,	76 },	// 85
	{ 31,	127,	0 },	// 86
	{ 79,	127,	63 },	// 87
	{ 19,	76,	0 },	// 88
	{ 47,	76,	38 },	// 89
	{ 0,	255,	0 },	// 90
	{ 127,	255,	127 },	// 91
	{ 0,	204,	0 },	// 92
	{ 102,	204,	102 },	// 93
	{ 0,	153,	0 },	// 94
	{ 76,	153,	76 },	// 95
	{ 0,	127,	0 },	// 96
	{ 63,	127,	63 },	// 97
	{ 0,	76,	0 },	// 98
	{ 38,	76,	38 },	// 99
	{ 0,	255,	63 },	// 100
	{ 127,	255,	159 },	// 101
	{ 0,	204,	51 },	// 102
	{ 102,	204,	127 },	// 103
	{ 0,	153,	38 },	// 104
	{ 76,	153,	95 },	// 105
	{ 0,	127,	31 },	// 106
	{ 63,	127,	79 },	// 107
	{ 0,	76,	19 },	// 108
	{ 38,	76,	47 },	// 109
	{ 0,	255,	127 },	// 110
	{ 127,	255,	191 },	// 111
	{ 0,	204,	102 },	// 112
	{ 102,	204,	153 },	// 113
	{ 0,	153,	76 },	// 114
	{ 76,	153,	114 },	// 115
	{ 0,	127,	63 },	// 116
	{ 63,	127,	95 },	// 117
	{ 0,	76,	38 },	// 118
	{ 38,	76,	57 },	// 119
	{ 0,	255,	191 },	// 120
	{ 127,	255,	223 },	// 121
	{ 0,	204,	153 },	// 122
	{ 102,	204,	178 },	// 123
	{ 0,	153,	114 },	// 124
	{ 76,	153,	133 },	// 125
	{ 0,	127,	95 },	// 126
	{ 63,	127,	111 },	// 127
	{ 0,	76,	57 },	// 128
	{ 38,	76,	66 },	// 129
	{ 0,	255,	255 },	// 130
	{ 127,	255,	255 },	// 131
	{ 0,	204,	204 },	// 132
	{ 102,	204,	204 },	// 133
	{ 0,	153,	153 },	// 134
	{ 76,	153,	153 },	// 135
	{ 0,	127,	127 },	// 136
	{ 63,	127,	127 },	// 137
	{ 0,	76,	76 },	// 138
	{ 38,	76,	76 },	// 139
	{ 0,	191,	255 },	// 140
	{ 127,	223,	255 },	// 141
	{ 0,	153,	204 },	// 142
	{ 102,	178,	204 },	// 143
	{ 0,	114,	153 },	// 144
	{ 76,	133,	153 },	// 145
	{ 0,	95,	127 },	// 146
	{ 63,	111,	127 },	// 147
	{ 0,	57,	76 },	// 148
	{ 38,	66,	76 },	// 149
	{ 0,	127,	255 },	// 150
	{ 127,	191,	255 },	// 151
	{ 0,	102,	204 },	// 152
	{ 102,	153,	204 },	// 153
	{ 0,	76,	153 },	// 154
	{ 76,	114,	153 },	// 155
	{ 0,	63,	127 },	// 156
	{ 63,	95,	127 },	// 157
	{ 0,	38,	76 },	// 158
	{ 38,	57,	76 },	// 159
	{ 0,	63,	255 },	// 160
	{ 127,	159,	255 },	// 161
	{ 0,	51,	204 },	// 162
	{ 102,	127,	204 },	// 163
	{ 0,	38,	153 },	// 164
	{ 76,	95,	153 },	// 165
	{ 0,	31,	127 },	// 166
	{ 63,	79,	127 },	// 167
	{ 0,	19,	76 },	// 168
	{ 38,	47,	76 },	// 169
	{ 0,	0,	255 },	// 170
	{ 127,	127,	255 },	// 171
	{ 0,	0,	204 },	// 172
	{ 102,	102,	204 },	// 173
	{ 0,	0,	153 },	// 174
	{ 76,	76,	153 },	// 175
	{ 0,	0,	127 },	// 176
	{ 63,	63,	127 },	// 177
	{ 0,	0,	76 },	// 178
	{ 38,	38,	76 },	// 179
	{ 63,	0,	255 },	// 180
	{ 159,	127,	255 },	// 181
	{ 51,	0,	204 },	// 182
	{ 127,	102,	204 },	// 183
	{ 38,	0,	153 },	// 184
	{ 95,	76,	153 },	// 185
	{ 31,	0,	127 },	// 186
	{ 79,	63,	127 },	// 187
	{ 19,	0,	76 },	// 188
	{ 47,	38,	76 },	// 189
	{ 127,	0,	255 },	// 190
	{ 191,	127,	255 },	// 191
	{ 102,	0,	204 },	// 192
	{ 153,	102,	204 },	// 193
	{ 76,	0,	153 },	// 194
	{ 114,	76,	153 },	// 195
	{ 63,	0,	127 },	// 196
	{ 95,	63,	127 },	// 197
	{ 38,	0,	76 },	// 198
	{ 57,	38,	76 },	// 199
	{ 191,	0,	255 },	// 200
	{ 223,	127,	255 },	// 201
	{ 153,	0,	204 },	// 202
	{ 178,	102,	204 },	// 203
	{ 114,	0,	153 },	// 204
	{ 133,	76,	153 },	// 205
	{ 95,	0,	127 },	// 206
	{ 111,	63,	127 },	// 207
	{ 57,	0,	76 },	// 208
	{ 66,	38,	76 },	// 209
	{ 255,	0,	255 },	// 210
	{ 255,	127,	255 },	// 211
	{ 204,	0,	204 },	// 212
	{ 204,	102,	204 },	// 213
	{ 153,	0,	153 },	// 214
	{ 153,	76,	153 },	// 215
	{ 127,	0,	127 },	// 216
	{ 127,	63,	127 },	// 217
	{ 76,	0,	76 },	// 218
	{ 76,	38,	76 },	// 219
	{ 255,	0,	191 },	// 220
	{ 255,	127,	223 },	// 221
	{ 204,	0,	153 },	// 222
	{ 204,	102,	178 },	// 223
	{ 153,	0,	114 },	// 224
	{ 153,	76,	133 },	// 225
	{ 127,	0,	95 },	// 226
	{ 127,	63,	111 },	// 227
	{ 76,	0,	57 },	// 228
	{ 76,	38,	66 },	// 229
	{ 255,	0,	127 },	// 230
	{ 255,	127,	191 },	// 231
	{ 204,	0,	102 },	// 232
	{ 204,	102,	153 },	// 233
	{ 153,	0,	76 },	// 234
	{ 153,	76,	114 },	// 235
	{ 127,	0,	63 },	// 236
	{ 127,	63,	95 },	// 237
	{ 76,	0,	38 },	// 238
	{ 76,	38,	57 },	// 239
	{ 255,	0,	63 },	// 240
	{ 255,	127,	159 },	// 241
	{ 204,	0,	51 },	// 242
	{ 204,	102,	127 },	// 243
	{ 153,	0,	38 },	// 244
	{ 153,	76,	95 },	// 245
	{ 127,	0,	31 },	// 246
	{ 127,	63,	79 },	// 247
	{ 76,	0,	19 },	// 248
	{ 76,	38,	47 },	// 249
	{ 51,	51,	51 },	// 250
	{ 91,	91,	91 },	// 251
	{ 132,	132,	132 },	// 252
	{ 173,	173,	173 },	// 253
	{ 214,	214,	214 },	// 254
	{ 255,	255,	255 },	// 255
};

HeeksColor::HeeksColor(Aci_t aci)
{
        aci = abs(aci);		// hidden layers have negative color values
	if (aci >= 256)         // undereferenced color-by layer -> black
	  aci = 0;
	red   = aci2rgb[aci].red;
	green = aci2rgb[aci].green;
	blue  = aci2rgb[aci].blue;
}
