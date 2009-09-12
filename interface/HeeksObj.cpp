// HeeksObj.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "HeeksObj.h"
#include "PropertyString.h"
#include "PropertyInt.h"
#include "PropertyColor.h"
#include "PropertyCheck.h"
#include "../tinyxml/tinyxml.h"
#ifdef HEEKSCAD
#include "ObjList.h"
#include "../src/Gripper.h"
#include "../src/HeeksFrame.h"
#include "../src/ObjPropsCanvas.h"
#endif

HeeksObj::HeeksObj(void): m_id(0), m_layer(0), m_visible(true){}

HeeksObj::HeeksObj(const HeeksObj& ho): m_id(0), m_layer(0), m_visible(true){operator=(ho);}

const HeeksObj& HeeksObj::operator=(const HeeksObj &ho)
{
	// don't copy the ID or the owner
	m_layer = ho.m_layer;
	m_visible = ho.m_visible;

	return *this;
}

HeeksObj::~HeeksObj()
{
	std::list<HeeksObj*>::iterator it;
	for(it = m_owners.begin(); it!= m_owners.end(); ++it)
	{
		(*it)->Remove(this);
	}
}

HeeksObj* HeeksObj::MakeACopyWithID()
{
	HeeksObj* ret = MakeACopy();
	ret->m_id = m_id;
	return ret;
}

void on_edit_string(const wxChar* value, HeeksObj* object)
{
	object->OnEditString(value);

	// to do , reconnect these two
//	wxGetApp().WasModified(object_for_properties);
//	wxGetApp().Repaint();
}

static void on_set_color(HeeksColor value, HeeksObj* object)
{
	object->SetColor(value);


#ifdef HEEKSCAD
	wxGetApp().m_frame->m_properties->OnApply2();
	wxGetApp().Repaint();
#else
	heeksCAD->PropertiesOnApply2();
	heeksCAD->Repaint();
#endif
}

static void on_set_id(int value, HeeksObj* object)
{
	object->SetID(value);
}

static void on_set_visible(bool value, HeeksObj* object)
{
	object->m_visible = value;
#ifdef HEEKSCAD
	wxGetApp().Repaint();
#else
	heeksCAD->Repaint();
#endif
}

void HeeksObj::GetProperties(std::list<Property *> *list)
{
	bool editable = CanEditString();
	list->push_back(new PropertyString(_("object type"), GetTypeString(), NULL));
	if(GetShortString())list->push_back(new PropertyString(_("object title"), GetShortString(), this, editable ? on_edit_string : NULL));
	if(UsesID())list->push_back(new PropertyInt(_("ID"), m_id, this, on_set_id));
	const HeeksColor* c = GetColor();
	if(c)
	{
		list->push_back ( new PropertyColor ( _("color"),  *c, this, on_set_color ) );
	}
	list->push_back(new PropertyCheck(_("visible"), m_visible, this, on_set_visible));
}

bool HeeksObj::GetScaleAboutMatrix(double *m)
{
#ifdef HEEKSCAD
	// return the bottom left corner of the box
	CBox box;
	GetBox(box);
	if(!box.m_valid)return false;
	gp_Trsf mat;
	mat.SetTranslationPart(gp_Vec(box.m_x[0], box.m_x[1], box.m_x[2]));
	extract(mat, m);
	return true;
#else
	return false;
#endif
}

void HeeksObj::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
#ifdef HEEKSCAD
	CBox box;
	GetBox(box);
	if(!box.m_valid)return;
	list->push_back(GripData(GripperTypeTranslate,box.m_x[0],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[3],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[0],box.m_x[4],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeScale,box.m_x[3],box.m_x[4],box.m_x[2],NULL));
#endif
}

void HeeksObj::OnRemove()
{
	KillGLLists();
}

void HeeksObj::SetID(int id)
{
#ifdef HEEKSCAD
	wxGetApp().SetObjectID(this, id);
#else
	heeksCAD->SetObjectID(this, id);
#endif
}

void HeeksObj::WriteBaseXML(TiXmlElement *element)
{
#ifdef HEEKSCAD
	wxGetApp().ObjectWriteBaseXML(this, element);
#else
	heeksCAD->ObjectWriteBaseXML(this, element);
#endif
}

void HeeksObj::ReadBaseXML(TiXmlElement* element)
{
#ifdef HEEKSCAD
	wxGetApp().ObjectReadBaseXML(this, element);
#else
	heeksCAD->ObjectReadBaseXML(this, element);
#endif
}

bool HeeksObj::OnVisibleLayer()
{
	// to do, support multiple layers.
	return true;
}

HeeksObj* HeeksObj::Owner()
{
	if(m_owners.size() == 0)return NULL;
	return *m_owners.begin();
}

void HeeksObj::SetOwner(HeeksObj *obj)
{
	m_owners.clear();
	AddOwner(obj);
}

bool HeeksObj::HasOwner(HeeksObj *obj)
{
	std::list<HeeksObj*>::iterator it;
	for(it = m_owners.begin(); it!= m_owners.end(); ++it)
	{
		if(*it == obj)
			return true;
	}
	return false;
}

bool HeeksObj::HasOwner()
{
	return !m_owners.empty();
}

void HeeksObj::RemoveOwners()
{
	m_owners.clear();
}

void HeeksObj::RemoveOwner(HeeksObj* obj)
{
	m_owners.remove(obj);
}

void HeeksObj::AddOwner(HeeksObj *obj)
{
	m_owners.push_back(obj);
}

HeeksObj* HeeksObj::GetFirstOwner()
{
	m_owners_it = m_owners.begin();
	return GetNextOwner();
}

HeeksObj* HeeksObj::GetNextOwner()
{
	if(m_owners_it != m_owners.end())
		return *m_owners_it++;
	return NULL;
}

