#include "wellknownprogramsthirdpartypm.h"
#include "wpmutils.h"

void WellKnownProgramsThirdPartyPM::scanDotNet(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    // http://stackoverflow.com/questions/199080/how-to-detect-what-net-framework-versions-and-service-packs-are-installed

    Package* p = new Package("com.microsoft.DotNetRedistributable",
            ".NET redistributable runtime");
    p->url = "http://msdn.microsoft.com/en-us/netframework/default.aspx";
    p->description = QObject::tr(".NET runtime");

    QString err = rep->savePackage(p);
    delete p;

    WindowsRegistry wr;
    if (err.isEmpty()) {
        err = wr.open(HKEY_LOCAL_MACHINE,
                "Software\\Microsoft\\NET Framework Setup\\NDP", false,
                KEY_READ);
    }

    if (err.isEmpty()) {
        QStringList entries = wr.list(&err);
        if (err.isEmpty()) {
            for (int i = 0; i < entries.count(); i++) {
                QString v_ = entries.at(i);
                Version v;
                if (v_.startsWith("v") && v.setVersion(
                        v_.right(v_.length() - 1))) {
                    WindowsRegistry r;
                    err = r.open(wr, v_, KEY_READ);
                    if (err.isEmpty())
                        detectOneDotNet(installed, rep, r, v_);
                }
            }
        }
    }
}

void WellKnownProgramsThirdPartyPM::detectOneDotNet(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        const WindowsRegistry& wr,
        const QString& keyName) const
{
    QString package("com.microsoft.DotNetRedistributable");
    Version keyVersion;

    Version oneOne(1, 1);
    Version four(4, 0);
    Version two(2, 0);

    Version v;
    bool found = false;
    if (keyName.startsWith("v") && keyVersion.setVersion(
            keyName.right(keyName.length() - 1))) {
        if (keyVersion.compare(oneOne) < 0) {
            // not yet implemented
        } else if (keyVersion.compare(two) < 0) {
            v = keyVersion;
            found = true;
        } else if (keyVersion.compare(four) < 0) {
            QString err;
            QString value_ = wr.get("Version", &err);
            if (err.isEmpty() && v.setVersion(value_)) {
                found = true;
            }
        } else {
            WindowsRegistry r;
            QString err = r.open(wr, "Full", KEY_READ);
            if (err.isEmpty()) {
                QString value_ = r.get("Version", &err);
                if (err.isEmpty() && v.setVersion(value_)) {
                    found = true;
                }
            }
        }
    }

    if (found) {
        PackageVersion* pv = new PackageVersion(package);
        pv->version = v;
        rep->savePackageVersion(pv);
        delete pv;

        InstalledPackageVersion* ipv = new InstalledPackageVersion(package, v,
                "");
        installed->append(ipv);
    }
}

