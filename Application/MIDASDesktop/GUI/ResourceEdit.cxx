#include "ResourceEdit.h"
#include "MidasResourceDescTable.h"

ResourceEdit::ResourceEdit(midasDatabaseProxy* database)
{
  this->m_database = database;
}

ResourceEdit::~ResourceEdit()
{
}

void ResourceEdit::save(QTableWidgetItem* row)
{
  QTableWidgetMidasCommunityDescItem* commRow = NULL;
  QTableWidgetMidasCollectionDescItem* collRow = NULL;
  QTableWidgetMidasItemDescItem* itemRow = NULL;
  QTableWidgetMidasBitstreamDescItem* bitstreamRow = NULL;

  if((commRow = dynamic_cast<QTableWidgetMidasCommunityDescItem*>(row)) != NULL)
    {
    QString data = row->data(Qt::DisplayRole).toString();
    mdo::Community* comm = commRow->getModelData();

    switch(commRow->getField())
      {
      case COMMUNITY_NAME:
        break;
      case COMMUNITY_DESCRIPTION:
        break;
      case COMMUNITY_COPYRIGHT:
        break;
      case COMMUNITY_INTRODUCTORY:
        break;
      case COMMUNITY_LINKS:
        break;
      default:
        break;
      }
    }
  else if((collRow = dynamic_cast<QTableWidgetMidasCollectionDescItem*>(row)) != NULL)
    {
    mdo::Collection* coll = collRow->getModelData();
    }
  else if((itemRow = dynamic_cast<QTableWidgetMidasItemDescItem*>(row)) != NULL)
    {
    mdo::Item* item = itemRow->getModelData();
    }
  else if((bitstreamRow = dynamic_cast<QTableWidgetMidasBitstreamDescItem*>(row)) != NULL)
    {
    mdo::Bitstream* bitstream = bitstreamRow->getModelData();
    }
}