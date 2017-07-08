CONFIG += \
    sailfishapp_i18n \
    sailfishapp_i18n_idbased

TRANSLATIONS += \
    translations/harbour-storeman.ts \
    translations/harbour-storeman-nl.ts \
    translations/harbour-storeman-pl.ts \
    translations/harbour-storeman-ru.ts \
    translations/harbour-storeman-sv.ts

OTHER_FILES += \
    $$TRANSLATIONS \
    translations/update_translations.py

TRANSLATION_SOURCES += $$PWD/../../ornplugin/src
