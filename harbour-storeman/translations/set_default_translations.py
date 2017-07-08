#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import glob
import xml.etree.ElementTree as ET

def main():
    # Get default ts root
    default_ts = ET.parse('harbour-storeman.ts').getroot()
    # Iterate over other ts files
    for ts_file in glob.glob('harbour-storeman-*.ts'):
        print(ts_file)
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
