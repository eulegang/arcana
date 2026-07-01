import gdb
from enum import IntEnum, auto

voidp = gdb.lookup_type("char").pointer()
charp = voidp
shortp = gdb.lookup_type("uint16_t").pointer()
intp = gdb.lookup_type("int").pointer()
longp = gdb.lookup_type("long").pointer()


class CSymbolTablePrinter:
    "Prints a symbol_table"

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        assert self.__val.address is not None

        length = self.__val["len"]
        cap = self.__val["cap"]

        indexes = (self.__val.address + 1).cast(shortp)
        entries = (indexes.cast(shortp) + cap).cast(charp)

        content = []
        for sym in range(length):
            off = int(indexes[sym])
            content.append(f"{sym} = {(entries + off).string()}")

        return f"{length} {{{', '.join(content)}}}"


class CSymbolTreePrinter:
    "Prints a symbol_tree"

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        assert self.__val.address is not None
        Entry = gdb.lookup_type("struct symbol_entry")

        length = self.__val["len"]
        entries = (self.__val.address + 1).cast(Entry.pointer())

        content = []
        for x in range(length):
            entry = entries[x]

            if entry["flags"] == 1:
                content.append("scope")
            else:
                content.append(f"{entry['sym']}")

        return f"{length}: {', '.join(content)}"


def symbols(val: gdb.Value):
    if val.type.name == "symbol_table":
        return CSymbolTablePrinter(val)
    if val.type.name == "symbol_tree":
        return CSymbolTreePrinter(val)
    return None


class Category(IntEnum):
    meta = 0
    bitset = auto()
    enum = auto()
    struct = auto()
    primitive = auto()
    derive = auto()
    fn = auto()
    alias = auto()


class TypeIdPrinter:
    "Prints a symbol_tree"

    def __init__(self, val: gdb.Value):
        self.__val = val

    def to_string(self):
        payload = self.__val["payload"]

        cat = Category(int((payload >> 28) & 0xF))
        id = int(payload & 0x0FFF_FFFF)

        if cat == Category.meta:
            if id == 0:
                return "null"
            elif id == 1:
                return "poison"

        return f"({cat.name}){id}"


def types(val: gdb.Value):
    if val.type.name == "arcana::types::type_id":
        return TypeIdPrinter(val)
    return None


prog = gdb.current_progspace()
if prog:
    prog.pretty_printers.append(symbols)
    prog.pretty_printers.append(types)
