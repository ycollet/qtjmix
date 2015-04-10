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

#ifndef MIXER_STRIP_H
#define MIXER_STRIP_H

#include <QtWidgets>

#include "awl/volknob.h"
#include "awl/panknob.h"
#include "awl/mslider.h"

#include <jack/jack.h>

class SetMixerParamDialog;

class Mixer_Strip : public QFrame
{
  Q_OBJECT

 public:
  enum mixer_strip_t {mono, stereo};

  Mixer_Strip(QWidget * parent = 0, int _stripNumber = 1, int _nb_channel = 1);
  float           get_fine_volume() const;
  float           get_coarse_volume() const;
  float           get_volume() const;

  // Method to tune the volume widgets
  float           get_coarse_min() const;
  float           get_coarse_max() const;
  float           get_coarse_current() const;
  void            set_coarse_min(float);
  void            set_coarse_max(float);
  void            set_coarse_current(float);
  
  float           get_fine_min() const;
  float           get_fine_max() const;
  float           get_fine_current() const;
  void            set_fine_min(float);
  void            set_fine_max(float);
  void            set_fine_current(float);
  
  QString         get_strip_title() const;

  bool            get_mute() const;
  void            set_mute(bool);
  bool            get_solo() const;
  void            set_solo(bool);
  void            set_nb_channel(int);
  int             get_nb_channel() const;
  float           get_pan() const;
  void            set_pan(float);
  jack_port_t *   get_input_port(int) const;
  void            set_input_port(int, jack_port_t *);
  jack_port_t *   get_output_port(int) const;
  void            set_output_port(int, jack_port_t *);
  void            set_rms_level(int, float);
  float           get_rms_level(int) const;
  void            set_att_rms_level(float);
  float           get_att_rms_level() const;
  void            set_max_level(int, float);
  float           get_max_level(int) const;
  void            set_att_max_level(float);
  float           get_att_max_level() const;
  mixer_strip_t   get_mixer_type() const;
  void            set_mixer_type(mixer_strip_t);
  void            set_title(QString);
  QString         get_title() const;
  void            set_jack_client(jack_client_t *);
  jack_client_t * get_jack_client() const;
  void            set_strip_id(int);
  int             get_strip_id() const;
 protected slots:
  void            volume_changed(double, int);
  void            pan_changed(double, int);
  void            mute_changed(int);
  void            solo_changed(int);
  void            change_parameters();
 protected:
  virtual void    focusInEvent(QFocusEvent * event);
  virtual void    focusOutEvent(QFocusEvent * event);
  virtual void    mouseReleaseEvent(QMouseEvent * event);
 private:
  jack_client_t     *client;
  mixer_strip_t      type;
  float              pan;
  float              volume;
  int                nb_channel;
  float              att_rms_level;
  float              att_max_level;
  std::vector<float> rms_level;
  std::vector<float> max_level;
  std::vector<float> rms_level_n_1;
  std::vector<float> max_level_n_1;
  QLabel            *stripLabel;
  QLabel            *typeLabel;
  Awl::VolKnob      *fineDial;
  Awl::PanKnob      *panDial;
  Awl::MeterSlider  *coarseDial;
  QCheckBox         *muteButton;
  QCheckBox         *soloButton;
  std::vector<jack_port_t *> input_port;
  std::vector<jack_port_t *> output_port;
  QString            title;
  int                strip_id; 
  SetMixerParamDialog * paramDialog;
};

#endif
