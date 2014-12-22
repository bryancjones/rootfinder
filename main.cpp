#include "Search.h"
#include "App.h"
#include <stdio.h>

int main(const int argc, const char* argv[]) {
  printf("17mss3, Melanie Subbiah, mss3@williams.edu\n16bcj2, Bryan Jones, bcj2@williams.edu\n");
  std::string caption = "Masterpiece";
  //It is better to provide a window with even dimensions
  Search masterpiece(caption, 100, 100);
  masterpiece.run();
  return 0;
}
