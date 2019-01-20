
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>
#include <queue>
#include <memory>
#include <vector>
#include <string>

struct node
{
  node(char character, unsigned frequency)
    :left(nullptr), right(nullptr), character(character), frequency(frequency) {}

  node(const std::shared_ptr<node>& n1, const std::shared_ptr<node>& n2);

  void generate_codes(std::vector<std::string>& codes, std::string& code) const;
  

  std::shared_ptr<node> left;
  std::shared_ptr<node> right;
  unsigned frequency;
  char character;
};

using FreqContainer = std::array<unsigned, 256>;

std::vector<std::string> generate_codes(FreqContainer frequencies);
void write_compressed(const std::vector<std::string>& codes, FreqContainer frequencies);
void write_decompressed(const std::vector<std::string>& codes, FreqContainer frequencies);
void compress(FreqContainer frequencies);
void decompress(FreqContainer frequencies);


node::node(const std::shared_ptr<node>& n1, const std::shared_ptr<node>& n2)
{
  frequency = n1->frequency + n2->frequency;

  if (n1->frequency < n2->frequency){
    left = n1;
    right = n2;
  } else{
    left = n2;
    right = n1;
  }
}

void node::generate_codes(std::vector<std::string>& codes, std::string & code) const 
{
  if (!left && !right)
  {
    codes[character] = code;
    return;
  }

  if (left)
  {
    code += '0';
    left->generate_codes(codes, code);
    code.erase(code.end() - 1);
  }

  if (right)
  {
    code += '1';
    right->generate_codes(codes, code);
    code.erase(code.end() - 1);
  }
}



void write_compressed(const std::vector<std::string>& codes, FreqContainer frequencies)
{  
  size_t size = frequencies.size();
  
  for(unsigned i = 0; i < size; ++i)
    std::cout.write((const char*)&frequencies[i], sizeof(frequencies[i]));
   
  char curr_char;
  char curr_byte = 0;
  int bit_counter = 0;
    
  std::cin.clear();
  std::cin.seekg(0);
  std::cin >> std::noskipws;

  while(std::cin >> curr_char){
    for(unsigned i = 0; i < codes[curr_char].size(); i++, bit_counter++){
      if(bit_counter == 8){
        std::cout<< curr_byte;
        curr_byte = 0;
        bit_counter = 0;
      }

      if(codes[curr_char][i] == '1')
                curr_byte = curr_byte | (0x01 << bit_counter);
    }
  }
  
  if(bit_counter)
    std::cout << curr_byte;
}

void write_decompressed(const std::vector<std::string>& codes, FreqContainer frequencies)
{
  char curr_byte = 0;
  std::string code;
  size_t size = frequencies.size();

  while(std::cin.read(&curr_byte,1))
  {
    for(int i = 0; i < 8; ++i) {
      if((curr_byte >> i) & 0x01)
        code += '1';
      else
        code += '0';

      for(size_t i = 0; i < size; ++i) {
        if(codes[i] == code) {
          if(frequencies[i]) {
            std::cout << (char) i;
            code.clear();
            --frequencies[i];
            break;
          }
          else
            return;
        }
      }
    }
  }
}

std::vector<std::string> generate_codes(FreqContainer frequencies)
{
  auto cmp = [](const std::shared_ptr<node>& a, const std::shared_ptr<node>& b) {return a->frequency > b->frequency; };

  std::priority_queue<std::shared_ptr<node>, std::vector<std::shared_ptr<node>>, decltype(cmp)> nodes(cmp);

  for (size_t i = 0; i < frequencies.size(); ++i)
    if (frequencies[i])
      nodes.emplace(new node((char)i, frequencies[i]));

  while (nodes.size() > 1)
  {
    auto n1 = nodes.top();
    nodes.pop();
    auto n2 = nodes.top();
    nodes.pop();

    nodes.emplace(new node(n1, n2));
  }

  std::vector<std::string> codes(frequencies.size());
  std::string code;

  nodes.top()->generate_codes(codes, code);

  return codes;
}

void compress(FreqContainer frequencies)
{
  std::cin >> std::noskipws;
  char c;
  while (std::cin >> c)
    ++frequencies[c];

  write_compressed(generate_codes(frequencies), frequencies);
}

void decompress(FreqContainer frequencies)
{
  auto size = frequencies.size();
  
  std::cin >> std::noskipws;

  for(size_t i = 0; i < size; ++i){
    std::cin.read((char *)&frequencies[i],4);
  }

  write_decompressed(generate_codes(frequencies), frequencies);
}


int main(int argc, char** argv)
{
  FreqContainer frequencies = {0};
  if(argc == 2){
    if(argv[1][0] == '-' && argv[1][1] == 'd')
      decompress(frequencies);
  }else{
    compress(frequencies);
  }
  
  return 0;
}