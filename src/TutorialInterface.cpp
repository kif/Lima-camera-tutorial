//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "TutorialInterface.h"
#include "TutorialDetInfoCtrlObj.h"
#include "TutorialSyncCtrlObj.h"
#include "TutorialCamera.h"
 
using namespace lima;
using namespace lima::Tutorial;

class Interface::_Callback : public Camera::Callback
{
  DEB_CLASS_NAMESPC(DebModCamera, "Interface::_Callback", "Tutorial");
public:
  _Callback(Interface& i) : m_interface(i) {}
  virtual bool newFrame(int frame_id,const unsigned char* srcPt)
  {
  }
private:
  Interface& m_interface;
};

Interface::Interface(const char* dev_path)
{
  DEB_CONSTRUCTOR();
}

Interface::~Interface()
{
  DEB_DESTRUCTOR();
}

void Interface::getCapList(CapList &cap_list) const
{
  DEB_MEMBER_FUNCT();
}

void Interface::reset(ResetLevel reset_level)
{
  //Not managed
}

void Interface::prepareAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::startAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::stopAcq()
{
  DEB_MEMBER_FUNCT();
}

void Interface::getStatus(StatusType &status)
{
  DEB_MEMBER_FUNCT();

  
  DEB_RETURN() << DEB_VAR1(status);
}

int Interface::getNbHwAcquiredFrames()
{
  DEB_MEMBER_FUNCT();
}
