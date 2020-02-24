#include "network.hpp"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <utility>
#include <array>
#include <sstream>
#include <cmath>

std::mutex mlock;

std::vector<pattern> pattern::patterns = std::vector<pattern>();
std::vector<network> network::networks = std::vector<network>();

PerlinNoise pattern::perlin = PerlinNoise(time(NULL));

extern const char * const patternFile = "patterns.dat";
extern const char * const networkFile = "selection.dat";

void pattern::loadAll(){
  std::fstream f;
  f.open(patternFile, std::ios_base::in);
  if(!f){
    std::cerr << "Erorr while opening patterns file" << std::endl;
  }
  std::string line;
  unsigned pat_id;
  unsigned s_l = std::string("pattern:").length();
  while(std::getline(f, line)){
    if(line.at(0) == '#') continue;
    if(line.find("pattern:") == 0){
     pat_id = std::stoi(line.substr(s_l));
     pattern::patterns.push_back({.width=0, .height=0, .index=pat_id});
     continue;
    }
    pattern * lastref = &pattern::patterns.at(patterns.size()-1);
    lastref->width = line.length();
    lastref->height++;
    for(unsigned i = 0; i < line.length(); i++){
      lastref->lweight.push_back(line.at(i)-'0');
    }

  }
  f.close();
}

network::data network::lineToEntry(std::string const & line){
  std::vector<std::string> split;
  std::istringstream iss(line);

  for(std::string token; std::getline(iss, token, ':');){
    split.push_back(std::move(token));
  }

  data d;
  d.digit = std::stoi(split.at(0));
  d.patNo = std::stoi(split.at(1));
  d.weight = std::stod(split.at(2));
  d.xPercentage = std::stod(split.at(3));
  d.yPercentage = std::stod(split.at(4));
  d.xSpan = std::stod(split.at(5));

  return d;
}

void network::saveAll(){
  std::fstream f;
  f.open(networkFile, std::ios_base::out | std::ios_base::trunc);
  if(!f){
    std::cerr << "error while opening networks file" << std::endl;
  }
  f << "#[Digit]:[Pattern number]:[weight]:[xPercentage]:[yPercentage]:[xSpan]\n";
  for(unsigned i = 0; i < network::networks.size(); i++){
    network * n = &network::networks.at(i);
    for(unsigned j = 0; j < n->entry.size(); j++){
      data * d = &n->entry.at(j);
      f << std::fixed <<  d->digit << ":" << d->patNo << ":" << d->weight << ":" << d->xPercentage << ":" << d->yPercentage << ":" << d->xSpan << "\n";
    }
    f << "-end\n";
  }

  f.close();
}

void network::loadAll(){
  std::fstream f;
  f.open(networkFile, std::ios_base::in);
  if(!f){
    std::cerr << "Error while opening networks file" << std::endl;
  }
  std::string line;
  network n;
  while(std::getline(f, line)){
    if(line.at(0) == '#') continue;
    if(line == "-end"){
      n.gGuess = 0;
      n.bGuess = 0;
      networks.push_back(n);
      n.entry.clear();
    }
    if(line.length() > 5){
      n.entry.push_back(lineToEntry(line));
    }
  }
  f.close();

  std::cout << "Loaded " << networks.size() << " networks." << std::endl;
  for(unsigned i = 0; i < networks.size(); i++){
    network * n = &networks.at(i);
    for(unsigned j = 0; j < n->entry.size(); j++){
      std::cout << "Network " << i << ":" << std::endl;
      std::cout << "\txSpan: " << n->entry.at(j).xSpan << std::endl;
      std::cout << "\tDigit: " << n->entry.at(j).digit << "\n\tPattern number: " << n->entry.at(j).patNo << std::endl;
      std::cout << "\tWeight: " << n->entry.at(j).weight << "\n\txPercentage: " << n->entry.at(j).xPercentage << std::endl;
      std::cout << "\tyPercentage: " << n->entry.at(j).yPercentage << std::endl;
    }
  }
}

void pattern::savePattern(pattern * p){
  std::fstream f;
  f.open(patternFile, std::ios_base::app | std::ios_base::out);
  if(!f){
    std::cerr << "Error while opening patterns file for saving" << std::endl;
  }
  unsigned wr = p->width;
  std::string line = "";
  f << "pattern:"+std::to_string(p->index)+"\n";
  for(unsigned i = 0; i < p->lweight.size(); i++){
    if(wr){
       line += p->lweight.at(i)+'0';
       wr--;
    }
    else{
      f << line + '\n';
      line = "";
      wr = p->width;
    }
  }
  f.close();
}

int pattern::mutate(unsigned no){
  int m_nums = rand() % 30;
  pattern p;
  bool found = false;
  for(unsigned i = 0; i < pattern::patterns.size(); i++){
    if(pattern::patterns.at(i).index == no){
      p = pattern::patterns.at(i);
      found  = true;
      break;
    }
  }
  if(!found) return -1;
  for(int i = 0; i < m_nums; i++){
    int idx = rand() % p.lweight.size();
    p.lweight.at(idx) += rand() % 10 - 5;
    if(p.lweight.at(idx) < 0) p.lweight.at(idx) = 0;
    if(p.lweight.at(idx) > 9) p.lweight.at(idx) = 9;
  }
  p.index = std::max((unsigned)patterns.size(), patterns.at(patterns.size()-1).index+1);
  patterns.emplace_back(p);
  savePattern(&patterns.at(patterns.size()-1));
  return p.index;
}

