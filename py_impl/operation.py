class Operation:
    def __init__(self) -> None:
        self.node = None
        self.error = None

    @property
    def failed(self):
        return self.error is not None

    def success(self, node):
        self.node = node
        return node

    def failure(self, error):
        self.error = error
