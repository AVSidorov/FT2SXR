import paramiko
from scp import SCPClient
import posixpath as ospath
from core.fileutils import today_dir, work_dir
import time


class Storage:
    def __init__(self, path='/mnt/nvme0n1p1', wdir=None, protocol='scp', **kwargs):
        self.params = dict()
        self.params['hostname'] = '192.168.0.242'
        self.params['username'] = "adc_user"
        self.params['password'] = "adc_user"
        self.params['look_for_keys'] = False
        self.params['allow_agent'] = False
        self.path = path
        self.work_dir = today_dir(wdir)
        self.protocol = protocol

        if len(kwargs) != 0:
            self.params = kwargs

    def get(self, filename, path=None):
        if filename[0] == '/':
            filename = '.'+filename

        if path is None:
            path = self.work_dir

        if self.protocol == 'scp':
            client = paramiko.SSHClient()
            client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            try:
                client.connect(**self.params)
                scp = SCPClient(client.get_transport())
                scp.get(ospath.join(self.path, filename), self.work_dir)
            except:
                pass
            finally:
                client.close()

    def put(self, filename, path):
        if path[0] == '/':
            path = '.'+path
        if self.protocol == 'scp':
            client = paramiko.SSHClient()
            client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            try:
                client.connect(**self.params)
                ssh = client.invoke_shell()
                ssh.send(f'mkdir -p {ospath.join(self.path, ospath.split(path)[0])}\n')
                time.sleep(1)
                scp = SCPClient(client.get_transport())
                scp.put(ospath.abspath(filename), ospath.join(self.path, path))
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
            ssh = client.invoke_shell()
            ssh.send(f'ls -1ARl --color=never {self.path}\n')
            time.sleep(1)

            while ssh.recv_ready():
                out += ssh.recv(1024).decode()
        except:
            pass
        finally:
            client.close()

        lines = out.split('\r\n')
        tree = dict()
        p = None
        for line in lines[1:-1]:
            if all((len(line) != 0, line.find('total') != 0)):
                if all((line[0] == '/', line[-1] == ':')):
                    p = tree
                    for dir_name in line.strip('/').rstrip(':').split('/'):
                        if not dir_name in p:
                            p[dir_name] = dict()
                        p = p[dir_name]
                elif p is not None:
                    fields = line.split()
                    p[fields[8]] = dict()
                    if fields[0][0] == '-':
                        p[fields[8]]['mode'] = fields[0][1:]
                        p[fields[8]]['user'] = fields[2]
                        p[fields[8]]['group'] = fields[3]
                        p[fields[8]]['size'] = fields[4]
                        p[fields[8]]['date'] = fields[5:8]
                    elif fields[0][0] == 'l':
                        p[fields[8]] = fields[-1]
        for dir_name in self.path.strip('/').rstrip(':').split('/'):
            tree=tree[dir_name]
        return tree
