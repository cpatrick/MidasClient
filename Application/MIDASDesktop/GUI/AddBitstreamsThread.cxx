#include "AddBitstreamsThread.h"
#include "mdsDatabaseAPI.h"
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "mdoItem.h"
#include "MidasItemTreeItem.h"

AddBitstreamsThread::AddBitstreamsThread()
{
  m_ParentItem = NULL;
}

AddBitstreamsThread::~AddBitstreamsThread()
{
}

void AddBitstreamsThread::SetFiles(const QStringList& files)
{
  m_Files = files;
}

void AddBitstreamsThread::SetParentItem(MidasItemTreeItem* parentItem)
{
  m_ParentItem = parentItem;
}

void AddBitstreamsThread::run()
{
  emit enableActions(false);
  unsigned int currTime = static_cast<unsigned int>(
    kwsys::SystemTools::GetTime() * 1000);
  srand (currTime); //seed RNG (must be done in each thread)

  mds::DatabaseAPI db;
  bool copyIntoTree = db.GetSettingBool(mds::DatabaseAPI::UNIFIED_TREE);

  int current = 0;
  int total = m_Files.size();
  for(QStringList::iterator i = m_Files.begin(); i != m_Files.end(); ++i)
    {
    std::string path = i->toStdString();
    kwsys::SystemTools::ConvertToUnixSlashes(path);
    std::string name = kwsys::SystemTools::GetFilenameName(path.c_str());
    std::string uuid = midasUtils::GenerateUUID();

    std::stringstream size;
    size << midasUtils::GetFileLength(path.c_str());

    mdo::Item* item = new mdo::Item;
    item->SetId(m_ParentItem->getItem()->GetId());
    item->SetUuid(m_ParentItem->getItem()->GetUuid().c_str());

    // Don't delete this object - tree will delete it
    mdo::Bitstream* bitstream = new mdo::Bitstream;
    bitstream->SetName(name.c_str());
    bitstream->SetSize(size.str());
    bitstream->SetLastModified(kwsys::SystemTools::ModifiedTime(path.c_str()));
    bitstream->SetUuid(uuid.c_str());
    bitstream->SetPath(path);
    bitstream->SetParentId(m_ParentItem->getItem()->GetId());
    bitstream->SetParentItem(item);
    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(bitstream);
    mdsBitstream.MarkAsDirty();
    mdsBitstream.Commit();

    current++;
    emit progress(current, total, name.c_str());
    }
  emit enableActions(true);
  emit threadComplete();
}