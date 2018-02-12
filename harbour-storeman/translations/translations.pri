CONFIG += \
    sailfishapp_i18n \
    sailfishapp_i18n_idbased \
    sailfishapp_i18n_unfinished

TRANSLATIONS += \
    translations/harbour-storeman.ts \
    translations/harbour-storeman-da.ts \
    translations/harbour-storeman-de_DE.ts \
    translations/harbour-storeman-el.ts \
    translations/harbour-storeman-es.ts \
    translations/harbour-storeman-fi_FI.ts \
    translations/harbour-storeman-fr.ts \
    translations/harbour-storeman-hu_HU.ts \
    translations/harbour-storeman-it.ts \
    translations/harbour-storeman-nl.ts \
    translations/harbour-storeman-pl.ts \
    translations/harbour-storeman-ru.ts \
    translations/harbour-storeman-sv.ts

OTHER_FILES += \
    $$TRANSLATIONS \
    $$PWD/update_translations.py

TRANSLATION_SOURCES += $$PWD/../../ornplugin/src
