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

#include <iostream>
#include <sstream>

#include <math.h>

#include <jack/jack.h>

#include "mixer_strip.h"
#include "awl/fastlog.h"
#include "set_mixer_param_dialog.h"

#define DB_VOL_MAX  3.0e1
#define DB_VOL_MIN  3.0e-3
#define DB_VOL_INIT 3.0
#define PAN_INIT    0.0

#define ATT_RMS_LEVEL 0.95
#define ATT_MAX_LEVEL 0.99


Mixer_Strip::Mixer_Strip(QWidget * _parent, int _stripNumber, int _nb_channel) : QFrame(_parent)
{
  std::stringstream tmpLabel;

  client        = NULL;
  volume        = DB_VOL_INIT;
  pan           = PAN_INIT;
  att_max_level = ATT_MAX_LEVEL;
  att_rms_level = ATT_RMS_LEVEL;
  nb_channel    = _nb_channel;
  paramDialog   = NULL;

  input_port.resize(nb_channel,    NULL);
  output_port.resize(nb_channel,   NULL);
  rms_level.resize(nb_channel,     0.0);
  max_level.resize(nb_channel,     0.0);
  rms_level_n_1.resize(nb_channel, 0.0);
  max_level_n_1.resize(nb_channel, 0.0);

  stripLabel = new QLabel(this);
  typeLabel  = new QLabel(this);

  tmpLabel.str("");
  tmpLabel << QT_TR_NOOP("Strip ");
  tmpLabel << _stripNumber;

  title = QString(tmpLabel.str().c_str());

  stripLabel->setText(tmpLabel.str().c_str());

  if (nb_channel==1)
    {
      typeLabel->setText(tr("mono"));
      type = mono;
    }
  else
    {
      typeLabel->setText(tr("stereo"));
      type = stereo;
    }

  coarseDial = new Awl::MeterSlider(this);
  coarseDial->setLog(true);
  coarseDial->setMinLogValue(DB_VOL_MIN);
  coarseDial->setMaxLogValue(DB_VOL_MAX);
  coarseDial->setValue(DB_VOL_INIT);
  coarseDial->setChannel(nb_channel);
  coarseDial->setToolTip(tr("Coarse tuning"));
  connect(coarseDial, SIGNAL(valueChanged(double, int)), this, SLOT(volume_changed(double, int)));

  fineDial = new Awl::VolKnob(this);
  fineDial->setLog(true);
  fineDial->setMinLogValue(DB_VOL_MIN);
  fineDial->setMaxLogValue(DB_VOL_MAX);
  fineDial->setValue(DB_VOL_INIT);
  fineDial->setToolTip(tr("Fine tuning"));
  connect(fineDial, SIGNAL(valueChanged(double, int)), this, SLOT(volume_changed(double, int)));

  panDial = new Awl::PanKnob(this);
  panDial->setRange(-50.0,50.0);
  panDial->setValue(0.0);
  panDial->setToolTip(tr("Panoramic tuning"));
  if (type==mono) panDial->setEnabled(false);
  else            panDial->setEnabled(true);
  connect(panDial, SIGNAL(valueChanged(double, int)), this, SLOT(pan_changed(double, int)));

  muteButton = new QCheckBox(this);
  muteButton->setCheckState(Qt::Unchecked);
  muteButton->setText(tr("Mute"));
  muteButton->setToolTip(tr("Mute the mixer strip"));
  connect(muteButton, SIGNAL(stateChanged(int)), this, SLOT(mute_changed(int)));

  soloButton = new QCheckBox(this);
  soloButton->setCheckState(Qt::Unchecked);
  soloButton->setText(tr("Solo"));
  soloButton->setToolTip(tr("Solo the mixer strip"));
  connect(soloButton, SIGNAL(stateChanged(int)), this, SLOT(solo_changed(int)));

  QVBoxLayout *stripLayout = new QVBoxLayout(this);
  stripLayout->addWidget(stripLabel);
  stripLayout->addWidget(typeLabel);
  stripLayout->addWidget(panDial);
  stripLayout->addWidget(fineDial);
  stripLayout->addWidget(coarseDial);
  stripLayout->addWidget(muteButton);
  stripLayout->addWidget(soloButton);
  stripLayout->setAlignment(stripLabel, Qt::AlignHCenter);
  stripLayout->setAlignment(typeLabel,  Qt::AlignHCenter);
  stripLayout->setAlignment(panDial,    Qt::AlignHCenter);
  stripLayout->setAlignment(fineDial,   Qt::AlignHCenter);
  stripLayout->setAlignment(coarseDial, Qt::AlignHCenter);
  stripLayout->setAlignment(muteButton, Qt::AlignLeft);
  stripLayout->setAlignment(soloButton, Qt::AlignLeft);

  // Initialize the volume
  volume = 0.5*(fineDial->value() / fineDial->maxValue() + coarseDial->value() / coarseDial->maxValue());

  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->setLayout(stripLayout);
  this->setLineWidth(1);
  this->setFrameShape(Panel);
  this->setFocusPolicy(Qt::ClickFocus);
  this->show();
}

