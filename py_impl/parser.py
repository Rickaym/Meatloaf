from abc import ABCMeta, abstractmethod
from typing import Callable, List, NoReturn, Optional, Tuple

from .errors import MeatloafError, SyntaxError
from .lexer import Lexer, Morpheme, Token, NaiveTypes
from .operation import Operation


class Node(metaclass=ABCMeta):
    def __init__(self, token: Token) -> None:
        self.token = token

    def __repr__(self) -> str:
        return repr(self.token)

    @abstractmethod
    def visit(self, stack, operation: Operation) -> NoReturn:
        raise Exception(f"No visit method defined {self.__class__.__name__}")


class NumNode(Node):
    def visit(self, stack, operation: Operation) -> Morpheme:
        return operation.success(self.token.meaning)


class NamespaceNode(Node):
    def visit(self, stack, operation: Operation) -> Morpheme:
        ans = stack.namespace.get(self.token)
        if isinstance(ans, MeatloafError):
            return operation.failure(ans)
        return operation.success(ans)


class BinaryOperation(Node):
    def __init__(self, superior: Node, op_token: Token, inferior: Node) -> None:
        self.superior = superior
        self.op_token = op_token
        self.inferior = inferior
        super().__init__(self.superior.token)

    def __repr__(self) -> str:
        return f'BO({self.superior}, {self.op_token}, {self.inferior})'.upper()

    def visit(self, stack, operation: Operation):
        self.superior.visit(stack, operation)
        nw_op = Operation()
        self.inferior.visit(stack, nw_op)

        if operation.failed is True:
            return

        if nw_op.failed is True:
            return operation.failure(nw_op.error)

        return operation.success(self.token.meaning.identity.infix(
                        operation.node, self.op_token, nw_op.node, type=self.superior.__class__, stack=stack
                        ))


class UnaryOperation(Node):
    def __init__(self, op_token: Token, node: Node) -> None:
        self.op_token = op_token
        self.applies = node
        super().__init__(self.applies.token)

    def __repr__(self) -> str:
        return f'UO({self.op_token}, {self.applies})'.upper()

    def visit(self, stack, operation: Operation):
        self.applies.visit(stack, operation)
        if operation.failed is True and not isinstance(self.applies, NamespaceNode):
            return
        elif operation.failed is True and isinstance(self.applies, NamespaceNode):
            operation.node = operation.error
            operation.error = None

        return operation.success(self.token.meaning.identity.infix(operation.node, self.op_token, type=self.applies.__class__, stack=stack))


class Parser:
    def __init__(self, tokens: List[Token]) -> None:
        self.tokens = tokens
        self.cur_token: Optional[Token] = None
        self.index = -1
        self.advance()

    def advance(self):
        self.index += 1
        if self.index < len(self.tokens):
            self.cur_token = self.tokens[self.index]
        return self.cur_token

    @staticmethod
    def parse(tokens: List[Token]):
        instance = Parser(tokens)
        reses = []
        op = Operation()
        while instance.cur_token.type != NaiveTypes.eof:
            instance.statement(op)
            if op.failed:
                return op.error
            reses.append(op.node)
        return reses

    def factor(self, operation: Operation) -> Tuple[bool, Node]:
        tk = self.cur_token
        self.advance()
        if tk.type is NaiveTypes.infix:
            self.factor(operation)
            if operation.failed is True:
                return
            if tk.meaning.unary is False:
                pos = tk.position
                pos.end = operation.node.token.position.end
                return operation.failure(SyntaxError(f"Err, attempted unsupported unary operation between id {operation.node.token.type} and {tk.meaning}.", Lexer.text, pos))
            return operation.success(UnaryOperation(tk, operation.node))
        elif tk.type is NaiveTypes.num:
            return operation.success(NumNode(tk))
        elif tk.type is NaiveTypes.circumfix:
            char = tk.meaning
            self.expr(operation)
            if operation.failed is True:
                return
            if self.cur_token.meaning == char.delimiter:
                self.advance()
                return
            else:
                tp_head = tk.position
                tp_head.end = self.cur_token.position.end
                return operation.failure(SyntaxError(f"Err, {char.delimiter} following expected.", Lexer.text, tp_head))
        elif tk.type is NaiveTypes.namespace:
            return operation.success(NamespaceNode(tk))
        return operation.failure(SyntaxError(f"Err, {tk.type} unexpected.", Lexer.text, tk.position))

    def term(self, operation: Operation):
        return self.bin_operate(self.factor, operation, "*/~")

    def expr(self, operation: Operation):
        return self.bin_operate(self.term, operation, "+-")

    def statement(self, operation: Operation):
        return self.bin_operate(self.expr, operation, "=")

    def bin_operate(self, get_operand: Callable, operation: Operation, allowed: List[str]):
        get_operand(operation)
        if operation.failed is True:
            return
        superior = operation.node
        while str(self.cur_token.meaning) in allowed:
            op_token = self.cur_token
            self.advance()
            get_operand(operation)
            if operation.failed is True:
                return
            superior = BinaryOperation(superior, op_token, operation.node)
        return operation.success(superior)
