#ifndef ORNREPO_H
#define ORNREPO_H


#include <QString>

struct OrnRepo
{
    bool enabled;
    QString alias;
    QString author;
};

using OrnRepoList = QList<OrnRepo>;

#endif // ORNREPO_H
