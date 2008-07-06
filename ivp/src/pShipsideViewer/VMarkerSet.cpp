/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VMarkerSet.cpp                                       */
/*    DATE: July 5th, 2008                                       */
/*****************************************************************/

#include "VMarkerSet.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VMarkerSet::VMarkerSet()
{
  m_marker_scale_global    = 1.0;
  m_marker_viewable_all    = true;
  m_marker_viewable_labels = true;

  m_label_color_default = colorParse("white");
  m_label_color         = m_label_color_default;
  m_label_color_index   = 0;
}

//-----------------------------------------------------------
// Procedure: addVMarker()
//      Note: return(true) if the given vmarker results in a new entry
//            return(false) if the given marker replaces an old entry

bool VMarkerSet::addVMarker(string mtype, double xpos, double ypos,
			    double mscale, string label, string colors)
{
  // First check to see if the given VMarker matches the type and label
  // of a VMarker already added. If the label is the empty string, it
  // will be assumed to be unique.
  int found_ix = -1;
  unsigned int i, vsize = m_marker_type.size();
  if(label != "") {
    for(i=0; i<vsize; i++) {
      if((mtype == m_marker_type[i]) && (label == m_marker_label[i]))
	found_ix = i;
    }
  }

  vector<vector<double> > color_vectors;
  vector<string> svector = parseString(colors, ':');
  vsize = svector.size();
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<double> cvect = colorParse(svector[i]);
    color_vectors.push_back(cvect);
  }

  if(found_ix == -1) {
    m_marker_type.push_back(mtype);
    m_marker_xpos.push_back(xpos);
    m_marker_ypos.push_back(ypos);
    m_marker_scale.push_back(mscale);
    m_marker_label.push_back(label);
    m_marker_colors.push_back(svector);
    m_marker_color_vectors.push_back(color_vectors);
    return(true);
  }
  else {
    m_marker_xpos[found_ix] = xpos;
    m_marker_ypos[found_ix] = ypos;
    m_marker_scale[found_ix] = mscale;
    m_marker_colors[found_ix] = svector;
    m_marker_color_vectors[found_ix] = color_vectors;
    return(false);
  }
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VMarkerSet::setParam(string param, string value)
{
  if(param == "viewable_all")
    return(setBooleanOnString(m_marker_viewable_all, value));
  else if(param == "viewable_labels")
    return(setBooleanOnString(m_marker_viewable_labels, value));
  else if(param == "label_color") {
    if(tolower(value) == "toggle") {
      m_label_color_index++;
      if(m_label_color_index > 9)
	m_label_color_index = 0;

      if(m_label_color_index == 0) 
	m_label_color = m_label_color_default;
      else if(m_label_color_index == 1)
	m_label_color = colorParse("DarkBlue");
      else if(m_label_color_index == 2)
	m_label_color = colorParse("Yellow");
      else if(m_label_color_index == 3)
	m_label_color = colorParse("Red");
      else if(m_label_color_index == 4)
	m_label_color = colorParse("Green");
      else if(m_label_color_index == 5)
	m_label_color = colorParse("Azure");
      else if(m_label_color_index == 6)
	m_label_color = colorParse("Khaki");
      else if(m_label_color_index == 7)
	m_label_color = colorParse("DarkCyan");
      else if(m_label_color_index == 8)
	m_label_color = colorParse("Blue");
      else
	m_label_color = colorParse("Coral");
    }
    else {
      vector<double> cvect = colorParse(value);
      if(tolower(value)!="black") {
	if((cvect[0]==0) && (cvect[1]==0) && (cvect[2]==0))
	  return(false);
      }    
      m_label_color_default = cvect;
      m_label_color         = cvect;
      m_label_color_index   = 0;
    }
  }
  else
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VMarkerSet::setParam(string param, double value)
{
  if(param == "mod_scale_all") {
    if((m_marker_scale_global * value) > 0.1)
      m_marker_scale_global *= value;
  }
  else if(param == "set_scale_all") {
    if(value > 0.1)
      m_marker_scale_global = value;
  }
  else
    return(false);
  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool VMarkerSet::viewable(const string& str)
{
  if(str == "all")
    return(m_marker_viewable_all);
  else if(str == "labels")
    return(m_marker_viewable_labels);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getMarkerType()

string VMarkerSet::getMarkerType(int ix)
{
  if((ix >= 0) && (ix < m_marker_type.size()))
    return(m_marker_type[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerXPos()

double VMarkerSet::getMarkerXPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_xpos.size()))
    return(m_marker_xpos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerYPos()

double VMarkerSet::getMarkerYPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_ypos.size()))
    return(m_marker_ypos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerScale()

double VMarkerSet::getMarkerScale(int ix)
{
  if((ix >= 0) && (ix < m_marker_scale.size()))
    return(m_marker_scale[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerLabel()

string VMarkerSet::getMarkerLabel(int ix)
{
  if((ix >= 0) && (ix < m_marker_label.size()))
    return(m_marker_label[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerColorString

string VMarkerSet::getMarkerColorString(int mix, int cix)
{
  if((mix < 0) && (mix >= m_marker_colors.size()))
    return("");
  
  if((cix < 0) && (cix >= m_marker_colors[mix].size()))
    return("");
  
  return(m_marker_colors[mix][cix]);
}


//-----------------------------------------------------------
// Procedure: getMarkerColorVector

vector<vector<double> > VMarkerSet::getMarkerColorVectors(int mix)
{
  vector<vector<double> > fail_vector;

  if((mix < 0) && (mix >= m_marker_color_vectors.size()))
    return(fail_vector);
  else
    return(m_marker_color_vectors[mix]);
}

