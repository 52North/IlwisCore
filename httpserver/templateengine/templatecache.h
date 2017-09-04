#ifndef TEMPLATECACHE_H
#define TEMPLATECACHE_H

#include "templateloader.h"
#include <QCache>

/**
  Caching template loader, reduces the amount of I/O and improves performance
  on remote file systems. The cache has a limited size, it prefers to keep
  the last recently used files. Optionally, the maximum time of cached entries
  can be defined to enforce a reload of the template file after a while.
  <p>
  In case of local file system, the use of this cache is optionally, since
  the operating system caches files already.
  <p>
  Loads localized versions of template files. If the caller requests a file with the
  name "index" and the suffix is ".tpl" and the requested locale is "de_DE, de, en-US",
  then files are searched in the following order:

  - index-de_DE.tpl
  - index-de.tpl
  - index-en_US.tpl
  - index-en.tpl
  - index.tpl
  <p>
  The following settings are required:
  <code><pre>
  path=.
  suffix=.tpl
  encoding=UTF-8
  cacheSize=1000000
  cacheTime=60000
  </pre></code>
  The path is relative to the directory of the config file. In case of windows, if the
  settings are in the registry, the path is relative to the current working directory.
  <p>
  Files are cached as long as possible, when cacheTime=0.
  @see TemplateLoader
*/

class TemplateCache : public TemplateLoader {
    Q_OBJECT
    Q_DISABLE_COPY(TemplateCache);
public:

    /**
      Constructor.
      @param settings configurations settings
      @param parent Parent object
    */
    TemplateCache(QObject* parent=0);

protected:

    /**
      Try to get a file from cache or filesystem.
      @param localizedName Name of the template with locale to find
      @return The template document, or empty string if not found
    */
    virtual QString tryFile(QString localizedName);

private:

    struct CacheEntry {
        QString document;
        qint64 created;
    };

    /** Timeout for each cached file */
    int cacheTimeout;

    /** Cache storage */
    QCache<QString,CacheEntry> cache;

};

#endif // TEMPLATECACHE_H
