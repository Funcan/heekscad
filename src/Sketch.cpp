// Sketch.cpp

#include "stdafx.h"
#include "Sketch.h"
#include "HLine.h"
#include "HArc.h"
#include "HeeksFrame.h"
#include "ObjPropsCanvas.h"
#include "../interface/PropertyInt.h"
#include "../interface/PropertyChoice.h"
#include "../tinyxml/tinyxml.h"

std::string CSketch::m_sketch_order_str[MaxSketchOrderTypes] = {
	std::string("empty"),
	std::string("open"),
	std::string("reverse"),
	std::string("bad"),
	std::string("good"),
	std::string("clockwise"),
	std::string("counter-clockwise")
};

CSketch::CSketch()
{
}

CSketch::CSketch(const CSketch& c)
{
	operator=(c);
}

CSketch::~CSketch()
{
}

const CSketch& CSketch::operator=(const CSketch& c)
{
	// just copy all the lines and arcs, not the id
	ObjList::operator =(c);

	return *this;
}

static std::map<int, int> order_map_for_properties; // maps drop-down index to SketchOrderType

static void on_set_order_type(int value, HeeksObj* object)
{
	std::map<int, int>::iterator FindIt = order_map_for_properties.find(value);
	if(FindIt != order_map_for_properties.end())
	{
		int order = FindIt->second;
		if(((CSketch*)object)->ReOrderSketch((SketchOrderType)order))
		{
			wxGetApp().m_frame->m_properties->RefreshByRemovingAndAddingAll();
		}
	}
}

static bool SketchOrderAvailable(SketchOrderType old_order, SketchOrderType new_order)
{
	// can we change from the older order type to the new order type?
	if(old_order == new_order)return true;

	switch(old_order)
	{
	case SketchOrderTypeOpen:
		switch(new_order)
		{
		case SketchOrderTypeReverse:
			return true;
		}
		break;

	case SketchOrderTypeBad:
		switch(new_order)
		{
		case SketchOrderTypeGood:
			return true;
		}
		break;

	case SketchOrderTypeCloseCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCCW:
			return true;
		}
		break;

	case SketchOrderTypeCloseCCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCW:
			return true;
		}
		break;
	}

	return false;
}

void CSketch::GetProperties(std::list<Property *> *list)
{
	list->push_back(new PropertyInt(_("Number of elements"), ObjList::GetNumChildren(), this));

	int initial_index = 0;
	std::list< wxString > choices;
	SketchOrderType sketch_order = GetSketchOrder();
	order_map_for_properties.clear();
	int j = 0;
	for(int i = 0; i< MaxSketchOrderTypes; i++)
	{
		if((SketchOrderType)i == sketch_order)initial_index = j;

		if(SketchOrderAvailable(sketch_order, (SketchOrderType)i))
		{
			order_map_for_properties.insert(std::pair<int, int>(j, i));
			choices.push_back(m_sketch_order_str[i].c_str());
			j++;
		}
	}
		
	list->push_back ( new PropertyChoice ( _("order"), choices, initial_index, this, on_set_order_type ) );

	ObjList::GetProperties(list);
}

HeeksObj *CSketch::MakeACopy(void)const
{
	return (ObjList*)(new CSketch(*this));
}

void CSketch::WriteXML(TiXmlNode *root)
{
	TiXmlElement * element = new TiXmlElement( "Sketch" );  
	root->LinkEndChild( element );
	WriteBaseXML(element);
}

// static member function
HeeksObj* CSketch::ReadFromXMLElement(TiXmlElement* pElem)
{
	CSketch* new_object = new CSketch;
	new_object->ReadBaseXML(pElem);

	return (ObjList*)new_object;
}

void CSketch::SetColor(const HeeksColor &col)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		object->SetColor(col);
	}
}

const HeeksColor* CSketch::GetColor()const
{
	if(m_objects.size() == 0)return NULL;
	return m_objects.front()->GetColor();
}

