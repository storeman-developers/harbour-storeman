#ifndef ORNREPO_H
#define ORNREPO_H


#include <QString>

struct OrnRepo
{
    bool enabled;
    QString alias;
    QString author;
};

typedef QList<OrnRepo> OrnRepoList;

#endif // ORNREPO_H
