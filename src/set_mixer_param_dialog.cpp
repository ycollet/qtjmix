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

#include "set_mixer_param_dialog.h"

#include <QtWidgets>

#include <jack/jack.h>

SetMixerParamDialog::SetMixerParamDialog(Mixer_Strip *_strip, QWidget *parent) : QDialog(parent)
{
  strip = _strip;
  
  QGridLayout *setParamLayout = new QGridLayout(this);
  QDoubleValidator *dblValid  = new QDoubleValidator(this);

  setWindowTitle(QString("QtJMix '") + strip->get_strip_title() + QString(tr("' Tuning parameters")));
  setWindowIcon(QIcon(":/images/MainIcon"));
  
  QLineEdit *coarseMinValLE    = new QLineEdit(parent);
  QLabel *coarseMinValLabel    = new QLabel(tr("coarse min value:"), parent);
  QHBoxLayout *coarseMinLayout = new QHBoxLayout(parent);
  QWidget *coarseMinWidget     = new QWidget(parent);
  coarseMinValLabel->setBuddy(coarseMinValLE);
  coarseMinValLE->setText(QString::number(strip->get_coarse_min()));
  coarseMinValLE->setCursorPosition(0);
  coarseMinValLE->setValidator(dblValid);
  coarseMinValLE->setEchoMode(QLineEdit::Normal);
  coarseMinLayout->addWidget(coarseMinValLabel);
  coarseMinLayout->addWidget(coarseMinValLE);
  coarseMinLayout->setAlignment(coarseMinValLabel, Qt::AlignLeft);
  coarseMinLayout->setAlignment(coarseMinValLE,    Qt::AlignLeft);
  coarseMinWidget->setLayout(coarseMinLayout);
  connect(coarseMinValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_coarse_min_value(const QString &)));

  QLineEdit *coarseMaxValLE    = new QLineEdit(parent);
  QLabel *coarseMaxValLabel    = new QLabel(tr("coarse max value:"), parent);
  QHBoxLayout *coarseMaxLayout = new QHBoxLayout(parent);
  QWidget *coarseMaxWidget     = new QWidget(parent);
  coarseMaxValLabel->setBuddy(coarseMaxValLE);
  coarseMaxValLE->setText(QString::number(strip->get_coarse_max()));
  coarseMaxValLE->setCursorPosition(0);
  coarseMaxValLE->setValidator(dblValid);
  coarseMaxValLE->setEchoMode(QLineEdit::Normal);
  coarseMaxLayout->addWidget(coarseMaxValLabel);
  coarseMaxLayout->addWidget(coarseMaxValLE);
  coarseMaxLayout->setAlignment(coarseMaxValLabel, Qt::AlignLeft);
  coarseMaxLayout->setAlignment(coarseMaxValLE,    Qt::AlignLeft);
  coarseMaxWidget->setLayout(coarseMaxLayout);
  connect(coarseMaxValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_coarse_max_value(const QString &)));

  QLineEdit *coarseCurrentValLE    = new QLineEdit(parent);
  QLabel *coarseCurrentValLabel    = new QLabel(tr("coarse current value:"), parent);
  QHBoxLayout *coarseCurrentLayout = new QHBoxLayout(parent);
  QWidget *coarseCurrentWidget     = new QWidget(parent);
  coarseCurrentValLabel->setBuddy(coarseCurrentValLE);
  coarseCurrentValLE->setText(QString::number(strip->get_coarse_current()));
  coarseCurrentValLE->setCursorPosition(0);
  coarseCurrentValLE->setValidator(dblValid);
  coarseCurrentValLE->setEchoMode(QLineEdit::Normal);
  coarseCurrentLayout->addWidget(coarseCurrentValLabel);
  coarseCurrentLayout->addWidget(coarseCurrentValLE);
  coarseCurrentLayout->setAlignment(coarseCurrentValLabel, Qt::AlignLeft);
  coarseCurrentLayout->setAlignment(coarseCurrentValLE,    Qt::AlignLeft);
  coarseCurrentWidget->setLayout(coarseCurrentLayout);
  connect(coarseCurrentValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_coarse_current_value(const QString &)));

  QLineEdit *fineMinValLE    = new QLineEdit(parent);
  QLabel *fineMinValLabel    = new QLabel(tr("fine min value:"), parent);
  QHBoxLayout *fineMinLayout = new QHBoxLayout(parent);
  QWidget *fineMinWidget     = new QWidget(parent);
  fineMinValLabel->setBuddy(fineMinValLE);
  fineMinValLE->setText(QString::number(strip->get_fine_min()));
  fineMinValLE->setCursorPosition(0);
  fineMinValLE->setValidator(dblValid);
  fineMinValLE->setEchoMode(QLineEdit::Normal);
  fineMinLayout->addWidget(fineMinValLabel);
  fineMinLayout->addWidget(fineMinValLE);
  fineMinLayout->setAlignment(fineMinValLabel, Qt::AlignLeft);
  fineMinLayout->setAlignment(fineMinValLE,    Qt::AlignLeft);
  fineMinWidget->setLayout(fineMinLayout);
  connect(fineMinValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_fine_min_value(const QString &)));

  QLineEdit *fineMaxValLE    = new QLineEdit(parent);
  QLabel *fineMaxValLabel    = new QLabel(tr("fine max value:"), parent);
  QHBoxLayout *fineMaxLayout = new QHBoxLayout(parent);
  QWidget *fineMaxWidget     = new QWidget(parent);
  fineMaxValLabel->setBuddy(fineMaxValLE);
  fineMaxValLE->setText(QString::number(strip->get_fine_max()));
  fineMaxValLE->setCursorPosition(0);
  fineMaxValLE->setValidator(dblValid);
  fineMaxValLE->setEchoMode(QLineEdit::Normal);
  fineMaxLayout->addWidget(fineMaxValLabel);
  fineMaxLayout->addWidget(fineMaxValLE);
  fineMaxLayout->setAlignment(fineMaxValLabel, Qt::AlignLeft);
  fineMaxLayout->setAlignment(fineMaxValLE,    Qt::AlignLeft);
  fineMaxWidget->setLayout(fineMaxLayout);
  connect(fineMaxValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_fine_max_value(const QString &)));

  QLineEdit *fineCurrentValLE    = new QLineEdit(parent);
  QLabel *fineCurrentValLabel    = new QLabel(tr("fine current value:"), parent);
  QHBoxLayout *fineCurrentLayout = new QHBoxLayout(parent);
  QWidget *fineCurrentWidget     = new QWidget(parent);
  fineCurrentValLabel->setBuddy(fineCurrentValLE);
  fineCurrentValLE->setText(QString::number(strip->get_fine_current()));
  fineCurrentValLE->setCursorPosition(0);
  fineCurrentValLE->setValidator(dblValid);
  fineCurrentValLE->setEchoMode(QLineEdit::Normal);
  fineCurrentLayout->addWidget(fineCurrentValLabel);
  fineCurrentLayout->addWidget(fineCurrentValLE);
  fineCurrentLayout->setAlignment(fineCurrentValLabel, Qt::AlignLeft);
  fineCurrentLayout->setAlignment(fineCurrentValLE,    Qt::AlignLeft);
  fineCurrentWidget->setLayout(fineCurrentLayout);
  connect(fineCurrentValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_fine_current_value(const QString &)));

  QLineEdit *attRMSValLE    = new QLineEdit(parent);
  QLabel *attRMSValLabel    = new QLabel(tr("att. rms (vis.):"), parent);
  QHBoxLayout *attRMSLayout = new QHBoxLayout(parent);
  QWidget *attRMSWidget     = new QWidget(parent);
  attRMSValLabel->setBuddy(attRMSValLE);
  attRMSValLE->setText(QString::number(strip->get_att_rms_level()));
  attRMSValLE->setCursorPosition(0);
  attRMSValLE->setValidator(dblValid);
  attRMSValLE->setEchoMode(QLineEdit::Normal);
  attRMSLayout->addWidget(attRMSValLabel);
  attRMSLayout->addWidget(attRMSValLE);
  attRMSLayout->setAlignment(attRMSValLabel, Qt::AlignLeft);
  attRMSLayout->setAlignment(attRMSValLE,    Qt::AlignLeft);
  attRMSWidget->setLayout(attRMSLayout);
  connect(attRMSValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_att_rms_value(const QString &)));

  QLineEdit *attMAXValLE    = new QLineEdit(parent);
  QLabel *attMAXValLabel    = new QLabel(tr("att. max (vis.):"), parent);
  QHBoxLayout *attMAXLayout = new QHBoxLayout(parent);
  QWidget *attMAXWidget     = new QWidget(parent);
  attMAXValLabel->setBuddy(attMAXValLE);
  attMAXValLE->setText(QString::number(strip->get_att_max_level()));
  attMAXValLE->setCursorPosition(0);
  attMAXValLE->setValidator(dblValid);
  attMAXValLE->setEchoMode(QLineEdit::Normal);
  attMAXLayout->addWidget(attMAXValLabel);
  attMAXLayout->addWidget(attMAXValLE);
  attMAXLayout->setAlignment(attMAXValLabel, Qt::AlignLeft);
  attMAXLayout->setAlignment(attMAXValLE,    Qt::AlignLeft);
  attMAXWidget->setLayout(attMAXLayout);
  connect(attMAXValLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_att_max_value(const QString &)));

  setParamLayout->addWidget(coarseMaxWidget);
  setParamLayout->addWidget(coarseCurrentWidget);
  setParamLayout->addWidget(coarseMinWidget);

  setParamLayout->addWidget(fineMaxWidget);
  setParamLayout->addWidget(fineCurrentWidget);
  setParamLayout->addWidget(fineMinWidget);

  setParamLayout->addWidget(attRMSWidget);
  setParamLayout->addWidget(attMAXWidget);

  if (strip->get_mixer_type()==Mixer_Strip::mono)
    {
      QLineEdit *monoInLE       = new QLineEdit(parent);
      QLabel *monoInLabel       = new QLabel(tr("mono In label:"), parent);
      QHBoxLayout *monoInLayout = new QHBoxLayout(parent);
      QWidget *monoInWidget     = new QWidget(parent);
      monoInLabel->setBuddy(monoInLE);
      monoInLE->setText(QString(jack_port_name(strip->get_input_port(0))));
      monoInLE->setCursorPosition(0);
      monoInLE->setEchoMode(QLineEdit::Normal);
      monoInLayout->addWidget(monoInLabel);
      monoInLayout->addWidget(monoInLE);
      monoInLayout->setAlignment(monoInLabel, Qt::AlignLeft);
      monoInLayout->setAlignment(monoInLE,    Qt::AlignLeft);
      monoInWidget->setLayout(monoInLayout);
      connect(monoInLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_mono_in_name(const QString &)));

      setParamLayout->addWidget(monoInWidget);

      QLineEdit *monoOutLE       = new QLineEdit(parent);
      QLabel *monoOutLabel       = new QLabel(tr("mono Out label:"), parent);
      QHBoxLayout *monoOutLayout = new QHBoxLayout(parent);
      QWidget *monoOutWidget     = new QWidget(parent);
      monoOutLabel->setBuddy(monoOutLE);
      monoOutLE->setText(QString(jack_port_name(strip->get_output_port(0))));
      monoOutLE->setCursorPosition(0);
      monoOutLE->setEchoMode(QLineEdit::Normal);
      monoOutLayout->addWidget(monoOutLabel);
      monoOutLayout->addWidget(monoOutLE);
      monoOutLayout->setAlignment(monoOutLabel, Qt::AlignLeft);
      monoOutLayout->setAlignment(monoOutLE,    Qt::AlignLeft);
      monoOutWidget->setLayout(monoOutLayout);
      connect(monoOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_mono_out_name(const QString &)));

      setParamLayout->addWidget(monoOutWidget);
    }
  else
    {
      QLineEdit *stereoInLLE       = new QLineEdit(parent);
      QLabel *stereoInLLabel       = new QLabel(tr("stereo left In label:"), parent);
      QHBoxLayout *stereoInLLayout = new QHBoxLayout(parent);
      QWidget *stereoInLWidget     = new QWidget(parent);
      stereoInLLabel->setBuddy(stereoInLLE);
      stereoInLLE->setText(QString(jack_port_name(strip->get_input_port(0))));
      stereoInLLE->setCursorPosition(0);
      stereoInLLE->setEchoMode(QLineEdit::Normal);
      stereoInLLayout->addWidget(stereoInLLabel);
      stereoInLLayout->addWidget(stereoInLLE);
      stereoInLLayout->setAlignment(stereoInLLabel, Qt::AlignLeft);
      stereoInLLayout->setAlignment(stereoInLLE,    Qt::AlignLeft);
      stereoInLWidget->setLayout(stereoInLLayout);
      connect(stereoInLLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_stereo_in_left_name(const QString &)));

      setParamLayout->addWidget(stereoInLWidget);

      QLineEdit *stereoInRLE       = new QLineEdit(parent);
      QLabel *stereoInRLabel       = new QLabel(tr("stereo right In label:"), parent);
      QHBoxLayout *stereoInRLayout = new QHBoxLayout(parent);
      QWidget *stereoInRWidget     = new QWidget(parent);
      stereoInRLabel->setBuddy(stereoInRLE);
      stereoInRLE->setText(QString(jack_port_name(strip->get_input_port(1))));
      stereoInRLE->setCursorPosition(0);
      stereoInRLE->setEchoMode(QLineEdit::Normal);
      stereoInRLayout->addWidget(stereoInRLabel);
      stereoInRLayout->addWidget(stereoInRLE);
      stereoInRLayout->setAlignment(stereoInRLabel, Qt::AlignLeft);
      stereoInRLayout->setAlignment(stereoInRLE,    Qt::AlignLeft);
      stereoInRWidget->setLayout(stereoInRLayout);
      connect(stereoInRLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_stereo_in_right_name(const QString &)));

      setParamLayout->addWidget(stereoInRWidget);

      QLineEdit *stereoOutLLE       = new QLineEdit(parent);
      QLabel *stereoOutLLabel       = new QLabel(tr("stereo left Out label:"), parent);
      QHBoxLayout *stereoOutLLayout = new QHBoxLayout(parent);
      QWidget *stereoOutLWidget     = new QWidget(parent);
      stereoOutLLabel->setBuddy(stereoOutLLE);
      stereoOutLLE->setText(QString(jack_port_name(strip->get_output_port(0))));
      stereoOutLLE->setCursorPosition(0);
      stereoOutLLE->setEchoMode(QLineEdit::Normal);
      stereoOutLLayout->addWidget(stereoOutLLabel);
      stereoOutLLayout->addWidget(stereoOutLLE);
      stereoOutLLayout->setAlignment(stereoOutLLabel, Qt::AlignLeft);
      stereoOutLLayout->setAlignment(stereoOutLLE,    Qt::AlignLeft);
      stereoOutLWidget->setLayout(stereoOutLLayout);
      connect(stereoOutLLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_stereo_out_left_name(const QString &)));

      setParamLayout->addWidget(stereoOutLWidget);

      QLineEdit *stereoOutRLE       = new QLineEdit(parent);
      QLabel *stereoOutRLabel       = new QLabel(tr("stereo right Out label:"), parent);
      QHBoxLayout *stereoOutRLayout = new QHBoxLayout(parent);
      QWidget *stereoOutRWidget     = new QWidget(parent);
      stereoOutRLabel->setBuddy(stereoOutRLE);
      stereoOutRLE->setText(QString(jack_port_name(strip->get_output_port(1))));
      stereoOutRLE->setCursorPosition(0);
      stereoOutRLE->setEchoMode(QLineEdit::Normal);
      stereoOutRLayout->addWidget(stereoOutRLabel);
      stereoOutRLayout->addWidget(stereoOutRLE);
      stereoOutRLayout->setAlignment(stereoOutRLabel, Qt::AlignLeft);
      stereoOutRLayout->setAlignment(stereoOutRLE,    Qt::AlignLeft);
      stereoOutRWidget->setLayout(stereoOutRLayout);
      connect(stereoOutRLE, SIGNAL(textChanged(const QString &)), this, SLOT(set_stereo_out_right_name(const QString &)));

      setParamLayout->addWidget(stereoOutRWidget);
    }

  QPushButton *okButton     = new QPushButton(tr("&Ok"), parent);
  QPushButton *cancelButton = new QPushButton(tr("&Cancel"), parent);
  QHBoxLayout *buttonLayout = new QHBoxLayout(parent);
  QWidget *buttonWidget     = new QWidget(parent);
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  buttonLayout->setAlignment(okButton,     Qt::AlignHCenter);
  buttonLayout->setAlignment(cancelButton, Qt::AlignHCenter);
  buttonWidget->setLayout(buttonLayout);

  setParamLayout->addWidget(buttonWidget);

  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->setLayout(setParamLayout);

  connect(okButton,     SIGNAL(clicked()), this, SIGNAL(accepted()));
  connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(rejected()));

  value_changed    = false;
  mono_out         = QString("");
  mono_in          = QString("");
  stereo_out_left  = QString("");
  stereo_out_right = QString("");
  stereo_in_left   = QString("");
  stereo_in_right  = QString("");
}

