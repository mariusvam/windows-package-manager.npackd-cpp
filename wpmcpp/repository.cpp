#include <windows.h>
#include <shlobj.h>

#include "repository.h"
#include "qhttp.h"
#include "qtemporaryfile.h"
#include "downloader.h"
#include "qsettings.h"
#include "qdom.h"

#include "downloader.h"

Repository* Repository::def = 0;

Repository::Repository()
{
}

bool Repository::load(QString* errMsg)
{
    this->packageVersions.clear();
    QUrl* url = getRepositoryURL();
    bool r = false;
    errMsg->clear();
    if (url) {
        QTemporaryFile* f = download(*url, errMsg);
        if (f) {
            QDomDocument doc;
            int errorLine;
            int errorColumn;
            if (doc.setContent(f, errMsg, &errorLine, &errorColumn)) {
                QDomElement root = doc.documentElement();
                for(QDomNode n = root.firstChild(); !n.isNull();
                        n = n.nextSibling()) {
                    if (n.isElement()) {
                        QDomElement e = n.toElement();
                        if (e.nodeName() == "version") {
                            PackageVersion* a = new PackageVersion(
                                    e.attribute("package"));
                            QString url = e.elementsByTagName("url").at(0).
                                          firstChild().nodeValue();
                            a->download.setUrl(url);
                            QString name = e.attribute("name", "1.0");
                            a->setVersion(name);
                            this->packageVersions.append(a);
                        }
                    }
                }
                r = true;
            } else {
                errMsg->prepend("XML parsing failed: ");
            }
            delete f;
        } else {
            errMsg->prepend("Download failed: ");
        }
    } else {
        errMsg->append("No repository defined");
    }
    return r;
}

QUrl* Repository::getRepositoryURL()
{
    QSettings s("WPM", "Windows Package Manager");
    QString v = s.value("repository", "").toString();
    if (v != "")
        return new QUrl(v);
    else
        return 0;
}

void Repository::setRepositoryURL(QUrl& url)
{
    QSettings s("WPM", "Windows Package Manager");
    s.setValue("repository", url.toString());
}

Repository* Repository::getDefault()
{
    if (!def) {
        def = new Repository();
    }
    return def;
}

QString Repository::getProgramFilesDir()
{
    WCHAR dir[MAX_PATH];
    SHGetFolderPath(0, CSIDL_PROGRAM_FILES, NULL, 0, dir);
    return  QString::fromUtf16 (reinterpret_cast<ushort*>(dir));
}

QTemporaryFile* Repository::download(const QUrl &url, QString* errMsg)
{
    errMsg->clear();
    QTemporaryFile* file = new QTemporaryFile();
    if (file->open()) {
        Downloader d;
        bool r = d.download(url, file, errMsg);
        file->close();

        if (!r) {
            delete file;
            file = 0;
        }
    } else {
        errMsg->append("Error opening file: ").append(file->fileName());
        delete file;
        file = 0;
    }
    return file;
}
