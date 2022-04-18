import matplotlib.widgets

from core.core import Core
from core.sxr_protocol_pb2 import MainPacket, AdcStatus
import matplotlib.pyplot as plt
import os
import datetime
import numpy as np


class DumpPlotter(Core):
    def __init__(self, parent=None, type='samples'):
        super().__init__(parent=parent)
        self.address = 12
        self._status = None
        self.type = type  # 'samples' or 'msec'
        if self.type not in ('samples', 'msec'):
            self.type = 'samples'

        td = datetime.date.today()
        wdir = format(td.year - 2000, '02d') + format(td.month, '02d') + format(td.day, '02d')
        self.dump_file = os.path.join(os.path.join(os.path.abspath('./'), wdir), 'data_0.bin')

    @property
    def status(self):
        return self._status

    @status.setter
    def status(self, status):
        if isinstance(status, bytes):
            self._status = AdcStatus()
            self._status.ParseFromString(status)
        elif isinstance(status, AdcStatus):
            self._status = status

    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == 1:
            if request.command in (0, 1):
                self.status.ParseFromString(request.data)
            if request.command == 2:

                n_ch = 0
                if len(self.status.board_status) > 0:
                    if len(self.status.board_status[0].channel_mask) > 0:
                        mask = int().from_bytes(self.status.board_status[0].channel_mask, 'big')
                        n_ch = bin(mask).count("1")
                    elif len(self.status.board_status[0].channel_status) > 0:
                        n_ch = sum([ch.enabled for ch in self.status.board_status[0].channel_status])
                if n_ch > 0 and os.path.exists(self.dump_file):
                    dump = np.fromfile(self.dump_file, dtype=np.int16)
                    dump = dump.reshape((-1, n_ch)).T
                    cols = (_ for _ in dump)

                    rate = self.status.sampling_rate
                    samples = self.status.samples

                    for col, row in zip(cols, range(1, n_ch+1)):
                        plt.subplot(n_ch, 1, row)
                        if self.type == 'samples':
                            plt.plot(col)
                        elif self.type == 'msec':
                            plt.plot(np.linspace(0, samples/rate, samples)*1000, col)
                    plt.show()
