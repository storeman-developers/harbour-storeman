# Translations (l10n / i18n)

You can help localising Storeman to your language using [Transifex](https://www.transifex.com/mentaljam/harbour-storeman) or [Qt Linguist](https://doc.qt.io/qt-5/qtlinguist-index.html).

Note that for Storeman principally translations designated with a country code only (e.g., `pt`) shall be created and maintained.  Only if a complete and well maintained translation for the sole country code exists, a country specific variant with a locale will be accepted (e.g., `nl_BE`).

[Transifex](https://www.transifex.com/mentaljam/harbour-storeman) is the preferred way of pushing your translations.
Please don't send PRs with translations directly to GitHub if you have a Transifex account.

Note that translations for Storeman are **ID based** Qt ts files.  Hence, if you want to compile a translation file for testing, you should run the lrelease command with the "`-idbased`" option, for example:

    lrelease -idbased harbour-storeman.ts

If you want to test your translation before publishing, you should compile it and copy the resulting `qm` file to (requires root priviledges)

    /usr/share/harbour-storeman/translations

Storeman tries to automatically load a translation file based on your system locale setting.  You can also run the application with a selected locale from the terminal.  For example, for the Russian language the command is:

    export LANG=ru; harbour-storeman
