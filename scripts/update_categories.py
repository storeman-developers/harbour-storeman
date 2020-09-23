#!/usr/bin/env python3

from urllib import request
import json
import re

URL     = 'https://openrepos.net/api/v1/categories'
PATTERN = re.compile(r'[ &]+')

def process(json_array):
    for o in json_array:
        tid  = o['tid'].rjust(4)
        name = o['name']
        trid = PATTERN.sub('-', name).lower()
        print(f'//% "{name}"\n{{ {tid}, QT_TRID_NOOP("orn-cat-{trid}") }},')
        if 'childrens' in o:
            process(o['childrens'])

if __name__ == '__main__':
    response   = request.urlopen(URL)
    categories = json.loads(response.read().decode('UTF-8'))
    process(categories)
