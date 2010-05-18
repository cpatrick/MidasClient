#ifndef __MidasApplication_H
#define __MidasApplication_H

#include <QApplication>

class DionysusUI; 

class MidasApplication: public QApplication
{
  Q_OBJECT

public:
  MidasApplication ( int & argc, char ** argv);
  int exec(); 
  bool notify ( QObject * receiver, QEvent * event ); 

  static MidasApplication * instance(); 

private:
  static MidasApplication* m_instance; 

  DionysusUI * dionysus; 
};

#endif //__MidasApplication_H