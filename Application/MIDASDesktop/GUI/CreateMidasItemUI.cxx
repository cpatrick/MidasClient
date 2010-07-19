#include "CreateMidasItemUI.h"

#include <QtGui> 
#include <Qt>
#include <QDate>

#include "MIDASDesktopUI.h"
#include "mwsSettings.h"
#include "mdoItem.h"
#include "midasSynchronizer.h"
#include "Utils.h"

#include "MidasTreeItem.h"
#include "MidasTreeModel.h"
#include "MidasTreeModelClient.h"
#include "MidasCollectionTreeItem.h"
#include "MidasClientGlobal.h"

CreateMidasItemUI::CreateMidasItemUI(MIDASDesktopUI *parent, CreateMidasItemUI::Types type):
  QDialog(parent), parent(parent)
{
  this->setType(type);
  setupUi(this); // this sets up GUI

  connect( this->addAuthorButton, SIGNAL( released() ), this, SLOT( addAuthor() ) ); 
  connect( this->addKeywordButton, SIGNAL( released() ), this, SLOT( addKeyword() ) ); 
}

void CreateMidasItemUI::setType(Types type)
{
  this->type = type;
  if (type == CreateMidasItemUI::ClientItem)
    {
    QWidget::setWindowTitle(tr("Add item")); 
    }
}

void CreateMidasItemUI::reset()
{
  this->authorsEdit->clear();
  this->titleEdit->clear();
  this->keywordsEdit->clear();
  this->issueDateEdit->setDate(QDate::currentDate());
  this->descriptionEdit->clear();
  this->abstractEdit->clear();
  this->sponsorEdit->clear();
}

int CreateMidasItemUI::exec()
{
  this->reset();
  return QDialog::exec();
}

void CreateMidasItemUI::accept()
{
  if(this->titleEdit->text() == "")
    {
    parent->GetLog()->Error("You must supply a title for the item.");
    QDialog::accept();
    return;
    }
  
  if(this->type == CreateMidasItemUI::ClientItem)
    {
    QModelIndex selected = this->parent->getTreeViewClient()->getSelectedModelIndex();
    MidasCollectionTreeItem* parentColl =
      reinterpret_cast<MidasCollectionTreeItem*>(
      const_cast<MidasTreeItem*>(
      reinterpret_cast<MidasTreeModelClient*>(
      this->parent->getTreeViewClient()->model())->midasTreeItem(selected)));

    parent->getDatabaseProxy()->Open();
    std::string path = parent->getDatabaseProxy()->GetRecordByUuid(
      parent->getDatabaseProxy()->GetUuid(
      midasResourceType::COLLECTION,
      parentColl->getCollection()->GetId())).Path;
    parent->getDatabaseProxy()->Close();
    
    path += "/" + titleEdit->text().toStdString();
    kwsys::SystemTools::MakeDirectory(path.c_str());

    parent->getSynchronizer()->SetOperation(midasSynchronizer::OPERATION_ADD);
    parent->getSynchronizer()->SetResourceType(midasResourceType::ITEM);
    parent->getSynchronizer()->SetResourceHandle(path);
    
    std::stringstream text;
    if(parent->getSynchronizer()->Perform() == 0)
      {    
      text << "Successfully added item " << path << ".";
      parent->GetLog()->Message(text.str());
      parent->updateClientTreeView();
      }
    else
      {
      text << "Failed to add item at " << path << ".";
      parent->GetLog()->Error(text.str());
      }
    }
  else
    {
    //disabled server side creation
    }
  QDialog::accept();
}

void CreateMidasItemUI::addAuthor()
{
  QString value = QInputDialog::getText( this, tr("Add author"), tr("Separate First and Last name using a comma") ).simplified(); 
  if ( !value.isEmpty() )
    {
    QString current = this->authorsEdit->text(); 
    this->authorsEdit->setText(current.isEmpty() ? value : current + MIDAS_AUTHORS_DELIMITER + " " + value); 
    }
}

void CreateMidasItemUI::addKeyword()
{
  QString value = QInputDialog::getText( this, tr("Add keyword"), tr("Keyword") ).simplified(); 
  if ( !value.isEmpty() )
    {
    QString current = this->keywordsEdit->text(); 
    this->keywordsEdit->setText(current.isEmpty() ? value : current + " " + MIDAS_KEYWORDS_DELIMITER + " " + value); 
    }
}
