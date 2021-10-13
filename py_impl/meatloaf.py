from raw.bread.errors import MeatloafError
import time

from raw.bread.interpreter import Interpreter, NamespaceTable, Stack
from raw.bread.parser import Parser
from raw.bread.lexer import Lexer
from time import time

"""
TODO: Make Interpreted errors better
"""
print("\nWelcome to the Meatloaf interactive grill\n Get ready to make your meat and we'll bread em.\nVersion 0.0.0.a\n")
global_namespace = NamespaceTable()
global_stack = Stack("<program>", namespace=global_namespace)

while True:
    start = time()

    status = Lexer.tokenize(input(f"->- "))
    if isinstance(status, MeatloafError):
        print("...", repr(ast).replace("\n", "\n    ", -1))
        continue
    else:
        print("...", status)

    ast = Parser.parse(status)
    if isinstance(ast, MeatloafError):
        print("...", repr(ast).replace("\n", "\n    ", -1))
        continue
    else:
        print("...", ast)

    results = Interpreter.interpret(global_stack, ast)
    print(results)