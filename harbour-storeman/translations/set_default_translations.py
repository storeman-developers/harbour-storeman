#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os, glob
import xml.etree.ElementTree as ET

def main():
    # Get translations dir
    trdir = os.path.dirname(os.path.abspath(__file__))
    # Get default ts root
    ts_path = os.path.join(trdir, 'harbour-storeman.ts')
    default_ts = ET.parse(ts_path).getroot()
    # Iterate over other ts files
    ts_tmpl = os.path.join(trdir, 'harbour-storeman-*.ts')
    for ts_file in glob.glob(ts_tmpl):
        print(os.path.basename(ts_file))
        # Get current ts tree, root and context
        current_tree = ET.parse(ts_file)
        current_ts = current_tree.getroot()
        current_context = current_ts[0]
        # Get all messages
        messages = current_ts.findall('.//message')
        # Get messages with unfinished translations for current ts
        unfinished = current_ts.findall('.//message/translation[@type="unfinished"]..')
        for message in unfinished:
            # Find default messages for unfinished
            default_message = default_ts.find('.//message[@id="{}"]'.format(message.attrib['id']))
            # Replace current unfinished message with default one
            current_context.insert(messages.index(message) + 1, default_message)
            current_context.remove(message)
        # Save changes
        current_tree.write(ts_file, encoding='utf-8', xml_declaration=True)

if __name__ == '__main__':
    main()
