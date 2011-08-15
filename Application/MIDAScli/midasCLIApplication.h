#ifndef __midasCLIApplication_H
#define __midasCLIApplication_H

#include <QApplication>

#include "midasStandardIncludes.h"

class midasCLI;

class midasCLIApplication : public QApplication
{
  Q_OBJECT

public:
  midasCLIApplication ( int & argc, char ** argv);
  ~midasCLIApplication();

  int exec(); 
  bool notify ( QObject * receiver, QEvent* event );

private:
  midasCLI* m_CLI;
  std::vector<std::string> m_Args;
};

#endif //__MidasApplication_H