SketchOrderType CSketch::GetSketchOrder()
{
	if(m_objects.size() == 0)return SketchOrderTypeEmpty;

	HeeksObj* prev_object = NULL;
	HeeksObj* first_object = NULL;

	bool well_ordered = true;

	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;

		if(prev_object)
		{
			double prev_e[3], s[3];
			if(!prev_object->GetEndPoint(prev_e)){well_ordered = false; break;}
			if(!object->GetStartPoint(s)){well_ordered = false; break;}
			if(!(make_point(prev_e).IsEqual(make_point(s), wxGetApp().m_geom_tol))){well_ordered = false; break;}
		}

		if(first_object == NULL)first_object = object;
		prev_object = object;
	}

	if(well_ordered)
	{
		if(prev_object && first_object)
		{
			double e[3], s[3];
			if(prev_object->GetEndPoint(e))
			{
				if(first_object->GetStartPoint(s))
				{
					if(make_point(e).IsEqual(make_point(s), wxGetApp().m_geom_tol))
					{
						// closed
						if(GetClosedSketchTurningNumber() > 0)return SketchOrderTypeCloseCCW;
						return SketchOrderTypeCloseCW;
					}
				}
			}
		}

		return SketchOrderTypeOpen;
	}

	return SketchOrderTypeBad;
}

bool CSketch::ReOrderSketch(SketchOrderType new_order)
{
	SketchOrderType old_order = GetSketchOrder();
	bool done = false;

	switch(old_order)
	{
	case SketchOrderTypeOpen:
		switch(new_order)
		{
		case SketchOrderTypeReverse:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;

	case SketchOrderTypeBad:
		switch(new_order)
		{
		case SketchOrderTypeBad:
			break;
		default:
			ReLinkSketch();
			done = true;
			break;
		}
		break;

	case SketchOrderTypeCloseCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCCW:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;

	case SketchOrderTypeCloseCCW:
		switch(new_order)
		{
		case SketchOrderTypeCloseCW:
			ReverseSketch();
			done = true;
			break;
		default:
			break;
		}
		break;
	}

	return done;
}

void CSketch::ReLinkSketch()
{
	CSketchRelinker relinker(m_objects);

	if(relinker.Do())
	{
		wxGetApp().StartHistory();
		wxGetApp().DeleteUndoably(m_objects);
		wxGetApp().AddUndoably(relinker.m_new_list, this);
		wxGetApp().EndHistory();
	}
}

void CSketch::ReverseSketch()
{
	if(m_objects.size() == 0)return;

	std::list<HeeksObj*> new_list;

	for(std::list<HeeksObj*>::iterator It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		HeeksObj* copy = object->MakeACopy();

		// reverse object
		switch(object->GetType()){
			case LineType:
				((HLine*)copy)->Reverse();
				break;
			case ArcType:
				((HArc*)copy)->Reverse();
				break;
		}

		new_list.push_front(copy);
	}

	wxGetApp().StartHistory();
	wxGetApp().DeleteUndoably(m_objects);
	wxGetApp().AddUndoably(new_list, this);
	wxGetApp().EndHistory();
}

static gp_Vec GetSegmentVector(HeeksObj* object, double fraction)
{
	switch(object->GetType())
	{
	case LineType:
		return ((HLine*)object)->GetSegmentVector(fraction);
	case ArcType:
		return ((HArc*)object)->GetSegmentVector(fraction);
	default:
		return gp_Vec(0, 0, 0);
	}
}

double CSketch::GetAngleAtJunction(HeeksObj* prev_object, HeeksObj* object)
{
	gp_Vec prev_end_vector = GetSegmentVector(prev_object, 1.0);
	gp_Vec start_vector = GetSegmentVector(object, 0.0);

	double prev_segment_curvature = GetSegmentCurvature(prev_object);
	double segment_curvature = GetSegmentCurvature(object);

	return GetAngleBetweenVectors(prev_end_vector, start_vector, prev_segment_curvature, segment_curvature);
}

double CSketch::GetAngleBetweenVectors(const gp_Vec& v0, const gp_Vec& v1, double prev_segment_curvature, double segment_curvature)
{
	double start_angle = atan2(v0.Y(), v0.X());
	double end_angle = atan2(v1.Y(), v1.X());
	if(end_angle < start_angle)end_angle += 6.2831853071795;

	double diff = end_angle - start_angle;
	if(fabs(diff - 3.1415926535897932) < 0.000000000001){
		// back on itself
		double curvature_diff = segment_curvature - prev_segment_curvature;
		if(fabs(curvature_diff) < 0.00000000001)
		{
			// not the best case where they come in and go out at exactly the same direction and curve
		}
		else
		{
			if(curvature_diff< 0)diff = -3.1415926535897932;
		}
	}
	else if(diff > 3.1415926535897932){
		diff -= 6.2831853071795; // anti-clockwise direction more than 180 degrees is a clockwise direction
	}

	return diff;
}

int CSketch::GetSegmentType(HeeksObj* object)
{
	if(object->GetType() == ArcType)
	{
		if(((HArc*)object)->m_circle.Axis().Direction().Z() > 0)return 1;
		return -1;
	}

	return 0;
}

double CSketch::GetSegmentCurvature(HeeksObj* object)
{
	if(object->GetType() == ArcType)
	{
		int dir = (((HArc*)object)->m_circle.Axis().Direction().Z() > 0) ? 1:-1;
		return 1 / ((HArc*)object)->m_circle.Radius() * dir;
	}

	return 0.0;
}

int CSketch::GetClosedSketchTurningNumber()
{
	double turning_angle = 0.0;

	HeeksObj* prev_object = NULL;
	HeeksObj* first_object = NULL;

	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;

		// internal angle
		if(object->GetType() == ArcType)
		{
			gp_Vec v0 = ((HArc*)object)->GetSegmentVector(0.0);
			gp_Vec v1 = ((HArc*)object)->GetSegmentVector(1.0);
			double start_angle = atan2(v0.Y(), v0.X());
			double end_angle = atan2(v1.Y(), v1.X());
			bool ccw = (((HArc*)object)->m_circle.Axis().Direction().Z() > 0);
			if(ccw)
			{
				if(start_angle < end_angle)start_angle += 6.2831853071795;
			}
			else
			{
				if(end_angle < start_angle)end_angle += 6.2831853071795;
			}
			turning_angle += end_angle - start_angle;
		}

		if(prev_object)
		{
			turning_angle += GetAngleAtJunction(prev_object, object);
		}

		if(first_object == NULL)first_object = object;
		prev_object = object;
	}

	if(first_object && prev_object)
	{
		turning_angle += GetAngleAtJunction(prev_object, first_object);
	}

	double turning_number = turning_angle / 6.2831853071795;

	// round up or down to nearest int
	if(turning_number > 0.00001)turning_number += 0.5;
	else if(turning_number < -0.00001)turning_number -= 0.5;

	int i_turning_number = (int)turning_number;

	return i_turning_number;
}

