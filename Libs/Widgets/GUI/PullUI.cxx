/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "PullUI.h"
#include "SynchronizerThread.h"
#include "midasSynchronizer.h"
#include "midasProgressReporter.h"
#include "mwsWebAPI.h"

/** Constructor */
PullUI::PullUI(QWidget* parent, midasSynchronizer* synch)
  : QDialog(parent), m_Synch(synch)
{
  setupUi(this);
  resetState();

  m_SynchronizerThread = NULL;
  m_TypeName = "Resource";

  connect(cloneRadioButton, SIGNAL(released() ), this, SLOT(radioButtonChanged() ) );
  connect(pullRadioButton, SIGNAL(released() ), this, SLOT(radioButtonChanged() ) );
}

PullUI::~PullUI()
{
  if( m_SynchronizerThread && m_SynchronizerThread->isRunning() )
    {
    m_SynchronizerThread->Cancel();
    m_SynchronizerThread->wait();
    }
  delete m_SynchronizerThread;
}

SynchronizerThread * PullUI::getSynchronizerThread()
{
  return m_SynchronizerThread;
}

void PullUI::resetState()
{
  this->m_PullId = 0;
  this->m_ResourceType = midasResourceType::NONE;
  this->cloneRadioButton->setChecked(true);
  this->radioButtonChanged();
}

void PullUI::setClone()
{
  cloneRadioButton->setChecked(true);
  pullRadioButton->setChecked(false);
}

void PullUI::setPull()
{
  cloneRadioButton->setChecked(false);
  pullRadioButton->setChecked(true);
}

void PullUI::setRecursive(bool value)
{
  recursiveCheckBox->setChecked(value);
}

void PullUI::radioButtonChanged()
{
  bool val = pullRadioButton->isChecked();

  recursiveCheckBox->setEnabled(val);
}

void PullUI::setPullId(int id)
{
  m_PullId = id;
}

void PullUI::setResourceType(int type)
{
  m_ResourceType = type;
}

void PullUI::setResourceName(std::string name)
{
  m_Name = name;
}

void PullUI::init()
{
  if( m_PullId )
    {
    pullRadioButton->setChecked(true);
    }
  else
    {
    cloneRadioButton->setChecked(true);
    }
  this->radioButtonChanged();
}

int PullUI::exec()
{
  this->init();
  return QDialog::exec();
}

void PullUI::accept()
{
  if( m_SynchronizerThread )
    {
    disconnect(m_SynchronizerThread);
    }
  delete m_SynchronizerThread;

  m_SynchronizerThread = new SynchronizerThread;
  m_SynchronizerThread->SetSynchronizer(m_Synch);

  if( cloneRadioButton->isChecked() )
    {
    m_Synch->SetOperation(midasSynchronizer::OPERATION_CLONE);
    m_Synch->SetRecursive(true);
    m_Synch->GetLog()->Message("Cloning the server repository");
    m_Synch->GetLog()->Status("Cloning the server repository");

    connect(m_SynchronizerThread, SIGNAL(performReturned(int) ), this, SLOT(cloned(int) ) );
    }
  else // pull
    {
    if( SERVER_IS_MIDAS3 )
      {
      switch( m_ResourceType )
        {
        case midas3ResourceType::COMMUNITY:
          m_Synch->SetResourceType3(midas3ResourceType::COMMUNITY);
          m_TypeName = "Community";
          break;
        case midas3ResourceType::FOLDER:
          m_Synch->SetResourceType3(midas3ResourceType::FOLDER);
          m_TypeName = "Folder";
          break;
        case midas3ResourceType::ITEM:
          m_Synch->SetResourceType3(midas3ResourceType::ITEM);
          m_TypeName = "Item";
          break;
        case midas3ResourceType::BITSTREAM:
          m_Synch->SetResourceType3(midas3ResourceType::BITSTREAM);
          m_TypeName = "Bitstream";
          break;
        default:
          break;
        }
      }
    else
      {
      switch( m_ResourceType )
        {
        case midasResourceType::COMMUNITY:
          m_Synch->SetResourceType(midasResourceType::COMMUNITY);
          m_TypeName = "Community";
          break;
        case midasResourceType::COLLECTION:
          m_Synch->SetResourceType(midasResourceType::COLLECTION);
          m_TypeName = "Collection";
          break;
        case midasResourceType::ITEM:
          m_Synch->SetResourceType(midasResourceType::ITEM);
          m_TypeName = "Item";
          break;
        case midasResourceType::BITSTREAM:
          m_Synch->SetResourceType(midasResourceType::BITSTREAM);
          m_TypeName = "Bitstream";
          break;
        default:
          break;
        }
      }
    std::stringstream idStr;
    idStr << m_PullId;
    m_Synch->SetServerHandle(idStr.str() );
    m_Synch->SetOperation(midasSynchronizer::OPERATION_PULL);
    m_Synch->SetRecursive(recursiveCheckBox->isChecked() );
    connect(m_SynchronizerThread, SIGNAL(enableActions(bool) ), this, SIGNAL(enableActions(bool) ) );
    }

  emit startingSynchronizer();
  m_SynchronizerThread->start();

  QDialog::accept();
}

void PullUI::pulled(int rc)
{
  std::stringstream text;

  if( rc == MIDAS_OK )
    {
    text << "Successfully pulled " << m_TypeName << ": " << m_Name;
    m_Synch->GetLog()->Message(text.str() );
    }
  else if( rc == MIDAS_CANCELED )
    {
    text << "Pull canceled by user";
    m_Synch->GetLog()->Message(text.str() );
    }
  else
    {
    text << "Failed to pull " << m_TypeName << ": " << m_Name;
    m_Synch->GetLog()->Error(text.str() );
    }
  emit pulledResources();

  m_Synch->GetLog()->Status(text.str() );
  if( m_Synch->GetProgressReporter() )
    {
    m_Synch->GetProgressReporter()->ResetProgress();
    }
}

void PullUI::cloned(int rc)
{
  std::string text;

  if( rc == MIDAS_OK )
    {
    text = "Successfully cloned the MIDAS repository.";
    m_Synch->GetLog()->Message(text);
    }
  else if( rc == MIDAS_CANCELED )
    {
    text = "Clone canceled by user";
    m_Synch->GetLog()->Message(text);
    }
  else
    {
    text = "Failed to clone the MIDAS repository.";
    m_Synch->GetLog()->Error(text);
    }
  emit pulledResources();

  m_Synch->GetLog()->Status(text);
  if( m_Synch->GetProgressReporter() )
    {
    m_Synch->GetProgressReporter()->ResetProgress();
    }
}

