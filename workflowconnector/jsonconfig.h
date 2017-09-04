#ifndef JSONCONFIG_H
#define JSONCONFIG_H

class JsonConfig
{
    bool _isValidSystem;
    bool _isValidUser;

    // System config:
    // members for WMS GetMap query
    QString _geo_host;
    QString _geo_port;
    QString _geo_srs;
    QString _geo_Xmin;
    QString _geo_Xmax;
    QString _geo_Ymin;
    QString _geo_Ymax;
    QString _wms_version;
    QString _wms_width;
    QString _wms_height;
    QString _out_type;
    QString _out_ext;
    int _version;

    // User config:
    QString _workspace;
    QString _userMainFolder;    // main user data folder

public:
    JsonConfig();

    bool isValid() { return _isValidSystem && _isValidUser; }

    bool loadSystemConfig(const QString name);
    bool loadUserConfig(const QString name);

    QString getLocalName(const QString localName);
    QString getWMSGetMapURL(const QString layer, QString& layerName);
    QString getBBox();
    QString getSRS();
};

#endif // JSONCONFIG_H
