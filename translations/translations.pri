TRANSLATIONS += \
    translations/harbour-storeman.ts \
    translations/harbour-storeman-cs.ts \
    translations/harbour-storeman-da.ts \
    translations/harbour-storeman-de.ts \
    translations/harbour-storeman-el.ts \
    translations/harbour-storeman-es.ts \
    translations/harbour-storeman-et.ts \
    translations/harbour-storeman-fi.ts \
    translations/harbour-storeman-fr.ts \
    translations/harbour-storeman-hu.ts \
    translations/harbour-storeman-it.ts \
    translations/harbour-storeman-nl.ts \
    translations/harbour-storeman-nl_BE.ts \
    translations/harbour-storeman-no.ts \
    translations/harbour-storeman-pl.ts \
    translations/harbour-storeman-pt.ts \
    translations/harbour-storeman-ru.ts \
    translations/harbour-storeman-sk.ts \
    translations/harbour-storeman-sl.ts \
    translations/harbour-storeman-sv.ts \
    translations/harbour-storeman-tt.ts \
    translations/harbour-storeman-zh.ts

qm.input    = TRANSLATIONS
qm.output   = translations/${QMAKE_FILE_BASE}.qm
qm.commands = @echo "compiling ${QMAKE_FILE_NAME}"; \
                lrelease -idbased -silent ${QMAKE_FILE_NAME} -qm ${QMAKE_FILE_OUT}
qm.CONFIG   = target_predeps no_link

QMAKE_EXTRA_COMPILERS += qm

translations.files = $$OUT_PWD/translations/*.qm
translations.path  = $$PREFIX/share/$$TARGET/translations
translations.CONFIG += no_check_exist

INSTALLS += translations