float SetMixerParamDialog::get_coarse_min_value() const
{
  return coarse_min;
}

float SetMixerParamDialog::get_coarse_max_value() const
{
  return coarse_max;
}

float SetMixerParamDialog::get_coarse_current_value() const
{
  return coarse_current;
}

float SetMixerParamDialog::get_fine_min_value() const
{
  return fine_min;
}

float SetMixerParamDialog::get_fine_max_value() const
{
  return fine_max;
}

float SetMixerParamDialog::get_fine_current_value() const
{
  return fine_current;
}

float SetMixerParamDialog::get_att_rms_value() const
{
  return att_rms;
}

float SetMixerParamDialog::get_att_max_value() const
{
  return att_max;
}

void SetMixerParamDialog::set_coarse_min_value(const QString & _value)
{
  coarse_min = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_coarse_max_value(const QString & _value)
{
  coarse_max = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_coarse_current_value(const QString & _value)
{
  coarse_current = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_fine_min_value(const QString & _value)
{
  fine_min = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_fine_max_value(const QString & _value)
{
  fine_max = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_fine_current_value(const QString & _value)
{
  fine_current = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_att_rms_value(const QString & _value)
{
  att_rms = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_att_max_value(const QString & _value)
{
  att_max = _value.toFloat();
  value_changed = true;
}

void SetMixerParamDialog::set_mono_in_name(const QString & _value)
{
  mono_in = _value;
  value_changed = true;
}

void SetMixerParamDialog::set_mono_out_name(const QString & _value)
{
  mono_out = _value;
  value_changed = true;
}

void SetMixerParamDialog::set_stereo_in_left_name(const QString & _value)
{
  stereo_in_left = _value;
  value_changed = true;
}

void SetMixerParamDialog::set_stereo_in_right_name(const QString & _value)
{
  stereo_in_right = _value;
  value_changed = true;
}

void SetMixerParamDialog::set_stereo_out_left_name(const QString & _value)
{
  stereo_out_left = _value;
  value_changed = true;
}

void SetMixerParamDialog::set_stereo_out_right_name(const QString & _value)
{
  stereo_out_right = _value;
  value_changed = true;
}

bool SetMixerParamDialog::get_value_changed() const
{
  return value_changed;
}

QString SetMixerParamDialog::get_mono_out_name() const
{
  return mono_out;
}

QString SetMixerParamDialog::get_mono_in_name() const
{
  return mono_in;
}

QString SetMixerParamDialog::get_stereo_out_left_name() const
{
  return stereo_out_left;
}

QString SetMixerParamDialog::get_stereo_out_right_name() const
{
  return stereo_out_right;
}

QString SetMixerParamDialog::get_stereo_in_left_name() const
{
  return stereo_in_left;
}

QString SetMixerParamDialog::get_stereo_in_right_name() const
{
  return stereo_in_right;
}
