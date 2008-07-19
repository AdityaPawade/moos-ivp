/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: July 9th, 2008                                       */
/*****************************************************************/

#include <iostream>
#include "VPlug_GeoShapes.h"
#include "MBUtils.h"
#include "XYBuildUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_GeoShapes::VPlug_GeoShapes()
{
  setColorMapping("polygon_edge_color", "yellow");
  setColorMapping("polygon_vertex_color", "red");
  setColorMapping("polygon_label_color", "khaki");
  setColorMapping("seglist_edge_color", "white");
  setColorMapping("seglist_vertex_color", "dark_blue");
  setColorMapping("seglist_label_color", "orange");
  setColorMapping("circle_edge_color", "yellow");
  setColorMapping("grid_edge_color", "white");
  setColorMapping("point_vertex_color", "yellow");

  m_viewable_map["polygon_viewable_labels"] = true;
  m_viewable_map["polygon_viewable_all"]    = true;
  m_viewable_map["seglist_viewable_all"]    = true;
  m_viewable_map["point_viewable_all"]      = true;

  m_gsize_map["polygon_edge_width"]  = 1.0;
  m_gsize_map["polygon_vertex_size"] = 5.0;
  m_gsize_map["seglist_edge_width"]  = 1.0;
  m_gsize_map["seglist_vertex_size"] = 5.0;
  m_gsize_map["grid_edge_width"]     = 2.0;
  m_gsize_map["circle_edge_width"]   = 2.0;
  m_gsize_map["point_vertex_size"]   = 4.0;
}

//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: The possibly accepted parameters are unlimited, but 
//            must fit one of the below patterns. The following is 
//            a list (perhaps incomplete) of acceptable parameters.
//    Each accepting a "color string": 
//         polygon_edge_color
//         polygon_vertex_color
//         polygon_label_color
//         seglist_edge_color
//         seglist_vertex_color
//         seglist_label_color
//         grid_vertex_color
//         circle_edge_color
//         point_vertex_color
//         point_label_color
//    Each accepting a "bool string": 
//         polygon_viewable_all
//         polygon_viewable_labels
//         seglist_viewable_all
//         seglist_viewable_labels
//         grid_viewable_all
//         grid_viewable_labels
//         circle_viewable_all
//         circle_viewable_labels
//         point_viewable_all
//         point_viewable_labels
//    Each accepting a "double string": 
//         polygon_line_width
//         polygon_vertex_size
//         seglist_line_width
//         seglist_vertex_size
//         grid_line_width
//         circle_line_width
//         point_vertex_size

