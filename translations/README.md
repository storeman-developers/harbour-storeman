# Translations

You can help to localize harbour-storeman to your language using [Transifex](https://www.transifex.com/mentaljam/harbour-storeman) or [Qt Linguist](http://doc.qt.io/qt-5/qtlinguist-index.html).

[Transifex](https://www.transifex.com/mentaljam/harbour-storeman) is the preferred way of pushing your translations. Please don't send PRs with translations directly to GitHub if you have a Transifex account.

Note that harbour-storeman translations are **ID based** Qt ts files. So if you want to compile a translation file for testing you should run the lrelease command with the "-idbased" option, for example:

    lrelease -idbased harbour-storeman.ts

If you want to test your translation before publishing you should compile it and place produced qm file to (root access is required)

    /usr/share/harbour-storeman/translations

The application tries to load translation files automatically basing on your system locale settings. Also you can run application with selected locale from terminal. For example for Russian language the command is

    LANG=ru harbour-storeman
