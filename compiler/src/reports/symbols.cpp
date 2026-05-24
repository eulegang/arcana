#include "../reports.h"
#include <chroma.h>
#include <iostream>

void report::symbols(const SymbolTable &table) {

  for (const auto &sym : table) {
    std::cout << chroma::cyan << sym << chroma::clear << ":\t" << chroma::green
              << table.resolve(sym) << chroma::clear << std::endl;
  }
}
