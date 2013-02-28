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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include "TutorialCamera.h"

using namespace lima;
using namespace lima::Tutorial;

Camera::Camera(Camera::Callback* cbk,const char* video_device):
  m_cbk(cbk),
  m_fd(-1),
  m_nb_frames(1),
  m_acq_frame_id(-1),
  m_acq_started(false)
{
  DEB_CONSTRUCTOR();

  memset(&m_buffer,0,sizeof(m_buffer));
  m_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  
  m_fd = v4l2_open(video_device,O_RDWR);
  if(m_fd < -1)
    THROW_HW_ERROR(Error) << "Error opening: " << video_device 
			  << "(" << strerror(errno) << ")";

  struct v4l2_capability cap;
  int ret = v4l2_ioctl(m_fd, VIDIOC_QUERYCAP, &cap);

  if(ret == -1) 
    THROW_HW_ERROR(Error) << "Error querying cap: " << strerror(errno);

  DEB_TRACE() << DEB_VAR1(cap.driver) << ","
	      << DEB_VAR1(cap.card) << ","
	      << DEB_VAR1(cap.bus_info);

  m_det_model = (char*)cap.card;
  if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    THROW_HW_ERROR(Error) << "Error: dev. doesn't have VIDEO_CAPTURE cap.";


  struct v4l2_requestbuffers requestbuff;
  requestbuff.count = sizeof(m_buffers) / sizeof(unsigned char*);
  requestbuff.type = m_buffer.type;
  requestbuff.memory = V4L2_MEMORY_MMAP;
  ret = v4l2_ioctl(m_fd,VIDIOC_REQBUFS,&requestbuff);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "req. buffers: " << strerror(errno);

  for (int i = 0;i < sizeof(m_buffers) / sizeof(unsigned char*);++i)
    {
      m_buffer.index = i;
      ret = v4l2_ioctl(m_fd, VIDIOC_QUERYBUF, &m_buffer);
      if (ret == -1) 
	THROW_HW_ERROR(Error) << "querying buffer " 
			      << i << ": " << strerror(errno);
      if (m_buffer.memory != V4L2_MEMORY_MMAP)
	THROW_HW_ERROR(Error)<< "memory type " 
			     << m_buffer.memory << ": not MMAP";
			
      int prot_flags = PROT_READ | PROT_WRITE;
      void *p = v4l2_mmap(NULL, m_buffer.length, prot_flags, 
			  MAP_SHARED, m_fd, m_buffer.m.offset);
      if (p == MAP_FAILED) 
	THROW_HW_ERROR(Error) << "mapping buffer " 
			      << i << ": " << strerror(errno);
      memset(p, 0, m_buffer.length);
      m_buffers[i] = (unsigned char *)p;
    }
}

Camera::~Camera()
{
  stopAcq();

  for(int i = 0;i < sizeof(m_buffers) / sizeof(unsigned char*);++i)
    v4l2_munmap(m_buffers[i], m_buffer.length);

  v4l2_close(m_fd);
}

void Camera::getMaxImageSize(Size& max_size)
{
  DEB_MEMBER_FUNCT();
  
  struct v4l2_format format;
  
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  int ret = v4l2_ioctl(m_fd,VIDIOC_G_FMT,&format);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't get the format: " << strerror(errno);

  max_size = Size(format.fmt.pix.width,format.fmt.pix.height);
  DEB_RETURN() << DEB_VAR1(max_size);
}

void Camera::getCurrImageType(ImageType& image_format)
{
  DEB_MEMBER_FUNCT();

  struct v4l2_format format;
  
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  int ret = v4l2_ioctl(m_fd,VIDIOC_G_FMT,&format);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't get the format: " << strerror(errno);
  
  switch(format.fmt.pix.pixelformat)
    {
    case V4L2_PIX_FMT_GREY: 	image_format = Bpp8;break;
    case V4L2_PIX_FMT_Y16: 	image_format = Bpp16;break;
    default:
      THROW_HW_ERROR(NotSupported) << "Not yet managed";
    }
  DEB_RETURN() << DEB_VAR1(image_format);
}

void Camera::setCurrImageType(ImageType image_format)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(image_format);

  struct v4l2_format format;
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  int ret = v4l2_ioctl(m_fd,VIDIOC_G_FMT,&format);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't get the format: " << strerror(errno);

  switch(image_format)
    {
    case Bpp8:
      format.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;break;
    case Bpp16:
      format.fmt.pix.pixelformat = V4L2_PIX_FMT_Y16;break;
    default:
       THROW_HW_ERROR(NotSupported) << "Not yet managed";
    }

  
  ret = v4l2_ioctl(m_fd,VIDIOC_S_FMT,&format);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't set the format: " << strerror(errno);
}

void Camera::getDetectorModel(std::string& det_model)
{
  DEB_MEMBER_FUNCT();

  det_model = m_det_model;
  
  DEB_RETURN() << DEB_VAR1(det_model);
}

void Camera::getExpTime(double &exp_time)
{
  DEB_MEMBER_FUNCT();

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
  int ret = v4l2_ioctl(m_fd,VIDIOC_G_CTRL,&ctrl);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't get exposure time" << strerror(errno);
  
  exp_time = ctrl.value / 1000;

  DEB_RETURN() << DEB_VAR1(exp_time);
}

void Camera::setExpTime(double exp_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(exp_time);

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
  ctrl.value = exp_time * 1000;
  int ret = v4l2_ioctl(m_fd,VIDIOC_S_CTRL,&ctrl);
  if(ret == -1)
    THROW_HW_ERROR(Error) << "Can't set exposure time" << strerror(errno);
}

void Camera::setNbHwFrames(int nb_frames)
{
  m_nb_frames = nb_frames;
}

void Camera::getNbHwFrames(int &nb_frames)
{
  nb_frames = m_nb_frames;
}

void Camera::reset(HwInterface::ResetLevel)
{
  DEB_MEMBER_FUNCT();

  THROW_HW_ERROR(NotSupported) << "Not implemented yet";
}

void Camera::prepareAcq()
{
  m_acq_frame_id = -1;
}

void Camera::startAcq()
{
  m_acq_started = true;
}

void Camera::stopAcq()
{
  m_acq_started = false;
}

void Camera::getStatus(HwInterface::StatusType& status)
{
  status.set(m_acq_started ? HwInterface::StatusType::Exposure : HwInterface::StatusType::Ready);
}

int Camera::getNbHwAcquiredFrames()
{
  return m_acq_frame_id + 1;
}
