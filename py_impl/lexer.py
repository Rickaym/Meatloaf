from typing import Any, List, Optional, Union

from .errors import IllegalMorpheme, MeatloafError

#         ----- Understanding the lexer -----
#  The Lexer's purpose is to iterate through the source code character by character and resolve each of them into a token array.
#  A Token is an arbitrary representation of a Morpheme with significance in it's occurance position in the source code (there are
#  other different morpheme representations).
#
#  A morpheme is a container of a type and a value, the type of that value in compiler language is irrelevant to the context of Meatloaf.
#
#  When lexxing the source code, a type guide and a type head is used to memoize certain properties of the lexer. A type guide is in charge of
#  feeding the lexer the right character and to keep track of it's line number, column number and the index number - whereas the type head is
#  used by individual tokens to mark it's location.
#
#
#         ----- Understanding the Type manager -----
#
#  A potential morphemic character is resolved into two types, number and namespace using the native type manager. (class:NativeTypes)
#  Number and Namespace are the starting denominator of all identities. An identity is a representation or a type of a morpheme.
#  For example, the morpheme `1.2` belongs to the Identity class:Num. Whilst `abc`, `+` etc.. belings to the Identity class:Namespace until
#  it is found to be a registered subtype.
#
#  A subtype is an arbitary concept of some subclasses of class:Identity that isn't directly resovled by the native type manager, instead
#  subtypes are resolved by the namespace resolver. The resolver will look at the current morpheme and pass it into the subtypes to evaluate
#  their relatedness. If any such relations are found, the namespace resolver will return a Token of that subtype. If not, it will
#  fall-back as a namespace.


Stack = None
Node = None


class Position:
    __slots__ = ("start", "end", "column", "line")
    def __init__(self, start=1, end=None, column=1, line=1, guide: Optional["TypeGuide"]=None) -> None:
        if guide is not None:
            self.start = guide.pos
            self.end = guide.pos+1
            self.column = guide.column
            self.line = guide.line
        else:
            self.start = start or 0
            self.end = end or start + 1
            self.column = column
            self.line = line

    def copy(self):
        return Position(self.start, self.end, self.column, self.line)


class TypeGuide:
    __slots__ = ("column", "line", "length", "last_line_max", "text", "pos", "char", "new_line")
    def __init__(self, text: str, starting_pos: int=-1) -> None:
        self.text = text
        self.pos = starting_pos
        self.char = ''
        self.length = len(text)
        self.column = -1
        self.line = 1
        self.last_line_max = 0
        self.new_line = False

    def advance(self):
        self.pos += 1
        self.column += 1

        if self.new_line is True:
            self.last_line_max = int(self.column)
            self.line += 1

        self.update_char()
        if self.char == "\n":
            self.new_line = True

    def retreat(self):
        self.pos -= 1
        self.column -= 1

        self.update_char()
        if self.char == "\n":
            self.column = int(self.last_line_max)
            self.line -= 1

    def update_char(self):
        self.char = self.text[self.pos] if self.pos < len(self.text) else None


class Lexer:
    __slots__ = ("type_guide")
    text = None
    delimiters = (' ', '\n')

    def __init__(self, text) -> None:
        Lexer.text = text
        self.type_guide = TypeGuide(text)
        self.type_guide.advance()

    @staticmethod
    def tokenize(text):
        self = Lexer(text)
        tokens = []
        while self.type_guide.char != None:
            if self.type_guide.char in self.delimiters:
                self.type_guide.advance()
                continue
            tk = None
            identity = NaiveTypes.resolve(self.type_guide.char)
            #print(identity, self.type_guide.char, end="")
            if identity is not None:
                tk = identity.get_token(self.type_guide)
            #print(" ", tk, "\n")
            if tk is not None:
                tokens.append(tk)
            else:
                return IllegalMorpheme(f"Erroneous '{self.type_guide.char or 'EOF'}'", self.text, position=Position(guide=self.type_guide))
            self.type_guide.advance()
        else:
            tokens.append(Token(NaiveTypes.eof, " ", position=Position(guide=self.type_guide)))

        return tokens


