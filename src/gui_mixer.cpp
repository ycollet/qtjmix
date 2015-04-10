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

#include <QtGui>
#include <QtWebKitWidgets>

#include <sstream>
#include <iostream>

#include "gui_mixer.h"
#include "mixer_strip.h"
#include "help_dialog.h"

#define CONVERT_ERROR(x)						\
  if (x)								\
    {									\
      std::cerr << qPrintable(QT_TRANSLATE_NOOP("gui_mixer", "Error while reading the configuration file.")) << std::endl; \
      return false;							\
    }

#ifdef WINDOWS
#  include <direct.h>
#  define GetCurrentDir _getcwd
#else
#  include <unistd.h>
#  define GetCurrentDir getcwd
#endif

// A New style to force the help menu to be written on the right
// Force the drawing of Separator.

class MixerStyle : public QCommonStyle
{
public:
  MixerStyle() {}
  
  int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
  {
    if (hint == QStyle::SH_DrawMenuBarSeparator) return 1;

    return QCommonStyle::styleHint(hint, option, widget, returnData);
  }
};

Gui_Mixer::Gui_Mixer(QMainWindow *parent) : QMainWindow(parent)
{
  setWindowTitle("QtJMix");
  setWindowIcon(QIcon(":/images/MainIcon"));

  // Initialize actions

  aboutAction = new QAction(tr("About &Qt"), parent);
  aboutAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about_dialog()));

  addMonoStripAction = new QAction(tr("Add mono mixer strip"), parent);
  addMonoStripAction->setStatusTip(tr("Add a new mono mixer strip"));
  connect(addMonoStripAction, SIGNAL(triggered()), this, SLOT(add_Mono_MixerStrip()));

  addStereoStripAction = new QAction(tr("Add stereo mixer strip"), parent);
  addStereoStripAction->setStatusTip(tr("Add a new stereo mixer strip"));
  connect(addStereoStripAction, SIGNAL(triggered()), this, SLOT(add_Stereo_MixerStrip()));

  removeSelectedStripAction = new QAction(tr("Remove selected mixer strip"), parent);
  removeSelectedStripAction->setStatusTip(tr("Remove the selected mixer strip"));
  connect(removeSelectedStripAction, SIGNAL(triggered()), this, SLOT(remove_SelectedMixerStrip()));

  exitAction = new QAction(tr("Exit"), parent);
  exitAction->setStatusTip(tr("Exit"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  fileOpenAction = new QAction(tr("&Open"), parent);
  fileOpenAction->setStatusTip(tr("Open a configuration file"));
  connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(open_File()));

  fileSaveAsAction = new QAction(tr("&Save as"), parent);
  fileSaveAsAction->setStatusTip(tr("Save a configuration file as"));
  connect(fileSaveAsAction, SIGNAL(triggered()), this, SLOT(save_File()));

  // Create Menus

  menuBar()->setStyle(new MixerStyle());

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(fileOpenAction);
  fileMenu->addAction(fileSaveAsAction);
  fileMenu->addAction(exitAction);

  stripMenu = menuBar()->addMenu(tr("&Strip"));
  stripMenu->addAction(addMonoStripAction);
  stripMenu->addAction(addStereoStripAction);
  stripMenu->addAction(removeSelectedStripAction);

  menuBar()->addSeparator();

  aboutMenu = menuBar()->addMenu(tr("&About"));
  aboutMenu->addAction(aboutAction);

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  index_mixer_strip = 1;
}

