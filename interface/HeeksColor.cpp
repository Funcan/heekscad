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


void HeeksColor::glMaterial(double opacity, GLenum face)const{
    GLfloat matf[4];
	matf[3] = (GLfloat)opacity;
	if(opacity < 1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(0);
	}
	matf[0] = 0.1f * (GLfloat)red /255;
	matf[1] = 0.1f * (GLfloat)green / 255;
	matf[2] = 0.1f * (GLfloat)blue / 255;
	glMaterialfv (face, GL_AMBIENT, matf);
	matf[0] = 0.8f * (GLfloat)red / 255;
	matf[1] = 0.8f * (GLfloat)green / 255;
	matf[2] = 0.8f * (GLfloat)blue / 255;
	glMaterialfv (face, GL_DIFFUSE, matf);
	matf[0] = 0.3f;
	matf[1] = 0.3f;
	matf[2] = 0.3f;
	glMaterialfv (face, GL_SPECULAR, matf);
	matf[0] = 0.2f;
	matf[1] = 0.2f;
	matf[2] = 0.2f;
	glMaterialfv (face, GL_EMISSION, matf);
    glMaterialf (face, GL_SHININESS, 128.0);
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
