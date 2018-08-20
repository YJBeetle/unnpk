#!/usr/bin/env python2
import zlib
import argparse

def _reverse_string(s):
    l = list(s)
    l = map(lambda x: chr(ord(x) ^ 154), l[0:128]) + l[128:]
    l.reverse()
    return ''.join(l)


def unnpk(data):
    asdf_dn = 'j2h56ogodh3se'
    asdf_dt = '=dziaq.'
    asdf_df = '|os=5v7!"-234'
    asdf_tm = asdf_dn * 4 + (asdf_dt + asdf_dn + asdf_df) * 5 + '!' + '#' + asdf_dt * 7 + asdf_df * 2 + '*' + '&' + "'"
    import rotor
    rotor = rotor.newrotor(asdf_tm)
    data = rotor.decrypt(data)
    data = zlib.decompress(data)
    data = _reverse_string(data)
    return data

def main():
    parser = argparse.ArgumentParser(description='unnpk tool')
    parser.add_argument("INPUT_NAME", help='input file')
    # parser.add_argument("OUTPUT_NAME", help='output file')
    args = parser.parse_args()
    input_file = args.INPUT_NAME
    # output_file = args.OUTPUT_NAME
    # if not output_file:
    #     output_file = input_file + '.unnpk'
    data = unnpk(open(input_file).read())
    # with open(output_file, 'wb') as fd:
    #     fd.write(data)
    print(data)

if __name__ == '__main__':
    main()
