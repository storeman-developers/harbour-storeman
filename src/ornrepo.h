#pragma once

#include <QString>

struct OrnRepo
{
    bool enabled;
    QString alias;
    QString author;
};

using OrnRepoList = QList<OrnRepo>;