void Gui_Mixer::init_2_strips()
{
  Mixer_Strip * tmp_strip = NULL;
  jack_port_t * tmp_port_out = NULL, * tmp_port_in = NULL;
  std::stringstream port_name;

  // Create the centra widget
  QWidget *centralWidget = new QWidget(this);

  // Initialize 2 Mono Mixer Strips

  //QHBoxLayout *mixerLayout = new QHBoxLayout(this);
  //The preceding line produces this message:
  //QLayout: Attempting to add QLayout "" to Gui_Mixer "", which already has a layout
  QHBoxLayout *mixerLayout = new QHBoxLayout(centralWidget);
  mixerLayout->addStretch(0);

  // Create a Mono Mixer strip
  tmp_strip = new Mixer_Strip(centralWidget, index_mixer_strip, 1);
  tmp_strip->set_jack_client(client);
  tmp_strip->set_strip_id(index_mixer_strip);

  // Create output port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_out";
#ifdef ENABLE_JACK
  tmp_port_out = jack_port_register(client, port_name.str().c_str(),
				    JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);
#endif

  // Create input port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_in";

#ifdef ENABLE_JACK
  tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				   JACK_DEFAULT_AUDIO_TYPE,
				   JackPortIsInput, 0);

  if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
    {
      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
      exit(1);
    }
#endif
  strips.push_back(tmp_strip);

  tmp_strip->set_output_port(0, tmp_port_out);
  tmp_strip->set_input_port(0, tmp_port_in);

  index_mixer_strip++;

  mixerLayout->addWidget(tmp_strip);
  mixerLayout->setAlignment(tmp_strip, Qt::AlignLeft);

  mixerLayout->addStretch(0);

  // Create a Mono Mixer strip
  tmp_strip = new Mixer_Strip(centralWidget, index_mixer_strip, 1);
  tmp_strip->set_jack_client(client);
  tmp_strip->set_strip_id(index_mixer_strip);

  // Create output port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_out";
#ifdef ENABLE_JACK
  tmp_port_out = jack_port_register(client, port_name.str().c_str(),
				    JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);
#endif

  // Create input port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_in";
#ifdef ENABLE_JACK
  tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				   JACK_DEFAULT_AUDIO_TYPE,
				   JackPortIsInput, 0);

  if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
    {
      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
      exit(1);
    }
#endif

  strips.push_back(tmp_strip);

  tmp_strip->set_input_port(0,  tmp_port_in);
  tmp_strip->set_output_port(0, tmp_port_out);

  index_mixer_strip++;

  mixerLayout->addWidget(tmp_strip);
  mixerLayout->setAlignment(tmp_strip, Qt::AlignLeft);

  mixerLayout->addStretch(0);

  centralWidget->setLayout(mixerLayout);
  centralWidget->resize(centralWidget->minimumWidth(),centralWidget->height());
  setCentralWidget(centralWidget);
  adjustSize();
}

void Gui_Mixer::add_Mono_MixerStrip()
{
  Mixer_Strip * tmp_strip = NULL;
  jack_port_t * tmp_port_out = NULL, * tmp_port_in = NULL;
  std::stringstream port_name;
  QWidget *mixerCentralWidget = this->centralWidget();

  // Create a Mono Mixer strip
  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 1);
  tmp_strip->set_jack_client(client);
  tmp_strip->set_strip_id(index_mixer_strip);

  // Create output port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_out";
#ifdef ENABLE_JACK
  tmp_port_out = jack_port_register(client, port_name.str().c_str(),
				    JACK_DEFAULT_AUDIO_TYPE,
				    JackPortIsOutput, 0);
#endif

  // Create input port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_m_in";
#ifdef ENABLE_JACK
  tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				   JACK_DEFAULT_AUDIO_TYPE,
				   JackPortIsInput, 0);

  if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
    {
      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
      exit(1);
    }
#endif

  strips.push_back(tmp_strip);

  tmp_strip->set_input_port(0, tmp_port_in);
  tmp_strip->set_output_port(0, tmp_port_out);

  index_mixer_strip++;

  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
}

void Gui_Mixer::add_Stereo_MixerStrip()
{
  Mixer_Strip * tmp_strip = NULL;
  jack_port_t * tmp_port_out_l = NULL, * tmp_port_in_l = NULL;
  jack_port_t * tmp_port_out_r = NULL, * tmp_port_in_r = NULL;
  std::stringstream port_name;
  QWidget *mixerCentralWidget = this->centralWidget();

  // Create a Stereo Mixer strip
  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 2);
  tmp_strip->set_jack_client(client);
  tmp_strip->set_strip_id(index_mixer_strip);

  // Create left output port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_l_out";
