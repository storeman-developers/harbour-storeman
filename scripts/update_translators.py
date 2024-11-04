#!/usr/bin/env python3

from os import path
import sys
import configparser
from getpass import getpass
import requests
import json

# This API is deprectated and does not work any longer:
URL = 'http://www.transifex.com/api/2/project/harbour-storeman/languages/'
# Either use Transifex's Python library: https://developers.transifex.com/reference/api-python-sdk
# Or use Transifex's REST API: https://developers.transifex.com/reference/get_languages

def credentials():
    '''Get Transifex user name and password'''
    trc_path = path.join(path.expanduser('~'), '.transifexrc')
    if path.isfile(trc_path):
        print('Fetching credentials from', trc_path)
        trc = configparser.ConfigParser()
        try:
            trc.read(trc_path)
            section  = trc['https://www.transifex.com']
            return section['username'], section['password']
        except Exception as e:
            print('Error reading .transifexrc:', e)
            sys.exit(1)
    else:
        return (
            input('Transifex username: '),
            getpass(prompt='Transifex password: ')
        )

def participants(writer, name, tr):
    writer.write(f'        {name}: [')
    arr    = tr[name]
    last_i = len(arr) - 1
    for i, p in enumerate(arr):
        writer.write(f'\n            ListElement {{ name: "{p}" }}')
        writer.write(',' if i < last_i else '\n        ')
    writer.write(']\n')

if __name__ == '__main__':
    auth  = credentials()
    print('Fetching', URL)
    data = requests.get(URL, auth=auth).content
    data = json.loads(data.decode('UTF-8'))
    model_file = path.normpath(path.dirname(__file__) + '/../qml/models/TranslatorsModel.qml')
    print('Writing', model_file)
    with open(model_file, 'w', encoding='UTF-8') as writer:
        writer.write('import QtQuick 2.0\n\nListModel {')
        for tr in data:
            writer.write('\n    ListElement {\n')
            writer.write(f'        locale: "{tr["language_code"]}"\n')
            participants(writer, 'coordinators', tr)
            participants(writer, 'translators',  tr)
            participants(writer, 'reviewers',    tr)
            writer.write('    }\n')
        writer.write('}\n')
