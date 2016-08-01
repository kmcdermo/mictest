#include <fstream>

void avgtime(int j, int k)
{
  std::ifstream input;
  input.open(Form("times_nth%i_nhits%i.txt",j,k),std::ios_base::in);
  float time;
  float times;
  int tot =0 ;
  while (input >> time){
    times +=  time;
    tot++;
  }
  std::cout << "nTh: " << j << " nHits/task: " << k  << " time:" << times / tot << std::endl;
  input.close();

  std::ofstream output;
  output.open("totals.csv",std::ios_base::app);
  output << "nTH," << j << ",nHitsPerTask," << k << ",time," << times / tot << std::endl;
  output.close();
}
