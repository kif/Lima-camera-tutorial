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
#include "TutorialDetInfoCtrlObj.h"
#include "TutorialInterface.h"
#include "TutorialCamera.h"

using namespace lima;
using namespace lima::Tutorial;
DetInfoCtrlObj::DetInfoCtrlObj(Camera& cam) : m_cam(cam)
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
  m_cam.getMaxImageSize(max_image_size);
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
  DEB_MEMBER_FUNCT();
  m_cam.getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
  def_image_type = Bpp8;
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
  curr_image_type = Bpp8;
}

void DetInfoCtrlObj::setCurrImageType(ImageType  curr_image_type)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(curr_image_type);
  if(curr_image_type != Bpp8)
    THROW_HW_ERROR(NotSupported) << DEB_VAR1(curr_image_type) << " is not available";
}

void DetInfoCtrlObj::getPixelSize(double& x_size,double &y_size)
{
  x_size = y_size = -1;		// Don't know
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
  det_type = "Tutorial";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
  m_cam.getDetectorModel(det_model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_mis_cb_gen.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  m_mis_cb_gen.unregisterMaxImageSizeCallback(cb);
}
