from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, TokamakStatus


class Tokamak(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.TOKAMAK, TokamakStatus())

        self.state.density = 0
        self.state.current = 0
        self.state.shotType = TokamakStatus.OH

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, jour = super().snapshot(request, response)
        jour.attrs['name'] = 'Main tokamak parameters'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = jour.create_dataset(f'{fname}', data=val, track_times=True)
            jour.attrs[fname] = val
            if fname in ('density',):
                dset.attrs['units'] = 'lines'
            elif fname in ('current',):
                dset.attrs['units'] = 'kA'
            elif fname in ('shotType',):
                dset.attrs['units'] = 'Heating type'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename