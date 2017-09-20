//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
// .NAME jobrequest.h
// .SECTION Description
// .SECTION See Also

#ifndef __smtk_extension_cumulus_jobrequest_h
#define __smtk_extension_cumulus_jobrequest_h

#include "smtk/extension/cumulus/Exports.h"
#include "smtk/extension/cumulus/girderrequest.h"
#include "smtk/extension/cumulus/job.h"

#include <QList>
#include <QSet>

class QNetworkReply;
struct cJSON;

namespace cumulus
{

class SMTKCUMULUSEXT_EXPORT JobRequest : public GirderRequest
{
  Q_OBJECT

public:
  JobRequest(const QString& girderUrl, const QString& girderToken, Job job, QObject* parent = 0);
  ~JobRequest();

  void virtual send() = 0;
  Job job() const { return this->m_job; };

protected:
  Job m_job;
};

class DeleteJobRequest : public JobRequest
{
  Q_OBJECT

public:
  DeleteJobRequest(
    const QString& girderUrl, const QString& girderToken, Job job, QObject* parent = 0);
  ~DeleteJobRequest();

  void send();

private slots:
  void finished(QNetworkReply* reply);
};

class TerminateJobRequest : public JobRequest
{
  Q_OBJECT

public:
  TerminateJobRequest(
    const QString& girderUrl, const QString& girderToken, Job job, QObject* parent = 0);
  ~TerminateJobRequest();

  void send();

private slots:
  void finished(QNetworkReply* reply);
};

class DownloadJobRequest : public JobRequest
{
  Q_OBJECT

public:
  DownloadJobRequest(QNetworkCookieJar* cookieJar, const QString& girderUrl,
    const QString& girderToken, const QString& downloadPath, Job job, QObject* parent = 0);
  ~DownloadJobRequest();
  QString path() { return m_downloadPath; };

  void send();

private slots:
  void downloadFolderFinished();

private:
  QSet<QString> m_foldersToDownload;
  QString m_downloadPath;
  QNetworkCookieJar* m_cookieJar;
};

// For updating job status to reflect download status
class PatchJobRequest : public JobRequest
{
  Q_OBJECT
public:
  PatchJobRequest(const QString& girderUrl, const QString& girderToken, Job job, cJSON* body,
    QObject* parent = 0);
  ~PatchJobRequest();

  void send();

private slots:
  void finished(QNetworkReply* reply);

private:
  cJSON* m_body;
};

} // end namespace cumulus

#endif
