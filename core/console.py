from PyQt5.QtCore import QCoreApplication
from core import Core
import sys
import signal


class Console(Core):
    def __init__(self, parent):
        super().__init__(parent)
        sys.stdout.write("Console started\n")
        self.channel0_slot(b'')

    def run(self):
        for command in sys.stdin:
            self.channel0.emit(command.encode())

    def channel0_slot(self, data: bytes):
        sys.stdout.write(">")
        sys.stdout.flush()


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QCoreApplication(sys.argv)

    core = Core(app)

    console = Console(app)
    console.channel0.connect(core.channel0)
    core.channel0.connect(console.channel0_slot)

    console.run()

    return app.exec()


