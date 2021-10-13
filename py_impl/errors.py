from typing import Any, List


def string_with_arrows(text: str, pos):
    start = pos.start - pos.column
    liner = pos.start - start
    snippet = text[start:pos.end].strip('\n')
    line_no = f"{pos.line} |"
    result =  f"\n{line_no}\t{snippet}\n{len(line_no)*' '}\t{'~' * liner}{'^' * (len(snippet)-liner)}"
    if '^' not in result:
        result += '^'
    return result

class MeatloafError:
    def __init__(self, report: str, text: str, position, *args: List[Any]) -> None:
        self.report = report
        self.text = text
        self.position = position

        self.args = args

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}: [{self.position.line}:{self.position.column}] {self.report}{string_with_arrows(self.text, self.position)}"


class SyntaxError(MeatloafError):
    pass

class IllegalMorpheme(MeatloafError):
    pass

class DataError(MeatloafError):
    pass