#include "full3Dstack.h"

using namespace EI;

void print_temperatures(energy_introspector_t *energy_introspector, int iter)
{
	string path, layer;
    char buffer[50];
	for(int i = 0; i < 6; i++)
	{
	  switch (i)
	  {
	  case 0:
		  path = "DRAM3_DIE:VAULT";
		  layer = "DRAM3_T";
		  break;
	  case 1:
		  path = "DRAM2_DIE:VAULT";
		  layer = "DRAM2_T";
		  break;
	  case 2:
		  path = "DRAM1_DIE:VAULT";
		  layer = "DRAM1_T";
		  break;
	  case 3:
		  path = "DRAM0_DIE:VAULT";
		  layer = "DRAM0_T";
		  break;
	  case 4:
		  path = "DL2_DIE:SPOT";
		  layer = "L2CACHE_T";
		  break;
	  case 5:
		  layer = "CORE_T";
		  break;
	  default:
		  exit(0);
	  }
	  for(int j = 0; j < 16; j++)
	  {
		  int n;
		  vector<double> core_power;
		  core_power.resize(5);

		  if (flag)
		  {
			  cout << layer << "\t";
			  flag = false;
		  }
		  if( i == 5 )
		  {
			  path = "CORE_DIE:FRT";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  core_power[0] = energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature");
			  }

			  path = "CORE_DIE:FPU";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  core_power[1] = energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature");
			  }

			  path = "CORE_DIE:INT";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  core_power[2] = energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature");
			  }

			  path = "CORE_DIE:SCH";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  core_power[3] = energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature");
			  }

			  path = "CORE_DIE:MEM";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  core_power[4] = energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature");
			  }

			  cout << double (std::accumulate(core_power.begin(), core_power.end(), 0.0) / core_power.size()) << "\t";
		  }
		  else
		  {
			  if (flag)
			  {
				  cout << layer << "\t";
				  flag = false;
			  }
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
				  cout << energy_introspector->pull_data<double>(double (TIME_STEP*iter + TIME_STEP),"partition",string(buffer),"temperature") << "\t";
			  }
			  else
			  {
				  cout << "Error in concatenating string." << endl;
				  exit(0);
			  }
		  }
	  }
	  cout << endl;
	  flag = true;
	}
}

void input_power(energy_introspector_t *energy_introspector, int iter)
{
	if(iter == 0)
	{
		cout << "INPUT POWER (W)" << endl;
		for(int i = 0; i < 6; i++)
		{

		  switch (i)
		  {
		  case 0:
			  cout << "DRAM3" << "\t";
			  break;
		  case 1:
			  cout << "DRAM2" << "\t";
			  break;
		  case 2:
			  cout << "DRAM1" << "\t";
			  break;
		  case 3:
			  cout << "DRAM0" << "\t";
			  break;
		  case 4:
			  cout << "L2CACHE" << "\t";
			  break;
		  case 5:
			  cout << "CORE*5" << "\t";
			  break;
		  default:
			  cout << "Error in INPUT POWER vector";
			  exit(0);
		  }
		  for(int j = 0; j < 16; j++)
			  cout << stackpower[i][j].total << "\t";
		  cout << endl;
		}
	}
	string path;
	char buffer[50];
	for(int i = 0; i < 6; i++)
	{
	  switch (i)
	  {
	  case 0:
		  path = "DRAM3_DIE:VAULT";
		  break;
	  case 1:
		  path = "DRAM2_DIE:VAULT";
		  break;
	  case 2:
		  path = "DRAM1_DIE:VAULT";
		  break;
	  case 3:
		  path = "DRAM0_DIE:VAULT";
		  break;
	  case 4:
		  path = "DL2_DIE:SPOT";
		  break;
	  case 5:
		  break;
	  default:
		  exit(0);
	  }
	  for(int j = 0; j < 16; j++)
	  {
		  int n;
		  if( i == 5 )
		  {
			  path = "CORE_DIE:FRT";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }

			  path = "CORE_DIE:FPU";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }

			  path = "CORE_DIE:INT";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }

			  path = "CORE_DIE:SCH";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }

			  path = "CORE_DIE:MEM";
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }
		  }
		  else
		  {
			  n = sprintf(buffer, "%s%d", path.c_str(), j);
			  if(n)
			  {
	//				  cout << buffer << endl;
				  energy_introspector->push_data<power_t>(double (TIME_STEP*iter + TIME_STEP), TIME_STEP,"partition",string(buffer),"power", stackpower[i][j]);
			  }
			  else
			  {
				  cout << "Error in concatenating string." << endl;
				  exit(0);
			  }
		  }

	  }
	//	  cout << endl;
	}
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cout << "Requires filename, number of time steps and period (s) to be passed." << endl;
    cout << "Usage: ./test-3dice filename.txt time_steps period" << endl;
    exit(0);
  }

  energy_introspector_t *energy_introspector = new energy_introspector_t((char*)"CONFIG/3DICE/3d-ice_test1.config");

  string line;
  string::size_type sz;
  ifstream powerfile;
  char delim = ' ';
  vector<string> split_line;
  powerfile.open(argv[1]);
  int T_S = atoi(argv[2]);
  double TIME_STEPS = atof(argv[3]);
  if (powerfile.is_open())
  {
	  int linenum = 0;
	  while( getline(powerfile,line) )
	  {
		  if( line == "DRAM3" || line == "DRAM2" || line == "DRAM1" || line == "DRAM0" || line == "L2CACHE" || line == "CORES" )
		  {
//			  cout << line << endl;
			  continue;
		  }
		  else
		  {
//			  cout << line << endl;
			  split_line = split(line, delim);
			  int square = 0;
			  vector<string>::iterator it;
			  for(square = 0, it = split_line.begin(); it != split_line.end(); ++it, square++)
			  {
//				  cout << "Square: " << square << "\t";
				  stackpower[linenum][square].total = atof((*it).c_str());
			  }
//			  cout << endl << "linenum: " << linenum << endl;
			  linenum++;
		  }
	  }
	  powerfile.close();
  }
  else
  {
	  cout << "Unable to open file." << endl;
	  exit(0);
  }


  for(int i = 0; i < T_S; i++)
  {
	  cout << "TIME: " << double(i*TIME_STEP) << "s" << endl;
	  input_power(energy_introspector, i);
  	  cout << "TEMPERATURE (K)" << endl;
	  energy_introspector->compute_temperature(double (TIME_STEP*i + TIME_STEP),TIME_STEP,"3DICE");
	  print_temperatures(energy_introspector, i);

//  grid_t<double> thermal_grid = energy_introspector->pull_data<grid_t<double> >(TIME_STEP,"package","3DICE","thermal_grid");

  /*for(int row_index = 0; row_index < 100; row_index++)
  {
    for(int col_index = 0; col_index < 100; col_index++)
      cout << thermal_grid.pull(col_index,row_index,0) << "  ";

    cout << endl;
  }*/
  }

  return 0;
}