unsigned pattern::createNew(){
  int width = rand () % 20 + 2;
  int height = rand () % 40 + 2;
  pattern p;
  p.index = std::max((unsigned)patterns.size(), patterns.at(patterns.size()-1).index+1);
  patterns.push_back(p);
  pattern * c = &patterns.at(pattern::patterns.size()-1);
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){
      double xp = ((double)x)/(double)width;
      double yp = ((double)y)/(double)height;
      double val = perlin.noise(xp, yp, 0.5);
      val *= 10;
      if(val >= 9) val = 9;
      if(val <= 0) val = 0;
      c->lweight.push_back(round(val));
    }
  }
  c->width = width;
  c->height = height;
  savePattern(c);
  return c->index;
}

void network::mutate(){
  int m_type = rand() % 7;
  double modif = 1.2-lastscore;
  if(m_type == 0){ //mutate a single pattern
    int d_id = rand() % entry.size();
    data d = entry.at(d_id);
    int newno = pattern::mutate(d.patNo);
    if(newno != -1) d.patNo = newno;
    entry.at(d_id) = d;
  }
  if(m_type == 1){//create a new random pattern
    data d;
    d.digit = rand() % 9 + 1;
    d.patNo = pattern::createNew();
    d.weight = ((double)(rand() % 1000 - 500))/1000.0;
    d.xPercentage = ((double)(rand() % 100 + 1))/100.0;
    d.yPercentage = ((double)(rand() % 100 + 1))/100.0;
    d.xSpan = 1;
    entry.push_back(d);
  }
  if(m_type == 2){//remove random pattern
    if(entry.size() > 20){
      entry.erase(entry.begin() + (rand() % entry.size()));
    }
  }
  if(m_type == 3){//alter weight of random pattern
    for(unsigned i = 0; i < 5; i++){
      int r = rand() % entry.size();
      entry.at(r).weight += modif*((double)(rand() % 500-250))/1000.0f;
    }
  }
  if(m_type == 4){//move random pattern
    double disp_x = ((double)(rand() % 1000))/1000.f;
    double disp_y = ((double)(rand() % 1000))/1000.f;
    int r = rand() % entry.size();
    data * d = &entry.at(r);
    d->xPercentage += disp_x*modif;
    d->yPercentage += disp_y*modif;
    if(d->xPercentage < 0) d->xPercentage = 0;
    else if(d->xPercentage > 0.95) d->xPercentage = 0.95;
    if(d->yPercentage < 0) d->yPercentage = 0;
    else if(d->yPercentage > 0.95) d->yPercentage = 0.95;
  }
  if(m_type == 5){//change xSpan
    for(int i = 0; i < 1 + (rand() % 3); i++){
      data * d = &entry.at(rand() % entry.size());
      double dsp = ((double)(rand() % 100)-50)/300.0;
      d->xSpan += dsp*modif;
      if(d->xSpan > 1) d->xSpan = 1;
      if(d->xSpan < 0) d->xSpan = 0;
    }
  }
  if(m_type == 6){//append existing pattern
    data d;
    d.digit = rand() % 9 + 1;
    d.patNo = pattern::patterns.at(rand() % pattern::patterns.size()).index;
    d.weight = ((double)(rand() % 1000 - 500))/500;
    d.xPercentage = ((double)(rand() % 95))/100;
    d.yPercentage = ((double)(rand() % 95))/100;
    d.xSpan = ((double)(rand() % 80 + 10))/100;
    entry.push_back(d);
  }
}

int network::analyse(int iWidth, int iHeight, int sWidth, int digit, unsigned char * imageData){
  double digitWeight[9];
  for(unsigned i = 0; i < 9; i++) digitWeight[i] = 0;
  for(unsigned i = 0; i < entry.size(); i++){
    double sum = 0;
    data * d = &entry.at(i);
    int ddig = d->digit;
    double xperc = d->xPercentage;
    double yperc = d->yPercentage;
    double xspan = d->xSpan;
    pattern * p = &pattern::patterns.at(d->patNo);
    double dsavw = xperc*iWidth;
    double dsavh = yperc*iHeight;
    int sx = iWidth;
    int sy = iHeight;
    if(xperc*iWidth + xspan*iWidth < sx) sx = xperc*iWidth + xspan*iWidth;
    if(xperc*iHeight + xspan*iWidth < sy) sy = xperc*iHeight + xspan*iWidth;
    int xdisp = sx-xperc*iWidth;
    int ydisp = sy-xperc*iHeight;
    int pwidth = p->width;
    int pheight = p->height;
    for(int x = (int)dsavw; x < sx; x++){
      for(int y = (int)dsavh; y < sy; y++){
        double xpl = x-dsavw;
        double ypl = y-dsavh;
        xpl /= xdisp;
        ypl /= ydisp;
        unsigned idx = (unsigned)(xpl*pwidth + ypl*pheight*pwidth);
        if(idx >= p->lweight.size()) idx = p->lweight.size()-1;

        double weight = p->lweight.at(idx) - 5;
        unsigned char * dt = imageData;
        dt += y*iWidth+x;
        if(*dt)
          sum += weight;
        else
          sum -= weight;
        //mlock.unlock();
      }
    }
    digitWeight[ddig-1] += sum;
  }
  double maxweight = digitWeight[0];
  int dgt = 1;
  for(unsigned i = 0; i < 9; i++){
    if(digitWeight[i] > maxweight){
      maxweight = digitWeight[i];
      dgt = i+1;
    }
  }
  if(digit == -1)
    return dgt;
  if(dgt == digit){
    gGuess++;
  }
  return dgt;
}