QString WellKnownProgramsThirdPartyPM::detectMSXML(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    QString err;

    QScopedPointer<Package> p(
            new Package("com.microsoft.MSXML",
            QObject::tr("Microsoft Core XML Services (MSXML)")));
    p->url = "http://www.microsoft.com/downloads/en/details.aspx?FamilyID=993c0bcf-3bcf-4009-be21-27e85e1857b1#Overview";
    p->description = QObject::tr("XML library");
    p->setChangeLog("http://msdn.microsoft.com/en-us/library/ms753751(v=vs.85).aspx");
    err = rep->savePackage(p.data());

    Version v;
    Version nullNull(0, 0);

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml2.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml3.dll");
        if (v.compare(nullNull) > 0) {
            v.prepend(3);
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml4.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml5.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("msxml6.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());
            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    return err;
}

void WellKnownProgramsThirdPartyPM::detectWindows(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    Version v;
    v.setVersion(osvi.dwMajorVersion, osvi.dwMinorVersion,
            osvi.dwBuildNumber);

    if (!WPMUtils::is64BitWindows()) {
        QScopedPointer<Package> p32(new Package("com.microsoft.Windows32",
                QObject::tr("Windows 32 bit")));
        p32->description = QObject::tr("operating system");
        p32->url = "http://www.microsoft.com/windows/";
        QScopedPointer<PackageVersion> pv32(new PackageVersion(p32->name, v));
        rep->savePackage(p32.data());
        rep->savePackageVersion(pv32.data());
        installed->append(new InstalledPackageVersion(p32->name, v,
                WPMUtils::getWindowsDir()));
    } else {
        QScopedPointer<Package> p64(new Package("com.microsoft.Windows64",
                QObject::tr("Windows 64 bit")));
        p64->description = QObject::tr("operating system");
        p64->url = "http://www.microsoft.com/windows/";
        QScopedPointer<PackageVersion> pv64(new PackageVersion(p64->name, v));
        rep->savePackage(p64.data());
        rep->savePackageVersion(pv64.data());
        installed->append(new InstalledPackageVersion(p64->name, v,
                WPMUtils::getWindowsDir()));
    }

    QScopedPointer<Package> p(new Package("com.microsoft.Windows",
            "Windows"));
    p->description = QObject::tr("operating system");
    p->url = "http://www.microsoft.com/windows/";
    rep->savePackage(p.data());
    QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
    rep->savePackageVersion(pv.data());

    // "" is used here as the installation directory as Npackd does not allow
    // multiple package versions to be installed in the same directory
    // in this case these would be com.microsoft.Windows and
    // com.microsoft.Windows64
    installed->append(new InstalledPackageVersion(p->name, v, ""));
}

void WellKnownProgramsThirdPartyPM::detectJRE(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        bool w64bit) const
{
    if (w64bit && !WPMUtils::is64BitWindows())
        return;

    QString package = w64bit ? "com.oracle.JRE64" :
            "com.oracle.JRE";

    QScopedPointer<Package> p(new Package(package, w64bit ? "JRE 64 bit" :
            QObject::tr("JRE")));
    p->description = QObject::tr("Java runtime");
    p->url = "http://www.java.com/";
    p->setChangeLog("http://en.wikipedia.org/wiki/Java_version_history");
    rep->savePackage(p.data());

    WindowsRegistry jreWR;
    QString err = jreWR.open(HKEY_LOCAL_MACHINE,
            "Software\\JavaSoft\\Java Runtime Environment", !w64bit, KEY_READ);
    if (err.isEmpty()) {
        QStringList entries = jreWR.list(&err);
        for (int i = 0; i < entries.count(); i++) {
            QString v_ = entries.at(i);
            v_ = v_.replace('_', '.');
            Version v;
            if (!v.setVersion(v_) || v.getNParts() <= 2)
                continue;

            WindowsRegistry wr;
            err = wr.open(jreWR, entries.at(i), KEY_READ);
            if (!err.isEmpty())
                continue;

            QString path = wr.get("JavaHome", &err);
            if (!err.isEmpty())
                continue;

            if (path.trimmed().isEmpty())
                continue;

            QDir d(path);
            if (!d.exists())
                continue;

            QScopedPointer<PackageVersion> pv(new PackageVersion(package, v));
            rep->savePackageVersion(pv.data());

            installed->append(new InstalledPackageVersion(package, v, path));
        }
    }
}

void WellKnownProgramsThirdPartyPM::detectJDK(
        QList<InstalledPackageVersion *> *installed, Repository *rep,
        bool w64bit) const
{
    QString package = w64bit ? "com.oracle.JDK64" : "com.oracle.JDK";

    if (w64bit && !WPMUtils::is64BitWindows())
        return;

    QScopedPointer<Package> p(new Package(package,
            w64bit ? QObject::tr("JDK 64 bit") : QObject::tr("JDK")));
    p->url = "http://www.oracle.com/technetwork/java/javase/overview/index.html";
    p->description = QObject::tr("Java development kit");
    p->setChangeLog("http://en.wikipedia.org/wiki/Java_version_history");
    rep->savePackage(p.data());

    WindowsRegistry wr;
    QString err = wr.open(HKEY_LOCAL_MACHINE,
            "Software\\JavaSoft\\Java Development Kit",
            !w64bit, KEY_READ);
    if (err.isEmpty()) {
        QStringList entries = wr.list(&err);
        if (err.isEmpty()) {
            for (int i = 0; i < entries.count(); i++) {
                QString v_ = entries.at(i);
                WindowsRegistry r;
                err = r.open(wr, v_, KEY_READ);
                if (!err.isEmpty())
                    continue;

                v_.replace('_', '.');
                Version v;
                if (!v.setVersion(v_) || v.getNParts() <= 2)
                    continue;

                QString path = r.get("JavaHome", &err);
                if (!err.isEmpty())
                    continue;

                if (path.trimmed().isEmpty())
                    continue;

                QDir d(path);
                if (!d.exists())
                    continue;

                QScopedPointer<PackageVersion> pv(
                        new PackageVersion(package, v));
                rep->savePackageVersion(pv.data());

                installed->append(new InstalledPackageVersion(package, v, path));
            }
        }
    }
}

QString WellKnownProgramsThirdPartyPM::detectMicrosoftInstaller(
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    QString err;

    QScopedPointer<Package> p(new Package("com.microsoft.WindowsInstaller",
            QObject::tr("Windows Installer")));
    p->url = "http://msdn.microsoft.com/en-us/library/cc185688(VS.85).aspx";
    p->description = QObject::tr("Package manager");

    err = rep->savePackage(p.data());

    Version v;
    Version nullNull(0, 0);
    if (err.isEmpty()) {
        v = WPMUtils::getDLLVersion("MSI.dll");
        if (v.compare(nullNull) > 0) {
            QScopedPointer<PackageVersion> pv(new PackageVersion(p->name, v));
            err = rep->savePackageVersion(pv.data());

            installed->append(new InstalledPackageVersion(p->name, v, ""));
        }
    }

    return err;
}

WellKnownProgramsThirdPartyPM::WellKnownProgramsThirdPartyPM(
        const QString &packageName)
{
    this->packageName = packageName;
}

void WellKnownProgramsThirdPartyPM::scan(Job* job,
        QList<InstalledPackageVersion *> *installed, Repository *rep) const
{
    detectWindows(installed, rep);
    scanDotNet(installed, rep);

    QString err = detectMSXML(installed, rep);
    if (!err.isEmpty())
        job->setErrorMessage(err);

    if (job->shouldProceed()) {
        err = detectMicrosoftInstaller(installed, rep);
        if (!err.isEmpty())
            job->setErrorMessage(err);
    }

    if (job->shouldProceed())
        detectJRE(installed, rep, false);

    if (job->shouldProceed()) {
        if (WPMUtils::is64BitWindows())
            detectJRE(installed, rep, true);
    }

    if (job->shouldProceed())
        detectJDK(installed, rep, false);

    if (job->shouldProceed()) {
        if (WPMUtils::is64BitWindows())
            detectJDK(installed, rep, true);
    }

    if (job->shouldProceed()) {
        Package* p = new Package("com.googlecode.windows-package-manager.Npackd",
                "Npackd");
        p->url = "http://code.google.com/p/windows-package-manager/";
        p->description = "package manager";

        err = rep->savePackage(p);
        if (!err.isEmpty())
            job->setErrorMessage(err);

        delete p;
    }

    if (job->shouldProceed()) {
        Package* p = new Package("com.googlecode.windows-package-manager.Npackd64",
                "Npackd 64 bit");
        p->url = "http://code.google.com/p/windows-package-manager/";
        p->description = "package manager";

        err = rep->savePackage(p);
        if (!err.isEmpty())
            job->setErrorMessage(err);

        delete p;
    }

    if (job->shouldProceed()) {
        Package* p = new Package("com.googlecode.windows-package-manager.NpackdCL",
                "NpackdCL");
        p->url = "http://code.google.com/p/windows-package-manager/";
        p->description = "command line interface to Npackd";

        err = rep->savePackage(p);
        if (!err.isEmpty())
            job->setErrorMessage(err);

        delete p;
    }

    if (job->shouldProceed()) {
        Version version(NPACKD_VERSION);
        PackageVersion* pv = new PackageVersion(packageName, version);
        err = rep->savePackageVersion(pv);
        if (!err.isEmpty())
            job->setErrorMessage(err);
        delete pv;

        InstalledPackageVersion* ipv = new InstalledPackageVersion(packageName,
                version, WPMUtils::getExeDir());
        installed->append(ipv);
    }

    if (job->shouldProceed())
        job->setProgress(1);
    job->complete();
}

