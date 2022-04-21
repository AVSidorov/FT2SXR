import os.path

import paramiko
from scp import SCPClient
import posixpath as ospath
import time
from data.filenode import FileNode
import pytest


class Storage(FileNode):
    def __init__(self):
        super().__init__()
        self.storage = self
        self.nodes = list()
        self.nodes.append(self)

    def add(self, path):
        cur = self
        for dir_name in path.strip('/').rstrip('/').split('/'):
            if dir_name not in cur:
                cur[dir_name] = FileNode(name=dir_name, parent=cur)
                self.nodes.append(cur[dir_name])
            cur = cur[dir_name]

    def __call__(self, path):
        current = self
        for dir_name in path.strip('/').rstrip('/').split('/'):
            if dir_name in current:
                current = current[dir_name]
            else:
                return None
        return current

    def visit(self, func):
        for node in self.all_in_deep():
            func(node.name)

    def all_in_deep(self, start=None):
        if start is None:
            start = self
        else:
            start = self(start)
        visited = set()
        self.nodes = list()
        stack = list()
        stack.append(start)
        while len(stack) > 0:
            cur = stack[-1]
            if cur not in visited:
                visited.add(cur)
                self.nodes.append(cur)
                stack.extend([cur[_] for _ in reversed(cur)])
            else:
                stack.pop(-1)

    def all_in_width(self, start=None):
        if start is None:
            start = self
        else:
            start = self(start)
        visited = set()
        self.nodes = list()
        stack = list()
        stack.append(start)
        while len(stack) > 0:
            cur = stack[0]
            if cur not in visited:
                visited.add(cur)
                self.nodes.append(cur)
                stack.extend([cur[_] for _ in cur])
            else:
                stack.pop(0)


class InSysStorage(Storage):
    def __init__(self):
        super().__init__()
        self.name = '/InSysADC'
        self.params = dict()
        self.params['hostname'] = '192.168.0.242'
        self.params['username'] = "adc_user"
        self.params['password'] = "adc_user"
        self.params['look_for_keys'] = False
        self.params['allow_agent'] = False
        self.path = '/mnt/nvme0n1p1'

    def abspath_internal(self, path: str = ''):
        path = './' + path
        return ospath.normpath(ospath.join(self.path, path))

    def get(self, filename: str, path: str = None):
        filename = self.abspath_internal(filename)

        if path is None:
            path = os.path.abspath('./')

        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        try:
            client.connect(**self.params)
            scp = SCPClient(client.get_transport())
            scp.get(ospath.join(self.path, filename), path)
        except:
            pass
        finally:
            client.close()

    def put(self, filename: str, path: str):
        path = self.abspath_internal(path)
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        try:
            client.connect(**self.params)
            _, stdout, stderr = client.exec_command(f'mkdir -p {path}')
            time.sleep(1)
            err = stderr.read()
            if len(err) == 0:
                scp = SCPClient(client.get_transport())
                scp.put(ospath.abspath(filename), path)
        except:
            pass
        finally:
            client.close()

    def get_tree(self):
        out = ''
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        try:
            client.connect(**self.params)
            _, stdout, stderr = client.exec_command(f'ls -1ARl --color=never {self.path}')
            time.sleep(1)
            if len(stderr.read()) == 0:
                out = stdout.read().decode()
        except:
            pass
        finally:
            client.close()

        lines = out.split('\n')
        folder = None
        for line in lines:
            if all((len(line) != 0, line.find('total') != 0)):
                if all((line[0] == '/', line[-1] == ':')):
                    folder = '/'+line[len(self.path):].rstrip(':/').strip('/')
                elif folder is not None:
                    fields = line.split()
                    self.add(folder+'/'+fields[8])

    def exist(self, path: str):
        path = self.abspath_internal(path)

        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        try:
            client.connect(**self.params)
            _, stdout, stderr = client.exec_command(f'ls -d {path}')
            out = stdout.readline().replace('\n', '')
            err = stderr.read()
            if len(err) == 0 and ospath.normpath(out) == ospath.normpath(path):
                return True
            elif len(err) > 0:
                return False
            else:
                return None

        except:
            pass
        finally:
            client.close()


def test_abspath():
    st = InSysStorage()
    assert st.abspath_internal() == st.path
    assert st.abspath_internal('') == st.path
    assert st.abspath_internal('a') == st.path + '/'+'a'
    assert st.abspath_internal('/a') == st.path + '/'+'a'
    assert st.abspath_internal('./a') == st.path + '/'+'a'
    assert st.abspath_internal('.a') == st.path + '/'+'.a'
    assert st.abspath_internal('/.a') == st.path + '/'+'.a'
