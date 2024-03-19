# Translations (l10n / i18n)

You can help localising Storeman to your language using [Transifex](https://app.transifex.com/mentaljam/harbour-storeman) or [Qt Linguist](https://doc.qt.io/qt-5/qtlinguist-index.html).

Note that for Storeman principally translations designated with a country code only (e.g. `pt`) shall be created and maintained.  Only if a complete and well maintained translation for the sole country code exists, a country specific variant with a locale will be accepted (e.g. `nl_BE`).

[Transifex](https://app.transifex.com/mentaljam/harbour-storeman) is the preferred way of submitting translations.  Please do not send pull requests (PRs) with translations directly to GitHub, if you have a Transifex account.

If you do not want to use Atlassian's Transifex, alternatives are [Qt Linguist](https://doc.qt.io/qt-5/linguist-translators.html) or to perform this manually, which is tediously and error prone, hence only suitable for small changes.  The resulting changes must be submitted as a pull request.

### Testing translations

Note that translations for Storeman are utilising *ID based* Qt `.ts` files.  Hence, to compile a translation file for testing, the `lrelease` command must be executed with the option `-idbased` to convert the translation files (`.ts` files) into Qt message files (`.qm` files), either from [within Qt Linguist](https://doc.qt.io/qtcreator/creator-editor-external.html) or directly [at the command line](https://doc.qt.io/qt-5/linguist-manager.html):
```
lrelease -idbased harbour-storeman.ts
```
If you want to test your translation before publishing, you should compile it and copy the resulting `.qm` file(s) to (requires root privileges):
```
 /usr/share/harbour-storeman/translations
```
Storeman tries to automatically load a translation file according to your system locale setting.  You can also run the application with a selected locale from the terminal.  For example, for the Swedish language the command is:
```
export LANG=sv; harbour-storeman
```

### Updating the source `.ts` file with source strings from source code 

Developers and release managers can use the `lupdate` process, either from [within Qt Linguist](https://doc.qt.io/qtcreator/creator-editor-external.html) or directly [at the command line](https://doc.qt.io/qt-5/linguist-manager.html) (mind to [include all files with translatable strings](https://github.com/storeman-developers/harbour-storeman/pull/431#issuecomment-1659024529), e.g. by `lupdate qml/ src/ *.desktop -ts translations/harbour-storeman.ts`), or tediously perform this manually, which hence is only suitable for small changes.

---------------------------------------------

**Note**: When updating this README, mind to also update [its counterpart for the SailfishOS:Chum GUI app](https://github.com/sailfishos-chum/sailfishos-chum-gui//blob/main/translations/README.md).
