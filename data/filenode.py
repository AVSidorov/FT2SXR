class FileNode(dict):
    def __init__(self, *, name='/', storage=None, parent=None, file=None):
        self.name = name
        if parent is not None:
            if hasattr(parent, 'name'):
                self.name = parent.name.rstrip('/')+'/'+name.strip('/').rstrip('/').split('/')[-1]
        self.storage = storage
        if parent is not None:
            if hasattr(parent, 'storage'):
                self.storage = parent.storage
        self.parent = parent
        self.file = file

    def __hash__(self) -> int:
        return id(self)

    def __repr__(self) -> str:
        return self.name

