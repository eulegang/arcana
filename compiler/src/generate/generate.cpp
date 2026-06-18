#include "../generate.h"

void gen::Generator::generate() {
  TypesComponent{emitter, unit}.generate();
  ForeignComponent{emitter, unit}.generate();
  EntriesComponent{emitter, unit}.generate();
  EntryComponent{emitter, unit}.generate();
}
