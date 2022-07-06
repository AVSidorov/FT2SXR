import h5py

from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands
from core.sxr_protocol_pb2 import AmpStatus, AdcStatus  # temporary for wiring
from dev.insys.adc import ADC
from dev.amptek.px5 import PX5
from dev.tubl.amplifier import Amplifier
from core.fileutils import today_dir
import os
import numpy as np


class Ft2SXR(Dev):
    def __init__(self, parent=None, wdir=None):
        super().__init__(parent, SystemStatus.SXR, SystemStatus())
        core = self.get_origin_core()

        if wdir is None:
            self.wdir = today_dir()
        else:
            self.wdir = wdir

        self.request_to_sys = MainPacket()
        self.request_to_dev = MainPacket()
        self.request_to_dev.sender = self.address
        self.request_to_dev.command = Commands.INFO

        self.devs_queue = list()

        self.state.state = SystemStatus.IDLE

        self.adc = ADC(self)
        self.px5 = PX5(self)
        self.amp = Amplifier(self)

        # connect devs to message system
        if core is not None:
            self.channel0.connect(core.channel0)
            core.channel0.connect(self.channel0_slot)

            self.adc.channel0.connect(core.channel0)       # out Main Packets (commands)
            self.adc.channel1.connect(core.channel1)       # out BRD_ctrl packets (from exam_adc)
            core.channel0.connect(self.adc.channel0_slot)  # in Main Packets (commands)

            self.px5.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.px5.channel0_slot)  # in Main Packets (commands)

            self.amp.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.amp.channel0_slot)  # in Main Packets (commands)

        self.state.devs.append(SystemStatus.ADC)
        self.state.devs.append(SystemStatus.AMP)
        self.state.devs.append(SystemStatus.PX5)
        
        # devices wiring
        self.state.binds.add()

        self.state.binds[-1].source.dev = SystemStatus.SDD
        self.state.binds[-1].source.connector = SystemStatus.CON_OUT

        self.state.binds[-1].receivers.add()
        self.state.binds[-1].receivers[-1].dev = SystemStatus.AMP
        self.state.binds[-1].receivers[-1].connector = AmpStatus.CON_IN

        self.state.binds[-1].receivers.add()
        self.state.binds[-1].receivers[-1].dev = SystemStatus.PX5
        self.state.binds[-1].receivers[-1].connector = SystemStatus.CON_IN

        self.state.binds.add()

        self.state.binds[-1].source.dev = SystemStatus.AMP
        self.state.binds[-1].source.connector = AmpStatus.CON_CH_A

        self.state.binds[-1].receivers.add()
        self.state.binds[-1].receivers[-1].dev = SystemStatus.ADC
        self.state.binds[-1].receivers[-1].connector = AdcStatus.CON_CH_1

        self.state.binds.add()

        self.state.binds[-1].source.dev = SystemStatus.AMP
        self.state.binds[-1].source.connector = AmpStatus.CON_CH_B

        self.state.binds[-1].receivers.add()
        self.state.binds[-1].receivers[-1].dev = SystemStatus.ADC
        self.state.binds[-1].receivers[-1].connector = AdcStatus.CON_CH_2

    def command_to_devs(self, command: Commands = None, response: bool = False):
        # store initial request inside function
        if self.request_to_sys.IsInitialized():
            pkt = self.request_to_sys.SerializeToString()
        else:
            pkt = None

        # destroy initial request to supress reaction on ACK packets in queue function
        self.request_to_sys.command = Commands.INFO
        for dev in self.state.devs:
            self.request_to_dev.address = dev
            self.request_to_dev.sender = self.address
            self.request_to_dev.command = command

            if self.request_to_dev.IsInitialized():
                self.channel0.emit(self.request_to_dev.SerializeToString())
        # restore initial request to self.request for responsing
        if pkt is not None:
            self.request.ParseFromString(pkt)
        self._response(response)

    def command_to_dev_from_queue(self, command: Commands = None, response: bool = False):
        # Command que have two "params" devs in que and command should be sent.
        # If response from one device coincide with current command in que (in self.request_to_dev packet)
        # next request (command to next device) will be sent

        if command == self.request_to_dev.command and len(self.devs_queue) > 0:
            dev = self.devs_queue.pop()
            self.request_to_dev.address = dev

            if self.request_to_dev.IsInitialized():
                self.channel0.emit(self.request_to_dev.SerializeToString())
        else:
            # "Clear command (INFO command require no acknowledgment)
            self.request_to_dev.command = Commands.INFO
            self.request_to_dev.data = b''
            # return response so as full que is processed
            if self.request_to_sys.IsInitialized():
                self.request.ParseFromString(self.request_to_sys.SerializeToString())
            self._response(response)

    def start(self, response: bool = False):
        self.command_to_devs(Commands.START, response)

    def stop(self,  response: bool = False):
        self.command_to_devs(Commands.STOP, response)

    def snapshot(self, request: MainPacket = None,  response: bool = False):
        hf, sxr = super().snapshot(f'{self.wdir}/?', response)
        sxr.attrs['name'] = 'SXR diagnostics'
        sxr.attrs['comments'] = ''
        filename = os.path.abspath(os.path.join(self.wdir, hf.filename))

        # make wiring table
        wiring = wiring_tab(self.state.binds)

        # writing array
        dt = h5py.string_dtype(encoding='utf-8')
        dset = sxr.create_dataset('wiring', shape=wiring.shape, dtype=dt)
        dset[:] = wiring

        binds = sxr.require_group('BINDS')

        for bind in self.state.binds:
            bnd = binds.require_group(f'{SystemStatus.EnumDev.Name(bind.source.dev)}_'
                                     f'{connector2str(bind.source.dev,bind.source.connector, is_in=False)}')
            for recv in bind.receivers:
                bnd.attrs[f'{SystemStatus.EnumDev.Name(recv.dev)}'] =\
                          f'{connector2str(recv.dev,recv.connector, is_in=True)}'

        hf.close()

        if self.parent() is not None:
            self.request_to_dev.command = Commands.SNAPSHOT
            self.request_to_dev.data = f'/SXR@{filename}'.encode()

            self.devs_queue.extend(self.state.devs)
            self.command_to_dev_from_queue(Commands.SNAPSHOT, response)

    def channel0_slot(self, data: bytes):
        # store initial request
        # this lines can destroy storing mechanism in original slot
        self.request.ParseFromString(data)
        if all((self.request.command != Commands.INFO,
                self.request.command in Commands.values(),
                self.request.address == self.address)):
            self.request_to_sys.ParseFromString(data)

        # here processed commands
        super().channel0_slot(data)

        # check for responses (ACK)
        if self.request.address == self.address:
            if self.request.command ^ 0xFFFFFFFF in Commands.values():
                self.command_to_dev_from_queue(self.request.command ^ 0xFFFFFFFF, response=True)
                # Command que have two "params" devs in que and command should be sent.
                # If response from one device coincide with current command in que (in self.request_to_dev packet)
                # next request (command to next device) will be sent

    def shutdown(self, response: bool = False):
        if self.parent() is not None:
            self.request_to_dev.command = Commands.SHUTDOWN

            self.devs_queue.extend(self.state.devs)
            self.command_to_dev_from_queue(Commands.SHUTDOWN, response)