class Token:
    """
    A container for a morpheme type and the type head of that morpheme.
    """
    def __init__(self, type: "Identity", meaning: Union["Morpheme", Any], position: Position) -> None:
        self.type = type
        self.meaning = meaning if isinstance(meaning, Morpheme) else Morpheme(meaning, self.type, position)
        self.position = position
        self.__str__ = self.__repr__

    def __repr__(self) -> str:
        return f"<{self.type}{f':{self.meaning.value}' if self.meaning.value is not None else ''}>"


class Morpheme:
    """
    A morpheme is a character or a string that belongs to a specific identity.
    """
    def __init__(self, value: str, identity: "Identity", interfix: bool=False, delimiter: Optional[str]=None, unary: bool=False) -> None:
        """
        Rudimentary parameters
        ------------------------
        value: str              = The value represented by this morpheme. The type of this is irrevelant to the context of meatloaf.
        identity: Identity      = The type of the value represented by the morpheme relevant to the context of meatloaf.

        More parameters (insignificant to non-affix morphemes)
        -------------------------------------------------------
        interfix: bool           = Whether if an affix is required for validity.
        delimiter: Optional[str] = The delimiting character, only relevant to circumfixes.
        unary: bool              = Whether if an affix approve it's usage in unary operations.
        precedence: int          = The importance of this morpheme, only relevant in evaluation within compound statements.
        """
        self.value = value
        self.identity = identity

        # Some morphemes are only valid when they are followed by an interfix
        self.interfix = interfix
        self.delimiter = delimiter
        self.unary = unary

    def startswith(self, __s: str):
        return self.value.startswith(__s)

    def __repr__(self) -> str:
        return f"M({self.value}:{self.identity})"

    def __eq__(self, o: object) -> bool:
        return self.value == o

    def __ne__(self, o: object) -> bool:
        return not(self.__eq__(o))

    def __hash__(self) -> int:
        return hash(self.value)

    def __str__(self) -> str:
        return self.value.__str__()


class Identity:
    __slots__ = ("id", "value")

    """
    An identity is matched with a character in code to evaluate it's relationship.

    If it belongs to the identity, it is able to resolve a token until completion.
    An identity is also a container of it's type. This means an identity of `Num` will represent
    and hold any instances of `Num` found in the code.
    """
    def __init__(self, id, value: Optional[Any]=None) -> None:
        self.id = id
        self.value = value

    def get_token(self, guide: "TypeGuide"):
        """
        Resolves full value corresponding to the type identity.
        """

    @staticmethod
    def infix(me: Any, sign: Token, operand: Any, type: object=None, stack: Stack=None) -> Morpheme:
        """
        Evaluates the interaction between instances of the identity and an operand.
        An affix morpheme and the stack is provided for context.
        """

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}{f':{self.value}' if self.value is not None else ''}"

    def __iter__(self):
        return self.value.__iter__()


class Num(Identity):
    """
    Resolves all floating point numbers and integers.
    """
    @staticmethod
    def infix(me: Morpheme, sign: Token, operand: Optional[Morpheme]=None, type: Identity=None, stack: Stack=None) -> float:
        if operand is None:
            if sign.meaning == "-":
                return Morpheme(float.__mul__(float(me.value), -1.0), NaiveTypes.num)
        else:
            m, o = float(me.value), float(operand.value)
            if sign.meaning == "-":
                return Morpheme(float.__sub__(m, o), NaiveTypes.num)
            elif sign.meaning == "+":
                return Morpheme(float.__add__(m, o), NaiveTypes.num)
            elif sign.meaning == "*":
                return Morpheme(float.__mul__(m, o), NaiveTypes.num)
            elif sign.meaning == "/":
                return Morpheme(float.__truediv__(m, o), NaiveTypes.num)

    def get_token(self, guide: TypeGuide):
        result = ''
        dot_count = 0
        column, start, line = int(guide.column), int(guide.pos), int(guide.line)

        while guide.char != None and (NaiveTypes.is_num(guide.char) or guide.char == '.'):
            if guide.char == '.':
                if dot_count == 1:
                    break
                dot_count += 1
                result += '.'
            else:
                result += guide.char
            guide.advance()
        else:
            # to avoid overstepping
            guide.retreat()
        end_pos = int(guide.pos)+1
        if dot_count == 0:
            return Token(NaiveTypes.num, int(result), Position(start, end_pos, column=column, line=line))
        else:
            return Token(NaiveTypes.num, float(result), Position(start, end_pos, column=column, line=line))


class Str(Identity):
    def get_token(self, guide: TypeGuide):
        result = ""
        str_hint = guide.char
        column, start, line = int(guide.column), int(guide.pos), int(guide.line)
        guide.advance()
        while guide.char not in (str_hint, None):
            result += guide.char
            guide.advance()
        end_pos = guide.pos+1
        return Token(NaiveTypes.str, result, Position(start, end_pos, column=column, line=line))


class Affix(Identity):
    # Will be defined later down the line
    infixes: List["Infix"]
    circumfixes: List["Circumfix"]
    interfixes: List["Affix"]

    def get_token(self, guide: TypeGuide):
        infix_matches = list(filter(lambda s: s.startswith(guide.char), self.infixes))
        if len(infix_matches) > 0:
            return NaiveTypes.infix.get_token(guide, infix_matches)

        circumfix_matches = list(filter(lambda s: s.startswith(guide.char), self.circumfixes))
        if len(circumfix_matches) > 0:
            state, tk = NaiveTypes.circumfix.get_token(guide, circumfix_matches)
            if state is False:
                return True, None
            else:
                return state, tk
        return False, None


class Infix(Identity):
    def get_token(self, guide: TypeGuide, expects: List[Morpheme]):
        result = guide.char
        state = True
        # Parse through the text until the completed result is a valid entry of the
        # infixes.
        while result not in NaiveTypes.affix.infixes and guide.char is not None:
            guide.advance()
            result += guide.char
            for name in list(expects):
                if not name.startswith(result) and len(expects) > 1:
                    expects.remove(name)
        else:
            # Sometimes the validity of an affix depends on the upcoming character as well, therefore
            # we check that in a way that doesn't overstep in the type guide.
            upcoming = guide.text[guide.pos+1] if guide.pos+1 != guide.length else None
            if expects[0].interfix is True and upcoming not in Lexer.delimiters and upcoming is not None:
                state = False
                guide.advance()
        if result not in NaiveTypes.affix.infixes:
            state = False
        return state, Token(NaiveTypes.infix, expects[0], Position(guide=guide))


class Circumfix(Identity):
    def get_token(self, guide: TypeGuide, expects: List[Morpheme]):
        result = guide.char
        state = True
        while result not in NaiveTypes.affix.circumfixes and guide.char is not None:
            guide.advance()
            result += guide.char
            for name in list(expects):
                if not name.startswith(result) and len(expects) > 1:
                    expects.remove(name)
        else:
            upcoming = guide.text[guide.pos+1] if guide.pos+1 != guide.length else None
            if expects[0].interfix is True and upcoming not in Lexer.delimiters and upcoming is not None:
                state = False

        if result not in NaiveTypes.affix.circumfixes:
            state = False
        return state, Token(NaiveTypes.circumfix, expects[0], Position(guide=guide))


