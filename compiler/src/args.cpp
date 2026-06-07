#include <bit>
#include <cstdlib>
#include <getopt.h>

#include <iostream>
#include <string_view>

#include <chroma.h>
#include <mfile.h>

bool verbose = false;
std::string_view path;
uint16_t stops = 0;
std::string_view output;
std::string output_backing;

void parse_args(int argc, char **argv) {
  static struct option long_options[] = {
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {"tokenize", no_argument, 0, 't'},
      {"ast", no_argument, 0, 'a'},
      {"types", no_argument, 0, 'T'},
      {"symbols", no_argument, 0, 's'},
      {"llvm", required_argument, 0, 'l'},
      {"output", required_argument, 0, 'o'},
      {0, 0, 0, 0},
  };

  int c;
  while ((c = getopt_long(argc, argv, "vhtasTo:l", long_options, NULL)) != -1) {
    switch (c) {
    case 'v':
      verbose = true;
      break;

    case 't':
      stops |= 1;
      break;

    case 'a':
      stops |= 2;
      break;

    case 's':
      stops |= 4;
      break;

    case 'T':
      stops |= 8;
      break;

    case 'l':
      stops |= 16;
      break;

    case 'o':
      output = optarg;
      break;

    case 'h':
      std::cout << "sysl-tokenize [-v] [files...]" << std::endl;
      exit(0);
      break;
    }
  }

  if (optind < argc) {
    path = argv[optind++];
  }
}

void validate_args() {
  if (std::popcount(stops) > 1) {
    std::cerr << chroma::red << "multiple phase stops specified" << std::endl;

    exit(1);
  }

  if (path.empty()) {
    std::cerr << chroma::red << "file not specified" << std::endl;
    exit(1);
  }

  if (!path.ends_with(".arc")) {
    std::cerr << chroma::red << "file does not container arcana" << std::endl;
    exit(1);
  }

  if (output.empty()) {
    output_backing = path.substr(0, path.length() - 4);
    output_backing += ".o";
    output = output_backing;
  }
}

mfile read_file() {
  try {
    std::string p{path};
    mfile file{p};

    return file;
  } catch (...) {
    std::cerr << chroma::red << "file \"" << path << "\" does not exist"
              << std::endl;
    exit(1);
  }
}
