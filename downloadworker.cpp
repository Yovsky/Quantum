#include "downloadworker.h"

DownloadWorker::DownloadWorker(const QUrl &url, int chunkIndex, qint64 start, qint64 end, const QString &tempPath, bool isResuming)
    : m_url(url), m_start(start), m_end(end), m_tempPath(tempPath), m_chunkIndex(chunkIndex), m_isResuming(isResuming) {}

void DownloadWorker::StartDownload()
{
    qDebug() << "StartDownload called for chunk" << m_chunkIndex;
    m_file.setFileName(m_tempPath);

    qDebug() << "Opening:" << m_tempPath;

    if (m_isResuming)
    {
        if(!m_file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            qDebug() << "Failed to open file:" << m_file.errorString();
            emit ErrorOcc(m_file.errorString());
            return;
        }
    }
    else
    {
        if(!m_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            emit ErrorOcc(m_file.errorString());
            return;
        }
    }


    if (!manager)
        manager = new QNetworkAccessManager(this);
    QNetworkRequest request(m_url);

    QByteArray rangeHeader = "bytes=" + QByteArray::number(m_start) + "-" + QByteArray::number(m_end);
    request.setRawHeader("Range", rangeHeader);

    reply = manager->get(request);
    qDebug() << "GET issued for chunk" << m_chunkIndex
             << "URL:" << m_url
             << "Range:" << rangeHeader;

    connect(reply, &QNetworkReply::readyRead, this, &DownloadWorker::OnReadReady);
    connect(reply, &QNetworkReply::finished, this, &DownloadWorker::OnReplyFinished);
}

void DownloadWorker::OnReadReady()
{
    QByteArray data = reply->readAll();
    m_file.write(data);
    emit Progress(m_chunkIndex, data.size());
}

void DownloadWorker::OnReplyFinished()
{
    auto error = reply->error();
    m_file.close();
    QString errorStr = reply->errorString();
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    reply = nullptr;

    qDebug() << "Chunk" << m_chunkIndex
             << "finished with error:"
             << error
             << errorStr
             << "status:"
             << status;

    if(error != QNetworkReply::NoError)
    {
        if (retryCount < retryMax)
        {
            retryCount++;
            StartDownload();
            return;
        }
        emit ErrorOcc(errorStr);
    }
    else if (status != 206)
    {
        emit ErrorOcc("Server ignored range request");
    }
    else
    {
        retryCount = 0;
        emit Finished();    
    }
}