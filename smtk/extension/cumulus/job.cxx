#include "job.h"
#include "cJSON.h"

#include <QString>

namespace cumulus
{

Job::Job()
{

}

Job::Job(const QString &id, const QString &name, const QString &status,
    const QList<QString> &outputFolderIds)
{
  this->m_id = id;
  this->m_name = name;
  this->m_status = status;
  this->m_outputFolderIds = outputFolderIds;
}

Job::Job(const Job &job)
{
  this->m_id = job.id();
  this->m_name = job.name();
  this->m_status = job.status();
  this->m_outputFolderIds = job.outputFolderIds();
}

Job::~Job()
{

}

Job Job::fromJSON(cJSON *obj)
{
  cJSON *idItem = cJSON_GetObjectItem(obj, "_id");
  if (!idItem || idItem->type != cJSON_String) {
    return Job();
  }
  QString id(idItem->valuestring);

  cJSON *nameItem = cJSON_GetObjectItem(obj, "name");
  if (!nameItem || nameItem->type != cJSON_String) {
    return Job();
  }
  QString name(nameItem->valuestring);

  cJSON *statusItem = cJSON_GetObjectItem(obj, "status");
  if (!statusItem || statusItem->type != cJSON_String) {
    return Job();
  }
  QString status(statusItem->valuestring);

  QList<QString> outputFolderIds;
  cJSON *outputItem = cJSON_GetObjectItem(obj, "output");
  if (outputItem && outputItem->type == cJSON_Array) {

    for (cJSON* output = outputItem->child; output; output = output->next) {
      cJSON *folderIdItem = cJSON_GetObjectItem(output, "folderId");
      if (!folderIdItem || folderIdItem->type != cJSON_String) {
        continue;
      }

      outputFolderIds.append(QString(folderIdItem->valuestring));
    }
  }

  return Job(id, name, status, outputFolderIds);
}


} // end namespace