class Namespace(Identity):
    @staticmethod
    def infix(me: Morpheme, sign: Token, operand: Optional[Morpheme]=None, type: Identity=None, stack: Stack=None):
        if operand is None:
            if sign.meaning == "~":
                return stack.namespace.get(me)
            else:
                if isinstance(me, MeatloafError):
                    name = me.args[0]
                else:
                    name = me
                stack.namespace.set(name, Morpheme(None, NaiveTypes.namespace))
                return Morpheme(name, NaiveTypes.namespace)
        elif sign.meaning == "=":
            return stack.namespace.set(me.value, Morpheme(operand.value, operand.identity))
        elif sign.meaning == "*":
            return Morpheme(float.__mul__(float(me.value), float(operand.value)), NaiveTypes.num)
        elif sign.meaning == "+":
            return Morpheme(float.__add__(float(me.value), float(operand.value)), NaiveTypes.num)
        elif sign.meaning == "-":
            return Morpheme(float.__sub__(float(me.value), float(operand.value)), NaiveTypes.num)
        elif sign.meaning == "/":
            return Morpheme(float.__truediv__(float(me.value), float(operand.value)), NaiveTypes.num)
        return MeatloafError("Un-implemented report.", Lexer.text, Position())

    def get_token(self, guide: TypeGuide):
        result = ''
        matches = list(filter(lambda s: s.startswith(str(guide.char)), (*NaiveTypes.affix.infixes, *NaiveTypes.affix.circumfixes)))
        if len(matches) > 0:
            state, tk = NaiveTypes.affix.get_token(guide)
            if state is True:
                return tk
            else:
                result += tk.meaning.value
        while (guide.char not in NaiveTypes.affix.infixes
              and guide.char not in NaiveTypes.affix.circumfixes
              and guide.char not in Lexer.delimiters
              and guide.char is not None):
            result += guide.char
            guide.advance()
        else:
            guide.retreat()

        if len(result) > 0:
            return Token(NaiveTypes.namespace, result, Position(guide=guide))


class EOF(Identity):
    pass


class NaiveTypes:
    __slots__ = []

    # Register types here
    eof = EOF(0x0)
    infix = Infix(0x1)
    affix = Affix(0x2)
    circumfix = Circumfix(0x3)

    num = Num(0x5)
    str = Str(0x6)
    namespace = Namespace(0x7)

    @staticmethod
    def resolve(character):
        """Invoked under a character digit

        Returns the estimated type of the upcoming characters and of itself"""
        if NaiveTypes.is_num(character):
            return NaiveTypes.num
        else:
            return NaiveTypes.namespace

    @staticmethod
    def is_num(key):
        return key in "0123456789"


#
# Define infixes, circumfixes and affixes
#

Infix.PLUS = Morpheme(r"+", NaiveTypes.infix, unary=True)
Infix.MINUS = Morpheme(r"-", NaiveTypes.infix, unary=True)
Infix.MUL = Morpheme(r"*", NaiveTypes.infix)
Infix.DIV = Morpheme(r"/", NaiveTypes.infix)
Infix.EQ = Morpheme(r"=", NaiveTypes.infix)
Infix.EST = Morpheme(r"~", NaiveTypes.infix, unary=True)
Infix.EPS = Morpheme(r"...", NaiveTypes.infix)

Infix.RETURN = Morpheme(r"return", NaiveTypes.infix, interfix=True, unary=True)
Infix.NUM_TYPE = Morpheme(r"num", NaiveTypes.infix, interfix=True, unary=True)
Infix.STR_TYPE = Morpheme(r"str", NaiveTypes.infix, interfix=True, unary=True)

Affix.SPACE = Morpheme(r" ", NaiveTypes.affix)

Circumfix.LPARAN = Morpheme(r"(", NaiveTypes.circumfix, delimiter=r")")
Circumfix.RPARAN = Morpheme(r")", NaiveTypes.circumfix)
Circumfix.DQUOTE = Morpheme(r'"', NaiveTypes.circumfix, delimiter=r'"')
Circumfix.SQUOTE = Morpheme(r"'", NaiveTypes.circumfix, delimiter=r"'")

Affix.infixes = [val for val in Infix.__dict__.values() if isinstance(val, Morpheme)]
Affix.circumfixes = [val for val in Circumfix.__dict__.values() if isinstance(val, Morpheme)]
Affix.interfixes = (Affix.SPACE,)
