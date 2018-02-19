CONFIG += \
    sailfishapp_i18n \
    sailfishapp_i18n_idbased \
    sailfishapp_i18n_unfinished

TRANSLATIONS += \
    $$PWD/harbour-storeman.ts \
    $$PWD/harbour-storeman-da.ts \
    $$PWD/harbour-storeman-de_DE.ts \
    $$PWD/harbour-storeman-el.ts \
    $$PWD/harbour-storeman-es.ts \
    $$PWD/harbour-storeman-fi_FI.ts \
    $$PWD/harbour-storeman-fr.ts \
    $$PWD/harbour-storeman-hu_HU.ts \
    $$PWD/harbour-storeman-it.ts \
    $$PWD/harbour-storeman-nl.ts \
    $$PWD/harbour-storeman-pl.ts \
    $$PWD/harbour-storeman-pt_BR.ts \
    $$PWD/harbour-storeman-ru.ts \
    $$PWD/harbour-storeman-sv.ts

OTHER_FILES += \
    $$TRANSLATIONS \
    $$PWD/update_translations.py

TRANSLATION_SOURCES += $$PWD/../../ornplugin/src
