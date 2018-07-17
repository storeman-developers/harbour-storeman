#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from urllib import request
import json

URL = 'https://openrepos.net/api/v1/categories'
TEMPL = '''\
//% "{}"
{{ {}, QT_TRID_NOOP("orn-cat-{}") }}'''


def process(json_array):
    result = []
    for o in json_array:
        tid = o['tid'].rjust(4)
        name = o['name']
        trid = name.replace(' & ', '-').replace(' ', '-').lower()
        result.append(TEMPL.format(name, tid, trid))
        if 'childrens' in o:
            result.extend(process(o['childrens']))
    return result

def main():
    response = request.urlopen(URL)
    cats = json.loads(response.read().decode('UTF-8'))
    result = process(cats)
    print(',\n'.join(result))

if __name__ == '__main__':
    main()
