#ifndef __SearchThread_H_
#define __SearchThread_H_

#include <QThread>
#include "midasStandardIncludes.h"

class MIDASDesktopUI;

namespace mdo {
  class Object;
}

class SearchThread : public QThread
{
  Q_OBJECT
public:
  void SetParentUI(MIDASDesktopUI* parent);
  void SetWords(std::vector<std::string> words);
  void SetResults(std::vector<mdo::Object*>* results);

  virtual void run();

signals:
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
  std::vector<std::string> m_Words;
  std::vector<mdo::Object*>* m_Results;
};

#endif
