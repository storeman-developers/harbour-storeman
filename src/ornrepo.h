#pragma once

#include <QString>

struct OrnRepo
{
    OrnRepo(bool enabled, const QString &alias);

    bool enabled;
    QString alias;
    QString author;
};

using OrnRepoList = QList<OrnRepo>;
