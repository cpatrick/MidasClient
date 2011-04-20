#include "GUIFileOverwriteHandler.h"
#include "MIDASDesktopUI.h"

GUIFileOverwriteHandler::GUIFileOverwriteHandler(MIDASDesktopUI* parent)
: m_Parent(parent), m_ApplyToAll(false)
{
  connect(this, SIGNAL(displayDialog(const QString&) ),
          parent, SLOT(showFileOverwriteDialog(const QString&) ),
          Qt::BlockingQueuedConnection);
}

GUIFileOverwriteHandler::~GUIFileOverwriteHandler()
{
}

midasFileOverwriteHandler::Action GUIFileOverwriteHandler::HandleConflict(
  std::string path)
{
  if(!m_ApplyToAll)
    {
    emit displayDialog(path.c_str()); //modal dialog, blocking connection
    }
  return m_Action;
}

void GUIFileOverwriteHandler::chooseAction(int choice, bool applyToAll)
{
  this->actionChosen((Action)choice, applyToAll);
}

void GUIFileOverwriteHandler::actionChosen(Action action, bool applyToAll)
{
  m_ApplyToAll = applyToAll;
  m_Action = action;
}