#ifdef ENABLE_JACK
  tmp_port_out_l = jack_port_register(client, port_name.str().c_str(),
				      JACK_DEFAULT_AUDIO_TYPE,
				      JackPortIsOutput, 0);
#endif

  // Create left input port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_l_in";
#ifdef ENABLE_JACK
  tmp_port_in_l = jack_port_register(client, port_name.str().c_str(),
				     JACK_DEFAULT_AUDIO_TYPE,
				     JackPortIsInput, 0);

  if ((tmp_port_in_l == NULL) || (tmp_port_out_l == NULL))
    {
      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
      exit(1);
    }
#endif

  // Create right output port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_r_out";
#ifdef ENABLE_JACK
  tmp_port_out_r = jack_port_register(client, port_name.str().c_str(),
				      JACK_DEFAULT_AUDIO_TYPE,
				      JackPortIsOutput, 0);
#endif

  // Create right input port
  port_name.str("");
  port_name << "mixer_" << index_mixer_strip << "_r_in";
#ifdef ENABLE_JACK
  tmp_port_in_r = jack_port_register(client, port_name.str().c_str(),
				     JACK_DEFAULT_AUDIO_TYPE,
				     JackPortIsInput, 0);

  if ((tmp_port_in_r == NULL) || (tmp_port_out_r == NULL))
    {
      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
      exit(1);
    }
#endif

  strips.push_back(tmp_strip);

  tmp_strip->set_input_port(0,  tmp_port_in_l);
  tmp_strip->set_output_port(0, tmp_port_out_l);
  tmp_strip->set_input_port(1,  tmp_port_in_r);
  tmp_strip->set_output_port(1, tmp_port_out_r);

  index_mixer_strip++;

  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
}

void Gui_Mixer::remove_SelectedMixerStrip()
{
  int result = 0, i = 0, j = 0;
  std::vector<int> selected(0);
  std::vector<Mixer_Strip *> new_strips(0);
  
  for(i=0; i< strips.size(); i++)
    {
      if (!strips[i]->hasFocus())
	{
	  new_strips.push_back(strips[i]);
	}
      else
	{
	  // We remove the selected strip
	  // First remove jack ports
	  for(j = 0; j<strips[i]->get_nb_channel(); ++j)
	    {
#ifdef ENABLE_JACK
	      result = jack_port_unregister(client, strips[i]->get_input_port(j));
	      if (result)
		std::cerr << qPrintable(tr("QTJMix: problem while unregistering the input port of the mixer strip ")) << i << "." << std::endl;

	      result = jack_port_unregister(client, strips[i]->get_output_port(j));
	      if (result)
		std::cerr << qPrintable(tr("QTJMix: problem while unregistering the output port of the mixer strip ")) << i << "." << std::endl;
#endif
	      // Close the Widget
	      strips[i]->close();

	      // Remove the Widget from the layout
	      ((QHBoxLayout *)centralWidget()->layout())->removeWidget(strips[i]);
	      
	      // Delete it
	      ////delete strips[i];
	    }
	}
    }

  strips.resize(new_strips.size());
  strips = new_strips;

  this->centralWidget()->adjustSize();
  this->adjustSize();

#ifdef _DEBUG
  for(i=0; i<strips.size(); i++)
    {
      qDebug() << "DEBUG: Mixer strip title = " << qPrintable(strips[i]->get_title());
    }
#endif
}

void Gui_Mixer::set_jack_client(jack_client_t * _client)
{
  client = _client;
}

jack_client_t * Gui_Mixer::get_jack_client() const
{
  return client;
}

std::vector<Mixer_Strip *> & Gui_Mixer::get_strip_list()
{
  return strips;
}

int Gui_Mixer::about_dialog()
{
  HelpDialog * helpDialog = new HelpDialog(this);
  helpDialog->show();
}

float Gui_Mixer::get_volume(int stripNumber) const
{
  return strips[stripNumber]->get_volume();
}

