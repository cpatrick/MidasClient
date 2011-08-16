#include "midasCLIApplication.h"
#include "midasCLI.h"

midasCLIApplication::midasCLIApplication ( int & argc, char ** argv):
QCoreApplication(argc, argv)
{
  for(int i = 1; i < argc; i++)
    {
    this->m_Args.push_back(argv[i]);
    }

  m_CLI = new midasCLI;
}

midasCLIApplication::~midasCLIApplication()
{
  delete this->m_CLI;
}

int midasCLIApplication::exec()
{
  return m_CLI->Perform(this->m_Args);
}

bool midasCLIApplication::notify(QObject* receiver, QEvent* event)
{
  try
    {
    return QCoreApplication::notify(receiver, event); 
    }
  catch (const std::exception& e)
    {
    std::cerr << "Caught exception during notify to object " <<
      receiver->objectName().toStdString() << ". Message: " << e.what();
    }
  return false;
}

