import os
from copy import copy
from .px5 import parse_header,check_packet


def dump_read(file='dump.bin'):
    fsize = os.path.getsize(file)
    with open(file, 'rb') as f:
        data = f.read(fsize)

    pkts = list()
    from copy import copy
    while len(data)>0:
        pktL = parse_header(data[:8])
        if pktL is not None:
            if check_packet(data[:pktL+8]):
                pkts.append(copy(data[:pktL+8]))
                data = data[pktL+8:]
        else:
            pos = data.find(b'\xf5\xfa')
            data = data[pos:]

    return pkts
