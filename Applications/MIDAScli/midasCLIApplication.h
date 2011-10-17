#ifndef __midasCLIApplication_H
#define __midasCLIApplication_H

#include <QCoreApplication>

#include "midasStandardIncludes.h"

class midasCLI;

class midasCLIApplication : public QCoreApplication
{
  Q_OBJECT
public:
  midasCLIApplication( int & argc, char * * argv);
  ~midasCLIApplication();

  int exec();

  bool notify( QObject * receiver, QEvent* event );

private:
  midasCLI*                m_CLI;
  std::vector<std::string> m_Args;
};

#endif // __MidasApplication_H