bool Gui_Mixer::load_config_file(QString fileName)
{
  Mixer_Strip * tmp_strip = NULL;
  jack_port_t * tmp_port_out_l = NULL, * tmp_port_in_l = NULL;
  jack_port_t * tmp_port_out_r = NULL, * tmp_port_in_r = NULL;
  std::stringstream port_name;
  QWidget *mixerCentralWidget = this->centralWidget();

  int                  load_nbStrips = 0, result = 0;
  std::vector<QString> load_stripName;
  std::vector<int>     load_stripID;
  std::vector<int>     load_stripType;
  std::vector<float>   load_coarseMin, load_coarseMax, load_coarseCurrent;
  std::vector<float>   load_fineMin, load_fineMax, load_fineCurrent;
  std::vector<bool>    load_mute, load_solo;
  std::vector<float>   load_pan;
  bool convert_result = false;
  
  QFile file(fileName); 
  if (!file.open(QIODevice::ReadOnly))
    {
      std::cerr << qPrintable(tr("Cannot open file for reading: ")) << qPrintable(file.errorString()) << std::endl;
      return false;
    }

  // YC: review the add_MonoStrip and add_StereoStrip so as to simplify this "load" code.

  // Read the following file:
  // [NbStrips]
  // [mono/stereo] [stripName] [stripID] [coarsemin] [coarsemax] [coarsecurrent] [finemin] [finemax] [finecurrent] [mute] [solo] [pan]
  // ...
  
  QTextStream in(&file);
  
  // read the number of strips
  load_nbStrips = in.readLine().toInt();
  
  while(!in.atEnd())
    {
      QString line       = in.readLine();
      QStringList fields = line.split(' ');

      load_stripType.push_back(bool(fields.at(0).toInt(&convert_result))); CONVERT_ERROR(convert_result);
      load_stripName.push_back(fields.at(1));                              CONVERT_ERROR(convert_result);
      load_stripID.push_back(fields.at(2).toInt(&convert_result));         CONVERT_ERROR(convert_result);
      load_coarseMin.push_back(fields.at(3).toFloat(&convert_result));     CONVERT_ERROR(convert_result);
      load_coarseMax.push_back(fields.at(4).toFloat(&convert_result));     CONVERT_ERROR(convert_result);
      load_coarseCurrent.push_back(fields.at(5).toFloat(&convert_result)); CONVERT_ERROR(convert_result);
      load_fineMin.push_back(fields.at(6).toFloat(&convert_result));       CONVERT_ERROR(convert_result);
      load_fineMax.push_back(fields.at(7).toFloat(&convert_result));       CONVERT_ERROR(convert_result);
      load_fineCurrent.push_back(fields.at(8).toFloat(&convert_result));   CONVERT_ERROR(convert_result);
      load_mute.push_back(fields.at(9).toInt(&convert_result));            CONVERT_ERROR(convert_result);
      load_solo.push_back(fields.at(10).toInt(&convert_result));           CONVERT_ERROR(convert_result);
      load_pan.push_back(fields.at(11).toFloat(&convert_result));          CONVERT_ERROR(convert_result);
    }

  file.close();

  // Now, remove all strips and create new ones
  for(int i=0; i < strips.size(); i++)
    {
      // First remove jack ports
      for(int j=0; j<strips[i]->get_nb_channel(); ++j)
	{
#ifdef ENABLE_JACK
	  result = jack_port_unregister(client, strips[i]->get_input_port(j));
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the input port of the mixer strip ")) << i << "." << std::endl;
	  
	  result = jack_port_unregister(client, strips[i]->get_output_port(j));
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the output port of the mixer strip ")) << i << "." << std::endl;
#endif
	  
	  // Close the Widget
	  strips[i]->close();

	  // Remove the Widget from the layout
	  ((QHBoxLayout *)centralWidget()->layout())->removeWidget(tmp_strip);

	  // Delete the Widget
	  delete strips[i];
	}
    }

  strips.resize(0);

  // Create the new mixer strips
  for(int i=0; i<load_nbStrips; i++)
    {
      qDebug() << "DEBUG: strip type = " << load_stripType[i];

      switch(load_stripType[i])
	{
	case Mixer_Strip::mono:
	  qDebug() << "load config file: add a mono mixer";

	  // Create a Mono Mixer strip
	  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 1);
	  tmp_strip->set_jack_client(client);

	  // Create output port
	  port_name.str("");
	  port_name << "mixer_out_" << load_stripID[i];
#ifdef ENABLE_JACK
	  tmp_port_out_l = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif

	  // Create input port
	  port_name.str("");
	  port_name << "mixer_in_" << load_stripID[i];
#ifdef ENABLE_JACK
	  tmp_port_in_l = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);

	  if ((tmp_port_in_l == NULL) || (tmp_port_out_l == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  strips.push_back(tmp_strip);
	  
	  tmp_strip->set_input_port(0,  tmp_port_in_l);
	  tmp_strip->set_output_port(0, tmp_port_out_l);

	  tmp_strip->set_coarse_min(load_coarseMin[i]);
	  tmp_strip->set_coarse_max(load_coarseMax[i]);
	  tmp_strip->set_coarse_current(load_coarseCurrent[i]);

	  tmp_strip->set_fine_min(load_fineMin[i]);
	  tmp_strip->set_fine_max(load_fineMax[i]);
	  tmp_strip->set_fine_current(load_fineCurrent[i]);

	  tmp_strip->set_title(load_stripName[i]);

	  tmp_strip->set_mute(load_mute[i]);
	  tmp_strip->set_solo(load_solo[i]);
	  tmp_strip->set_pan(load_pan[i]);

	  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
	  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
	  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
	  break;

	case Mixer_Strip::stereo:
	  qDebug() << "load config file: add a stereo mixer";

	  // Create a Stereo Mixer strip
	  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 2);
	  tmp_strip->set_jack_client(client);

	  // Create left output port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_l_out";
#ifdef ENABLE_JACK
	  tmp_port_out_l = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif
	  
	  // Create left input port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_l_in";
#ifdef ENABLE_JACK
	  tmp_port_in_l = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);
	  
	  if ((tmp_port_in_l == NULL) || (tmp_port_out_l == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  // Create right output port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_r_out";
#ifdef ENABLE_JACK
	  tmp_port_out_r = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif
	  
	  // Create right input port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_r_in";
#ifdef ENABLE_JACK
	  tmp_port_in_r = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);
	  
	  if ((tmp_port_in_r == NULL) || (tmp_port_out_r == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  strips.push_back(tmp_strip);
	  
	  tmp_strip->set_input_port(0,  tmp_port_in_l);
	  tmp_strip->set_output_port(0, tmp_port_out_l);
	  tmp_strip->set_input_port(1,  tmp_port_in_r);
	  tmp_strip->set_output_port(1, tmp_port_out_r);
	  
	  tmp_strip->set_coarse_min(load_coarseMin[i]);
	  tmp_strip->set_coarse_max(load_coarseMax[i]);
	  tmp_strip->set_coarse_current(load_coarseCurrent[i]);

	  tmp_strip->set_fine_min(load_fineMin[i]);
	  tmp_strip->set_fine_max(load_fineMax[i]);
	  tmp_strip->set_fine_current(load_fineCurrent[i]);

	  tmp_strip->set_title(load_stripName[i]);

	  tmp_strip->set_mute(load_mute[i]);
	  tmp_strip->set_solo(load_solo[i]);
	  tmp_strip->set_pan(load_pan[i]);

	  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
	  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
	  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
	  break;
	}
    }
  
  this->adjustSize();

  // Search for the maximum strip id
  index_mixer_strip = -1;
  for(int i=0; i<load_nbStrips; i++)
    index_mixer_strip = std::max(load_stripID[i],index_mixer_strip);
  index_mixer_strip++;
}

bool Gui_Mixer::save_config_file(QString fileName) const
{
  QFile file(fileName); 
  if (!file.open(QIODevice::WriteOnly))
    {
      std::cerr << qPrintable(tr("Cannot open file for writing: ")) << qPrintable(file.errorString()) << std::endl;
      return false;
    }

  // Write the following file:
  // [NbStrips]
  // [mono/stereo] [stripName] [stripID] [coarsemin] [coarsemax] [coarsecurrent] [finemin] [finemax] [finecurrent] [mute] [solo] [pan]
  // ...

  QTextStream out(&file);
  out << strips.size() << endl;
  for(int i=0; i<strips.size(); i++)
    {
      out << strips[i]->get_mixer_type() << " ";
      out << qPrintable(strips[i]->get_title().replace(' ','_')) << " ";
      out << strips[i]->get_strip_id() << " ";
      out << strips[i]->get_coarse_min() << " " << strips[i]->get_coarse_max() << " " << strips[i]->get_coarse_current() << " ";
      out << strips[i]->get_fine_min() << " " << strips[i]->get_fine_max() << " " << strips[i]->get_fine_current() << " ";
      out << strips[i]->get_mute() << " " << strips[i]->get_solo() << " " << strips[i]->get_pan() << endl;
    }

  file.close();
}

void Gui_Mixer::open_File()
{
  char cCurrentPath[FILENAME_MAX];

  GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));

  //if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  //  {
  //    return errno;
  //  }

  QString fileName = QFileDialog::getOpenFileName(this, tr("Open QtJMix configuration file"), 
						  cCurrentPath,
						  tr("qtjmix config files (*.qtjmix)"));

  if (!fileName.isEmpty()) load_config_file(fileName);
}

