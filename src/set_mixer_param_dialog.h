//=============================================================================
//  QtJMix
//  Qt Jack Mixer
//  $Id:$
//
//  Copyright (C) 2013-2015 by Yann Collette and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef SET_MIXER_PARAM_DIALOG_H
#define SET_MIXER_PARAM_DIALOG_H

#include <QWidget>

#include "mixer_strip.h"

class SetMixerParamDialog : public QDialog
{
  Q_OBJECT

 public:
  SetMixerParamDialog(Mixer_Strip *, QWidget * parent = 0);
  float   get_coarse_min_value() const;
  float   get_coarse_max_value() const;
  float   get_coarse_current_value() const;
  float   get_fine_min_value() const;
  float   get_fine_max_value() const;
  float   get_fine_current_value() const;
  float   get_att_rms_value() const;
  float   get_att_max_value() const;
  bool    get_value_changed() const;
  QString get_mono_out_name() const;
  QString get_mono_in_name() const;
  QString get_stereo_out_left_name() const;
  QString get_stereo_out_right_name() const;
  QString get_stereo_in_left_name() const;
  QString get_stereo_in_right_name() const;
 protected slots:
  void set_coarse_min_value(const QString &);
  void set_coarse_max_value(const QString &);
  void set_coarse_current_value(const QString &);
  void set_fine_min_value(const QString &);
  void set_fine_max_value(const QString &);
  void set_fine_current_value(const QString &);
  void set_att_rms_value(const QString &);
  void set_att_max_value(const QString &);
  void set_mono_in_name(const QString &);
  void set_mono_out_name(const QString &);
  void set_stereo_in_left_name(const QString &);
  void set_stereo_in_right_name(const QString &);
  void set_stereo_out_left_name(const QString &);
  void set_stereo_out_right_name(const QString &);
 private:
  float   coarse_min;
  float   coarse_max;
  float   coarse_current;
  float   fine_min;
  float   fine_max;
  float   fine_current;
  float   att_rms;
  float   att_max;
  bool    value_changed;
  QString mono_in;
  QString mono_out;
  QString stereo_in_left;
  QString stereo_in_right;
  QString stereo_out_left;
  QString stereo_out_right;

  Mixer_Strip * strip;
};
#endif
