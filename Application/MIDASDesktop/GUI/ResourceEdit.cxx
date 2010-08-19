#include "ResourceEdit.h"
#include "MidasResourceDescTable.h"
#include "midasUtils.h"
#include "midasDatabaseProxy.h"
#include "mdoCommunity.h"
#include "mdoCollection.h"
#include "mdoItem.h"
#include "mdoBitstream.h"

ResourceEdit::ResourceEdit(midasDatabaseProxy* database)
{
  this->m_database = database;
}

ResourceEdit::~ResourceEdit()
{
}

void ResourceEdit::Save(QTableWidgetItem* row)
{
  QTableWidgetMidasCommunityDescItem* commRow = NULL;
  QTableWidgetMidasCollectionDescItem* collRow = NULL;
  QTableWidgetMidasItemDescItem* itemRow = NULL;
  QTableWidgetMidasBitstreamDescItem* bitstreamRow = NULL;

  std::string data = row->data(Qt::DisplayRole).toString().toStdString();
  midasUtils::StringTrim(data);

  this->m_database->Open();
  if((commRow = dynamic_cast<QTableWidgetMidasCommunityDescItem*>(row)) != NULL)
    {
    this->SaveCommunity(commRow->getModelData(), commRow->getField(), data);
    }
  else if((collRow = dynamic_cast<QTableWidgetMidasCollectionDescItem*>(row)) != NULL)
    {
    this->SaveCollection(collRow->getModelData(), collRow->getField(), data);
    }
  else if((itemRow = dynamic_cast<QTableWidgetMidasItemDescItem*>(row)) != NULL)
    {
    this->SaveItem(itemRow->getModelData(), itemRow->getField(), data);
    }
  else if((bitstreamRow = dynamic_cast<QTableWidgetMidasBitstreamDescItem*>(row)) != NULL)
    {
    this->SaveBitstream(bitstreamRow->getModelData(), itemRow->getField(), data);
    }
  this->m_database->Close();
}

void ResourceEdit::SaveCommunity(mdo::Community* comm, MIDASFields field,
                                 std::string data)
{
  bool changed = false;
  switch(field)
    {
    case COMMUNITY_NAME:
      if(comm->GetName() != data)
        {
        comm->SetName(data.c_str());
        changed = true;
        }
      break;
    case COMMUNITY_DESCRIPTION:
      if(comm->GetDescription() != data)
        {
        comm->SetDescription(data.c_str());
        changed = true;
        }
      break;
    case COMMUNITY_INTRODUCTORY:
      if(comm->GetIntroductoryText() != data)
        {
        comm->SetIntroductoryText(data.c_str());
        changed = true;
        }
      break;
    case COMMUNITY_COPYRIGHT:
      if(comm->GetCopyright() != data)
        {
        comm->SetCopyright(data.c_str());
        changed = true;
        }
      break;
    case COMMUNITY_LINKS:
      if(comm->GetLinks() != data)
        {
        comm->SetLinks(data.c_str());
        changed = true;
        }
      break;
    default:
      return;
    }

  if(changed)
    {    
    this->m_database->SaveInfo(comm, true);

    this->Log->Status("Community saved successfully");
    this->Log->Message("Community saved successfully");

    emit DataModified(comm->GetUuid());
    }
}

void ResourceEdit::SaveCollection(mdo::Collection* coll, MIDASFields field,
                                 std::string data)
{
  bool changed = false;
  switch(field)
    {
    case COLLECTION_NAME:
      if(coll->GetName() != data)
        {
        coll->SetName(data.c_str());
        changed = true;
        }
      break;
    case COLLECTION_DESCRIPTION:
      if(coll->GetDescription() != data)
        {
        coll->SetDescription(data.c_str());
        changed = true;
        }
      break;
    }

  if(changed)
    {
    this->m_database->SaveInfo(coll, true);

    this->Log->Status("Collection saved successfully");
    this->Log->Message("Collection saved successfully");

    emit DataModified(coll->GetUuid());
    }
}

void ResourceEdit::SaveItem(mdo::Item* item, MIDASFields field,
                            std::string data)
{
  bool changed = false;
  switch(field)
    {
    case ITEM_TITLE:
      if(item->GetTitle() != data)
        {
        item->SetTitle(data.c_str());
        changed = true;
        }
      break;
    case ITEM_ABSTRACT:
      if(item->GetAbstract() != data)
        {
        item->SetAbstract(data.c_str());
        changed = true;
        }
      break;
    case ITEM_DESCRIPTION:
      if(item->GetDescription() != data)
        {
        item->SetDescription(data.c_str());
        changed = true;
        }
      break;
    /*case ITEM_KEYWORDS:

      if(item->GetKeywords() != data)
        {
        // todo we have to parse the keywords string here
        //item->SetKeywords(data.c_str());
        changed = true;
        }
      break;
    case ITEM_AUTHORS:
      if(item->Get != data)
        {
        // todo we have to parse the keywords string here
        //item->SetAutho(data.c_str());
        changed = true;
        }
      break;*/
    }

  if(changed)
    {
    this->m_database->SaveInfo(item, true);

    this->Log->Status("Item saved successfully");
    this->Log->Message("Item saved successfully");

    emit DataModified(item->GetUuid());
    }
}

void ResourceEdit::SaveBitstream(mdo::Bitstream* bitstream, MIDASFields field,
                                 std::string data)
{
  //nothing to do at this time
}