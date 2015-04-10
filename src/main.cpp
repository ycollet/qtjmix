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

#include <QtWidgets>
#include <QDebug>

#include <iostream>
#include <sstream>
#include <string>

#include <signal.h>
#include <stdio.h>

#include <jack/jack.h>
#include <jack/types.h>
#include <jack/session.h>

#include "gui_mixer.h"

jack_client_t *client = NULL;
int qtjmix_quit = 0;

static void signal_handler(int sig)
{
  jack_client_close(client);
  std::cerr << QT_TRANSLATE_NOOP("main","signal received, exiting ...") << std::endl;
  exit(0);
}

int qtjmix_process(jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in = NULL, *out = NULL;
  std::vector<Mixer_Strip *> * ptrMixerStrip = (std::vector<Mixer_Strip *> *)arg;
  float volume = 0.0, rms_level = 0.0, max_level = -1.0, pan[2] = {0.0, 0.0};
  int i, j, k;

  for(i = 0; i<ptrMixerStrip->size(); i++)
    {
      if (!(*ptrMixerStrip)[i]->get_mute()) volume = (*ptrMixerStrip)[i]->get_volume();
      else                                  volume = 0.0;

      if ((*ptrMixerStrip)[i]->get_mixer_type()==Mixer_Strip::stereo)
	{
	  pan[0] = ((*ptrMixerStrip)[i]->get_pan() + 50.0) / 100.0;
	  pan[1] = 1.0 - pan[0];
	}

      for(j = 0; j<(*ptrMixerStrip)[i]->get_nb_channel(); j++)
	{
	  out = (jack_default_audio_sample_t*)jack_port_get_buffer((*ptrMixerStrip)[i]->get_output_port(j), nframes);
	  in  = (jack_default_audio_sample_t*)jack_port_get_buffer((*ptrMixerStrip)[i]->get_input_port(j),  nframes);

	  rms_level = 0.0;
	  max_level = 0.0;

	  if ((*ptrMixerStrip)[i]->get_mixer_type()==Mixer_Strip::stereo)
	    {
	      for(k = 0; k<nframes; k++) 
		{
		  out[k] = volume * pan[j] * in[k];
		  rms_level += out[k]*out[k];
		  max_level = (max_level < out[k] ? out[k] : max_level);
		}
	    }
	  else
	    {
	      for(k = 0; k<nframes; k++) 
		{
		  out[k] = volume * in[k];
		  rms_level += out[k]*out[k];
		  max_level = (max_level < out[k] ? out[k] : max_level);
		}
	      rms_level /= (float)nframes;
	    }
	  (*ptrMixerStrip)[i]->set_rms_level(j, rms_level);
	  (*ptrMixerStrip)[i]->set_max_level(j, max_level);
	}
    }

  return 0;      
}

void qtjmix_session_callback(jack_session_event_t *event, void *arg)
{
  std::stringstream retval;

  qDebug() << "DEBUG: session notification";
  qDebug() << "path " << event->session_dir << ", uuid " << event->client_uuid << ", type: " << (event->type == JackSessionSave ? "save" : "quit");
  
  // Build the command line
  retval.str("");
  retval << "qtjmix " << event->client_uuid;
  event->command_line = strdup(retval.str().c_str());
  
  jack_session_reply(client, event);
  
  if (event->type==JackSessionSaveAndQuit)
    qtjmix_quit = 1;
  
  jack_session_event_free(event);
}

void jack_shutdown(void *arg)
{
  exit(1);
}

int main(int argc, char * argv[])
{
  int result = 0;
  const char *client_name = "QtJMIX";
  const char *server_name = NULL;
  jack_status_t status;

#ifdef ENABLE_JACK
  if (argc==1)        client = jack_client_open(client_name, JackNullOption, &status);
  else if (argc == 2) client = jack_client_open(client_name, JackSessionID, &status, argv[1]);
  
  if (client == NULL) 
    {
      std::cerr << QT_TRANSLATE_NOOP("main","jack_client_open() failed, status = ") << status << ")." << std::endl;
      if (status & JackServerFailed) 
	{
	  std::cerr << QT_TRANSLATE_NOOP("main","Unable to connect to JACK server.") << std::endl;
	}
      exit (1);
    }

  if (status & JackServerStarted) 
    {
      std::cerr << QT_TRANSLATE_NOOP("main","JACK server started.") << std::endl;
    }

  if (status & JackNameNotUnique) 
    {
      client_name = jack_get_client_name(client);
      std::cerr << QT_TRANSLATE_NOOP("main","unique name `") << client_name << QT_TRANSLATE_NOOP("main","' assigned") << std::endl;
    }

  jack_on_shutdown(client, jack_shutdown, 0);
#endif

  /* install a signal handler to properly quits jack client */
#ifdef WIN32
  signal(SIGINT,  signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGTERM, signal_handler);
#else
  signal(SIGQUIT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP,  signal_handler);
  signal(SIGINT,  signal_handler);
#endif

  QApplication app(argc, argv);
  app.setStyle("fusion");

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator myappTranslator;
  if (myappTranslator.load("qtjmix_" + QLocale::system().name().toLower()))
    {
      app.installTranslator(&myappTranslator);
    }
  else
    {
      std::cerr << QT_TRANSLATE_NOOP("main","failed to load translation file ") << "qtjmix" << qPrintable(QLocale::system().name().toLower()) << "." << std::endl;
    }

  Gui_Mixer * mainMixer = new Gui_Mixer(NULL);

  mainMixer->set_jack_client(client);
  
  if (argc==2) mainMixer->load_config_file(argv[1]);
  else         
    {
      mainMixer->init_2_strips();
      mainMixer->LoadSettings();
    }
  
  mainMixer->show();

  mainMixer->set_sample_rate(jack_get_sample_rate(client));
  mainMixer->set_buffer_size(jack_get_buffer_size(client));

#ifdef ENABLE_JACK
  /* Tell the JACK server that we are ready to roll.  Our process() callback will start running now. */
  jack_set_process_callback(client, qtjmix_process, (void *)&mainMixer->get_strip_list());

  /* tell the JACK server to call `session_callback()' if the session is saved. */
  jack_set_session_callback (client, qtjmix_session_callback, NULL);

  /* Start the jack client */
  if (jack_activate(client))
    {
      std::cerr << QT_TRANSLATE_NOOP("main","cannot activate client.") << std::endl;
      exit (1);
    }
#endif

  // if qtjmix_quit == 1, we should quit
  result = app.exec();

#ifdef ENABLE_JACK
  jack_client_close(client);
#endif

  return result;
}