bool CSketchRelinker::TryAdd(HeeksObj* object, bool front_not_back)
{
	if(m_added_from_old_set.find(object) == m_added_from_old_set.end())
	{
		double old_point[3];
		double new_point[3];
		if(front_not_back)m_new_front->GetEndPoint(old_point);
		else m_new_back->GetStartPoint(old_point);

		// try the object, the right way round
		if(front_not_back)object->GetStartPoint(new_point);
		else object->GetEndPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopy();
			m_new_list.push_back(new_object);
			if(front_not_back)m_new_front = new_object;
			else m_new_back = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}

		// try the object, the wrong way round
		if(front_not_back)object->GetEndPoint(new_point);
		else object->GetStartPoint(new_point);
		if(make_point(old_point).IsEqual(make_point(new_point), wxGetApp().m_geom_tol))
		{
			HeeksObj* new_object = object->MakeACopy();

			// reverse object
			switch(new_object->GetType()){
			case LineType:
				((HLine*)new_object)->Reverse();
				break;
			case ArcType:
				((HArc*)new_object)->Reverse();
				break;
			}

			m_new_list.push_back(new_object);
			if(front_not_back)m_new_front = new_object;
			else m_new_back = new_object;
			m_added_from_old_set.insert(object);
			return true;
		}
	}

	return false;
}

bool CSketchRelinker::AddNext()
{
	if(m_new_front)
	{
		bool added_at_front = false;
		std::list<HeeksObj*>::const_iterator It = m_old_front;
		do{
			It++;
			if(It == m_old_list.end())It = m_old_list.begin();
			HeeksObj* object = *It;

			added_at_front = TryAdd(object, true);

		}while(It != m_old_front && !added_at_front);

		if(added_at_front)return true;
		m_new_front = NULL;
	}

	return false;
}

bool CSketchRelinker::Do()
{
	if(m_old_list.size() > 0)
	{
		HeeksObj* new_object = m_old_list.front()->MakeACopy();
		m_new_list.push_back(new_object);
		m_added_from_old_set.insert(m_old_list.front());
		m_old_front = m_old_list.begin();
		m_old_back = m_old_list.begin();
		m_new_front = new_object;
		m_new_back = new_object;

		while(AddNext()){}
	}

	return true;
}
