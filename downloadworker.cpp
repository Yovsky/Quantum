#include "downloadworker.h"

DownloadWorker::DownloadWorker(const QUrl &url, qint64 start, qint64 end, const QString &tempPath)
    : m_url(url), m_start(start), m_end(end), m_tempPath(tempPath) {}

void DownloadWorker::StartDownload()
{
    m_file.setFileName(m_tempPath);
    if(!m_file.open(QIODevice::WriteOnly))
    {
        emit ErrorOcc(m_file.errorString());
        return;
    }

    manager = new QNetworkAccessManager(this);
    QNetworkRequest request(m_url);

    QByteArray rangeHeader = "bytes=" + QByteArray::number(m_start) + "-" + QByteArray::number(m_end);
    request.setRawHeader("Range", rangeHeader);

    reply = manager->get(request);

    connect(reply, &QNetworkReply::readyRead, this, &DownloadWorker::OnReadReady);
    connect(reply, &QNetworkReply::finished, this, &DownloadWorker::OnReplyFinished);
}

void DownloadWorker::OnReadReady()
{
    QByteArray data = reply->readAll();
    m_file.write(data);
    emit Progress(data.size());
}

void DownloadWorker::OnReplyFinished()
{
    m_file.flush();
    m_file.close();

    if(reply->error() != QNetworkReply::NoError)
        emit ErrorOcc(reply->errorString());
    else
        emit Finished();
}