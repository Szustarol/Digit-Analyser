#ifndef NETWORK_HPP
#define NETWORK_HPP
#include <vector>
#include <atomic>
#include <string>
#include "../PerlinNoise.h"


extern const char * const patternFile;
extern const char * const networkFile;

struct pattern{
  static std::vector<pattern> patterns;
  static unsigned createNew();
  static void savePattern(pattern * p);
  std::vector <char> lweight;
  unsigned width;
  unsigned height;
  unsigned index;
  static int mutate(unsigned no);
  static void loadAll();
  static void save(unsigned index);
  static PerlinNoise perlin;
};

struct network{
  static std::vector<network> networks;
  static void loadAll();
  static void saveAll();
  struct data{
    int digit;
    unsigned patNo;
    double weight;
    double xPercentage;
    double yPercentage;
    double xSpan;
  };
  static data lineToEntry(std::string const & line);
  std::vector <data> entry;
  int analyse(int iWidth, int iHeight, int sWidth, int digit, unsigned char * imageData);
  double gGuess = 0; double bGuess = 0;
  double lastscore;
  void mutate();
};



#endif