bool VPlug_GeoShapes::setParam(string param, string value)
{
  if(strContains(param, "_viewable_"))
    return(setViewableMapping(param, value));
  else if(strContains(param, "_color"))
    return(setColorMapping(param, value));
  else if(strContains(param, "_width"))
    return(setGSizeMapping(param, value));
  else if(strContains(param, "_size"))
    return(setGSizeMapping(param, value));
  else
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: addPolyon

void VPlug_GeoShapes::addPolygon(const XYPolygon& new_poly)
{
  string new_label = new_poly.get_label();
  if(new_label == "") {
    m_polygons.push_back(new_poly);
    return;
  }

  for(int i=0; i<m_polygons.size(); i++) {
    if(m_polygons[i].get_label() == new_label) {
      m_polygons[i] = new_poly;
      return;
    }
  }
  m_polygons.push_back(new_poly);  
}

//-----------------------------------------------------------
// Procedure: addSegList

void VPlug_GeoShapes::addSegList(const XYSegList& new_segl)
{
  string new_label = new_segl.get_label();
  if(new_label == "") {
    m_seglists.push_back(new_segl);
    return;
  }
  
  for(int i=0; i<m_seglists.size(); i++) {
    if(m_seglists[i].get_label() == new_label) {
      m_seglists[i] = new_segl;
      return;
    }
  }
  m_seglists.push_back(new_segl);  
}

//-----------------------------------------------------------
// Procedure: updateGrid

void VPlug_GeoShapes::updateGrid(const string& delta)
{
  for(int i=0; i<m_grids.size(); i++)
    m_grids[i].processDelta(delta);
}

//-----------------------------------------------------------
// Procedure: addGrid

void VPlug_GeoShapes::addGrid(const XYGrid& new_grid)
{
  string new_label = new_grid.getLabel();
  if(new_label == "") {
    m_grids.push_back(new_grid);
    return;
  }
  
  for(int i=0; i<m_grids.size(); i++) {
    if(m_grids[i].getLabel() == new_label) {
      m_grids[i] = new_grid;
      return;
    }
  }

  m_grids.push_back(new_grid);
}

//-----------------------------------------------------------
// Procedure: addCircle

void VPlug_GeoShapes::addCircle(const XYCircle& circle)
{
  m_circles.push_back(circle);
}


//-----------------------------------------------------------
// Procedure: addHexagon

void VPlug_GeoShapes::addHexagon(const XYHexagon& hexagon)
{
  m_hexagons.push_back(hexagon);
}


//-----------------------------------------------------------
// Procedure: addPoint

void VPlug_GeoShapes::addPoint(const XYPoint& new_point)
{
  string new_label = new_point.get_label();
  string new_type  = new_point.get_type();
  for(int i=0; i<m_points.size(); i++) {
    if((m_points[i].get_label() == new_label) &&
       (m_points[i].get_type() == new_type)) {
      m_points[i] = new_point;
      return;
    }
  }
  
  m_points.push_back(new_point);
}

//-----------------------------------------------------------
// Procedure: addPolygon

bool VPlug_GeoShapes::addPolygon(const string& poly_str)
{
  XYPolygon new_poly = stringToPoly(poly_str);
  if(new_poly.size() == 0)
    return(false);
  m_polygons.push_back(new_poly);
  return(true);
}

//-----------------------------------------------------------
// Procedure: addSegList

bool VPlug_GeoShapes::addSegList(const string& segl_str)
{
  XYSegList new_segl = stringToSegList(segl_str);
  if(new_segl.size() == 0)
    return(false);
  m_seglists.push_back(new_segl);
  return(true);
}


//-----------------------------------------------------------
// Procedure: addPoint

bool VPlug_GeoShapes::addPoint(const string& point_str)
{
  XYPoint new_point = stringToPoint(point_str);
  addPoint(new_point);
  return(true);
}


//-------------------------------------------------------------
// Procedure: getPolygon(int)

XYPolygon VPlug_GeoShapes::getPolygon(int index)
{
  if((index < 0) || (index >= m_polygons.size())) {
    XYPolygon null_poly;
    return(null_poly);
  }
  else
    return(m_polygons[index]);
}

//-------------------------------------------------------------
// Procedure: getSegList(int)

XYSegList VPlug_GeoShapes::getSegList(int index)
{
  if((index < 0) || (index >= m_seglists.size())) {
    XYSegList null_segl;
    return(null_segl);
  }
  else
    return(m_seglists[index]);
}

//-------------------------------------------------------------
// Procedure: getGrid(int)

XYGrid VPlug_GeoShapes::getGrid(int index)
{
  if((index < 0) || (index >= m_grids.size())) {
    XYGrid null_grid;
    return(null_grid);
  }
  else
    return(m_grids[index]);
}

//-------------------------------------------------------------
// Procedure: getCircle(int)

XYCircle VPlug_GeoShapes::getCircle(int index)
{
  if((index < 0) || (index >= m_circles.size())) {
    XYCircle null_circle;
    return(null_circle);
  }
  else
    return(m_circles[index]);
}

//-------------------------------------------------------------
// Procedure: getHexagon(int)

XYHexagon VPlug_GeoShapes::getHexagon(int index)
{
  if((index < 0) || (index >= m_hexagons.size())) {
    XYHexagon null_hexagon;
    return(null_hexagon);
  }
  else
    return(m_hexagons[index]);
}

//-------------------------------------------------------------
// Procedure: getPoint(int)

XYPoint VPlug_GeoShapes::getPoint(int index)
{
  if((index < 0) || (index >= m_points.size())) {
    XYPoint null_point;
    return(null_point);
  }
  else
    return(m_points[index]);
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool VPlug_GeoShapes::setColorMapping(string attribute, string color_str)
{
  attribute = tolower(stripBlankEnds(attribute));
  color_str = stripBlankEnds(color_str);
  
  vector<double> cvect = colorParse(color_str);
  
  m_color_map[attribute] = cvect;
  if((cvect[0]==0) && (cvect[2]==0) && (cvect[2]==0) &&
     (tolower(color_str) != "black"))
    return(false);
  return(true);    
}


//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label, DarkKhaki"
//            "label, hex, bd, b7, 6b"
//            "label, 0.741, 0.718, 0.420"

bool VPlug_GeoShapes::setColorMapping(string str)
{
  string attribute = biteString(str, ',');
  return(setColorMapping(attribute, str));
}

//-------------------------------------------------------------
// Procedure: setViewableMapping

bool VPlug_GeoShapes::setViewableMapping(string param, string value)
{
  param = tolower(param);
  value = tolower(value);

  if((value == "true") or (value == "on")) {
    m_viewable_map[param] = true;
    return(true);
  }
  if((value == "false") or (value == "off")) {
    m_viewable_map[param] = false;
    return(true);
  }
  if(value != "toggle")
    return(false);
  else {
    map<string, bool>::iterator p;
    p = m_viewable_map.find(param);
    if(p == m_viewable_map.end())
      m_viewable_map[param] = true;
    else {
      bool curr_value = p->second;
      m_viewable_map[param] = !curr_value;
    }
  }
  return(true);
}


//-------------------------------------------------------------
// Procedure: setGSizeMapping
//      Note: Can accept string size args such as "+1", or "-10"

bool VPlug_GeoShapes::setGSizeMapping(string attribute, string gsize)
{
  attribute = tolower(stripBlankEnds(attribute));
  gsize = stripBlankEnds(gsize);

  bool add = false;
  bool sub = false;
  if(strContains(gsize, "+")) {
    gsize = findReplace(gsize, "+", "");
    add = true;
  }
  else if(strContains(gsize, "-")) {
    gsize = findReplace(gsize, "-", "");
    sub = true;
  }

  if(!isNumber(gsize))
    return(false);

  double dval = atof(gsize.c_str());
  if(dval < 0)
    return(false);
  
  double current_size = m_gsize_map[attribute];

  if(add)
    m_gsize_map[attribute] = current_size + dval;
  else if(sub) {
    if((current_size - dval) >= 0)
      m_gsize_map[attribute] = current_size - dval;
  }
  else
    m_gsize_map[attribute] = dval;

  return(true);    
}

//-------------------------------------------------------------
// Procedure: getColorMapping
//      Note: If the attribute is not found, the returned color
//            vector can be determined by the optional def_color
//            argument. 
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

vector<double> VPlug_GeoShapes::geocolor(const string& attribute, 
					 string def_color)
{
  map<string, vector<double> >::iterator p;
  p = m_color_map.find(attribute);
  if(p == m_color_map.end())
    p = m_color_map.find(tolower(attribute));

  if(p != m_color_map.end())
    return(p->second);
  else {
    vector<double> color_vector = colorParse(def_color);
    return(color_vector);
  }
}

//-----------------------------------------------------------
// Procedure: viewable
//      Note: If the attribute is not found, the returned bolean
//            can be determined by the optional default boolean value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

bool VPlug_GeoShapes::viewable(const string& attribute, 
			       bool default_value)
{
  map<string, bool>::iterator p;
  p = m_viewable_map.find(attribute);
  if(p == m_viewable_map.end())
    p = m_viewable_map.find(tolower(attribute));
  
  if(p != m_viewable_map.end())
    return(p->second);
  else 
    return(default_value);
}

//-----------------------------------------------------------
// Procedure: getGSizeMapping
//      Note: If the attribute is not found, the returned double
//            can be determined by the optional default double value.
//      Note: Attributes are case-insensitive by always converting
//            to lower case when added to the structure. The queried
//            attribute is converted to lower case only if the query
//            initially fails. 

double VPlug_GeoShapes::geosize(const string& attribute, 
				double default_value)
{
  map<string, double>::iterator p;
  p = m_gsize_map.find(attribute);
  if(p == m_gsize_map.end())
    p = m_gsize_map.find(tolower(attribute));

  if(p != m_gsize_map.end())
    return(p->second);
  else 
    return(default_value);
}