void Mixer_Strip::volume_changed(double _value, int _index)
{
  volume = 0.5*(fineDial->value() / fineDial->maxValue() + coarseDial->value() / coarseDial->maxValue());

  qDebug() << QT_TR_NOOP("DEBUG: volume changed = ") << volume << " index = " << _index;
  qDebug() << "coarse = " << coarseDial->value() << " / " << coarseDial->maxValue();
  qDebug() << "fine   = " << fineDial->value() << " / " << fineDial->maxValue();
}

void Mixer_Strip::mute_changed(int _value)
{
  // TODO
}

void Mixer_Strip::solo_changed(int _value)
{
  // TODO
}

void Mixer_Strip::pan_changed(double _value, int _index)
{
  pan = _value;
}

float Mixer_Strip::get_fine_volume() const
{
  return fineDial->value();
}

float Mixer_Strip::get_coarse_volume() const
{
  return coarseDial->value();
}

float Mixer_Strip::get_volume() const
{
  return volume;
}

bool Mixer_Strip::get_mute() const
{
  return muteButton->isChecked();
}

void Mixer_Strip::set_mute(bool _mute)
{
  muteButton->setChecked(_mute);
}

bool Mixer_Strip::get_solo() const
{
  return soloButton->isChecked();
}

void Mixer_Strip::set_solo(bool _solo)
{
  soloButton->setChecked(_solo);
}

jack_port_t * Mixer_Strip::get_output_port(int _index) const
{
  return output_port[_index];
}

void Mixer_Strip::set_output_port(int _index, jack_port_t * _port)
{
  output_port[_index] = _port;
}

jack_port_t * Mixer_Strip::get_input_port(int _index) const
{
  return input_port[_index];
}

void Mixer_Strip::set_input_port(int _index, jack_port_t * _port)
{
  input_port[_index] = _port;
}

void Mixer_Strip::set_rms_level(int _index, float _rms_level)
{
  rms_level_n_1[_index] = rms_level[_index];
  rms_level[_index] = att_rms_level * rms_level_n_1[_index] + (1.0 - att_rms_level) * fast_log10(_rms_level);
  if (rms_level[_index]>DB_VOL_MAX) rms_level[_index] = DB_VOL_MAX;
  if (rms_level[_index]<DB_VOL_MIN) rms_level[_index] = DB_VOL_MIN;
  coarseDial->setMeterVal(_index, rms_level[_index], max_level[_index]);
}

void Mixer_Strip::set_max_level(int _index, float _max_level)
{
  max_level_n_1[_index] = max_level[_index];
  max_level[_index] = att_max_level * max_level_n_1[_index] + (1.0 - att_max_level) * _max_level;
  coarseDial->setMeterVal(_index, rms_level[_index], max_level[_index]);
}

float Mixer_Strip::get_max_level(int _index) const
{
  return max_level[_index];
}

float Mixer_Strip::get_rms_level(int _index) const
{
  return rms_level[_index];
}

void Mixer_Strip::set_att_max_level(float _att_max_level)
{
  att_max_level = _att_max_level;
}

float Mixer_Strip::get_att_max_level() const
{
  return att_max_level;
}

void Mixer_Strip::set_att_rms_level(float _att_rms_level)
{
  att_rms_level = _att_rms_level;
}

float Mixer_Strip::get_att_rms_level() const
{
  return att_rms_level;
}

