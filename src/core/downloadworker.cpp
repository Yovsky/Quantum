#include "downloadworker.h"

DownloadWorker::DownloadWorker(const QUrl &url, int chunkIndex, qint64 start, qint64 end, const QString &tempPath, bool isResuming, downloadInformations Info)
    : m_url(url)
    , m_start(start)
    , m_end(end)
    , m_tempPath(tempPath)
    , m_chunkIndex(chunkIndex)
    , m_isResuming(isResuming)
    , info(Info)
{}

void DownloadWorker::StartDownload()
{
    m_Stopped = false;
    qDebug() << "StartDownload called for chunk" << m_chunkIndex;
    m_file.setFileName(info.tempPath);

    m_downloadOffset = m_start;

    if (!m_file.open(QIODevice::ReadWrite))
    {
        emit ErrorOcc(m_file.errorString());
        return;
    }

    if (!manager)
        manager = new QNetworkAccessManager(this);

    QNetworkRequest request(m_url);
    QByteArray rangeHeader = "bytes=" + QByteArray::number(m_start) + "-" + QByteArray::number(m_end);
    request.setRawHeader("Range", rangeHeader);

    reply = manager->get(request);
    qDebug() << "GET issued for chunk" << m_chunkIndex << "Range:" << rangeHeader;

    connect(reply, &QNetworkReply::readyRead, this, &DownloadWorker::OnReadReady);
    connect(reply, &QNetworkReply::finished, this, &DownloadWorker::OnReplyFinished);
}

void DownloadWorker::Stop()
{
    m_Stopped = true;
    if (!m_writeBuffer.isEmpty())
    {
        m_file.write(m_writeBuffer);
        m_writeBuffer.clear();
    }
    if (reply)
        reply->abort();
}

void DownloadWorker::OnReadReady()
{
    if (m_Stopped) return;
    QByteArray data = reply->readAll();
    m_writeBuffer.append(data);
    emit Progress(m_chunkIndex, data.size());

    if (m_writeBuffer.size() >= BUFFER_SIZE)
    {
        m_file.seek(m_downloadOffset);
        m_file.write(m_writeBuffer);
        m_downloadOffset += m_writeBuffer.size();
        m_writeBuffer.clear();
    }
}

void DownloadWorker::OnReplyFinished()
{
    if (!m_writeBuffer.isEmpty())
    {
        m_file.seek(m_downloadOffset);
        m_file.write(m_writeBuffer);
        m_writeBuffer.clear();
    }
    m_file.close();

    auto error = reply->error();
    QString errorStr = reply->errorString();
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    reply = nullptr;

    if (m_Stopped)
    {
        emit Finished();
        return;
    }

    qDebug() << "Chunk" << m_chunkIndex << "finished, error:" << error << "status:" << status;

    if (error != QNetworkReply::NoError)
    {
        if (retryCount < retryMax)
        {
            retryCount++;
            m_isResuming = true;
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