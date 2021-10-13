from raw.bread.operation import Operation
from typing import Optional, Union

from .lexer import Lexer, Morpheme, Position, Token
from .parser import Node
from .errors import DataError


class Stack:
    def __init__(self, name, antecedent: Optional["Stack"]=None, entry: Optional["Stack"]=None, namespace: "NamespaceTable"=None) -> None:
        self.name = name
        self.antecedent = antecedent
        self.namespace = namespace
        self.entry = entry


class NamespaceTable:
    def __init__(self) -> None:
        self.space = {}

    def set(self, name, val):
        # make an implicit pointer
        self.space[name] = val

    def reserve(self, name):
        self.space[name] = None

    def get(self, name: Union[Token, str]):
        oto = object()
        varname = name.meaning.value if isinstance(name, Token) else name
        ret = self.space.get(varname, oto)
        if ret is oto:
            del oto
            return DataError(f"Cannot resolve undefined variable {varname}", Lexer.text, Position(), varname)
        else:
            return ret


class Interpreter:
    def __init__(self, stack: Stack) -> None:
        self.stack = stack

    def visit(self, node: Node, operation: Operation) -> Morpheme:
        return operation.success(node.visit(self.stack, operation))

    @staticmethod
    def interpret(stack, ast):
        result = []
        instance = Interpreter(stack)
        op = Operation()
        for node in ast:
            instance.visit(node, op)
            if op.failed:
                return op.error
            else:
                result.append(op.node)
        return result