void Mixer_Strip::set_nb_channel(int _nb_channel)
{
  int result = 0, i = 0;
  std::stringstream port_name;
  jack_port_t *tmp_port_out = NULL, *tmp_port_in = NULL;

  if (nb_channel==_nb_channel) return;

  nb_channel = _nb_channel;
  input_port.resize(nb_channel, NULL);
  output_port.resize(nb_channel, NULL);
  rms_level.resize(nb_channel, 0.0);
  max_level.resize(nb_channel, 0.0);

  if (nb_channel==1) 
    {
      type = mono;
      typeLabel->setText(tr("mono"));

      for(i=0; i<input_port.size(); i++)
	{
	  result = jack_port_unregister(client, input_port[i]);
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the input port of the mixer strip ")) << i << "." << std::endl;
	  
	  result = jack_port_unregister(client, output_port[i]);
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the output port of the mixer strip ")) << i << "." << std::endl;
	}
      
      input_port.resize(1, NULL);
      output_port.resize(1, NULL);
      
      // Create input port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_m_in";
#ifdef ENABLE_JACK
      tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				       JACK_DEFAULT_AUDIO_TYPE,
				       JackPortIsInput, 0);
#endif
      
      // Create output port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_m_out";
#ifdef ENABLE_JACK
      tmp_port_out = jack_port_register(client, port_name.str().c_str(),
					JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsOutput, 0);
#endif

#ifdef ENABLE_JACK
      if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
	{
          std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	  exit (1);
        }
#endif
      
      set_input_port(0, tmp_port_in);
      set_output_port(0, tmp_port_out);
      rms_level.resize(1, 0.0);
      rms_level_n_1.resize(1, 0.0);
      max_level.resize(1, 0.0);
      max_level_n_1.resize(1, 0.0);
      panDial->setEnabled(false);
    }
  else
    {
      type = stereo;
      typeLabel->setText(tr("stereo"));

      for(i=0; i<input_port.size(); i++)
	{
	  result = jack_port_unregister(client, input_port[i]);
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the input port of the mixer strip ")) << i << "." << std::endl;
	  
	  result = jack_port_unregister(client, output_port[i]);
	  if (result)
	    std::cerr << qPrintable(tr("QTJMix: problem while unregistering the output port of the mixer strip ")) << i << "." << std::endl;
	}
      
      input_port.resize(2, NULL);
      output_port.resize(2, NULL);

      // Create left input port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_l_in";
#ifdef ENABLE_JACK
      tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				       JACK_DEFAULT_AUDIO_TYPE,
				       JackPortIsInput, 0);
#endif
      
      // Create left output port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_l_out" << strip_id;
#ifdef ENABLE_JACK
      tmp_port_out = jack_port_register(client, port_name.str().c_str(),
					JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsOutput, 0);
#endif

#ifdef ENABLE_JACK
      if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
	{
          std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	  exit (1);
        }
#endif
      
      set_input_port(0, tmp_port_in);
      set_output_port(0, tmp_port_out);

      // Create right input port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_r_in";
#ifdef ENABLE_JACK
      tmp_port_in = jack_port_register(client, port_name.str().c_str(),
				       JACK_DEFAULT_AUDIO_TYPE,
				       JackPortIsInput, 0);
#endif
      
      // Create right output port
      port_name.str("");
      port_name << "mixer_" << strip_id << "_r_out";
#ifdef ENABLE_JACK
      tmp_port_out = jack_port_register(client, port_name.str().c_str(),
					JACK_DEFAULT_AUDIO_TYPE,
					JackPortIsOutput, 0);
#endif

#ifdef ENABLE_JACK
      if ((tmp_port_in == NULL) || (tmp_port_out == NULL))
	{
          std::cerr << qPrintable(tr("no more JACK ports available")) << std::endl;
	  exit (1);
        }
#endif
      
      set_input_port(1, tmp_port_in);
      set_output_port(1, tmp_port_out);

      rms_level.resize(2, 0.0);
      rms_level_n_1.resize(2, 0.0);
      max_level.resize(2, 0.0);
      max_level_n_1.resize(2, 0.0);
      panDial->setEnabled(true);
    }
}

int Mixer_Strip::get_nb_channel() const
{
  return nb_channel;
}

float Mixer_Strip::get_pan() const
{
  return pan;
}

void Mixer_Strip::set_pan(float _pan)
{
  pan = _pan;
  panDial->setValue(pan);
}

Mixer_Strip::mixer_strip_t Mixer_Strip::get_mixer_type() const
{
  return type;
}

void Mixer_Strip::set_mixer_type(Mixer_Strip::mixer_strip_t _type)
{
  type = _type;
}

void Mixer_Strip::set_title(QString _title)
{
  title = _title;
  stripLabel->setText(title);
}

QString Mixer_Strip::get_title() const
{
  return title;
}

void Mixer_Strip::focusInEvent(QFocusEvent * event)
{
  qDebug() << "DEBUG: in focusInEvent";

  QFrame::focusInEvent(event);
  this->setLineWidth(2);
}

void Mixer_Strip::focusOutEvent(QFocusEvent * event)
{
  qDebug() << "DEBUG: in focusOutEvent";

  QFrame::focusOutEvent(event);
  this->setLineWidth(1);
}

