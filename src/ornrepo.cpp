#include "ornrepo.h"
#include "ornpm.h"

OrnRepo::OrnRepo(bool enabled, const QString &alias)
    : enabled{enabled}
    , alias{alias}
    , author{alias == OrnPm::storemanRepo ?
        //% "Storeman OBS Repository"
        qtTrId("orn-storeman-repo-name") :
        alias.mid(OrnPm::repoNamePrefix.size())
      }
{

}