void Gui_Mixer::save_File()
{
  char cCurrentPath[FILENAME_MAX];

  GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save QtJMix configuration file"), 
						  cCurrentPath,
						  tr("qtjmix config files (*.qtjmix)"));

  if (!fileName.isEmpty()) save_config_file(fileName);
}

void Gui_Mixer::set_sample_rate(jack_nframes_t _sample_rate)
{
  sample_rate = _sample_rate;
}

jack_nframes_t Gui_Mixer::get_sample_rate() const
{
  return sample_rate;
}

void Gui_Mixer::set_buffer_size(jack_nframes_t _buffer_size)
{
  buffer_size = _buffer_size;
}

jack_nframes_t Gui_Mixer::get_buffer_size() const
{
  return buffer_size;
}

void Gui_Mixer::LoadSettings()
{
  QSettings settings("AudioApp", "QtGuiMix");
  
  // Write the following file:
  // [NbStrips]
  // [mono/stereo] [stripName] [stripID] [coarsemin] [coarsemax] [coarsecurrent] [finemin] [finemax] [finecurrent] [mute] [solo] [pan]
  // ...
  
  Mixer_Strip * tmp_strip = NULL;
  jack_port_t * tmp_port_out_l = NULL, * tmp_port_in_l = NULL;
  jack_port_t * tmp_port_out_r = NULL, * tmp_port_in_r = NULL;
  std::stringstream port_name;
  QWidget *mixerCentralWidget = this->centralWidget();

  int                  load_nbStrips = 0, result = 0;
  std::vector<QString> load_stripName;
  std::vector<int>     load_stripID;
  std::vector<int>     load_stripType;
  std::vector<float>   load_coarseMin, load_coarseMax, load_coarseCurrent;
  std::vector<float>   load_fineMin, load_fineMax, load_fineCurrent;
  std::vector<bool>    load_mute, load_solo;
  std::vector<float>   load_pan;
  bool convert_result = false;
  
  // read the number of strips
  load_nbStrips = settings.value("NbStrips", 0).toInt();

  for(int i=0; i<load_nbStrips; i++)
    {
      load_stripType.push_back(settings.value(QString("MixerType/") + QString::number(i)).toInt());
      load_stripName.push_back(settings.value(QString("StripName/") + QString::number(i)).toString());
      load_stripID.push_back(settings.value(QString("StripID/") + QString::number(i)).toInt());
      load_coarseMin.push_back(settings.value(QString("CoarseMin/") + QString::number(i)).toDouble());
      load_coarseMax.push_back(settings.value(QString("CoarseMax/") + QString::number(i)).toDouble());
      load_coarseCurrent.push_back(settings.value(QString("CoarseCurrent/") + QString::number(i)).toDouble());
      load_fineMin.push_back(settings.value(QString("FineMin/") + QString::number(i)).toDouble());
      load_fineMax.push_back(settings.value(QString("FineMax/") + QString::number(i)).toDouble());
      load_fineCurrent.push_back(settings.value(QString("FineCurrent/") + QString::number(i)).toDouble());
      load_mute.push_back(settings.value(QString("Mute/") + QString::number(i)).toBool());
      load_solo.push_back(settings.value(QString("Solo/") + QString::number(i)).toBool());
      load_pan.push_back(settings.value(QString("Pan/") + QString::number(i)).toDouble());
    }
  
  // Now, remove all strips and create new ones
  for(int i=0; i < strips.size(); i++)
    {
      // First remove jack ports
      for(int j=0; j<strips[i]->get_nb_channel(); ++j)
	{
#ifdef ENABLE_JACK
	  result = jack_port_unregister(client, strips[i]->get_input_port(j));
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the input port of the mixer strip ")) << i << "." << std::endl;
	  
	  result = jack_port_unregister(client, strips[i]->get_output_port(j));
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the output port of the mixer strip ")) << i << "." << std::endl;
#endif
	  
	  // Close the Widget
	  strips[i]->close();

	  // Remove the Widget from the layout
	  ((QHBoxLayout *)centralWidget()->layout())->removeWidget(tmp_strip);

	  // Delete the Widget
	  delete strips[i];
	}
    }

  strips.resize(0);
  
  // Create the new mixer strips
  for(int i=0; i<load_nbStrips; i++)
    {
      qDebug() << "DEBUG: strip type = " << load_stripType[i];

      switch(load_stripType[i])
	{
	case Mixer_Strip::mono:
	  qDebug() << "load settings: add a mono mixer";

	  // Create a Mono Mixer strip
	  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 1);
	  tmp_strip->set_jack_client(client);

	  // Create output port
	  port_name.str("");
	  port_name << "mixer_out_" << load_stripID[i];
#ifdef ENABLE_JACK
	  tmp_port_out_l = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif

	  // Create input port
	  port_name.str("");
	  port_name << "mixer_in_" << load_stripID[i];
#ifdef ENABLE_JACK
	  tmp_port_in_l = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);

	  if ((tmp_port_in_l == NULL) || (tmp_port_out_l == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  strips.push_back(tmp_strip);
	  
	  tmp_strip->set_input_port(0,  tmp_port_in_l);
	  tmp_strip->set_output_port(0, tmp_port_out_l);

	  tmp_strip->set_coarse_min(load_coarseMin[i]);
	  tmp_strip->set_coarse_max(load_coarseMax[i]);
	  tmp_strip->set_coarse_current(load_coarseCurrent[i]);

	  tmp_strip->set_fine_min(load_fineMin[i]);
	  tmp_strip->set_fine_max(load_fineMax[i]);
	  tmp_strip->set_fine_current(load_fineCurrent[i]);

	  tmp_strip->set_title(load_stripName[i]);

	  tmp_strip->set_mute(load_mute[i]);
	  tmp_strip->set_solo(load_solo[i]);
	  tmp_strip->set_pan(load_pan[i]);

	  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
	  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
	  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
	  break;

	case Mixer_Strip::stereo:
	  qDebug() << "load settings: add a stereo mixer";

	  // Create a Stereo Mixer strip
	  tmp_strip = new Mixer_Strip(mixerCentralWidget, index_mixer_strip, 2);
	  tmp_strip->set_jack_client(client);

	  // Create left output port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_l_out";
#ifdef ENABLE_JACK
	  tmp_port_out_l = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif
	  
	  // Create left input port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_l_in";
#ifdef ENABLE_JACK
	  tmp_port_in_l = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);
	  
	  if ((tmp_port_in_l == NULL) || (tmp_port_out_l == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  // Create right output port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_r_out";
#ifdef ENABLE_JACK
	  tmp_port_out_r = jack_port_register(client, port_name.str().c_str(),
					      JACK_DEFAULT_AUDIO_TYPE,
					      JackPortIsOutput, 0);
#endif
	  
	  // Create right input port
	  port_name.str("");
	  port_name << "mixer_" << load_stripID[i] << "_r_in";
#ifdef ENABLE_JACK
	  tmp_port_in_r = jack_port_register(client, port_name.str().c_str(),
					     JACK_DEFAULT_AUDIO_TYPE,
					     JackPortIsInput, 0);
	  
	  if ((tmp_port_in_r == NULL) || (tmp_port_out_r == NULL))
	    {
	      std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	      exit(1);
	    }
#endif
	  
	  strips.push_back(tmp_strip);
	  
	  tmp_strip->set_input_port(0,  tmp_port_in_l);
	  tmp_strip->set_output_port(0, tmp_port_out_l);
	  tmp_strip->set_input_port(1,  tmp_port_in_r);
	  tmp_strip->set_output_port(1, tmp_port_out_r);
	  
	  tmp_strip->set_coarse_min(load_coarseMin[i]);
	  tmp_strip->set_coarse_max(load_coarseMax[i]);
	  tmp_strip->set_coarse_current(load_coarseCurrent[i]);

	  tmp_strip->set_fine_min(load_fineMin[i]);
	  tmp_strip->set_fine_max(load_fineMax[i]);
	  tmp_strip->set_fine_current(load_fineCurrent[i]);

	  tmp_strip->set_title(load_stripName[i]);

	  tmp_strip->set_mute(load_mute[i]);
	  tmp_strip->set_solo(load_solo[i]);
	  tmp_strip->set_pan(load_pan[i]);

	  ((QHBoxLayout *)centralWidget()->layout())->addWidget(tmp_strip);
	  ((QHBoxLayout *)centralWidget()->layout())->setAlignment(tmp_strip, Qt::AlignLeft);
	  ((QHBoxLayout *)centralWidget()->layout())->addStretch(0);
	  break;
	}
    }
  
  this->adjustSize();

  // Search for the maximum strip id
  index_mixer_strip = -1;
  for(int i=0; i<load_nbStrips; i++)
    index_mixer_strip = std::max(load_stripID[i],index_mixer_strip);
  index_mixer_strip++;
}

void Gui_Mixer::SaveSettings()
{
  QSettings settings("AudioApp", "QtGuiMix");
  
  // Write the following file:
  // [NbStrips]
  // [mono/stereo] [stripName] [stripID] [coarsemin] [coarsemax] [coarsecurrent] [finemin] [finemax] [finecurrent] [mute] [solo] [pan]
  // ...
  
  settings.setValue("NbStrips", (int)strips.size());
  for(int i=0; i<strips.size(); i++)
    {
      settings.setValue(QString("MixerType/")     + QString::number(i), strips[i]->get_mixer_type());
      settings.setValue(QString("StripName/")     + QString::number(i), strips[i]->get_title().replace(' ','_'));
      settings.setValue(QString("StripID/")       + QString::number(i), strips[i]->get_strip_id());
      settings.setValue(QString("CoarseMin/")     + QString::number(i), (double)strips[i]->get_coarse_min());
      settings.setValue(QString("CoarseMax/")     + QString::number(i), (double)strips[i]->get_coarse_max());
      settings.setValue(QString("CoarseCurrent/") + QString::number(i), (double)strips[i]->get_coarse_current());
      settings.setValue(QString("FineMin/")       + QString::number(i), (double)strips[i]->get_fine_min());
      settings.setValue(QString("FineMax/")       + QString::number(i), (double)strips[i]->get_fine_max());
      settings.setValue(QString("FineCurrent/")   + QString::number(i), (double)strips[i]->get_fine_current());
      settings.setValue(QString("Mute/")          + QString::number(i), strips[i]->get_mute());
      settings.setValue(QString("Solo/")          + QString::number(i), strips[i]->get_solo());
      settings.setValue(QString("Pan/")           + QString::number(i), (double)strips[i]->get_pan());
    }
}

void Gui_Mixer::closeEvent(QCloseEvent *event)
{
  SaveSettings();
  event->accept();
}
