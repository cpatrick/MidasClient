#include "SearchThread.h"

#include "MIDASDesktopUI.h"
#include "mwsSearch.h"
#include "mdoObject.h"

void SearchThread::SetParentUI(MIDASDesktopUI* parent)
{
  this->m_Parent = parent;
}

void SearchThread::SetWords(std::vector<std::string> words)
{
  this->m_Words = words;
}

void SearchThread::SetResults(std::vector<mdo::Object*>* results)
{
  this->m_Results = results;
}

void SearchThread::run()
{
  *m_Results = mws::Search::SearchServer(m_Words);
  emit threadComplete();
}