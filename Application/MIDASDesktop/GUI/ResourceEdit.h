class midasDatabaseProxy;
class QTableWidgetItem;

class ResourceEdit
{
public:

  ResourceEdit(midasDatabaseProxy* database);
  ~ResourceEdit();

  void save(QTableWidgetItem* row);

protected:
  midasDatabaseProxy* m_database;
};