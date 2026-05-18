#include "symbols.h"
#include <iostream>

void report_symbols(const SymbolTable &table) {

  for (const auto &sym : table) {
    std::cout << sym << ":\t" << table.resolve(sym) << "\n";
  }

  std::cout << std::flush;
}
