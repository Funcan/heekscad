// HeeksCAD.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksColor.h"
#include "../interface/ObjList.h"
#include "glfont2.h"

class TransientObject;
class MagDragWindow;
class ViewRotating;
class ViewZooming;
class CInputMode;
class CSelectMode;
class DigitizeMode;
class MarkedList;
class GripperMode;
class HeeksObj;
class MarkedObject;
class Gripper;
class CViewPoint;
class UndoEngine;
class Observer;
class CHeeksFrame;
class wxDynamicLibrary;
class CoordinateSystem;
class HRuler;
class wxConfigBase;
class wxAuiManager;
class CSketch;

#define MAX_RECENT_FILES 20

enum GraphicsTextMode
{
	GraphicsTextModeNone,
	GraphicsTextModeInputTitle,
	GraphicsTextModeWithHelp
};

enum FileOpenOrImportType
{
	FileOpenOrImportTypeOther,
	FileOpenTypeHeeks,
	FileImportTypeHeeks,
	FileOpenOrImportTypeDxf
};

class HeeksCADapp : public wxApp, public ObjList
{
private:
	std::set<Observer*> observers;
	UndoEngine *history;
	std::map<HeeksObj*,std::list<HeeksObj*> > m_transient_objects;
	std::map< int, std::map<int, HeeksObj*> > used_ids; // map of group type ( usually same as object type ) to "map of ID to object"
	std::map< int, int > next_id_map;
	std::map< std::string, HeeksObj*(*)(TiXmlElement* pElem) > xml_read_fn_map;

	void render_screen_text2(const wxChar* str);
	void RenderDatumOrCurrentCoordSys(bool select);

protected:
    wxLocale m_locale; // locale we'll be using
	bool m_locale_initialised;

public:
	HeeksCADapp();
	~HeeksCADapp();

	wxPoint cur_mouse_pos;
	HeeksColor current_color;
	HeeksColor construction_color;
	HeeksColor background_color;
	bool m_gl_font_initialized;
	int m_rotate_mode;
	bool m_antialiasing;
	bool digitize_end;
	bool digitize_inters;
	bool digitize_centre;
	bool digitize_midpoint;
	bool digitize_nearest;
	bool digitize_coords;
	bool digitize_screen;
	bool digitize_tangent;
	double digitizing_radius; // for ambiguous arcs and circles
	bool draw_to_grid;
	bool autosolve_constraints;
	bool allow3DRotaion;
	double digitizing_grid;
	bool mouse_wheel_forward_away; // true for forwards/backwards = zoom out / zoom in, false for reverse
	bool ctrl_does_rotate; // true - rotate on Ctrl, pan when not Ctrl      false - rotate when not Ctrl, pan when Ctrl
	//gp_Trsf digitizing_matrix;
	CoordinateSystem *m_current_coordinate_system;
	CInputMode *input_mode_object;
	MagDragWindow *magnification;
	ViewRotating *viewrotating;
	ViewZooming *viewzooming;
	CSelectMode *m_select_mode;
	DigitizeMode *m_digitizing;
	GripperMode* gripper_mode;
	int grid_mode;
	Gripper *drag_gripper;
	gp_Pnt grip_from, grip_to;
	Gripper *cursor_gripper;
	CHeeksFrame *m_frame;
	MarkedList *m_marked_list;
	bool m_doing_rollback;
	wxString m_filepath;
	bool m_light_push_matrix;
	std::list<HeeksObj*> m_hidden_for_drag;
	bool m_show_grippers_on_drag;
	double m_geom_tol;
	std::list<wxDynamicLibrary*> m_loaded_libraries;
	std::list< void(*)() > m_on_glCommands_list;
	std::list< wxToolBarBase* > m_external_toolbars;
	std::list< void(*)() > m_AddToolBars_list;
	std::list<wxWindow*> m_hideable_windows;
	HRuler* m_ruler;
	bool m_show_ruler;
	bool m_show_datum_coords_system;
	bool m_datum_coords_system_solid_arrows;
	std::list< wxString > m_recent_files;
	bool m_in_OpenFile;
	bool m_mark_newly_added_objects;
	wxString m_version_number;
	std::list< void(*)(wxSizeEvent&) > m_on_graphics_size_list;
	std::list< void(*)(wxMouseEvent&) > m_lbutton_up_callbacks;
	std::list< void(*)(bool) > m_on_save_callbacks;
	std::list< bool(*)() > m_is_modified_callbacks;
	int m_transform_gl_list;
	gp_Trsf m_drag_matrix;
	bool m_extrude_removes_sketches;
	bool m_loft_removes_sketches;
	bool m_font_created;
	glfont::GLFont m_gl_font;
	bool m_sketch_mode;
	CSketch* m_sketch;
	unsigned int m_font_tex_number;
	GraphicsTextMode m_graphics_text_mode;
	bool m_print_scaled_to_page;
	wxPrintData *m_printData;
	wxPageSetupDialogData* m_pageSetupData;
	FileOpenOrImportType m_file_open_or_import_type;
	double m_view_units; // units to display to the user ( but everything is stored as mm ), 1.0 for mm, 25.4 for inches

