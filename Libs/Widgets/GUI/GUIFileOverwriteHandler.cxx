#include "GUIFileOverwriteHandler.h"
#include "FileOverwriteUI.h"

GUIFileOverwriteHandler::GUIFileOverwriteHandler(FileOverwriteUI* dialog)
  : m_Dialog(dialog), m_ApplyToAll(false)
{
  connect(this, SIGNAL( displayDialog() ),
          dialog, SLOT( exec() ),
          Qt::BlockingQueuedConnection);
}

GUIFileOverwriteHandler::~GUIFileOverwriteHandler()
{
}

midasFileOverwriteHandler::Action GUIFileOverwriteHandler::HandleConflict(
  std::string path)
{
  if( !m_ApplyToAll )
    {
    m_Dialog->setPath(path);
    emit displayDialog(); // modal dialog, blocking connection
    m_ApplyToAll = m_Dialog->ShouldApplyToAll();
    }
  return m_Dialog->ShouldOverwrite() ? Overwrite : UseExisting;
}