void Mixer_Strip::mouseReleaseEvent(QMouseEvent * event)
{
  qDebug() << "DEBUG: in mouseReleaseEvent";

  QFrame::mouseReleaseEvent(event);

  if (event->button()==Qt::RightButton)
    {
      paramDialog = new SetMixerParamDialog(this);
      paramDialog->setModal(true);
      paramDialog->show();
      connect(paramDialog, SIGNAL(accepted()), this, SLOT(change_parameters()));
    }
}

void Mixer_Strip::change_parameters()
{
  qDebug() << "DEBUG: in change_parameters";

  if ((paramDialog) && (paramDialog->get_value_changed()))
    {
      qDebug() << "DEBUG: in change_parameters - value changed";
      
      this->set_coarse_min(paramDialog->get_coarse_min_value());
      this->set_coarse_max(paramDialog->get_coarse_max_value());
      this->set_coarse_current(paramDialog->get_coarse_current_value());
      
      this->set_fine_min(paramDialog->get_fine_min_value());
      this->set_fine_max(paramDialog->get_fine_max_value());
      this->set_fine_current(paramDialog->get_fine_current_value());
      
      this->set_att_rms_level(paramDialog->get_att_rms_value());
      this->set_att_max_level(paramDialog->get_att_max_value());
      
      if (this->get_mixer_type()==Mixer_Strip::mono)
	{
	  jack_port_set_name(input_port[0],  qPrintable(paramDialog->get_mono_in_name()));
	  jack_port_set_name(output_port[0], qPrintable(paramDialog->get_mono_out_name()));
	}
      else
	{
	  jack_port_set_name(input_port[0],  qPrintable(paramDialog->get_stereo_in_left_name()));
	  jack_port_set_name(input_port[1],  qPrintable(paramDialog->get_stereo_in_right_name()));
	  jack_port_set_name(output_port[0], qPrintable(paramDialog->get_stereo_in_left_name()));
	  jack_port_set_name(output_port[1], qPrintable(paramDialog->get_stereo_in_right_name()));
	}

      delete paramDialog;
      paramDialog = NULL;
    }
}

float Mixer_Strip::get_coarse_min() const
{
  return coarseDial->minValue();
}

float Mixer_Strip::get_coarse_max() const
{
  return coarseDial->maxValue();
}

float Mixer_Strip::get_coarse_current() const
{
  return coarseDial->value();
}

void  Mixer_Strip::set_coarse_min(float _minValue)
{
  coarseDial->setMinValue(_minValue);
  if (_minValue >= coarseDial->maxValue()) coarseDial->setMaxValue(_minValue + 10);
}

void  Mixer_Strip::set_coarse_max(float _maxValue)
{
  coarseDial->setMinValue(_maxValue);
  if (_maxValue <= coarseDial->minValue()) coarseDial->setMinValue(_maxValue - 10);
}

void  Mixer_Strip::set_coarse_current(float _Value)
{
  coarseDial->setValue(_Value);
  if (_Value > coarseDial->maxValue()) coarseDial->setValue(coarseDial->maxValue());
  if (_Value < coarseDial->minValue()) coarseDial->setValue(coarseDial->minValue());
}

float Mixer_Strip::get_fine_min() const
{
  return fineDial->minValue();
}

float Mixer_Strip::get_fine_max() const
{
  return fineDial->maxValue();
}

float Mixer_Strip::get_fine_current() const
{
  return fineDial->value();
}

void  Mixer_Strip::set_fine_min(float _minValue)
{
  fineDial->setMinValue(_minValue);
  if (_minValue >= fineDial->maxValue()) fineDial->setMaxValue(_minValue + 10);
}

void  Mixer_Strip::set_fine_max(float _maxValue)
{
  fineDial->setMaxValue(_maxValue);
  if (_maxValue <= fineDial->minValue()) fineDial->setMinValue(_maxValue - 10);
}

void  Mixer_Strip::set_fine_current(float _Value)
{
  fineDial->setValue(_Value);
  if (_Value > fineDial->maxValue()) fineDial->setValue(fineDial->maxValue());
  if (_Value < fineDial->minValue()) fineDial->setValue(fineDial->minValue());
}

QString Mixer_Strip::get_strip_title() const
{
  return stripLabel->text();
}

void Mixer_Strip::set_jack_client(jack_client_t * _client)
{
  client = _client;
}

jack_client_t * Mixer_Strip::get_jack_client() const
{
  return client;
}

void Mixer_Strip::set_strip_id(int _strip_id)
{
  strip_id = _strip_id;
}

int Mixer_Strip::get_strip_id() const
{
  return strip_id;
}
