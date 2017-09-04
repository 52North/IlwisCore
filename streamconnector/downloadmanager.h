#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDataStream>
#include "ilwis.h"
#include "identity.h"
#include "resource.h"
#include "iooptions.h"
#include "connectorinterface.h"
#include "rawconverter.h"
#include "versionedserializer.h"

namespace Ilwis {

class IlwisObject;
class IOOptions;

namespace Stream {

class VersionedSerializer;


class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(const Ilwis::Resource &resource);
    bool loadMetaData(IlwisObject *object, const IOOptions &options);
    bool loadData(IlwisObject *object, const IOOptions &options);
    std::vector<Resource> loadItems();

protected slots:
    void readReady();
    void readReadyRaster();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finishedMetadata();
    void finishedData();
    void finishedDataExplorer();

private:
    Resource _resource;
    std::unique_ptr<VersionedSerializer> _versionedConnector;
    QByteArray _bytes;
    std::vector<Resource> _items;
    IlwisObject *_object = 0;

    //raster only
    quint32 _blockSizeBytes =0;
    quint32 _currentBlock=0;
    bool _initialRasterData = true;
    RawConverter _converter;

    void copyData(bool lastBlock=false);
};
}
}

#endif // DOWNLOADMANAGER_H
