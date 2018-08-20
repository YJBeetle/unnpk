#! /usr/bin/env python
# -*- coding: utf-8 -*-
import os
import zlib
import rotor
import marshal
import binascii
import argparse
import pymarshal
class PYCEncryptor(object):
    def __init__(self):
        self.opcode_encrypt_map = {
            1: 38, 2: 46, 3: 37, 4: 66, 5: 12, 10: 35, 11: 67, 12: 81, 13: 32, 15: 9, 19: 63, 20: 70,
            21: 44, 22: 36, 23: 39, 24: 57, 25: 10, 26: 52, 28: 49, 30: 86, 31: 87, 32: 88, 33: 89,
            40: 24, 41: 25, 42: 26, 43: 27, 50: 14, 51: 15, 52: 16, 53: 17, 54: 8, 55: 21, 56: 55,
            57: 82, 58: 34, 59: 22, 60: 65, 61: 6, 62: 58, 63: 71, 64: 43, 65: 30, 66: 19, 67: 5,
            68: 60, 71: 53, 72: 42, 73: 3, 74: 48, 75: 84, 76: 77, 77: 78, 78: 85, 79: 47, 80: 51,
            81: 54, 82: 50, 83: 83, 84: 74, 85: 64, 86: 31, 87: 72, 88: 45, 89: 33, 90: 145, 91: 159,
            92: 125, 93: 149, 94: 157, 95: 132, 96: 95, 97: 113, 98: 111, 99: 138, 100: 153, 101: 101,
            102: 135, 103: 90, 104: 99, 105: 151, 106: 96, 107: 114, 108: 134, 109: 116, 110: 156,
            111: 105, 112: 130, 113: 137, 114: 148, 115: 172, 116: 155, 119: 103, 120: 158, 121: 128,
            122: 110, 124: 97, 125: 104, 126: 118, 130: 93, 131: 131, 132: 136, 133: 115, 134: 100, 135: 120,
            136: 129, 137: 102, 140: 140, 141: 141, 142: 142, 143: 94, 146: 109, 147: 123
        }
        self.opcode_decrypt_map = {self.opcode_encrypt_map[key]: key for key in self.opcode_encrypt_map}
        self.pyc27_header = "\x03\xf3\x0d\x0a\x00\x00\x00\x00"
    def _decrypt_file(self, filename):
        os.path.splitext(filename)
        content = open(filename).read()
        try:
            m = pymarshal.loads(content)
        except:
            try:
                m = marshal.loads(content)
            except Exception as e:
                print("[!] error: %s" % str(e))
                return None
        return m.co_filename.replace('\\', '/'), pymarshal.dumps(m, self.opcode_decrypt_map)
    def decrypt_file(self, input_file, output_file=None):
        result = self._decrypt_file(input_file)
        if not result:
            return
        pyc_filename, pyc_content = result
        if not output_file:
            output_file = os.path.basename(pyc_filename) + '.pyc'
        with open(output_file, 'wb') as fd:
            fd.write(self.pyc27_header + pyc_content)
def main():
    parser = argparse.ArgumentParser(description='onmyoji py decrypt tool')
    parser.add_argument("INPUT_NAME", help='input file')
    parser.add_argument("OUTPUT_NAME", help='output file')
    args = parser.parse_args()
    encryptor = PYCEncryptor()
    encryptor.decrypt_file(args.INPUT_NAME, args.OUTPUT_NAME)
if __name__ == '__main__':
    main()