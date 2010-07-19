#include "SearchThread.h"

#include "MIDASDesktopUI.h"
#include "mwsSearch.h"
#include "mdoObject.h"

SearchThread::SearchThread(MIDASDesktopUI* parent)
{
  m_Parent = parent;
}

SearchThread::~SearchThread()
{
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