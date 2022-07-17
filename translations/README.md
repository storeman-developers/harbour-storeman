# Translations (l10n / i18n)

You can help to localize harbour-storeman to your language using [Transifex](https://www.transifex.com/mentaljam/harbour-storeman) or [Qt Linguist](http://doc.qt.io/qt-5/qtlinguist-index.html).

Note that for Storeman primarily translations designated with a country code only (e.g., `pt`) shall be created and maintained.  Only if a complete and well maintained translation for the sole country code exists, a country specific variant with a locale will be accepted (e.g., `nl_BE`).

[Transifex](https://www.transifex.com/mentaljam/harbour-storeman) is the preferred way of pushing your translations. Please don't send PRs with translations directly to GitHub if you have a Transifex account.

Note that harbour-storeman translations are **ID based** Qt ts files. So if you want to compile a translation file for testing you should run the lrelease command with the "-idbased" option, for example:

    lrelease -idbased harbour-storeman.ts

If you want to test your translation before publishing you should compile it and place produced qm file to (root access is required)

    /usr/share/harbour-storeman/translations

The application tries to load a translation file automatically based on your system locale setting.  You can also run the application with a selected locale from the terminal.  For example, for the Russian language the command is:

    export LANG=ru; harbour-storeman
