from os import error
from bread.interpreter import Interpreter
from bread.parser import Parser
from bread.lexer import Lexer
from time import time

def interpret(path: str):
    start = time()
    with open(path, "r", encoding="utf-8") as payload:
        lex = Lexer(payload.read())
        tks, error = lex.tokenize()
    if error:
        print(error)
    else:
        print(tks)
    parser = Parser(tks)
    ast = parser.parse()
    if isinstance(ast, list):
        print(ast, time()-start)
    else:
        print(ast)
        return

    preter = Interpreter()
    for ln in ast:
        preter.visit(ln)

interpret("hello_world.meat")