	double m_min_correlation_factor;
	double m_max_scale_threshold;
	int m_number_of_sample_points;
	bool m_property_grid_validation;

	//WxApp override
	int OnRun();
	bool OnExceptionInMainLoop();

	// HeeksObj's virtual functions
	void GetBox(CBox &box);
	void glCommands(bool select, bool marked, bool no_color);
	bool CanAdd(HeeksObj* object){return true;}
	int GetType()const{return DocumentType;}

	virtual bool OnInit();
	int OnExit();
	void CreateLights(void);
	void DestroyLights(void);
	void FindMarkedObject(const wxPoint &point, MarkedObject* marked_object);
	void SetInputMode(CInputMode *i);
	void Repaint(bool soon = false);
	void RecalculateGLLists();
	void SetLikeNewFile(void);
	bool IsModified(void);
	void ClearHistory(void);
	void glCommandsAll(bool select, const CViewPoint &view_point);
	double GetPixelScale(void);
	void DoDropDownMenu(wxWindow *wnd, const wxPoint &point, MarkedObject* marked_object, bool dont_use_point_for_functions, bool from_graphics_canvas, bool control_pressed);
	void on_menu_event(wxCommandEvent& event);
	void DoToolUndoably(Tool *);
	void Undo(void);
	void Redo(void);
	void WentTransient(HeeksObj* obj, TransientObject* tobj);
	void ClearTransients();
	std::map<HeeksObj*,std::list<HeeksObj*> >& GetTransients();
	bool Add(HeeksObj* object, HeeksObj* prev_object);
	void Remove(HeeksObj* object);
	void Remove(std::list<HeeksObj*> objects);
	void Transform(std::list<HeeksObj*> objects, double *m);
	void Reset();
	HeeksObj* ReadXMLElement(TiXmlElement* pElem);
	void ObjectWriteBaseXML(HeeksObj *object, TiXmlElement *element);
	void ObjectReadBaseXML(HeeksObj *object, TiXmlElement* element);
	void InitializeXMLFunctions();
	void OpenXMLFile(const wxChar *filepath,HeeksObj* paste_into = NULL);
	void OpenSVGFile(const wxChar *filepath);
	void OpenSTLFile(const wxChar *filepath);
	void OpenDXFFile(const wxChar *filepath);
	void OpenRS274XFile(const wxChar *filepath);
	bool OpenImageFile(const wxChar *filepath);
	bool OpenFile(const wxChar *filepath, bool import_not_open = false, HeeksObj* paste_into = NULL);
	void SaveDXFFile(const wxChar *filepath);
	void SaveSTLFile(const std::list<HeeksObj*>& objects, const wxChar *filepath);
	void SaveCPPFile(const std::list<HeeksObj*>& objects, const wxChar *filepath);
	void SaveXMLFile(const std::list<HeeksObj*>& objects, const wxChar *filepath, bool for_clipboard = false);
	void SaveXMLFile(const wxChar *filepath){SaveXMLFile(m_objects, filepath);}
	bool SaveFile(const wxChar *filepath, bool use_dialog = false, bool update_recent_file_list = true, bool set_app_caption = true);
	void CreateUndoPoint();
	void Changed();
	gp_Trsf GetDrawMatrix(bool get_the_appropriate_orthogonal);
	void GetOptions(std::list<Property *> *list);
	void DeleteMarkedItems();
	void glColorEnsuringContrast(const HeeksColor &c);
	void RegisterObserver(Observer* observer);
	void RemoveObserver(Observer* observer);
	void ObserversOnChange(const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed, const std::list<HeeksObj*>* modified);
	void ObserversMarkedListChanged(bool selection_cleared, const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed);
	void ObserversFreeze();
	void ObserversThaw();
	const wxChar* GetKnownFilesWildCardString(bool open = true)const;
	const wxChar* GetKnownFilesCommaSeparatedList(bool open = true)const;
	void GetTools(MarkedObject* marked_object, std::list<Tool*>& t_list, const wxPoint& point, bool from_graphics_canvas, bool control_pressed);
	wxString GetExeFolder()const;
	wxString GetResFolder()const;
	void get_2d_arc_segments(double xs, double ys, double xe, double ye, double xc, double yc, bool dir, bool want_start, double pixels_per_mm, void(*callbackfunc)(const double* xy));
	void PassMouseWheelToGraphics(wxMouseEvent& event);
	int PickObjects(const wxChar* str, long marking_filter = -1, bool just_one = false);
	bool PickPosition(const wxChar* str, double* pos, void(*callback)(const double*) = NULL);
	void glSphere(double radius, const double* pos = NULL);
	void OnNewOrOpen(bool open);
	void RegisterHideableWindow(wxWindow* w);
	void RemoveHideableWindow(wxWindow* w);
	void RegisterReadXMLfunction(const char* type_name, HeeksObj*(*read_xml_function)(TiXmlElement* pElem));
	void GetRecentFilesProfileString();
	void WriteRecentFilesProfileString(wxConfigBase &config);
	void InsertRecentFileItem(const wxChar* filepath);
	bool CheckForModifiedDoc(); // returns true, if OK to continue with file open etc.
	void SetFrameTitle();
	HeeksObj* GetIDObject(int type, int id);
	void SetObjectID(HeeksObj* object, int id);
	int GetNextID(int type);
	void RemoveID(HeeksObj* object); // only call this from ObjList::Remove()
	void ResetIDs();
	bool InputDouble(const wxChar* prompt, const wxChar* value_name, double &value);
	void RegisterOnGLCommands( void(*callbackfunc)() );
	void RemoveOnGLCommands( void(*callbackfunc)() );
	void RegisterOnGraphicsSize( void(*callbackfunc)(wxSizeEvent&) );
	void RemoveOnGraphicsSize( void(*callbackfunc)(wxSizeEvent&) );
	void RegisterOnMouseFn( void(*callbackfunc)(wxMouseEvent&) );
	void RemoveOnMouseFn( void(*callbackfunc)(wxMouseEvent&) );
	void RegisterOnSaveFn( void(*callbackfunc)(bool) );
	void RegisterIsModifiedFn( bool(*callbackfunc)() );
	void RegisterToolBar( wxToolBarBase* );
	void CreateTransformGLList(const std::list<HeeksObj*>& list, bool show_grippers_on_drag);
	void DestroyTransformGLList();
	bool IsPasteReady();
	void Paste(HeeksObj* paste_into);
	bool CheckForNOrMore(const std::list<HeeksObj*> &list, int min_num, int type, const wxString& msg, const wxString& caption);
	bool CheckForNOrMore(const std::list<HeeksObj*> &list, int min_num, int type1, int type2, const wxString& msg, const wxString& caption);
	void render_text(const wxChar* str);
	bool get_text_size(const wxChar* str, float* width, float* height);
	void render_screen_text(const wxChar* str1, const wxChar* str2);
	void OnInputModeTitleChanged();
	void OnInputModeHelpTextChanged();
	void PlotSetColor(const HeeksColor &c);
	void PlotLine(const double* s, const double* e);
	void PlotArc(const double* s, const double* e, const double* c);
	void InitialiseLocale();
	void create_font();
	CSketch* GetContainer();
	bool EndSketchMode();
	void SetStatusText();
};

DECLARE_APP(HeeksCADapp)

