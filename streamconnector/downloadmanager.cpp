#include <QEventLoop>
#include <cstring>
#include "kernel.h"
#include "resource.h"
#include "version.h"
#include "ilwisdata.h"
#include "raster.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "rawconverter.h"
#include "downloadmanager.h"

using namespace Ilwis;
using namespace Stream;

DownloadManager::DownloadManager(const Resource& resource) : _resource(resource)
{
}

std::vector<Resource> DownloadManager::loadItems(){
    QUrl url = _resource.url(true);

    QNetworkRequest request(url);

    QNetworkReply *reply = kernel()->network().get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finishedDataExplorer);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return _items;
}

bool DownloadManager::loadData(IlwisObject *object, const IOOptions &options){
    QUrl url = _resource.url(true);

    QUrlQuery query(url);
    if ( object->ilwisType() == itRASTER){
        RasterCoverage *raster = static_cast<RasterCoverage*>(object);
        _blockSizeBytes = raster->grid()->blockSize(0);
        if ( options.contains("blockindex")){
            _currentBlock = options["blockindex"].toInt();
            int layer = _currentBlock / raster->grid()->blocksPerBand();
            int relativeBlock = _currentBlock - layer * raster->grid()->blocksPerBand();
            unsigned int minLine = raster->grid()->maxLines() * relativeBlock ;
            unsigned int maxLine = std::min( minLine + raster->grid()->maxLines(), raster->size().ysize());
            query.addQueryItem("lines",QString("%1 %2 %3").arg(layer).arg(minLine).arg(maxLine));
        }
    }
    query.addQueryItem("datatype","data");
    url.setQuery(query);
    QString urltxt = url.toString();
    _object = object;
    QNetworkRequest request(url);

    QNetworkReply *reply = kernel()->network().get(request);

    if ( object->ilwisType() == itRASTER)
        connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReadyRaster);
    else
        connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finishedData);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

bool DownloadManager::loadMetaData(IlwisObject *object, const IOOptions &options)
{
    QUrl url = _resource.url(true);
    QUrlQuery query(url);
    query.addQueryItem("datatype","metadata");
    query.removeQueryItem("ilwistype"); // ilwistype might be too generic so we replace it we a more accurate version
    QString ilwtype = IlwisObject::type2Name(_resource.ilwisType()).toLower();
    query.addQueryItem("ilwistype",ilwtype);
    url.setQuery(query);
    _object = object;
    QNetworkRequest request(url);

    QNetworkReply *reply = kernel()->network().get(request);

    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readReady);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DownloadManager::error);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finishedMetadata);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

void DownloadManager::readReady()
{

    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        _bytes.append(reply->readAll());
    }
}

void DownloadManager::copyData(bool lastBlock)
{
    int bytesLeft = _bytes.size();
    while ( bytesLeft >= _blockSizeBytes || lastBlock){

        bytesLeft = _versionedConnector->loadGridBlock(_object, _currentBlock, _bytes, _converter, IOOptions());
        ++_currentBlock;
        quint32 delta = _bytes.size() - bytesLeft;
        std::memcpy(_bytes.data(), _bytes.data() + delta, bytesLeft);
        _bytes.resize(bytesLeft);
        lastBlock = false;

    }
}

void DownloadManager::readReadyRaster()
{

    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
         _bytes.append(reply->readAll());
        if ( _initialRasterData){
            VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
            if (factory){
                QBuffer buf(&_bytes);
                buf.open(QIODevice::ReadWrite);
                QDataStream stream(&buf);
                quint64 type;
                stream >> type;
                QString version;
                stream >> version;
                double mmin,  mmax, mscale;
                stream >> mmin >> mmax >> mscale;
                int pos = stream.device()->pos();
                _converter = RawConverter(mmin, mmax, mscale);
                switch (_converter.storeType()){ // calculate true blocksize in bytes
                case itUINT8:
                    break;
                case itINT16:
                   _blockSizeBytes *= 2; break;
                case itINT32:
                    _blockSizeBytes *= 4; break;
                default:
                    _blockSizeBytes *= 8; break;
                }
                quint32 bytesLeft = _bytes.size() - pos; // we chopped of a few bytes for the metadata; so we adjust the actual bytes for this.
                std::memcpy(_bytes.data(), _bytes.data() + pos, bytesLeft);
                _bytes.resize(bytesLeft);
                stream.device()->seek(0);
                _versionedConnector.reset( factory->create(version,type,stream));

            }

            if (!_versionedConnector)
                return ;

            _initialRasterData = false;
        }
        copyData();
    }
}


void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        qErrnoWarning("Error!");

        reply->deleteLater();
    }
}

void DownloadManager::finishedData()
{
    if ( _object->ilwisType() == itRASTER){
        copyData(true);
        return;
    }
    QBuffer buf(&_bytes);
    buf.open(QIODevice::ReadWrite);
    QDataStream stream(&buf);
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return ;
    _versionedConnector->loadData(_object, IOOptions());

    buf.close();
}
void DownloadManager::finishedDataExplorer() {
    QBuffer buf(&_bytes);
    buf.open(QIODevice::ReadWrite);
    QDataStream stream(&buf);
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return ;
    _versionedConnector->loadItems(_items);

    buf.close();
}

void DownloadManager::finishedMetadata()
{
    QBuffer buf(&_bytes);
    buf.open(QIODevice::ReadWrite);
    QDataStream stream(&buf);
    quint64 type;
    stream >> type;
    QString version;
    stream >> version;

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(version,type,stream));
    }

    if (!_versionedConnector)
        return ;
    _versionedConnector->loadItems(_items);

    buf.close();
}

