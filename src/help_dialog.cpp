//=============================================================================
//  QtJMix
//  Qt Jack Mixer
//  $Id:$
//
//  Copyright (C) 2013 by Yann Collette and others
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

#include <QtWebKit>

#include "help_dialog.h"

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent)
{
  QVBoxLayout *helpLayout = new QVBoxLayout(this);

  setWindowTitle("QtJMix Help");
  setWindowIcon(QIcon(":/images/MainIcon"));
  
  QWebView *view = new QWebView(this);

  view->setUrl(QUrl("qrc:///docs/main_html_doc"));
  view->show();

  QPushButton *okButton = new QPushButton(tr("&Ok"), parent);
  
  helpLayout->addWidget(view);
  helpLayout->addWidget(okButton);

  helpLayout->setAlignment(view, Qt::AlignHCenter);
  helpLayout->setAlignment(okButton, Qt::AlignHCenter);

  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->setLayout(helpLayout);

  connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
}
