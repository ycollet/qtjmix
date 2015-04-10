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

#ifndef GUI_MIXER_H
#define GUI_MIXER_H

#include <QtWidgets>

#include <vector>

#include "mixer_strip.h"

class Gui_Mixer : public QMainWindow
{
  Q_OBJECT

 public:
  Gui_Mixer(QMainWindow * parent = 0);
  void            init_2_strips();
  void            set_jack_client(jack_client_t *);
  jack_client_t * get_jack_client() const;
  void            set_sample_rate(jack_nframes_t);
  jack_nframes_t  get_sample_rate() const;
  void            set_buffer_size(jack_nframes_t);
  jack_nframes_t  get_buffer_size() const;
  bool            load_config_file(QString fileName);
  bool            save_config_file(QString fileName) const;
  float           get_volume(int stripNumber) const;
  std::vector<Mixer_Strip *> & get_strip_list();
 private slots:
  void  add_Mono_MixerStrip();
  void  add_Stereo_MixerStrip();
  void  remove_SelectedMixerStrip();
  void  open_File();
  void  save_File();
  int   about_dialog();
  
 protected:
  void closeEvent(QCloseEvent *event);
 
 public:
  void LoadSettings();
  void SaveSettings();
  
 private:
  std::vector<Mixer_Strip *> strips;
  QMenu         *fileMenu;
  QMenu         *stripMenu;
  QMenu         *aboutMenu;
  QAction       *addMonoStripAction;
  QAction       *addStereoStripAction;
  QAction       *removeSelectedStripAction;
  QAction       *fileOpenAction;
  QAction       *fileSaveAsAction;
  QAction       *exitAction;
  QAction       *aboutAction;
  jack_client_t *client;
  jack_nframes_t sample_rate;
  jack_nframes_t buffer_size;
  int            index_mixer_strip;
};

#endif
