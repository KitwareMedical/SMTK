#ifndef __smtk_extension_cumulus_job_h
#define __smtk_extension_cumulus_job_h

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QList>

struct cJSON;

namespace cumulus
{

class Job
{
public:
  Job();
  Job(const QString &id, const QString &name, const QString &status,
      const QList<QString> &outputFolderIds);
  Job(const Job &job);

  ~Job();
  QString id() const { return this->m_id; };
  QString name() const { return this->m_name; };
  QString status() const { return this->m_status; };
  QList<QString> outputFolderIds() const { return this->m_outputFolderIds; };
  bool isValid() const { return !this->m_id.isEmpty(); };

  static Job fromJSON(cJSON *obj);
private:
  QString m_id;
  QString m_name;
  QString m_status;
  QList<QString> m_outputFolderIds;
};

}; // end namespace

Q_DECLARE_METATYPE(cumulus::Job)

#endif
