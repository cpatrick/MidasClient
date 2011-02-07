#include "GUIFileOverwriteHandler.h"
#include "MIDASDesktopUI.h"

GUIFileOverwriteHandler::GUIFileOverwriteHandler(MIDASDesktopUI* parent)
: m_Parent(parent), m_ApplyToAll(false), m_Done(false)
{
  connect(this, SIGNAL(displayDialog(const QString&) ),
          parent, SLOT(showFileOverwriteDialog(const QString&) ) );
}

GUIFileOverwriteHandler::~GUIFileOverwriteHandler()
{
}

midasFileOverwriteHandler::Action GUIFileOverwriteHandler::HandleConflict(
  std::string path)
{
  m_Done = false;
  if(!m_ApplyToAll)
    {
    emit displayDialog(path.c_str());
    while(!m_Done)
      {
      //busy wait until we get an action choice from the user
      }
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
  m_Done = true;
}