def connector2str(dev: SystemStatus.EnumDev, connector, is_in: False) -> str:
    status_obj = SystemStatus
    if dev == SystemStatus.ADC:
        status_obj = AdcStatus
    elif dev == SystemStatus.AMP:
        status_obj = AmpStatus
    if is_in:
        return status_obj.ConnectorsIn.Name(connector).replace('CON_', '')
    else:
        return status_obj.ConnectorsOut.Name(connector).replace('CON_', '')


def wiring_tab(binds) -> np.ndarray:
    wiring = np.ndarray((1, 1), dtype='<U255')

    wiring[0, 0] = 'To/From'

    for bind in binds:
        con_out = connector2str(bind.source.dev, bind.source.connector, is_in=False)
        for recv in bind.receivers:
            con_in = connector2str(recv.dev, recv.connector, is_in=True)
            col_name = f'{SystemStatus.EnumDev.Name(bind.source.dev)}_{con_out}'
            row_name = f'{SystemStatus.EnumDev.Name(recv.dev)}_{con_in}'
            val = '1'
            if recv.dev == SystemStatus.ADC and con_in in [f'CH_{_}' for _ in range(1, 9)]:
                row_name = f'{SystemStatus.EnumDev.Name(recv.dev)}'
                val = con_in.replace('CH_', '')

            row = np.squeeze(np.argwhere(wiring[:, 0] == row_name))
            if row.size == 0:
                wiring = np.vstack((wiring, np.full((1, wiring.shape[1]), '0', dtype='<U255')))
                row = wiring.shape[0] - 1
                wiring[row, 0] = row_name

            col = np.squeeze(np.argwhere(wiring[0, :] == col_name))
            if col.size == 0:
                wiring = np.hstack((wiring, np.full((wiring.shape[0], 1), '0', dtype='<U255')))
                col = wiring.shape[1] - 1
                wiring[0, col] = col_name
            wiring[row, col] = val
    return wiring